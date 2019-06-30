/*
 * room_builder_ui.cpp
 *
 *  Created on: 28 мая 2019 г.
 *      Author: sadko
 */

#include <ui/plugins/room_builder_ui.h>
#include <plugins/room_builder.h>
#include <metadata/plugins.h>
#include <metadata/ports.h>

namespace lsp
{
    room_builder_ui::CtlFloatPort::CtlFloatPort(room_builder_ui *ui, const char *pattern, const port_t *meta):
        CtlPort(meta)
    {
        pUI         = ui;
        sPattern    = pattern;

        char name[0x100];
        ::sprintf(name, "/scene/object/*/%s", sPattern);
        osc::pattern_create(&sOscPattern, name);
        fValue      = get_default_value();
    }

    room_builder_ui::CtlFloatPort::~CtlFloatPort()
    {
        pUI         = NULL;
        sPattern    = NULL;
        osc::pattern_destroy(&sOscPattern);
    }

    const char *room_builder_ui::CtlFloatPort::name()
    {
        const char *format = NULL;
        return (osc::pattern_get_format(&sOscPattern, &format) == STATUS_OK) ? format : NULL;
    }

    float room_builder_ui::CtlFloatPort::get_value()
    {
        // Prepare the value
        char name[0x100];
        float value = 0.0f;
        ::sprintf(name, "/scene/object/%d/%s", int(pUI->nSelected), sPattern);

        // Fetch value
        KVTStorage *kvt = pUI->kvt_lock();
        status_t res = STATUS_NOT_FOUND;
        if (kvt != NULL)
        {
            res = kvt->get(name, &value);
            pUI->kvt_release();
        }

        // Return the limited value
        return fValue = (res == STATUS_OK) ?
                limit_value(pMetadata, value) :
                get_default_value();
    }

    void room_builder_ui::CtlFloatPort::set_value(float value)
    {
        // Do not update value
        if (fValue == value)
            return;

        // Prepare the value
        char name[0x100];
        sprintf(name, "/scene/object/%d/%s", int(pUI->nSelected), sPattern);
        value       = limit_value(pMetadata, value);

        // Obtain KVT storage
        KVTStorage *kvt = pUI->kvt_lock();
        if (kvt != NULL)
        {
            kvt_param_t param;
            param.type  = KVT_FLOAT32;
            param.f32   = value;

            // Write in silent mode
            if (kvt->put(name, &param, KVT_TO_DSP) == STATUS_OK)
            {
                fValue = value;
                pUI->kvt_write(kvt, name, &param);
            }
            pUI->kvt_release();
        }
    }

    bool room_builder_ui::CtlFloatPort::match(const char *id)
    {
        return (osc::pattern_match(&sOscPattern, id) == STATUS_OK);
    }

    bool room_builder_ui::CtlFloatPort::changed(KVTStorage *storage, const char *id, const kvt_param_t *value)
    {
        char name[0x100];
        ::sprintf(name, "/scene/object/%d/%s", int(pUI->nSelected), sPattern);
        if (::strcmp(name, id) != 0)
            return false;

        notify_all();
        return true;
    }

    static const char *UNNAMED_STR = "<unnamed>";

    room_builder_ui::CtlListPort::CtlListPort(room_builder_ui *ui, const port_t *meta):
        CtlPort(&sMetadata)
    {
        pUI         = ui;
        sMetadata   = *meta;
        nItems      = 0;
        nCapacity   = 0;
        pItems      = NULL;
        nSelectedReq= -1;

        osc::pattern_create(&sOscPattern, "/scene/object/*/name");
    }

    room_builder_ui::CtlListPort::~CtlListPort()
    {
        vKvtPorts.flush();

        if (pItems != NULL)
        {
            for (size_t i=0; i<nCapacity; ++i)
            {
                if ((pItems[i] != NULL) && (pItems[i] != UNNAMED_STR))
                    ::free(pItems[i]);
                pItems[i] = NULL;
            }

            ::free(pItems);
            pItems      = NULL;
        }
    }

