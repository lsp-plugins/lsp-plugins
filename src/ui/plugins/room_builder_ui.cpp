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

        char name[0x100];
        ::sprintf(name, "/scene/object/*/%s", sPattern);
        osc::pattern_create(&sOscPattern, name);
    }

    room_builder_ui::CtlFloatPort::~CtlFloatPort()
    {
        pUI         = NULL;
        sPattern    = NULL;
        osc::pattern_destroy(&sOscPattern);
    }

    float room_builder_ui::CtlFloatPort::get_value()
    {
        // Prepare the value
        char name[0x100];
        float value;
        ::sprintf(name, "/scene/object/%d/%s", int(pUI->nSelected), sPattern);

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

    bool room_builder_ui::CtlFloatPort::changed(KVTStorage *storage, const char *id, const kvt_param_t *value)
    {
        if (osc::pattern_match(&sOscPattern, id) != STATUS_OK)
            return false;
        return true;
    }

    static const char *UNNAMED_STR = "<unnamed>";

    room_builder_ui::CtlListPort::CtlListPort(room_builder_ui *ui, const port_t *meta):
        CtlPort(&sMetadata)
    {
        pUI         = ui;
        sMetadata   = *meta;
        nItems      = 0;
        nCapacity   = 0x10;

        // Generate empty list of strings
        pItems      = reinterpret_cast<char **>(::malloc(nCapacity * sizeof(char *)));
        if (pItems != NULL)
        {
            for (size_t i=0; i<nCapacity; ++i)
                pItems[i]   = NULL;
        }
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

    float room_builder_ui::CtlListPort::get_value()
    {
        return pUI->nSelected;
    }

    void room_builder_ui::CtlListPort::set_value(float value)
    {
        ssize_t index   = limit_value(pMetadata, value);
        if (index != pUI->nSelected)
        {
            pUI->nSelected  = index;

            // Notify all KVT ports
            for (size_t i=0, n=vKvtPorts.size(); i<n; ++i)
            {
                CtlPort *p = vKvtPorts.get(i);
                if (p != NULL)
                    p->notify_all();
            }
        }
    }

    void room_builder_ui::CtlListPort::add_port(CtlPort *port)
    {
        vKvtPorts.add(port);
    }

    void room_builder_ui::CtlListPort::set_list_item(size_t id, const char *value)
    {
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

    bool room_builder_ui::CtlListPort::changed(KVTStorage *storage, const char *id, const kvt_param_t *value)
    {
        if ((value->type == KVT_INT32) && (!strcmp(id, "/scene/objects")))
        {
            // Ensure that we have enough place to store object names
            size_t size     = (value->i32 < 0) ? 0 : value->i32;
            size_t capacity = ((size + 0x10) / 0x10) * 0x10;

            if (capacity > nCapacity)
            {
                char **list = reinterpret_cast<char **>(::realloc(pItems, capacity * sizeof(char *)));
                if (list == NULL)
                    return false;
                for (size_t i=nCapacity; i<capacity; ++i)
                    pItems[i]   = NULL;
                pItems      = list;
                nCapacity   = capacity;
                sMetadata.items     = const_cast<const char **>(pItems); // Update pointer to the list
            }

            // Allocate non-allocated strings
            char pname[0x100]; // Should be enough

            while (nItems < size)
            {
                sprintf(pname, "/scene/object/%d/name", int(nItems));
                const char *pval = NULL;
                status_t res = storage->get(pname, &pval);
                set_list_item(nItems++, (res == STATUS_OK) ? pval : NULL);
            }

            // Set the end of string list
            if ((pItems[nItems] != NULL) && (pItems[nItems] != UNNAMED_STR))
                ::free(pItems[nItems]);
            pItems[nItems] = NULL;

            // Notify all listeners
            set_value(pUI->nSelected);  // Update the current selected value
            sync_metadata();            // Call for metadata update
            notify_all();               // Notify all listeners on the port
        }
        else if ((value->type == KVT_STRING) && (strstr(id, "/scene/object/") == id))
        {
            id += strlen("/scene/object/");
            const char *end = ::strchr(id, '/');
            if (!strcmp(end, "/name"))
            {
                // Valid object number?
                char *endptr;
                errno = 0;
                long index = ::strtol(id, &endptr, 10);
                if ((errno == 0) && (*endptr == '/') && (index >= 0) && (index < ssize_t(nItems)))
                {
                    set_list_item(index, value->str);   // Update list element
                    sync_metadata();                    // Synchronize metadata
                }
            }
        }

        return false;
    }

    room_builder_ui::room_builder_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget)
    {
        nSelected       = 0;
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
        add_port(kvt_list);

        CtlFloatPort *p;

#define BIND_OSC_PORT(pattern, field)    \
        p = new CtlFloatPort(this, pattern, meta++); \
        if (p == NULL) \
            return STATUS_NO_MEM; \
        kvt_list->add_port(p); \
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

        return STATUS_OK;
    }

} /* namespace lsp */
