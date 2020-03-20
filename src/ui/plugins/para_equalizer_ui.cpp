/*
 * para_equalizer_ui.cpp
 *
 *  Created on: 10 сент. 2019 г.
 *      Author: sadko
 */

#include <ui/plugins/para_equalizer_ui.h>
#include <core/files/RoomEQWizard.h>

#include <metadata/plugins.h>

namespace lsp
{
    static const char *fmt_strings[] =
    {
        "%s_%d",
        NULL
    };

    static const char *fmt_strings_lr[] =
    {
        "%sl_%d",
        "%sr_%d",
        NULL
    };
    
    static const char *fmt_strings_ms[] =
    {
        "%sm_%d",
        "%ss_%d",
        NULL
    };

    para_equalizer_ui::para_equalizer_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget)
    {
        pRewImport  = NULL;
        pRewPath    = NULL;
        fmtStrings  = fmt_strings;

        if (::strstr(mdata->lv2_uid, "_lr") != NULL)
            fmtStrings      = fmt_strings_lr;
        else if (::strstr(mdata->lv2_uid, "_ms") != NULL)
            fmtStrings      = fmt_strings_ms;
    }
    
    para_equalizer_ui::~para_equalizer_ui()
    {
        pRewImport = NULL;
    }

    status_t para_equalizer_ui::slot_start_import_rew_file(LSPWidget *sender, void *ptr, void *data)
    {
        para_equalizer_ui *_this = static_cast<para_equalizer_ui *>(ptr);

        LSPFileDialog *dlg = _this->pRewImport;
        if (dlg == NULL)
        {
            dlg = new LSPFileDialog(&_this->sDisplay);
            _this->vWidgets.add(dlg);
            _this->pRewImport  = dlg;

            dlg->init();
            dlg->set_mode(FDM_OPEN_FILE);
            dlg->title()->set("titles.import_rew_filter_settings");
            dlg->action_title()->set("actions.import");

            LSPFileFilter *f = dlg->filter();
            {
                LSPFileFilterItem ffi;

                ffi.pattern()->set("*.req|*.txt");
                ffi.title()->set("files.roomeqwizard.all");
                ffi.set_extension("");
                f->add(&ffi);

                ffi.pattern()->set("*.req");
                ffi.title()->set("files.roomeqwizard.req");
                ffi.set_extension("");
                f->add(&ffi);

                ffi.pattern()->set("*.txt");
                ffi.title()->set("files.roomeqwizard.txt");
                ffi.set_extension("");
                f->add(&ffi);

                ffi.pattern()->set("*");
                ffi.title()->set("files.all");
                ffi.set_extension("");
                f->add(&ffi);
            }
            dlg->bind_action(slot_call_import_rew_file, ptr);
            dlg->slots()->bind(LSPSLOT_SHOW, slot_fetch_rew_path, _this);
            dlg->slots()->bind(LSPSLOT_HIDE, slot_commit_rew_path, _this);
        }

        return dlg->show(_this->pRoot);
    }

    status_t para_equalizer_ui::slot_call_import_rew_file(LSPWidget *sender, void *ptr, void *data)
    {
        para_equalizer_ui *_this = static_cast<para_equalizer_ui *>(ptr);
        LSPString path;
        status_t res = _this->pRewImport->get_selected_file(&path);
        if (res == STATUS_OK)
            res = _this->import_rew_file(&path);
        return STATUS_OK;
    }

    status_t para_equalizer_ui::slot_fetch_rew_path(LSPWidget *sender, void *ptr, void *data)
    {
        para_equalizer_ui *_this = static_cast<para_equalizer_ui *>(ptr);
        if ((_this == NULL) || (_this->pRewPath == NULL))
            return STATUS_BAD_STATE;

        LSPFileDialog *dlg = widget_cast<LSPFileDialog>(sender);
        if (dlg == NULL)
            return STATUS_OK;

        dlg->set_path(_this->pRewPath->get_buffer<char>());
        return STATUS_OK;
    }

    status_t para_equalizer_ui::slot_commit_rew_path(LSPWidget *sender, void *ptr, void *data)
    {
        para_equalizer_ui *_this = static_cast<para_equalizer_ui *>(ptr);
        if ((_this == NULL) || (_this->pRewPath == NULL))
            return STATUS_BAD_STATE;

        LSPFileDialog *dlg = widget_cast<LSPFileDialog>(sender);
        if (dlg == NULL)
            return STATUS_OK;

        const char *path = dlg->path();
        if (path != NULL)
        {
            _this->pRewPath->write(path, ::strlen(path));
            _this->pRewPath->notify_all();
        }

        return STATUS_OK;
    }

    status_t para_equalizer_ui::build()
    {
        status_t res = plugin_ui::build();
        if (res != STATUS_OK)
            return res;

        // Find REW port
        pRewPath        =  port(UI_CONFIG_PORT_PREFIX UI_DLG_REW_PATH_ID);

        // Add subwidgets
        LSPMenu *menu       = widget_cast<LSPMenu>(resolve(WUID_IMPORT_MENU));
        if (menu != NULL)
        {
            LSPMenuItem *child = new LSPMenuItem(&sDisplay);
            vWidgets.add(child);
            child->init();
            child->text()->set("actions.import_rew_filter_file");
            child->slots()->bind(LSPSLOT_SUBMIT, slot_start_import_rew_file, this);
            menu->add(child);
        }

        return STATUS_OK;
    }

    void para_equalizer_ui::set_port_value(const char *base, size_t id, float value)
    {
        char port_id[32];

        for (const char **fmt = fmtStrings; *fmt != NULL; ++fmt)
        {
            ::snprintf(port_id, sizeof(port_id)/sizeof(char), *fmt, base, int(id));
            CtlPort *p = port(port_id);
            if (p != NULL)
            {
                p->set_value(value);
                p->notify_all();
            }
        }
    }

    void para_equalizer_ui::set_filter_mode(size_t id, size_t value)
    {
        set_port_value("fm", id, value);
    }

    void para_equalizer_ui::set_filter_type(size_t id, size_t value)
    {
        set_port_value("ft", id, value);
    }

    void para_equalizer_ui::set_filter_frequency(size_t id, double value)
    {
        set_port_value("f", id, value);
    }

    void para_equalizer_ui::set_filter_quality(size_t id, double value)
    {
        set_port_value("q", id, value);
    }

    void para_equalizer_ui::set_filter_gain(size_t id, double value)
    {
        double gain = expf(0.05 * value * M_LN10);
        set_port_value("g", id, gain);
    }

    void para_equalizer_ui::set_filter_slope(size_t id, size_t slope)
    {
        set_port_value("s", id, slope - 1);
    }

    void para_equalizer_ui::set_filter_enabled(size_t id, bool enabled)
    {
        set_port_value("xm", id, (enabled) ? 0.0f : 1.0f);
    }

    void para_equalizer_ui::set_filter_solo(size_t id, bool solo)
    {
        set_port_value("xs", id, (solo) ? 1.0f : 0.0f);
    }

    status_t para_equalizer_ui::import_rew_file(const LSPString *path)
    {
        // Load settings
        room_ew::config_t *cfg = NULL;
        status_t res = room_ew::load(path, &cfg);
        if (res != STATUS_OK)
            return res;

        // Apply settings
        size_t fid = 0;
        for (size_t i=0; i<cfg->nFilters; ++i)
        {
            const room_ew::filter_t *f = &cfg->vFilters[i];

            // Perform parameter translation
            size_t mode     = para_equalizer_base_metadata::EFM_APO_DR;
            ssize_t type    = -1;
            double gain     = 0.0;
            double quality  = M_SQRT1_2;
            double freq     = f->fc;

            switch (f->filterType)
            {
                case room_ew::PK:
                    type    = para_equalizer_base_metadata::EQF_BELL;
                    gain    = f->gain;
                    quality = f->Q;
                    break;
                case room_ew::LS:
                    type    = para_equalizer_base_metadata::EQF_LOSHELF;
                    gain    = f->gain;
                    quality = 2.0/3.0;
                    break;
                case room_ew::HS:
                    type    = para_equalizer_base_metadata::EQF_HISHELF;
                    gain    = f->gain;
                    quality = 2.0/3.0;
                    break;
                case room_ew::LP:
                    type    = para_equalizer_base_metadata::EQF_LOPASS;
                    break;
                case room_ew::HP:
                    type    = para_equalizer_base_metadata::EQF_HIPASS;
                    break;
                case room_ew::LPQ:
                    type    = para_equalizer_base_metadata::EQF_LOPASS;
                    quality = f->Q;
                    break;
                case room_ew::HPQ:
                    type    = para_equalizer_base_metadata::EQF_HIPASS;
                    quality = f->Q;
                    break;
                case room_ew::LS6:
                    type    = para_equalizer_base_metadata::EQF_LOSHELF;
                    gain    = f->gain;
                    quality = M_SQRT2 / 3.0;
                    freq    = freq * 2.0 / 3.0;
                    break;
                case room_ew::HS6:
                    type    = para_equalizer_base_metadata::EQF_HISHELF;
                    gain    = f->gain;
                    quality = M_SQRT2 / 3.0;
                    freq    = freq / M_SQRT1_2;
                    break;
                case room_ew::LS12:
                    type    = para_equalizer_base_metadata::EQF_LOSHELF;
                    gain    = f->gain;
                    freq    = freq * 3.0 / 2.0;
                    break;
                case room_ew::HS12:
                    type    = para_equalizer_base_metadata::EQF_HISHELF;
                    gain    = f->gain;
                    freq    = freq * M_SQRT1_2;
                    break;
                case room_ew::NO:
                    type    = para_equalizer_base_metadata::EQF_NOTCH;
                    quality = 100.0 / 3.0;
                    break;
                case room_ew::AP:
                    type    = para_equalizer_base_metadata::EQF_ALLPASS;
                    quality = 0.0;
                    break;
                default: // Skip other filter types
                    break;
            }

            if (type < 0)
                continue;

            // Set-up parameters
            set_filter_mode(fid, mode);
            set_filter_type(fid, type);
            set_filter_slope(fid, 1);
            set_filter_frequency(fid, freq);
            set_filter_gain(fid, gain);
            set_filter_quality(fid, quality);
            set_filter_enabled(fid, f->enabled);
            set_filter_solo(fid, false);

            // Increment imported filter number
            ++fid;
        }

        // Reset state of all other filters
        for (; fid < 32; ++fid)
        {
            set_filter_type(fid, para_equalizer_base_metadata::EQF_OFF);
            set_filter_slope(fid, 1);
            set_filter_gain(fid, 1.0f);
            set_filter_quality(fid, 0.0f);
            set_filter_enabled(fid, true);
            set_filter_solo(fid, false);
        }

        return STATUS_OK;
    }

} /* namespace lsp */