    const char *room_builder_ui::CtlListPort::name()
    {
        return "/scene/objects";
    }

    float room_builder_ui::CtlListPort::get_value()
    {
        ssize_t index = pUI->nSelected;
        if (nItems > 0)
        {
            if (index >= ssize_t(nItems))
                index   = nItems-1;
            else if (index < 0)
                index   = 0;
        }
        else
            index = -1;

        return index;
    }

    void room_builder_ui::CtlListPort::set_value(float value)
    {
        ssize_t index   = value;
        if (index == pUI->nSelected)
            return;

        pUI->nSelected  = index;

        // Deploy new value to KVT
        KVTStorage *kvt = pUI->kvt_lock();
        if (kvt != NULL)
        {
            kvt->put("/scene/selected", float(index), KVT_RX);
            pUI->kvt_release();
        }

        // Notify all KVT ports
        for (size_t i=0, n=vKvtPorts.size(); i<n; ++i)
        {
            CtlPort *p = vKvtPorts.get(i);
            if (p != NULL)
                p->notify_all();
        }
    }

    void room_builder_ui::CtlListPort::add_port(CtlPort *port)
    {
        vKvtPorts.add(port);
    }

    void room_builder_ui::CtlListPort::set_list_item(size_t id, const char *value)
    {
        if (pItems == NULL)
            return;

        // Free previous value holder
        if ((pItems[id] != NULL) && (pItems[id] != UNNAMED_STR))
            ::free(pItems[id]);

        // Try to copy name of parameter
        if (value != NULL)
            pItems[id]  = ::strdup(value);
        else if (::asprintf(&pItems[id], "<unnamed #%d>", int(id)) < 0)
            pItems[id]  = NULL;

        // If all is bad, do this
        if (pItems[id] == NULL)
            pItems[id]  = const_cast<char *>(UNNAMED_STR);
    }

    bool room_builder_ui::CtlListPort::match(const char *id)
    {
        if (!strcmp(id, "/scene/objects"))
            return true;
        if (!strcmp(id, "/scene/selected"))
            return true;
        return osc::pattern_match(&sOscPattern, id);
    }

    bool room_builder_ui::CtlListPort::changed(KVTStorage *storage, const char *id, const kvt_param_t *value)
    {
        if ((value->type == KVT_INT32) && (!strcmp(id, "/scene/objects")))
        {
            // Ensure that we have enough place to store object names
            size_t size     = (value->i32 < 0) ? 0 : value->i32;
            if (nItems == size)
                return false;

            // Compute the capacity and adjust array size
            size_t capacity = ((size + 0x10) / 0x10) * 0x10;
            if (capacity > nCapacity)
            {
                char **list = reinterpret_cast<char **>(::realloc(pItems, capacity * sizeof(char *)));
                if (list == NULL)
                    return false;
                for (size_t i=nCapacity; i<capacity; ++i)
                    list[i]     = NULL;

                pItems          = list;
                nCapacity       = capacity;
                sMetadata.items = const_cast<const char **>(list);
            }

            // Allocate non-allocated strings
            char pname[0x100]; // Should be enough
            for (size_t i=nItems; i < size; ++i)
            {
                ::sprintf(pname, "/scene/object/%d/name", int(i));
                const char *pval = NULL;
                status_t res = storage->get(pname, &pval);
                set_list_item(i, (res == STATUS_OK) ? pval : NULL);
            }
            nItems  = size; // Update size

            // Set the end of string list
            if ((pItems[nItems] != NULL) && (pItems[nItems] != UNNAMED_STR))
                ::free(pItems[nItems]);
            pItems[nItems] = NULL;

            // Cleanup storage
            room_builder_base::kvt_cleanup_objects(storage, nItems);

            // Change selected value
            ssize_t index = pUI->nSelected;
            if (storage->get(id, &value) == STATUS_OK)
            {
                if (value->type == KVT_FLOAT32)
                    index   = value->f32;
            }

            if (index < 0)
                index = 0;
            else if (index >= ssize_t(nItems))
                index = nItems-1;
            set_value(index);           // Update the current selected value

            sync_metadata();            // Call for metadata update
            notify_all();               // Notify all bound listeners
            return true;
        }
        else if ((value->type == KVT_FLOAT32) && (!strcmp(id, "/scene/selected")))
        {
            set_value(value->f32);
        }
        else if ((value->type == KVT_STRING) && (::strstr(id, "/scene/object/") == id))
        {
            id += ::strlen("/scene/object/");

            char *endptr = NULL;
            errno = 0;
            long index = ::strtol(id, &endptr, 10);

            // Valid object number?
            if ((errno == 0) && (!::strcmp(endptr, "/name")) &&
                (index >= 0) && (index < ssize_t(nItems)))
            {
                set_list_item(index, value->str);   // Update list element
                sync_metadata();                    // Synchronize metadata
                return true;
            }
        }

        return false;
    }

