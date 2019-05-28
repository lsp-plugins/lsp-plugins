/*
 * room_builder_ui.cpp
 *
 *  Created on: 28 мая 2019 г.
 *      Author: sadko
 */

#include <ui/plugins/room_builder_ui.h>
#include <metadata/plugins.h>
#include <metadata/ports.h>

namespace lsp
{
    room_builder_ui::CtlFloatPort::CtlFloatPort(room_builder_ui *ui, size_t offset, const char *pattern, const port_t *meta):
        CtlPort(meta)
    {
        pUI         = ui;
        sPattern    = pattern;
        nOffset     = offset;
    }

    room_builder_ui::CtlFloatPort::~CtlFloatPort()
    {
        pUI         = NULL;
        sPattern    = NULL;
        nOffset     = 0;
    }

    float room_builder_ui::CtlFloatPort::get_value()
    {
        // The record is accessible?
        size_t selected = pUI->nSelected;
        if (selected >= pUI->vObjectProps.size())
            return get_default_value();

        obj_props_t *props = pUI->vObjectProps.get(selected);
        if (props == NULL)
            return get_default_value();

        float *ptr  = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(props) + nOffset);
        return limit_value(pMetadata, *ptr);
    }

    void room_builder_ui::CtlFloatPort::set_value(float value)
    {
        // The record is accessible?
        size_t selected = pUI->nSelected;
        if (selected >= pUI->vObjectProps.size())
            return;
        obj_props_t *props = pUI->vObjectProps.get(selected);
        if (props == NULL)
            return;

        // Limit the value
        value       = limit_value(pMetadata, value);

        // The value has changed?
        float *ptr  = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(props) + nOffset);
        if (*ptr == value)
            return;

        // Save new value
        *ptr        = value;

        // Emit new OSC message
        if (pUI->pOscOut == NULL)
            return;

        osc::packet_t packet;
        osc::forge_t forge;
        osc::forge_frame_t sframe, message;

        packet.data     = NULL;
        status_t res    = osc::forge_begin_dynamic(&sframe, &forge, 0x100);

        if (res == STATUS_OK)
        {
            char address[0x100]; // Should be enough
            int count = sprintf(address, "/scene/objects/%d/", int(selected));
            if (count <= 0)
                return;
            strcpy(&address[count], sPattern);

            if (osc::forge_begin_message(&message, &sframe, address) == STATUS_OK)
            {
                osc::forge_float32(&message, value);
                osc::forge_end(&message);
            }
            osc::forge_end(&sframe);
            res = osc::forge_close(&packet, &forge);
            osc::forge_destroy(&forge);

            // Write the packet data and free memory
            if (res == STATUS_OK)
                pUI->pOscOut->write(packet.data, packet.size);
            if (packet.data != NULL)
                osc::forge_free(packet.data);
        }
    }

    void room_builder_ui::CtlFloatPort::process_message(osc::parse_frame_t *message, size_t id, const char *path)
    {
        // Does the path match?
        if (strcmp(path, sPattern) != 0)
            return;

        // The target record is accessible?
        obj_props_t *props = pUI->vObjectProps.get(id);
        if (props == NULL)
            return;

        // Fetch the floating-point value
        float value;
        status_t res;
        osc::parse_token_t token;

        if (pMetadata->unit == U_BOOL)
        {
            bool bvalue = false;
            res = osc::parse_bool(message, &bvalue);
            value = (bvalue) ? 1.0f : 0.0f;
        }
        else
            res = osc::parse_float32(message, &value);

        if (res != STATUS_OK)
            return;
        res = osc::parse_token(message, &token);
        if ((res != STATUS_OK) || (token != osc::PT_EOR))
            return;

        lsp_trace("%s[%d] = %f", sPattern, int(id), value);

        // Synchronize value
        float *ptr  = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(props) + nOffset);
        if (*ptr == value)
            return;
        *ptr    = value;

        // Do we need to notify clients for changes?
        if (id == pUI->nSelected)
            notify_all();
    }

    void room_builder_ui::CtlFloatPort::init_default(size_t id, obj_props_t *props)
    {
        float value = get_default_value();

        // Synchronize value
        float *ptr  = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(props) + nOffset);
        if (*ptr == value)
            return;
        *ptr    = value;

        // Do we need to notify clients for changes?
        if (id == pUI->nSelected)
            notify_all();
    }

    room_builder_ui::CtlListPort::CtlListPort(room_builder_ui *ui, const port_t *meta):
        CtlPort(&sMetadata)
    {
        pUI         = ui;
        sMetadata   = *meta;
        nCapacity   = 0x10;

        // Generate empty list of strings
        pItems      = reinterpret_cast<char **>(malloc(nCapacity * sizeof(char *)));
        if (pItems != NULL)
        {
            for (size_t i=0; i<nCapacity; ++i)
                pItems[i]   = NULL;
        }
    }

    room_builder_ui::CtlListPort::~CtlListPort()
    {
        if (pItems != NULL)
        {
            free(pItems);
            pItems      = NULL;
        }
    }

    float room_builder_ui::CtlListPort::get_value()
    {
        return pUI->nSelected;
    }

    void room_builder_ui::notify_osc_all()
    {
        for (size_t i=0, n=vOscPorts.size(); i<n; ++i)
        {
            CtlFloatPort *p = vOscPorts.at(i);
            if (p != NULL)
            {
                lsp_trace("Notifying: %s", p->metadata()->id);
                if (!strcmp(p->metadata()->id, "osc:hue"))
                    lsp_trace("debug");
                p->notify_all();
            }
        }
    }

    void room_builder_ui::CtlListPort::set_value(float value)
    {
        size_t count = pUI->vObjectProps.size();
        if (count <= 0)
            return;

        size_t id = (value < count) ? value : count - 1;
        if (pUI->nSelected == id)
            return;

        pUI->nSelected = id;
        pUI->notify_osc_all();
    }

    void room_builder_ui::CtlListPort::sync_size()
    {
        size_t size     = pUI->vObjectProps.size();
        size_t capacity = ((size + 0x10) / 0x10) * 0x10;

        if (capacity > nCapacity)
        {
            char **list = reinterpret_cast<char **>(realloc(pItems, capacity * sizeof(char *)));
            if (list == NULL)
                return;
            for (size_t i=nCapacity; i<capacity; ++i)
                pItems[i]   = NULL;
            pItems      = list;
            nCapacity   = capacity;
        }

        sync_list();
    }

    void room_builder_ui::CtlListPort::sync_list()
    {
        size_t i, size     = pUI->vObjectProps.size();

        for (i=0; i<size; ++i)
        {
            obj_props_t *props = pUI->vObjectProps.get(i);
            if (pItems[i] != NULL)
                free(pItems[i]);
            if ((props != NULL) && (props->sName != NULL))
                pItems[i]   = strdup(props->sName);
            else
                pItems[i]   = strdup("<unnamed>");
        }

        // Update metadata and perform sync
        sMetadata.items     = const_cast<const char **>(pItems);
        sync_metadata();
    }

    room_builder_ui::CtlOscListener::CtlOscListener(room_builder_ui *ui)
    {
        pUI     = ui;
    }

    room_builder_ui::CtlOscListener::~CtlOscListener()
    {
    }

    void room_builder_ui::CtlOscListener::notify(CtlPort *port)
    {
        if (port != pUI->pOscIn)
            return;

        // Get OSC packet
        osc::packet_t *packet = port->get_buffer<osc::packet_t>();
        if ((packet == NULL) || (packet->data == NULL) || (packet->size == 0))
            return;

        // Parse OSC packet
        status_t res;
        osc::parser_t parser;
        osc::parse_frame_t root, message;

        res = osc::parse_begin(&root, &parser, packet->data, packet->size);
        if (res != STATUS_OK)
            return;

        const char *address = NULL;
        res = osc::parse_begin_message(&message, &root, &address);
        if (res == STATUS_OK)
            pUI->process_osc_message(address, &message);

        osc::parse_end(&root);
    }

    room_builder_ui::room_builder_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget),
        sOscListener(this)
    {
        nSelected       = 0;
        pOscIn          = NULL;
        pOscOut         = NULL;
        pListPort       = NULL;
    }
    
    room_builder_ui::~room_builder_ui()
    {
    }

    status_t room_builder_ui::init(IUIWrapper *wrapper, int argc, const char **argv)
    {
        status_t res = plugin_ui::init(wrapper, argc, argv);
        if (res != STATUS_OK)
            return res;

        const port_t *meta = room_builder_base_metadata::osc_ports;
        CtlFloatPort *p;

#define BIND_OSC_PORT(pattern, field)    \
        p = new CtlFloatPort(this, offsetof(obj_props_t, field), pattern, meta++); \
        if (p == NULL) return STATUS_NO_MEM; \
        vOscPorts.add(p); \
        add_port(p); \

        BIND_OSC_PORT("enabled", fEnabled);
        BIND_OSC_PORT("position/x", sPos.x);
        BIND_OSC_PORT("position/y", sPos.y);
        BIND_OSC_PORT("position/z", sPos.z);
        BIND_OSC_PORT("rotation/yaw", fYaw);
        BIND_OSC_PORT("rotation/pitch", fPitch);
        BIND_OSC_PORT("rotation/roll", fRoll);
        BIND_OSC_PORT("scale/x", fSizeX);
        BIND_OSC_PORT("scale/y", fSizeY);
        BIND_OSC_PORT("scale/z", fSizeZ);
        BIND_OSC_PORT("color/hue", fHue);
        BIND_OSC_PORT("material/absorption/outer", fAbsorption[0]);
        BIND_OSC_PORT("material/absorption/inner", fAbsorption[1]);
        BIND_OSC_PORT("material/dispersion/outer", fDispersion[0]);
        BIND_OSC_PORT("material/dispersion/inner", fDispersion[1]);
        BIND_OSC_PORT("material/dissipation/outer", fDissipation[0]);
        BIND_OSC_PORT("material/dissipation/inner", fDissipation[1]);
        BIND_OSC_PORT("material/transparency/outer", fTransparency[1]);
        BIND_OSC_PORT("material/transparency/inner", fTransparency[1]);
        BIND_OSC_PORT("material/sound_speed", fSndSpeed);

        pListPort   = new CtlListPort(this, meta++);
        add_port(pListPort);

        pOscIn      = port(LSP_LV2_OSC_PORT_OUT);
        pOscOut     = port(LSP_LV2_OSC_PORT_IN);

        if (pOscIn != NULL)
            pOscIn->bind(&sOscListener);

        notify_osc_all();

        return STATUS_OK;
    }

    void room_builder_ui::destroy()
    {
        for (size_t i=0, n=vObjectProps.size(); i<n; ++i)
        {
            obj_props_t *p = vObjectProps.get(i);
            if ((p != NULL) && (p->sName != NULL))
                free(p->sName);
        }
        vObjectProps.flush();

        vOscPorts.flush();
        pOscIn          = NULL;
        pOscOut         = NULL;
        pListPort       = NULL;
    }

    void room_builder_ui::process_osc_message(const char *address, osc::parser_frame_t *message)
    {
        status_t res;
        osc::parse_token_t token;

        // It is a number of objects?
        if (!strcmp(address, "/scene/num_objects"))
        {
            int32_t size;
            res = osc::parse_int32(message, &size);
            if (res != STATUS_OK)
                return;
            res = osc::parse_token(message, &token);
            if ((res != STATUS_OK) || (token != osc::PT_EOR))
                return;

            if (size >= 0)
            {
                resize_properties(size);
                notify_osc_all();
            }
            return;
        }

        // Check that it is our message
        if (strstr(address, "/scene/objects/") != address)
            return;
        address    += strlen("/scene/objects/");

        // Parse object identifier
        errno = 0;
        char *endptr = NULL;
        long id = strtol(address, &endptr, 10);
        if ((id < 0) || (errno != 0) || (*endptr != '/'))
            return;
        address = endptr + 1;

        // Name of the object?
        if (!strcmp(address, "name"))
        {
            // Property does exist?
            obj_props_t *props = vObjectProps.get(id);
            if (props == NULL)
                return;

            // Fetch the string value
            const char *name;
            res = osc::parse_string(message, &name);
            if (res != STATUS_OK)
                return;
            res = osc::parse_token(message, &token);
            if ((res != STATUS_OK) || (token != osc::PT_EOR))
                return;

            // Update object name
            if (props->sName != NULL)
                free(props->sName);
            props->sName = strdup(name);

            // Synchronize list
            if (pListPort != NULL)
                pListPort->sync_list();

            return;
        }

        // Call all OSC ports for value processing
        for (size_t i=0, n=vOscPorts.size(); i<n; ++i)
        {
            CtlFloatPort *p = vOscPorts.at(i);
            if (p != NULL)
                p->process_message(message, id, address);
        }
    }

    void room_builder_ui::resize_properties(size_t count)
    {
        if (vObjectProps.size() == count)
            return;

        while (vObjectProps.size() < count)
        {
            size_t id       = vObjectProps.size();
            obj_props_t *p  = vObjectProps.add();
            if (p == NULL)
                return;

            p->sName        = NULL;
            p->sPos.w       = 1.0f;
            for (size_t i=0, n=vOscPorts.size(); i<n; ++i)
            {
                CtlFloatPort *port = vOscPorts.at(i);
                if (port != NULL)
                    port->init_default(id, p);
            }
        }

        while (vObjectProps.size() > count)
        {
            obj_props_t p;
            if (!vObjectProps.pop(&p))
                return;
            if (p.sName != NULL)
                free(p.sName);
        }

        if (pListPort != NULL)
            pListPort->sync_size();
    }

} /* namespace lsp */
