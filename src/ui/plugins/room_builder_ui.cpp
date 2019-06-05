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
    room_builder_ui::CtlFloatPort::CtlFloatPort(room_builder_ui *ui, const char *pattern, const port_t *meta):
        CtlPort(meta)
    {
        pUI         = ui;
        sPattern    = pattern;
    }

    room_builder_ui::CtlFloatPort::~CtlFloatPort()
    {
        pUI         = NULL;
        sPattern    = NULL;
    }

    float room_builder_ui::CtlFloatPort::get_value()
    {
        // Prepare the value
        char name[0x100];
        float value;
        sprintf(name, "/scene/object/%d/%s", int(pUI->nSelected), sPattern);

        // Obtain KVT storage
        IUIWrapper *wrapper = pUI->wrapper();
        if (wrapper == NULL)
            return get_default_value();

        // Fetch value
        KVTStorage *kvt = wrapper->kvt_lock();
        status_t res = STATUS_NOT_FOUND;
        if (kvt != NULL)
        {
            res = kvt->get(name, &value);
            wrapper->kvt_release();
        }

        // Return the limited value
        return (res == STATUS_OK) ?
                limit_value(pMetadata, res) :
                get_default_value();
    }

    void room_builder_ui::CtlFloatPort::set_value(float value)
    {
        // Prepare the value
        char name[0x100];
        sprintf(name, "/scene/object/%d/%s", int(pUI->nSelected), sPattern);
        value       = limit_value(pMetadata, value);

        // Obtain KVT storage
        IUIWrapper *wrapper = pUI->wrapper();
        if (wrapper == NULL)
            return;

        KVTStorage *kvt = wrapper->kvt_lock();
        if (kvt != NULL)
        {
            kvt->put(name, value, KVT_COMMIT);  // Write in silent mode
            wrapper->kvt_release();
        }
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

    void room_builder_ui::CtlListPort::set_value(float value)
    {
        pUI->nSelected  = limit_value(pMetadata, value);
        pUI->notify_kvt_ports();
    }

    void room_builder_ui::CtlListPort::changed(const char *id, const kvt_param_t *oval, const kvt_param_t *nval)
    {
        if ((nval->type == KVT_UINT32) && (!strcmp(id, "/scene/objects")))
        {
            sMetadata.items     = nval->i32;
            sync_metadata();
        }
        else if ((nval->type == KVT_STRING) && (strstr(id, "/scene/object/") == id))
        {
            id += strlen("/scene/object/");
            const char *end = strchr(id, '/');
            if (!strcmp(end, "/name"))
            {
                // Vvalid object number?
                char *endptr;
                errno = 0;
                long index = strtol(id, &endptr, 10);
                if ((errno == 0) && (*endptr == '/'))
                {

                }
            }
        }


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
        pListPort       = NULL;
    }
    
    room_builder_ui::~room_builder_ui()
    {
    }

    void room_builder_ui::notify_kvt_ports()
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
