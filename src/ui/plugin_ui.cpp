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
    static char *escape_characters(const char *s)
    {
        if (s == NULL)
            return NULL;

        buffer_t sbuf;
        init_buf(&sbuf, strlen(s) + 32);

        while (*s != '\0')
        {
            switch (*s)
            {
                case '\"':
                    append_buf(&sbuf, "\\\"", 2);
                    break;
                case '\\':
                    append_buf(&sbuf, "\\\\", 2);
                    break;
                case '\n':
                    append_buf(&sbuf, "\\n", 2);
                    break;
                default:
                    append_buf(&sbuf, *s);
                    break;
            }
            s++;
        }

        return release_buf(&sbuf);
    }

    static ssize_t read_line(FILE *fd, buffer_t *buf)
    {
        while(true)
        {
            size_t avail    = buf->nCapacity - buf->nLength;
            if (avail < 1)
            {
                if (!extend_buf(buf, 4096))
                    return -1;
                avail    = buf->nCapacity - buf->nLength;
            }
            if (fgets(&buf->pString[buf->nLength], avail, fd) == NULL)
                return -2;

            size_t slen     = strlen(&buf->pString[buf->nLength]);
            if (memchr(&buf->pString[buf->nLength], '\n', slen) != NULL)
            {
                buf->nLength   += slen;
                return 0;
            }

            buf->nLength   += slen;
        }
        return -3;
    }
    
    static bool parse_line(buffer_t *buf, char **key, char **value)
    {
        const char *src = buf->pString;
        char *dst       = buf->pString;

        // Read the key
        *key            = dst;
        *value          = NULL;
        size_t k_chars  = 0;
        while (*src != '\0')
        {
            char ch = *(src++);
            if ((ch == ' ') || (ch == '\t'))
            {
                if (k_chars > 0)
                {
                    while (*src != '\0')
                    {
                        switch (*(src++))
                        {
                            case '=':
                                goto end_key;
                            case ' ':
                            case '\t':
                                break;
                            case '\n':
                            case '#':
                                *dst    = '\0';
                                *value  = dst;
                                return true;
                            default:
                                return false;
                        }
                    }
                    break;
                }
            }
            else if (ch == '=')
            {
                if (k_chars <= 0)
                    return false;
                break;
            }
            else if ((ch == '#') || (ch == '\n'))
            {
                if (k_chars <= 0)
                    return false;
                *dst    = '\0';
                *value  = dst;
                return true;
            }
            else if ((isalnum(ch)) || (ch == '_'))
            {
                *(dst++)    = ch;
                k_chars     ++;
            }
            else
                return false;
        }
        end_key:
        *(dst++) = '\0';

        // Read the value
        *value          = dst;
        size_t v_chars  = 0;
        bool quoted     = false;
        bool protector  = false;

        while (*src != '\0')
        {
            char ch = *(src++);
            switch (ch)
            {
                case '\"':
                    if (protector)
                    {
                        *(dst++)    = ch;
                        v_chars     ++;
                        protector   = false;
                    }
                    else if (!quoted)
                    {
                        if (v_chars > 0)
                            return false;
                        quoted      = true;
                    }
                    else
                        goto parse_end;
                    break;

                case '\\':
                    if (protector)
                    {
                        *(dst++)    = ch;
                        v_chars     ++;
                        protector   = false;
                    }
                    else
                        protector   = true;
                    break;

                case ' ':
                case '\t':
                case '#':
                    if (quoted)
                    {
                        if (protector)
                        {
                            *(dst++)    = '\\';
                            protector   = false;
                        }
                        *(dst++)    = ch;
                        v_chars     ++;
                    }
                    else if (protector)
                    {
                        *(dst++)    = ch;
                        v_chars     ++;
                        protector   = false;
                    }
                    else if (ch == '#')
                    {
                        *(dst++)    = '\0';
                        return true;
                    }
                    else if (v_chars > 0)
                        goto parse_end;
                    break;

                case '\n':
                    if (quoted)
                        return false;
                    *(dst++)    = '\0';
                    return true;

                case 'n':
                    if (protector)
                        ch          = '\n';
                    *(dst++)    = ch;
                    protector   = false;
                    v_chars     ++;
                    break;

                default:
                    if (protector)
                    {
                        *(dst++)    = '\\';
                        protector   = false;
                    }
                    *(dst++)    = ch;
                    v_chars     ++;
                    break;
            }
        }
        parse_end:
        *(dst++) = '\0';

        // Ensure that line ends correctly
        while (*src != '\0')
        {
            switch (*(src++))
            {
                case '\n':
                case '#':
                    return true;
                case ' ':
                case '\t':
                    break;
                default:
                    return false;
            }
        }

        return true;
    }

    void plugin_ui::serialize_ports(FILE *fd, cvector<CtlPort> &ports)
    {
        size_t n_ports = ports.size();

        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            CtlPort *up         = ports.at(i);
            if (up == NULL)
                continue;

            // Get metadata
            const port_t *p    = up->metadata();
            if (p == NULL)
                continue;

            // Skip output and proxy ports
            if (!IS_IN_PORT(p))
                continue;

            switch (p->role)
            {
                case R_PORT_SET:
                case R_CONTROL:
                {
                    // Serialize meta information
                    const char *unit = encode_unit(p->unit);
                    if (unit != NULL)
                        fprintf(fd, "# %s [%s]", p->name, unit);
                    else if (p->unit == U_BOOL)
                        fprintf(fd, "# %s", p->name);
                    else
                        fprintf(fd, "# %s", p->name);

                    if ((p->flags & (F_LOWER | F_UPPER)) || (p->unit == U_ENUM) || (p->unit == U_BOOL))
                    {
                        if (is_discrete_unit(p->unit) || (p->flags & F_INT))
                        {
                            if (p->unit != U_BOOL)
                            {
                                if (p->unit == U_ENUM)
                                {
                                    int value       = p->min + list_size(p->items) - 1;
                                    fprintf(fd, ": %d..%d", int(p->min), int(value));
                                }
                                else
                                    fprintf(fd, ": %d..%d", int(p->min), int(p->max));
                            }
                            else
                                fprintf(fd, ": true/false");
                        }
                        else
                            fprintf(fd, ": %.6f..%.6f", p->min, p->max);
                    }
                    fputs("\n", fd);

                    // Describe enum
                    if ((p->unit == U_ENUM) && (p->items != NULL))
                    {
                        int value   = p->min;
                        for (const char **item = p->items; *item != NULL; ++item)
                            fprintf(fd, "#   %d: %s\n", value++, *item);
                    }

                    // Serialize value
                    float value = up->get_value();
                    if (is_discrete_unit(p->unit) || (p->flags & F_INT))
                    {
                        if (p->unit == U_BOOL)
                            fprintf(fd, "%s = %s\n", p->id, (value >= 0.5f) ? "true" : "false");
                        else
                            fprintf(fd, "%s = %d\n", p->id, int(value));
                    }
                    else
                        fprintf(fd, "%s = %.6f\n", p->id, value);
                    fputs("\n", fd);
                    break;
                }
                case R_PATH:
                {
                    fprintf(fd, "# %s [pathname]\n", p->name);
                    const char *path    = up->get_buffer<const char>();
                    char *value         = escape_characters(path);
                    if (value != NULL)
                    {
                        fprintf(fd, "%s = \"%s\"\n", p->id, value);
                        lsp_free(value);
                    }
                    else
                        fprintf(fd, "%s = \"\"\n", p->id);
                    fputs("\n", fd);
                    break;
                }
                default:
                    break;
            }
        }
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

        // Clear ports
        vSortedPorts.clear();
        vConfigPorts.clear();
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

            // Button, switches, knobs and other controllers
            case WC_BUTTON:
            {
                LSPButton *btn = new LSPButton(&sDisplay);
                btn->init();
                vWidgets.add(btn);
                return new CtlButton(this, btn);
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
        char path[PATH_MAX];

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

        // Create additional ports
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

        // Read global configuration
        result = load_global_config();
        if (result != STATUS_OK)
            return result;

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
        return fopen(fname, (write) ? "w+" : "r");
    }

    status_t plugin_ui::export_settings(const char *filename)
    {
        // Open file for writing
        FILE *fd = fopen(filename, "w+");
        if (fd == NULL)
            return false;

        // Change locale
        char *saved_locale  = setlocale(1, "C");

        // Print the comment
        fputs("#-------------------------------------------------------------------------------\n", fd);
        fputs("#\n", fd);
        fputs("# This file contains configuration of the audio plugin.\n", fd);
        fprintf(fd, "#   Plugin name:         %s (%s)\n", pMetadata->name, pMetadata->description);
        fprintf(fd, "#   Plugin version:      %d.%d.%d\n",
                int(LSP_VERSION_MAJOR(pMetadata->version)),
                int(LSP_VERSION_MINOR(pMetadata->version)),
                int(LSP_VERSION_MICRO(pMetadata->version))
            );
        if (pMetadata->lv2_uid != NULL)
            fprintf(fd, "#   LV2 URI:             %s%s\n", LSP_URI(lv2), pMetadata->lv2_uid);
        if (pMetadata->vst_uid != NULL)
            fprintf(fd, "#   VST identifier:      %s\n", pMetadata->vst_uid);
        if (pMetadata->ladspa_id > 0)
            fprintf(fd, "#   LADSPA identifier:   %d\n", pMetadata->ladspa_id);
        fputs("#\n", fd);
        fputs("# (C) " LSP_FULL_NAME " \n", fd);
        fputs("#   " LSP_BASE_URI " \n#\n", fd);
        fputs("#-------------------------------------------------------------------------------\n", fd);
        fputs("\n", fd);

        serialize_ports(fd, vPorts);

        fputs("#-------------------------------------------------------------------------------\n", fd);

        // Return locale
        setlocale(1, saved_locale);

        // Close file
        fclose(fd);

        return true;
    }

    status_t plugin_ui::import_settings(const char *filename)
    {
        // Open file for writing
        FILE *fd = fopen(filename, "r");
        if (fd == NULL)
            return false;

        bool result     = deserialize_ports(fd, vPorts);

        // Close file
        fclose(fd);

        return result;
    }

    status_t plugin_ui::save_global_config()
    {
        FILE *fd    = open_config_file(true);
        if (fd == NULL)
            return false;

        // Change locale
        char *saved_locale  = setlocale(1, "C");

        // Print the comment
        fputs("#-------------------------------------------------------------------------------\n", fd);
        fputs("#\n", fd);
        fputs("# This file contains global configuration of plugins.\n", fd);
        fputs("#\n", fd);
        fputs("# (C) " LSP_FULL_NAME " \n", fd);
        fputs("#   " LSP_BASE_URI " \n#\n", fd);
        fputs("#-------------------------------------------------------------------------------\n", fd);
        fputs("\n", fd);

        serialize_ports(fd, vConfigPorts);

        fputs("#-------------------------------------------------------------------------------\n", fd);

        // Return locale
        setlocale(1, saved_locale);

        // Close file
        fclose(fd);

        return true;
    }

    status_t plugin_ui::load_global_config()
    {
        FILE *fd    = open_config_file(false);
        if (fd == NULL)
            return false;

        bool result     = deserialize_ports(fd, vConfigPorts);

        // Close file
        fclose(fd);

        return result;
    }

    bool plugin_ui::deserialize_ports(FILE *fd, cvector<CtlPort> &ports)
    {
        bool result = true;
        buffer_t line;
        init_buf(&line, 4096);

        while (true)
        {
            // Read line from file
            ssize_t read    = read_line(fd, &line);
            if (read < 0)
            {
                result      = false;
                break;
            }

            // Parse the line
            char *key = NULL, *value = NULL;
            if (parse_line(&line, &key, &value))
            {
                lsp_trace("Configuration: %s = %s", key, value);
                apply_changes(key, value, ports);
            }

            // Finally, clear the buffer
            clear_buf(&line);
        }

        // Free resources
        destroy_buf(&line);

        return result;
    }

    bool plugin_ui::apply_changes(const char *key, const char *value, cvector<CtlPort> &ports)
    {
        // Get UI port
        size_t n_ports  = ports.size();
        CtlPort *up     = NULL;
        for (size_t i=0; i<n_ports; ++i)
        {
            CtlPort *p      = ports.at(i);
            if (p == NULL)
                continue;
            const port_t *meta  = p->metadata();
            if ((meta == NULL) || (meta->id == NULL))
                continue;
            if (!strcmp(meta->id, key))
            {
                up          = p;
                break;
            }
        }
        if (up == NULL)
            return false;

        // Get metadata
        const port_t *p = up->metadata();
        if (p == NULL)
            return false;

        // Check that it's a control port
        if (!IS_IN_PORT(p))
            return false;

        // Apply changes
        switch (p->role)
        {
            case R_PORT_SET:
            case R_CONTROL:
            {
                if (is_discrete_unit(p->unit))
                {
                    if (p->unit == U_BOOL)
                    {
                        PARSE_BOOL(value,
                            up->set_value(__);
                            up->notify_all();
                        );
                    }
                    else
                    {
                        PARSE_INT(value,
                            up->set_value(__);
                            up->notify_all();
                        );
                    }
                }
                else
                {
                    PARSE_FLOAT(value,
                        up->set_value(__);
                        up->notify_all();
                    );
                }
                break;
            }
            case R_PATH:
            {
                size_t len      = strlen(value);
                up->write(value, len);
                up->notify_all();
                break;
            }
            default:
                return false;
        }
        return true;
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
