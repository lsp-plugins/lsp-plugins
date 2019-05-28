/*
 * room_builder_ui.cpp
 *
 *  Created on: 28 мая 2019 г.
 *      Author: sadko
 */

#include <ui/plugins/room_builder_ui.h>

namespace lsp
{
    room_builder_ui::CtlFloatPort::CtlFloatPort(room_builder_ui *ui, size_t offset, const char *pattern, const port_t *meta):
        CtlPort(meta)
    {
        pUI         = ui;
        sPattern    = pattern;
        fOldValue   = meta->start - 1.0f;
        nOffset     = offset;
    }

    room_builder_ui::CtlFloatPort::~CtlFloatPort()
    {
        pUI         = NULL;
        sPattern    = NULL;
        fOldValue   = 0.0f;
        nOffset     = 0;
    }

    float room_builder_ui::CtlFloatPort::get_value()
    {
        // The record is accessible?
        if (pUI->nSelected >= pUI->vObjectProps.size())
            return get_default_value();

        obj_props_t *props = pUI->vObjectProps.get(pUI->nSelected);
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
        fOldValue   = value;

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
            char address[0x100];
            sprintf(address, sPattern, int(selected));

            if (osc::forge_begin_message(&message, &sframe, address) == STATUS_OK)
            {
                osc::forge_float32(&sframe, value);
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

    void room_builder_ui::CtlFloatPort::osc_sync()
    {
        // The record is accessible?
        if (pUI->nSelected >= pUI->vObjectProps.size())
            return;

        obj_props_t *props = pUI->vObjectProps.get(pUI->nSelected);
        if (props == NULL)
            return;

        // Do we need to notify clients for changes?
        float *ptr  = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(props) + nOffset);
        if (*ptr != fOldValue)
        {
            fOldValue       = *ptr;
            notify_all();
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

    void room_builder_ui::CtlListPort::sync_size()
    {
        size_t size     = pUI->vObjectProps.size();
        size_t capacity = ((size + 0x10) / 0x10) * 0x10 + 1;

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
        }

        // Update metadata and perform sync
        sMetadata.items     = const_cast<const char **>(pItems);
        sync_metadata();
    }

    room_builder_ui::room_builder_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget)
    {
        nSelected       = 0;
        pOscOut         = NULL;
    }
    
    room_builder_ui::~room_builder_ui()
    {
    }

    status_t room_builder_ui::init(IUIWrapper *wrapper, int argc, const char **argv)
    {
        status_t res = plugin_ui::init(wrapper, argc, argv);
        if (res != STATUS_OK)
            return res;

        // TODO
        return res;
    }

} /* namespace lsp */
