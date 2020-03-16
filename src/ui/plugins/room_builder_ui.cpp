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
                char *s = const_cast<char *>(pItems[i].text);
                if ((s != NULL) && (s != UNNAMED_STR))
                    ::free(s);
                pItems[i].text = NULL;
            }

            ::free(pItems);
            pItems      = NULL;
        }

        osc::pattern_destroy(&sOscPattern);
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
            kvt_param_t p;
            p.type      = KVT_FLOAT32;
            p.f32       = index;
            kvt->put("/scene/selected", &p, KVT_RX);
            pUI->kvt_write(kvt, "/scene/selected", &p);
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
        char **v = const_cast<char **>(&pItems[id].text);

        // Free previous value holder
        if ((*v != NULL) && (*v != UNNAMED_STR))
            ::free(*v);

        // Try to copy name of parameter
        if (value != NULL)
            *v = ::strdup(value);
        else if (::asprintf(v, "<unnamed #%d>", int(id)) < 0)
            *v  = NULL;

        // If all is bad, do this
        if (*v == NULL)
            *v  = const_cast<char *>(UNNAMED_STR);
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
                port_item_t *list = reinterpret_cast<port_item_t *>(::realloc(pItems, capacity * sizeof(port_item_t)));
                if (list == NULL)
                    return false;
                for (size_t i=nCapacity; i<capacity; ++i)
                {
                    list[i].text    = NULL;
                    list[i].lc_key  = NULL;
                }

                pItems          = list;
                nCapacity       = capacity;
                sMetadata.items = list;
            }

            // Allocate non-allocated strings
            char pname[0x100]; // Should be enough
            for (size_t i=nItems; i < size; ++i)
            {
                ::snprintf(pname, sizeof(pname), "/scene/object/%d/name", int(i));
                const char *pval = NULL;
                status_t res = storage->get(pname, &pval);
                set_list_item(i, (res == STATUS_OK) ? pval : NULL);
            }
            nItems  = size; // Update size

            // Set the end of string list
            char **s = const_cast<char **>(&pItems[nItems].text);
            if ((*s != NULL) && (*s != UNNAMED_STR))
                ::free(*s);
            *s = NULL;

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

    room_builder_ui::CtlMaterialPreset::CtlMaterialPreset(room_builder_ui *ui)
    {
        pUI         = ui;
        pCBox       = NULL;
        hHandler    = -1;
        pSpeed      = NULL;
        pAbsorption = NULL;
        pSelected   = NULL;
    }

    room_builder_ui::CtlMaterialPreset::~CtlMaterialPreset()
    {
        pSpeed      = NULL;
        pAbsorption = NULL;
        pSelected   = NULL;
    }

    void room_builder_ui::CtlMaterialPreset::init(const char *preset, const char *selected, const char *speed, const char *absorption)
    {
        // Just bind ports
        pSpeed      = pUI->port(speed);
        pAbsorption = pUI->port(absorption);
        pSelected   = pUI->port(selected);

        // Fetch widget
        pCBox = widget_cast<LSPComboBox>(pUI->resolve("mpreset"));

        // Initialize list of presets
        LSPItem li;
        LSPString lc;
        if (pCBox != NULL)
        {
            // Initialize box
            li.text()->set("lists.room_bld.select_mat");
            li.set_value(-1.0f);
            pCBox->items()->add(&li);
            size_t i=0;
            for (const room_material_t *m = room_builder_base_metadata::materials; m->name != NULL; ++m)
            {
                if (m->lc_key != NULL)
                {
                    lc.set_ascii("lists.");
                    lc.append_ascii(m->lc_key);
                    li.text()->set(&lc);
                }
                else
                    li.text()->set_raw(m->name);
                li.set_value(i++);
                pCBox->items()->add(&li);
            }
            pCBox->set_selected(0);

            // Bind listener
            hHandler    = pCBox->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
        }

        // Bind handlers and notify changes
        if (pSpeed != NULL)
        {
            pSpeed->bind(this);
            pSpeed->notify_all();
        }
        if (pAbsorption != NULL)
        {
            pAbsorption->bind(this);
            pAbsorption->notify_all();
        }
        if (pSelected != NULL)
        {
            pSelected->bind(this);
            pSelected->notify_all();
        }
    }

    status_t room_builder_ui::CtlMaterialPreset::slot_change(LSPWidget *sender, void *ptr, void *data)
    {
        CtlMaterialPreset *_this = reinterpret_cast<CtlMaterialPreset *>(ptr);
        if (ptr == NULL)
            return STATUS_BAD_STATE;

        ssize_t sel = _this->pSelected->get_value();
        if (sel < 0)
            return STATUS_OK;

        ssize_t idx = (_this->pCBox != NULL) ? _this->pCBox->selected() - 1 : -1;
        if (idx < 0)
            return STATUS_OK;

        const room_material_t *m = &room_builder_base_metadata::materials[idx];

        if (_this->pAbsorption->get_value() != m->absorption)
        {
            _this->pAbsorption->set_value(m->absorption);
            _this->pAbsorption->notify_all();
        }

        if (_this->pSpeed->get_value() != m->speed)
        {
            _this->pSpeed->set_value(m->speed);
            _this->pSpeed->notify_all();
        }

        return STATUS_OK;
    }

    void room_builder_ui::CtlMaterialPreset::notify(CtlPort *port)
    {
        if (pCBox == NULL)
            return;

        float fAbsorption   = pAbsorption->get_value();
        float fSpeed        = pSpeed->get_value();

        // Find best match
        ssize_t idx = 0, i = 1;
        for (const room_material_t *m = room_builder_base_metadata::materials; m->name != NULL; ++m, ++i)
        {
            if ((m->speed == fSpeed) && (m->absorption == fAbsorption))
            {
                idx = i;
                break;
            }
        }

        // Set-up selected index in non-notify mode
        if (pCBox->selected() != idx)
        {
            pCBox->slots()->disable(LSPSLOT_CHANGE, hHandler);
            pCBox->set_selected(idx);
            pCBox->slots()->enable(LSPSLOT_CHANGE, hHandler);
        }
    }

    room_builder_ui::CtlKnobBinding::CtlKnobBinding(room_builder_ui *ui, bool reverse)
    {
        pUI         = ui;
        pOuter      = NULL;
        pInner      = NULL;
        pLink       = NULL;
        bReverse    = reverse;
    }

    room_builder_ui::CtlKnobBinding::~CtlKnobBinding()
    {
        pUI         = NULL;
        pOuter      = NULL;
        pInner      = NULL;
        pLink       = NULL;
        bReverse    = false;
    }

    void room_builder_ui::CtlKnobBinding::init(const char *outer, const char *inner, const char *link)
    {
        // Just bind ports
        pOuter      = pUI->port(outer);
        pInner      = pUI->port(inner);
        pLink       = pUI->port(link);

        // Bind handlers and notify changes
        if (pLink != NULL)
        {
            pLink->bind(this);
            pLink->notify_all();
        }
        if (pInner != NULL)
        {
            pInner->bind(this);
            pInner->notify_all();
        }
        if (pOuter != NULL)
        {
            pOuter->bind(this);
            pOuter->notify_all();
        }
    }

    void room_builder_ui::CtlKnobBinding::notify(CtlPort *port)
    {
        if (port == NULL)
            return;

        bool link = (pLink != NULL) ? pLink->get_value() >= 0.5f : false;
        if (!link)
            return;

        if (port == pLink)
            port = pOuter;

        if ((port == pInner) && (pInner != NULL))
        {
            const port_t *meta = pInner->metadata();
            float v = pInner->get_value();
            if (bReverse)
                v = meta->max - v;

            if (pOuter->get_value() != v)
            {
                pOuter->set_value(v);
                pOuter->notify_all();
            }
        }
        else if ((port == pOuter) && (pOuter != NULL))
        {
            const port_t *meta = pOuter->metadata();
            float v = pOuter->get_value();
            if (bReverse)
                v = meta->max - v;

            if (pInner->get_value() != v)
            {
                pInner->set_value(v);
                pInner->notify_all();
            }
        }
    }

    //-------------------------------------------------------------------------
    // Main class methods

    room_builder_ui::room_builder_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget),
        sPresets(this),
        sAbsorption(this, false),
        sTransparency(this, true),
        sDispersion(this, false),
        sDiffuse(this, false)
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

        sAbsorption.init("kvt:oabs", "kvt:iabs", "kvt:labs");
        sTransparency.init("kvt:otransp", "kvt:itransp", "kvt:ltransp");
        sDispersion.init("kvt:odisp", "kvt:idisp", "kvt:ldisp");
        sDiffuse.init("kvt:odiff", "kvt:idiff", "kvt:ldiff");

        return STATUS_OK;
    }

    status_t room_builder_ui::build()
    {
        status_t res = plugin_ui::build();
        if (res == STATUS_OK)
            sPresets.init("mpreset", "kvt:oid", "kvt:speed", "kvt:oabs");
        return res;
    }

} /* namespace lsp */
