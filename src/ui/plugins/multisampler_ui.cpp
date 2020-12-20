/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <ui/plugins/multisampler_ui.h>
#include <core/files/Hydrogen.h>
#include <core/system.h>
#include <core/io/Dir.h>

#include <metadata/plugins.h>

namespace lsp
{
    static const char * h2_system_paths[] =
    {
        "/usr/share/hydrogen",
        "/usr/local/share/hydrogen",
        "/opt/hydrogen",
        "/share/hydrogen",
        NULL
    };

    static const char * h2_user_paths[] =
    {
        ".hydrogen",
        ".h2",
        ".config/hydrogen",
        ".config/h2",
        NULL
    };

    multisampler_ui::multisampler_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget)
    {
        pHydrogenImport = NULL;
        pHydrogenPath   = NULL;
    }

    multisampler_ui::~multisampler_ui()
    {
        pHydrogenImport = NULL;      // Will be automatically destroyed from list of widgets

        // Destroy all information about drumkits
        for (size_t i=0, n=vDrumkits.size(); i<n; ++i)
        {
            h2drumkit_t *dk = vDrumkits.at(i);
            if (dk == NULL)
                continue;
            dk->pMenu       = NULL;
            delete dk;
        }
        vDrumkits.flush();
    }

    status_t multisampler_ui::build()
    {
        status_t res = plugin_ui::build();
        if (res != STATUS_OK)
            return res;

        lookup_hydrogen_files();
        sort_hydrogen_files();

        // Find hydrogen port
        pHydrogenPath   =  port(UI_CONFIG_PORT_PREFIX UI_DLG_HYDROGEN_PATH_ID);

        // Add subwidgets
        LSPMenu *menu       = widget_cast<LSPMenu>(resolve(WUID_IMPORT_MENU));
        if (menu != NULL)
        {
            LSPMenuItem *child = new LSPMenuItem(&sDisplay);
            vWidgets.add(child);
            child->init();
            child->text()->set("actions.import_hydrogen_drumkit_file");
            child->slots()->bind(LSPSLOT_SUBMIT, slot_start_import_hydrogen_file, this);
            menu->add(child);

            if (vDrumkits.size() > 0)
            {
                // Create menu item
                child           = new LSPMenuItem(&sDisplay);
                vWidgets.add(child);
                child->init();
                child->text()->set("actions.import_installed_hydrogen_drumkit");
                menu->add(child);

                // Create submenu
                menu            = new LSPMenu(&sDisplay);
                vWidgets.add(menu);
                menu->init();
                child->set_submenu(menu);

                // Add hydrogen files to menu
                add_hydrogen_files_to_menu(menu);
            }
        }

        return STATUS_OK;
    }

    void multisampler_ui::sort_hydrogen_files()
    {
        if (vDrumkits.size() < 2)
            return;

        // Simple selection sort
        for (size_t i=0, n=vDrumkits.size()-1; i<n; ++i)
            for (size_t j=i+1; j<=n; ++j)
            {
                h2drumkit_t *a = vDrumkits.at(i);
                h2drumkit_t *b = vDrumkits.at(j);
                if (a->sName.compare_to_nocase(&b->sName) > 0)
                    vDrumkits.swap(i, j);
            }
    }

    status_t multisampler_ui::add_drumkit(const io::Path *path, const hydrogen::drumkit_t *dk, bool system)
    {
        h2drumkit_t *drumkit = new h2drumkit_t();
        if (drumkit == NULL)
            return STATUS_NO_MEM;

        if (!drumkit->sName.set(&dk->name))
        {
            delete drumkit;
            return STATUS_NO_MEM;
        }

        if (drumkit->sPath.set(path) != STATUS_OK)
        {
            delete drumkit;
            return STATUS_NO_MEM;
        }

        drumkit->bSystem        = system;
        drumkit->pMenu          = NULL;

        if (!vDrumkits.add(drumkit))
        {
            delete drumkit;
            return STATUS_NO_MEM;
        }

        return STATUS_OK;
    }

    status_t multisampler_ui::scan_hydrogen_directory(const io::Path *path, bool system)
    {
        status_t res;
        io::Path dir, subdir;
        io::fattr_t fa;

        // Open the directory
        if ((res = dir.set(path)) != STATUS_OK)
            return res;
        if ((res = dir.append_child("data/drumkits")) != STATUS_OK)
            return res;

        io::Dir fd;
        if ((res = fd.open(&dir)) != STATUS_OK)
            return res;

        // Read the directory
        while ((res = fd.read(&subdir, true)) == STATUS_OK)
        {
            if ((subdir.is_dot()) || (subdir.is_dotdot()))
                continue;

            // Find all subdirectories
            if ((res = io::File::sym_stat(&subdir, &fa)) != STATUS_OK)
                continue;
            if (fa.type != io::fattr_t::FT_DIRECTORY)
                continue;

            // Lookup for drumkit file
            if ((res = subdir.append_child("drumkit.xml")) != STATUS_OK)
                continue;

            // Load drumkit settings
            hydrogen::drumkit_t dk;
            if ((res = hydrogen::load(&subdir, &dk)) != STATUS_OK)
                continue;

            // If all is OK, add drumkit metadata
            if ((res = add_drumkit(&subdir, &dk, system)) != STATUS_OK)
            {
                fd.close();
                return STATUS_NO_MEM;
            }
        }

        // Close the directory
        fd.close();

        return (res == STATUS_EOF) ? STATUS_OK : res;
    }

    void multisampler_ui::lookup_hydrogen_files()
    {
        // Lookup in system directories
        io::Path dir, subdir;
        for (const char **path = h2_system_paths; (path != NULL) && (*path != NULL); ++path)
        {
            if (dir.set(*path) != STATUS_OK)
                continue;

            scan_hydrogen_directory(&dir, true);
        }

        // Lookup in user's home directory
        if (system::get_home_directory(&dir) != STATUS_OK)
            return;

        for (const char **path = h2_user_paths; (path != NULL) && (*path != NULL); ++path)
        {
            if (subdir.set(&dir) != STATUS_OK)
                continue;
            if (subdir.append_child(*path) != STATUS_OK)
                continue;

            scan_hydrogen_directory(&subdir, false);
        }
    }

    void multisampler_ui::add_hydrogen_files_to_menu(LSPMenu *menu)
    {
        LSPString tmp;

        for (size_t i=0, n=vDrumkits.size(); i<n; ++i)
        {
            h2drumkit_t *h2 = vDrumkits.at(i);

            LSPMenuItem *child = new LSPMenuItem(&sDisplay);
            vWidgets.add(child);
            child->init();
            child->text()->set((h2->bSystem) ? "labels.file_display.system" : "labels.file_display.user");
            child->text()->params()->set_string("file", h2->sPath.as_string());
            if (h2->sPath.get_parent(&tmp) == STATUS_OK)
                child->text()->params()->set_string("parent", &tmp);
            if (h2->sPath.get_last(&tmp) == STATUS_OK)
                child->text()->params()->set_string("name", &tmp);
            child->text()->params()->set_string("title", &h2->sName);

            // Bind
            child->slots()->bind(LSPSLOT_SUBMIT, slot_import_hydrogen_file, this);
            menu->add(child);
            h2->pMenu       = child;
        }
    }

    status_t multisampler_ui::slot_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data)
    {
        multisampler_ui *_this = static_cast<multisampler_ui *>(ptr);
        if (_this == NULL)
            return STATUS_BAD_STATE;

        for (size_t i=0, n=_this->vDrumkits.size(); i<n; ++i)
        {
            h2drumkit_t *h2 = _this->vDrumkits.at(i);
            if (h2->pMenu == sender)
            {
                lsp_trace("Importing Hydrogen file from %s", h2->sPath.as_utf8());
                _this->import_hydrogen_file(h2->sPath.as_string());
                break;
            }
        }

        return STATUS_OK;
    }

    status_t multisampler_ui::slot_start_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data)
    {
        multisampler_ui *_this = static_cast<multisampler_ui *>(ptr);

        LSPFileDialog *dlg = _this->pHydrogenImport;
        if (dlg == NULL)
        {
            dlg = new LSPFileDialog(&_this->sDisplay);
            _this->vWidgets.add(dlg);
            _this->pHydrogenImport  = dlg;

            dlg->init();
            dlg->set_mode(FDM_OPEN_FILE);
            dlg->title()->set("titles.import_hydrogen_drumkit");
            dlg->action_title()->set("actions.import");

            LSPFileFilter *f = dlg->filter();
            {
                LSPFileFilterItem ffi;

                ffi.pattern()->set("*.xml");
                ffi.title()->set("files.hydrogen.xml");
                ffi.set_extension("");
                f->add(&ffi);

                ffi.pattern()->set("*");
                ffi.title()->set("files.all");
                ffi.set_extension("");
                f->add(&ffi);
            }
            dlg->bind_action(slot_call_import_hydrogen_file, ptr);
            dlg->slots()->bind(LSPSLOT_SHOW, slot_fetch_hydrogen_path, _this);
            dlg->slots()->bind(LSPSLOT_HIDE, slot_commit_hydrogen_path, _this);
        }

        return dlg->show(_this->pRoot);
    }

    status_t multisampler_ui::slot_call_import_hydrogen_file(LSPWidget *sender, void *ptr, void *data)
    {
        multisampler_ui *_this = static_cast<multisampler_ui *>(ptr);
        LSPString path;
        status_t res = _this->pHydrogenImport->get_selected_file(&path);
        if (res == STATUS_OK)
            res = _this->import_hydrogen_file(&path);
        return STATUS_OK;
    }

    status_t multisampler_ui::slot_fetch_hydrogen_path(LSPWidget *sender, void *ptr, void *data)
    {
        multisampler_ui *_this = static_cast<multisampler_ui *>(ptr);
        if ((_this == NULL) || (_this->pHydrogenPath == NULL))
            return STATUS_BAD_STATE;

        LSPFileDialog *dlg = widget_cast<LSPFileDialog>(sender);
        if (dlg == NULL)
            return STATUS_OK;

        dlg->set_path(_this->pHydrogenPath->get_buffer<char>());
        return STATUS_OK;
    }

    status_t multisampler_ui::slot_commit_hydrogen_path(LSPWidget *sender, void *ptr, void *data)
    {
        multisampler_ui *_this = static_cast<multisampler_ui *>(ptr);
        if ((_this == NULL) || (_this->pHydrogenPath == NULL))
            return STATUS_BAD_STATE;

        LSPFileDialog *dlg = widget_cast<LSPFileDialog>(sender);
        if (dlg == NULL)
            return STATUS_OK;

        const char *path = dlg->path();
        if (path != NULL)
        {
            _this->pHydrogenPath->write(path, ::strlen(path));
            _this->pHydrogenPath->notify_all();
        }

        return STATUS_OK;
    }

    void multisampler_ui::set_float_value(float value, const char *fmt...)
    {
        char port_id[32];
        va_list v;
        va_start(v, fmt);

        ::vsnprintf(port_id, sizeof(port_id)/sizeof(char), fmt, v);
        CtlPort *p = port(port_id);
        if (p != NULL)
        {
            p->set_value(value);
            p->notify_all();
        }

        va_end(v);
    }

    void multisampler_ui::set_path_value(const char *path, const char *fmt...)
    {
        char port_id[32];
        va_list v;
        va_start(v, fmt);

        ::vsnprintf(port_id, sizeof(port_id)/sizeof(char), fmt, v);
        CtlPort *p = port(port_id);
        if ((p != NULL) && (p->metadata()->role == R_PATH))
        {
            p->write(path, strlen(path));
            p->notify_all();
        }

        va_end(v);
    }

    status_t multisampler_ui::import_hydrogen_file(const LSPString *path)
    {
        // Load settings
        hydrogen::drumkit_t dk;
        status_t res = hydrogen::load(path, &dk);
        if (res != STATUS_OK)
            return res;

        // Get
        io::Path base, fpath;
        if ((res = base.set(path)) != STATUS_OK)
            return res;
        if ((res = base.remove_last()) != STATUS_OK)
            return res;

        for (size_t i=0, id=0; i < sampler_base_metadata::INSTRUMENTS_MAX; ++i)
        {
            hydrogen::instrument_t *inst = dk.instruments.get(i);
            size_t jd = 0;

            if (inst != NULL)
            {
                if (inst->layers.size() > 0)
                {
                    for (size_t j=0, m=inst->layers.size(); j<m; ++j)
                    {
                        hydrogen::layer_t *layer = inst->layers.get(j);
                        if (layer->file_name.is_empty())
                            continue;
                        if ((res = add_sample(&base, id, jd, layer)) != STATUS_OK)
                            return res;

                        ++jd; // Increment sample number
                    }
                }
                else if (!inst->file_name.is_empty())
                {
                    hydrogen::layer_t layer;
                    layer.min   = 0.0f;
                    layer.max   = 1.0f;
                    layer.gain  = inst->gain;
                    layer.pitch = 0.0f;
                    layer.file_name.set(&inst->file_name);

                    if ((res = add_sample(&base, id, jd, &layer)) != STATUS_OK)
                        return res;
                    ++jd; // Increment sample number
                }
            }

            // Reset non-used samples
            for (; jd < sampler_base_metadata::SAMPLE_FILES; ++jd)
            {
                if ((res = add_sample(&base, id, jd, NULL)) != STATUS_OK)
                    return res;
            }

            // Add instrument
            if ((res = add_instrument(id, inst)) != STATUS_OK)
                return res;
            ++id;
        }

        return STATUS_OK;
    }

    status_t multisampler_ui::add_sample(const io::Path *base, int id, int jd, const hydrogen::layer_t *layer)
    {
        io::Path path;
        status_t res;

        if (layer != NULL)
        {
            if ((res = path.set(base)) != STATUS_OK)
                return res;
            if ((res = path.append_child(&layer->file_name)) != STATUS_OK)
                return res;

            set_path_value(path.as_native(), "sf_%d_%d", id, jd);       // sample file
            set_float_value(layer->gain, "mk_%d_%d", id, jd);           // makeup gain
            set_float_value(layer->max * 100.0f, "vl_%d_%d", id, jd);   // velocity
        }
        else
        {
            set_path_value("", "sf_%d_%d", id, jd);                     // sample file
            set_float_value(GAIN_AMP_0_DB, "mk_%d_%d", id, jd);         // makeup gain
            set_float_value((100.0f * (8 - jd)) / sampler_base_metadata::SAMPLE_FILES, "vl_%d_%d", id, jd);    // velocity
        }

        set_float_value(1.0f, "on_%d_%d", id, jd);                      // enabled
        set_float_value(sampler_kernel_metadata::SAMPLE_LENGTH_MIN, "hc_%d_%d", id, jd);    // head cut
        set_float_value(sampler_kernel_metadata::SAMPLE_LENGTH_MIN, "tc_%d_%d", id, jd);    // tail cut
        set_float_value(sampler_kernel_metadata::SAMPLE_LENGTH_MIN, "fi_%d_%d", id, jd);    // fade in
        set_float_value(sampler_kernel_metadata::SAMPLE_LENGTH_MIN, "fo_%d_%d", id, jd);    // fade out
        set_float_value(sampler_kernel_metadata::PREDELAY_MIN, "pd_%d_%d", id, jd); // pre-delay
        set_float_value(-100.0f, "pl_%d_%d", id, jd);                   // pan left
        set_float_value(+100.0f, "pr_%d_%d", id, jd);                   // pan right

        return STATUS_OK;
    }

    status_t multisampler_ui::add_instrument(int id, const hydrogen::instrument_t *inst)
    {
        // Reset to defaults
        set_float_value(sampler_kernel_metadata::CHANNEL_DFL, "chan_%d", id);   // channel
        set_float_value(sampler_kernel_metadata::NOTE_DFL, "note_%d", id);      // note
        set_float_value(sampler_kernel_metadata::OCTAVE_DFL, "oct_%d", id);     // octave
        set_float_value(0.0f, "mgrp_%d", id);                                   // mute group
        set_float_value(0.0f, "mtg_%d", id);                                    // mute on stop
        set_float_value(sampler_base_metadata::DYNA_DFL, "dyna_%d", id);        // dynamics
        set_float_value(sampler_base_metadata::DRIFT_DFL, "drft_%d", id);       // time drifting
        set_float_value(1.0f, "ion_%d", id);                                    // instrument on
        set_float_value(0.0f, "ssel_%d", id);                                   // sample selector

        if (inst != NULL)
        {
            set_float_value(inst->volume, "imix_%d", id);                           // instrument mix gain

            // MIDI channel
            int channel = (inst->midi_out_channel >= 0) ? inst->midi_out_channel : inst->midi_in_channel;
            if (channel >= 0)
                set_float_value(channel, "chan_%d", id);    // channel

            // Midi Note/octave
            int note = (inst->midi_out_note >= 0) ? inst->midi_out_note : inst->midi_in_note;
            if (note >= 0)
            {
                int octave       = note / 12;
                note            %= 12;

                set_float_value(note, "note_%d", id);       // note
                set_float_value(octave, "oct_%d", id);      // octave
            }

            if (inst->mute_group >= 0)
                set_float_value(inst->mute_group + 1, "mgrp_%d", id);               // mute group

            set_float_value((inst->stop_note) ? 1.0f : 0.0f, "nto_%d", id);         // note off handling
            set_float_value(200.0f * (0.5f - inst->pan_left), "panl_%d", id);       // instrument pan left
            set_float_value(200.0f * (inst->pan_right - 0.5f), "panr_%d", id);      // instrument pan right
        }
        else
        {
            set_float_value(GAIN_AMP_0_DB, "imix_%d", id);                          // instrument mix gain
            set_float_value(0.0f, "nto_%d", id);                                    // note off handling
            set_float_value(-100.0f, "panl_%d", id);                                // instrument pan left
            set_float_value(+100.0f, "panr_%d", id);                                // instrument pan right
        }

        return STATUS_OK;
    }

}