    room_builder_ui::room_builder_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget)
    {
        nSelected       = -1;
    }
    
    room_builder_ui::~room_builder_ui()
    {
    }

    status_t room_builder_ui::init(IUIWrapper *wrapper, int argc, const char **argv)
    {
        status_t res = plugin_ui::init(wrapper, argc, argv);
        if (res != STATUS_OK)
            return res;

        const port_t *meta = room_builder_base_metadata::kvt_ports;

        // Create object identifier port
        CtlListPort *kvt_list = new CtlListPort(this, meta++);
        if (kvt_list == NULL)
            return STATUS_NO_MEM;
        add_custom_port(kvt_list);
        add_kvt_listener(kvt_list);

        CtlFloatPort *p;

#define BIND_KVT_PORT(pattern, field)    \
        p = new CtlFloatPort(this, pattern, meta++); \
        if (p == NULL) \
            return STATUS_NO_MEM; \
        kvt_list->add_port(p); \
        add_custom_port(p); \
        add_kvt_listener(p);

        BIND_KVT_PORT("enabled", fEnabled);
        BIND_KVT_PORT("position/x", sPos.x);
        BIND_KVT_PORT("position/y", sPos.y);
        BIND_KVT_PORT("position/z", sPos.z);
        BIND_KVT_PORT("rotation/yaw", fYaw);
        BIND_KVT_PORT("rotation/pitch", fPitch);
        BIND_KVT_PORT("rotation/roll", fRoll);
        BIND_KVT_PORT("scale/x", fSizeX);
        BIND_KVT_PORT("scale/y", fSizeY);
        BIND_KVT_PORT("scale/z", fSizeZ);
        BIND_KVT_PORT("color/hue", fHue);
        BIND_KVT_PORT("material/absorption/outer", fAbsorption[0]);
        BIND_KVT_PORT("material/absorption/inner", fAbsorption[1]);
        BIND_KVT_PORT("material/absorption/link", lnkAbsorption);
        BIND_KVT_PORT("material/dispersion/outer", fDispersion[0]);
        BIND_KVT_PORT("material/dispersion/inner", fDispersion[1]);
        BIND_KVT_PORT("material/dispersion/link", lnkDispersion);
        BIND_KVT_PORT("material/diffusion/outer", fDiffusion[0]);
        BIND_KVT_PORT("material/diffusion/inner", fDiffusion[1]);
        BIND_KVT_PORT("material/diffusion/link", lnkDiffusion);
        BIND_KVT_PORT("material/transparency/outer", fTransparency[1]);
        BIND_KVT_PORT("material/transparency/inner", fTransparency[1]);
        BIND_KVT_PORT("material/transparency/link", lnkTransparency);
        BIND_KVT_PORT("material/sound_speed", fSndSpeed);

        return STATUS_OK;
    }

} /* namespace lsp */
