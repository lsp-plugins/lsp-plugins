/*
 * pluginui.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/alloc.h>
#include <core/buffer.h>

#include <ui/ui.h>

#include <ui/serialize.h>

#include <metadata/metadata.h>
#include <metadata/ports.h>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

namespace lsp
{
    status_t plugin_ui::ConfigHandler::handle_parameter(const char *name, const char *value)
    {
        pUI->apply_changes(name, value, hPorts);
        return STATUS_OK;
    }

    status_t plugin_ui::ConfigSource::get_head_comment(LSPString *comment)
    {
        return (comment->set(pComment)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t plugin_ui::ConfigSource::get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags)
    {
        size_t n_ports = hPorts.size();

        while (nPortID < n_ports)
        {
            // Get port
            CtlPort *up         = hPorts.at(nPortID++);
            if (up == NULL)
                continue;

            // Get metadata
            const port_t *p    = up->metadata();
            if (p == NULL)
                continue;

            // Skip output and proxy ports
            if (!IS_IN_PORT(p))
                continue;

            // Format port value
            return format_port_value(up, name, value, comment, flags);
        }

        return STATUS_NO_DATA;
    }


    //--------------------------------------------------------------------------------------------------------

    const port_t plugin_ui::vConfigMetadata[] =
    {
        SWITCH(UI_MOUNT_STUD_PORT_ID, "Visibility of mount studs in the UI", 1.0f),
        PATH(UI_LAST_VERSION_PORT_ID, "Last version of the product installed"),
        PATH(UI_DLG_SAMPLE_PATH_ID, "Dialog path for selecting sample files"),
        PATH(UI_DLG_IR_PATH_ID, "Dialog path for selecting impulse response files"),
        PATH(UI_DLG_CONFIG_PATH_ID, "Dialog path for saving/loading configuration files"),
        PATH(UI_DLG_DEFAULT_PATH_ID, "Dialog default path for other files"),
        PORTS_END
    };

    const port_t plugin_ui::vTimeMetadata[] =
    {
        UNLIMITED_METER(TIME_SAMPLE_RATE_PORT, "Sample rate", U_HZ, DEFAULT_SAMPLE_RATE),
        UNLIMITED_METER(TIME_SPEED_PORT, "Playback speed", U_NONE, 0.0f),
        UNLIMITED_METER(TIME_FRAME_PORT, "Current frame", U_NONE, 0.0f),
        UNLIMITED_METER(TIME_NUMERATOR_PORT, "Numerator", U_NONE, 4.0f),
        UNLIMITED_METER(TIME_DENOMINATOR_PORT, "Denominator", U_NONE, 4.0f),
        UNLIMITED_METER(TIME_BEATS_PER_MINUTE_PORT, "Beats per Minute", U_BPM, BPM_DEFAULT),
        UNLIMITED_METER(TIME_TICK_PORT, "Current tick", U_NONE, 0.0f),
        UNLIMITED_METER(TIME_TICKS_PER_BEAT_PORT, "Ticks per Bar", U_NONE, 960.0f),

        PORTS_END
    };

    plugin_ui::plugin_ui(const plugin_metadata_t *mdata, void *root_widget)
    {
        pMetadata       = mdata;
        pWrapper        = NULL;
        pRoot           = NULL;
        pRootWidget     = root_widget;
    }

    plugin_ui::~plugin_ui()
    {
        destroy();
    }

    void plugin_ui::destroy()
    {
        // Destroy registry
        CtlRegistry::destroy();

        // Destroy widgets
        for (size_t i=0; i<vWidgets.size(); ++i)
        {
            LSPWidget *widget = vWidgets.at(i);
            if (widget != NULL)
            {
                widget->destroy();
                delete widget;
            }
        }

        vWidgets.clear();
        pRoot     = NULL;

        // Destroy switched ports
        for (size_t i=0; i<vSwitched.size(); ++i)
        {
            CtlSwitchedPort *p = vSwitched.at(i);
            if (p != NULL)
            {
                lsp_trace("Destroy switched port id=%s", p->id());
                delete p;
            }
        }

        // Destroy config ports
        for (size_t i=0; i<vConfigPorts.size(); ++i)
        {
            CtlPort *p = vConfigPorts.at(i);
            if (p != NULL)
            {
                lsp_trace("Destroy configuration port id=%s", p->metadata()->id);
                delete p;
            }
        }

        // Destroy time ports
        for (size_t i=0; i<vTimePorts.size(); ++i)
        {
            CtlPort *p = vTimePorts.at(i);
            if (p != NULL)
            {
                lsp_trace("Destroy timing port id=%s", p->metadata()->id);
                delete p;
            }
        }

        // Clear ports
        vSortedPorts.clear();
        vConfigPorts.clear();
        vTimePorts.clear();
        vPorts.clear();
        vSwitched.clear();
        vAliases.clear(); // Aliases will be destroyed as controllers

        // Destroy display
        sDisplay.destroy();
    }

    CtlWidget *plugin_ui::create_widget(const char *w_ctl)
    {
        widget_ctl_t type = widget_ctl(w_ctl);
        return (type != WC_UNKNOWN) ? create_widget(type) : NULL;
    }

    CtlWidget *plugin_ui::create_widget(widget_ctl_t w_class)
    {
        CtlWidget *w = build_widget(w_class);
        if (w != NULL)
            add_widget(w);
        return w;
    }

    void plugin_ui::set_title(const char *title)
    {
        if (pRoot != NULL)
            pRoot->set_title(title);
    }

    CtlWidget *plugin_ui::build_widget(widget_ctl_t w_class)
    {
        switch (w_class)
        {
            // Main plugin window
            case WC_PLUGIN:
            {
                LSPWindow *wnd  = new LSPWindow(&sDisplay, pRootWidget);
                wnd->init();
                vWidgets.add(wnd);
                pRoot = wnd;
                return new CtlPluginWindow(this, wnd);
            }

            // Different kind of boxes and grids
            case WC_HBOX:
            {
                LSPBox *box = new LSPBox(&sDisplay, true);
                box->init();
                vWidgets.add(box);
                return new CtlBox(this, box, O_HORIZONTAL);
            }
            case WC_VBOX:
            {
                LSPBox *box = new LSPBox(&sDisplay, false);
                box->init();
                vWidgets.add(box);
                return new CtlBox(this, box, O_VERTICAL);
            }
            case WC_BOX:
            {
                LSPBox *box = new LSPBox(&sDisplay);
                box->init();
                vWidgets.add(box);
                return new CtlBox(this, box);
            }
            case WC_HGRID:
            {
                LSPGrid *grid = new LSPGrid(&sDisplay, true);
                grid->init();
                vWidgets.add(grid);
                return new CtlGrid(this, grid, O_HORIZONTAL);
            }
            case WC_VGRID:
            {
                LSPGrid *grid = new LSPGrid(&sDisplay, false);
                grid->init();
                vWidgets.add(grid);
                return new CtlGrid(this, grid, O_VERTICAL);
            }
            case WC_GRID:
            {
                LSPGrid *grid = new LSPGrid(&sDisplay);
                grid->init();
                vWidgets.add(grid);
                return new CtlGrid(this, grid);
            }
            case WC_CELL:
            {
                LSPCell *cell = new LSPCell(&sDisplay);
                cell->init();
                vWidgets.add(cell);
                return new CtlCell(this, cell);
            }
            case WC_ALIGN:
            {
                LSPAlign *align = new LSPAlign(&sDisplay);
                align->init();
                vWidgets.add(align);
                return new CtlAlign(this, align);
            }
            case WC_GROUP:
            {
                LSPGroup *grp = new LSPGroup(&sDisplay);
                grp->init();
                vWidgets.add(grp);
                return new CtlGroup(this, grp);
            }
            case WC_CGROUP:
            {
                LSPComboGroup *grp = new LSPComboGroup(&sDisplay);
                grp->init();
                vWidgets.add(grp);
                return new CtlComboGroup(this, grp);
            }

            // Button, switches, knobs and other controllers
            case WC_BUTTON:
            {
                LSPButton *btn = new LSPButton(&sDisplay);
                btn->init();
                vWidgets.add(btn);
                return new CtlButton(this, btn);
            }
            case WC_TTAP:
            {
                LSPButton *btn = new LSPButton(&sDisplay);
                btn->init();
                vWidgets.add(btn);
                return new CtlTempoTap(this, btn);
            }
            case WC_SWITCH:
            {
                LSPSwitch *sw = new LSPSwitch(&sDisplay);
                sw->init();
                vWidgets.add(sw);
                return new CtlSwitch(this, sw);
            }
            case WC_KNOB:
            {
                LSPKnob *knob = new LSPKnob(&sDisplay);
                knob->init();
                vWidgets.add(knob);
                return new CtlKnob(this, knob);
            }
            case WC_SBAR:
            {
                LSPScrollBar *sbar = new LSPScrollBar(&sDisplay);
                sbar->init();
                vWidgets.add(sbar);
                return new CtlScrollBar(this, sbar);
            }
            case WC_VSBAR:
            {
                LSPScrollBar *sbar = new LSPScrollBar(&sDisplay, false);
                sbar->init();
                vWidgets.add(sbar);
                return new CtlScrollBar(this, sbar);
            }
            case WC_HSBAR:
            {
                LSPScrollBar *sbar = new LSPScrollBar(&sDisplay, true);
                sbar->init();
                vWidgets.add(sbar);
                return new CtlScrollBar(this, sbar);
            }
            case WC_FADER:
            {
                LSPFader *fader = new LSPFader(&sDisplay);
                fader->init();
                vWidgets.add(fader);
                return new CtlFader(this, fader);
            }
            case WC_LISTBOX:
            {
                LSPListBox *lbox = new LSPListBox(&sDisplay);
                lbox->init();
                vWidgets.add(lbox);
                return new CtlListBox(this, lbox);
            }
            case WC_COMBO:
            {
                LSPComboBox *cbox = new LSPComboBox(&sDisplay);
                cbox->init();
                vWidgets.add(cbox);
                return new CtlComboBox(this, cbox);
            }
            case WC_EDIT:
            {
                LSPEdit *edit = new LSPEdit(&sDisplay);
                edit->init();
                vWidgets.add(edit);
                return new CtlEdit(this, edit);
            }

            // Label
            case WC_LABEL:
            {
                LSPLabel *lbl = new LSPLabel(&sDisplay);
                lbl->init();
                vWidgets.add(lbl);
                return new CtlLabel(this, lbl, CTL_LABEL_TEXT);
            }
            case WC_PARAM:
            {
                LSPLabel *lbl = new LSPLabel(&sDisplay);
                lbl->init();
                vWidgets.add(lbl);
                return new CtlLabel(this, lbl, CTL_LABEL_PARAM);
            }
            case WC_VALUE:
            {
                LSPLabel *lbl = new LSPLabel(&sDisplay);
                lbl->init();
                vWidgets.add(lbl);
                return new CtlLabel(this, lbl, CTL_LABEL_VALUE);
            }
            case WC_HLINK:
            {
                LSPHyperlink *hlink = new LSPHyperlink(&sDisplay);
                hlink->init();
                vWidgets.add(hlink);
                return new CtlHyperlink(this, hlink, CTL_LABEL_TEXT);
            }

            // Indication
            case WC_INDICATOR:
            {
                LSPIndicator *ind = new LSPIndicator(&sDisplay);
                ind->init();
                vWidgets.add(ind);
                return new CtlIndicator(this, ind);
            }
            case WC_LED:
            {
                LSPLed *led = new LSPLed(&sDisplay);
                led->init();
                vWidgets.add(led);
                return new CtlLed(this, led);
            }
            case WC_METER:
            {
                LSPMeter *mtr = new LSPMeter(&sDisplay);
                mtr->init();
                vWidgets.add(mtr);
                return new CtlMeter(this, mtr);
            }

            // Separator
            case WC_HSEP:
            {
                LSPSeparator *sep = new LSPSeparator(&sDisplay, true);
                sep->init();
                vWidgets.add(sep);
                return new CtlSeparator(this, sep, O_HORIZONTAL);
            }
            case WC_VSEP:
            {
                LSPSeparator *sep = new LSPSeparator(&sDisplay, false);
                sep->init();
                vWidgets.add(sep);
                return new CtlSeparator(this, sep, O_VERTICAL);
            }
            case WC_SEP:
            {
                LSPSeparator *sep = new LSPSeparator(&sDisplay);
                sep->init();
                vWidgets.add(sep);
                return new CtlSeparator(this, sep);
            }

            // File
            case WC_FILE:
            {
                LSPAudioFile *af = new LSPAudioFile(&sDisplay);
                af->init();
                vWidgets.add(af);
                return new CtlAudioFile(this, af);
            }
            case WC_SAVE:
            {
                LSPSaveFile *save = new LSPSaveFile(&sDisplay);
                save->init();
                vWidgets.add(save);
                return new CtlSaveFile(this, save);
            }

            // Graph object
            case WC_GRAPH:
            {
                LSPGraph *gr = new LSPGraph(&sDisplay);
                gr->init();
                vWidgets.add(gr);
                return new CtlGraph(this, gr);
            }
            case WC_AXIS:
            {
                LSPAxis *axis = new LSPAxis(&sDisplay);
                axis->init();
                vWidgets.add(axis);
                return new CtlAxis(this, axis);
            }
            case WC_CENTER:
            {
                LSPCenter *cnt = new LSPCenter(&sDisplay);
                cnt->init();
                vWidgets.add(cnt);
                return new CtlCenter(this, cnt);
            }
            case WC_BASIS:
            {
                LSPBasis *basis = new LSPBasis(&sDisplay);
                basis->init();
                vWidgets.add(basis);
                return new CtlBasis(this, basis);
            }
            case WC_MARKER:
            {
                LSPMarker *mark = new LSPMarker(&sDisplay);
                mark->init();
                vWidgets.add(mark);
                return new CtlMarker(this, mark);
            }
            case WC_MESH:
            {
                LSPMesh *mesh = new LSPMesh(&sDisplay);
                mesh->init();
                vWidgets.add(mesh);
                return new CtlMesh(this, mesh);
            }
            case WC_FBUFFER:
            {
                LSPFrameBuffer *fb = new LSPFrameBuffer(&sDisplay);
                fb->init();
                vWidgets.add(fb);
                return new CtlFrameBuffer(this, fb);
            }
            case WC_TEXT:
            {
                LSPText *text = new LSPText(&sDisplay);
                text->init();
                vWidgets.add(text);
                return new CtlText(this, text);
            }
            case WC_DOT:
            {
                LSPDot *dot = new LSPDot(&sDisplay);
                dot->init();
                vWidgets.add(dot);
                return new CtlDot(this, dot);
            }
            case WC_FRAC:
            {
                LSPFraction *frac = new LSPFraction(&sDisplay);
                frac->init();
                vWidgets.add(frac);
                return new CtlFraction(this, frac);
            }

            case WC_PORT:
            {
                CtlPortAlias *alias = new CtlPortAlias(this);
                vAliases.add(alias);
                return alias;
            }

            default:
                return NULL;
        }

        return NULL;
    }

    status_t plugin_ui::init(IUIWrapper *wrapper, int argc, const char **argv)
    {
        // Some variables
        char path[PATH_MAX + 1];

        // Store pointer to wrapper
        pWrapper    = wrapper;

        // Initialize display
        status_t result = sDisplay.init(argc, argv);
        if (result != STATUS_OK)
            return result;

        LSPTheme *theme = sDisplay.theme();
        if (theme == NULL)
            return STATUS_UNKNOWN_ERR;

        #ifdef LSP_USE_EXPAT
            strncpy(path, "res/ui/theme.xml", PATH_MAX);
        #else
            strncpy(path, "theme.xml", PATH_MAX);
        #endif /* LSP_USE_EXPAT */

        lsp_trace("Loading theme from file %s", path);
        result = load_theme(sDisplay.theme(), path);
        if (result != STATUS_OK)
            return result;

        // Final tuning of the theme
        theme->get_color(C_LABEL_TEXT, theme->font()->color());
        font_parameters_t fp;
        theme->font()->get_parameters(&fp); // Cache font parameters for further user

        // Create additional ports (ui)
        for (const port_t *p = vConfigMetadata; p->id != NULL; ++p)
        {
            switch (p->role)
            {
                case R_CONTROL:
                {
                    CtlPort *up = new CtlControlPort(p, this);
                    if (up != NULL)
                        vConfigPorts.add(up);
                    break;
                }

                case R_PATH:
                {
                    CtlPort *up = new CtlPathPort(p, this);
                    if (up != NULL)
                        vConfigPorts.add(up);
                    break;
                }

                default:
                    lsp_error("Could not instantiate configuration port id=%s", p->id);
                    break;
            }
        }

        // Create additional ports (time)
        for (const port_t *p = vTimeMetadata; p->id != NULL; ++p)
        {
            switch (p->role)
            {
                case R_METER:
                {
                    CtlValuePort *vp = new CtlValuePort(p);
                    if (vp != NULL)
                        vTimePorts.add(vp);
                    break;
                }
                default:
                    lsp_error("Could not instantiate time port id=%s", p->id);
                    break;
            }
        }

        // Read global configuration
        result = load_global_config();
        if (result != STATUS_OK)
            lsp_error("Error while loading global configuration file");

        // Generate path to UI schema
        #ifdef LSP_USE_EXPAT
            snprintf(path, PATH_MAX, "res/ui/%s", pMetadata->ui_resource);
        #else
            strncpy(path, pMetadata->ui_resource, PATH_MAX);
        #endif /* LSP_USE_EXPAT */
        lsp_trace("Generating UI from file %s", path);

        ui_builder bld(this);
        if (!bld.build(path))
        {
            lsp_error("Could not build UI from file %s", path);
            return STATUS_UNKNOWN_ERR;
        }

        // Return successful status
        return STATUS_OK;
    }

    void plugin_ui::position_updated(const position_t *pos)
    {
        size_t i = 0;
        vTimePorts[i++]->commitValue(pos->sampleRate);
        vTimePorts[i++]->commitValue(pos->speed);
        vTimePorts[i++]->commitValue(pos->frame);
        vTimePorts[i++]->commitValue(pos->numerator);
        vTimePorts[i++]->commitValue(pos->denominator);
        vTimePorts[i++]->commitValue(pos->beatsPerMinute);
        vTimePorts[i++]->commitValue(pos->tick);
        vTimePorts[i++]->commitValue(pos->ticksPerBeat);
    }

    void plugin_ui::sync_meta_ports()
    {
        for (size_t i=0, count=vTimePorts.size(); i < count; ++i)
        {
            CtlValuePort *vp = vTimePorts.at(i);
            if (vp != NULL)
                vp->sync();
        }
    };

    status_t plugin_ui::add_port(CtlPort *port)
    {
        if (!vPorts.add(port))
            return STATUS_NO_MEM;

        lsp_trace("added port id=%s", port->metadata()->id);
        return STATUS_OK;
    }

    bool plugin_ui::create_directory(const char *path)
    {
        struct stat fattr;
        if (stat(path, &fattr) != 0)
        {
            int code = errno;
            if (code != ENOENT)
                return false;
            if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
                return true;

            lsp_error("Error while trying to create configuration directory %s", path);
            return false;
        }
        return S_ISDIR(fattr.st_mode);
    }

    FILE *plugin_ui::open_config_file(bool write)
    {
        char fname[PATH_MAX];
        const char *homedir     = getenv("HOME");
        if (homedir == NULL)
            return NULL;

        snprintf(fname, PATH_MAX-1, "%s/.config", homedir);
        if (!create_directory(fname))
            return NULL;

        snprintf(fname, PATH_MAX-1, "%s/.config/%s", homedir, LSP_ARTIFACT_ID);
        if (!create_directory(fname))
            return NULL;

        snprintf(fname, PATH_MAX-1, "%s/.config/%s/%s.cfg", homedir, LSP_ARTIFACT_ID, LSP_ARTIFACT_ID);
        return fopen(fname, (write) ? "w" : "r");
    }

    status_t plugin_ui::export_settings(const char *filename)
    {
        LSPString c;

        c.append_utf8       ("This file contains configuration of the audio plugin.\n");
        c.fmt_append_utf8   ("  Plugin name:         %s (%s)\n", pMetadata->name, pMetadata->description);
        c.fmt_append_utf8   ("  Plugin version:      %d.%d.%d\n",
                int(LSP_VERSION_MAJOR(pMetadata->version)),
                int(LSP_VERSION_MINOR(pMetadata->version)),
                int(LSP_VERSION_MICRO(pMetadata->version))
            );
        if (pMetadata->lv2_uid != NULL)
            c.fmt_append_utf8   ("  LV2 URI:             %s%s\n", LSP_URI(lv2), pMetadata->lv2_uid);
        if (pMetadata->vst_uid != NULL)
            c.fmt_append_utf8   ("  VST identifier:      %s\n", pMetadata->vst_uid);
        if (pMetadata->ladspa_id > 0)
            c.fmt_append_utf8   ("  LADSPA identifier:   %d\n", pMetadata->ladspa_id);
        c.append            ('\n');
        c.append_utf8       ("(C) " LSP_FULL_NAME " \n");
        c.append_utf8       ("  " LSP_BASE_URI " \n");

        ConfigSource cfg(this, vPorts, &c);

        return config::save(filename, &cfg, true);
    }

    status_t plugin_ui::import_settings(const char *filename)
    {
        ConfigHandler handler(this, vPorts);
        return config::load(filename, &handler);
    }

    status_t plugin_ui::save_global_config()
    {
        FILE *fd    = open_config_file(true);
        if (fd == NULL)
            return false;

        LSPString c;

        c.append_utf8       ("This file contains global configuration of plugins.\n");
        c.append            ('\n');
        c.append_utf8       ("(C) " LSP_FULL_NAME " \n");
        c.append_utf8       ("  " LSP_BASE_URI " \n");

        ConfigSource cfg(this, vConfigPorts, &c);

        status_t status = config::save(fd, &cfg, true);

        // Close file
        fclose(fd);

        return status;
    }

    status_t plugin_ui::load_global_config()
    {
        FILE *fd    = open_config_file(false);
        if (fd == NULL)
            return false;

        ConfigHandler handler(this, vConfigPorts);
        status_t status = config::load(fd, &handler);

        // Close file
        fclose(fd);

        return status;
    }

    bool plugin_ui::apply_changes(const char *key, const char *value, cvector<CtlPort> &ports)
    {
        // Get UI port
        size_t n_ports  = ports.size();
        for (size_t i=0; i<n_ports; ++i)
        {
            CtlPort *p      = ports.at(i);
            if (p == NULL)
                continue;
            const port_t *meta  = p->metadata();
            if ((meta == NULL) || (meta->id == NULL))
                continue;
            if (!strcmp(meta->id, key))
                return set_port_value(p, value);
        }
        return false;
    }

    size_t plugin_ui::rebuild_sorted_ports()
    {
        size_t count = vPorts.size();
        vSortedPorts.clear();

        for (size_t i=0; i<count; ++i)
            vSortedPorts.add(vPorts.at(i));

        count   = vSortedPorts.size();

        // Sort by port ID
        if (count >= 2)
        {
            for (size_t i=0; i<(count-1); ++i)
                for (size_t j=i+1; j<count; ++j)
                {
                    CtlPort *a  = vSortedPorts.at(i);
                    CtlPort *b  = vSortedPorts.at(j);
                    if ((a == NULL) || (b == NULL))
                        continue;
                    const port_t *am    = a->metadata();
                    const port_t *bm    = b->metadata();
                    if ((am == NULL) || (bm == NULL))
                        continue;
                    if (strcmp(am->id, bm->id) > 0)
                        vSortedPorts.swap_unsafe(i, j);
                }
        }

//        #ifdef LSP_TRACE
//            for (size_t i=0; i<count; ++i)
//                lsp_trace("sorted port idx=%d, id=%s", int(i), vSortedPorts[i]->metadata()->id);
//        #endif /* LSP_TRACE */

        return count;
    }

    CtlPort *plugin_ui::port(const char *name)
    {
        // Check aliases
        size_t n_aliases = vAliases.size();

        for (size_t i=0; i<n_aliases; ++i)
        {
            CtlPortAlias *pa = vAliases.at(i);
            if ((pa->id() == NULL) || (pa->alias() == NULL))
                continue;

            if (!strcmp(name, pa->id()))
            {
                name    = pa->alias();
                break;
            }
        }

        // Check that port name contains index
        if (strchr(name, '[') != NULL)
        {
            // Try to find switched port
            size_t count = vSwitched.size();
            for (size_t i=0; i<count; ++i)
            {
                CtlSwitchedPort *p  = vSwitched.at(i);
                if (p == NULL)
                    continue;
                const char *p_id    = p->id();
                if (p_id == NULL)
                    continue;
                if (!strcmp(p_id, name))
                    return p;
            }

            // Create new switched port
            CtlSwitchedPort *s   = new CtlSwitchedPort(this);
            if (s == NULL)
                return NULL;

            if (s->compile(name))
            {
                if (vSwitched.add(s))
                    return s;
            }

            delete s;
            return NULL;
        }

        // Check that port name contains "ui:" prefix
        if (strstr(name, UI_CONFIG_PORT_PREFIX) == name)
        {
            const char *ui_id = &name[strlen(UI_CONFIG_PORT_PREFIX)];

            // Try to find configuration port
            size_t count = vConfigPorts.size();
            for (size_t i=0; i<count; ++i)
            {
                CtlPort *p          = vConfigPorts.at(i);
                if (p == NULL)
                    continue;
                const char *p_id    = p->metadata()->id;
                if (p_id == NULL)
                    continue;
                if (!strcmp(p_id, ui_id))
                    return p;
            }
        }

        // Check that port name contains "ui:" prefix
        if (strstr(name, TIME_PORT_PREFIX) == name)
        {
            const char *ui_id = &name[strlen(TIME_PORT_PREFIX)];

            // Try to find configuration port
            size_t count = vTimePorts.size();
            for (size_t i=0; i<count; ++i)
            {
                CtlPort *p          = vTimePorts.at(i);
                if (p == NULL)
                    continue;
                const char *p_id    = p->metadata()->id;
                if (p_id == NULL)
                    continue;
                if (!strcmp(p_id, ui_id))
                    return p;
            }
        }

        // Do usual stuff
        size_t count = vPorts.size();
        if (vSortedPorts.size() != count)
            count = rebuild_sorted_ports();

        // Try to find the corresponding port
        ssize_t first = 0, last = count - 1;
        while (first <= last)
        {
            size_t center       = (first + last) >> 1;
            CtlPort *p          = vSortedPorts.at(center);
            if (p == NULL)
                break;
            const port_t *ctl   = p->metadata();
            if (ctl == NULL)
                break;

            int cmp     = strcmp(name, ctl->id);
            if (cmp < 0)
                last    = center - 1;
            else if (cmp > 0)
                first   = center + 1;
            else
                return p;

        }
        return NULL;
    }


} /* namespace lsp */
