/*
 * plugin_ui.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/alloc.h>
#include <core/buffer.h>
#include <core/system.h>
#include <core/resource.h>
#include <core/io/Path.h>
#include <core/io/Dir.h>
#include <core/io/NativeFile.h>

#include <ui/ui.h>

#include <ui/serialize.h>

#include <metadata/metadata.h>
#include <metadata/ports.h>

#include <ctype.h>
#include <string.h>

namespace lsp
{
    plugin_ui::ConfigHandler::~ConfigHandler()
    {
        for (size_t i=0, n=vNotify.size(); i<n; ++i)
        {
            char *p = vNotify.get(i);
            if (p != NULL)
                ::free(p);
        }
        vNotify.flush();
    }

    status_t plugin_ui::ConfigHandler::handle_parameter(const char *name, const char *value, size_t flags)
    {
        add_notification(name);
        pUI->apply_changes(name, value, hPorts, bPreset);
        return STATUS_OK;
    }

    status_t plugin_ui::ConfigHandler::handle_kvt_parameter(const char *name, const kvt_param_t *value, size_t flags)
    {
        if (pKVT == NULL)
            return STATUS_OK;

        pKVT->put(name, value, KVT_RX);
        pUI->kvt_write(pKVT, name, value);

        return STATUS_OK;
    }

    void plugin_ui::ConfigHandler::add_notification(const char *id)
    {
        char *notify = ::strdup(id);
        if (notify == NULL)
            return;

        if (!vNotify.add(notify))
            ::free(notify);
    }

    void plugin_ui::ConfigHandler::notify_all()
    {
        for (size_t i=0, n=vNotify.size(); i<n; ++i)
        {
            char *p = vNotify.get(i);
            if (p == NULL)
                continue;

            if (p[0] == '/') // KVT ?
            {
                const kvt_param_t *param = NULL;
                if ((pKVT->get(p, &param) == STATUS_OK) && (param != NULL))
                    pUI->kvt_write(pKVT, p, param);
            }
            else
            {
                CtlPort *ctl = pUI->port(p);
                if (ctl != NULL)
                    ctl->notify_all();
            }
            ::free(p);
        }
        vNotify.flush();
    }

    status_t plugin_ui::ConfigSource::get_head_comment(LSPString *comment)
    {
        return (comment->set(pComment)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t plugin_ui::ConfigSource::get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags)
    {
        // Get regular ports
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

            // Try to format port value
            status_t res = format_port_value(up, name, value, comment, flags);

            // Skip port if it has bad, non-serializable type
            if (res == STATUS_BAD_TYPE)
                continue;

            return res;
        }

        // Get KVT parameters
        while ((pIter != NULL) && (pIter->next() == STATUS_OK))
        {
            const kvt_param_t *p;

            // Get KVT parameter
            status_t res = pIter->get(&p);
            if (res == STATUS_NOT_FOUND)
                continue;
            else if (res != STATUS_OK)
            {
                lsp_warn("Could not get parameter: code=%d", int(res));
                break;
            }

            // Skip transient and private parameters
            if ((pIter->is_transient()) || (pIter->is_private()))
                continue;

            // Get parameter name
            const char *pname = pIter->name();
            if (pname == NULL)
                continue;
            if (!name->set_ascii(pname))
            {
                lsp_warn("Failed to do set_ascii");
                continue;
            }

            // Serialize state
            bool fmt = false;
            switch (p->type)
            {
                case KVT_INT32:
                    fmt     = value->fmt_ascii("%li", (signed long)(p->i32));
                    *flags  = config::SF_TYPE_I32;
                    break;
                case KVT_UINT32:
                    fmt     = value->fmt_ascii("%lu", (unsigned long)(p->u32));
                    *flags  = config::SF_TYPE_U32;
                    break;
                case KVT_INT64:
                    fmt = value->fmt_ascii("%lli", (signed long long)(p->i64));
                    *flags  = config::SF_TYPE_I64;
                    break;
                case KVT_UINT64:
                    fmt = value->fmt_ascii("%llu", (unsigned long long)(p->u64));
                    *flags  = config::SF_TYPE_U64;
                    break;
                case KVT_FLOAT32:
                    fmt = value->fmt_ascii("%f", p->f32);
                    *flags  = config::SF_TYPE_F32;
                    break;
                case KVT_FLOAT64:
                    fmt = value->fmt_ascii("%f", p->f64);
                    *flags  = config::SF_TYPE_F64;
                    break;
                case KVT_STRING:
                    fmt = value->set_utf8(p->str);
                    *flags  = config::SF_TYPE_STR | config::SF_QUOTED;
                    break;
                case KVT_BLOB:
                {
                    fmt = value->fmt_ascii("%s:%ld:",
                            (p->blob.ctype != NULL) ? p->blob.ctype : "",
                            (long)(p->blob.size)
                        );
                    if ((p->blob.size > 0) && (p->blob.data == NULL))
                        break;

                    // Base-64 encode blob value
                    if (p->blob.size > 0)
                    {
                        size_t dst_size = 0x10 + ((p->blob.size * 4) / 3);
                        char *base64 = reinterpret_cast<char *>(::malloc(dst_size));
                        if (base64 == NULL)
                            break;

                        size_t dst_left = dst_size, src_left = p->blob.size;
                        dsp::base64_enc(base64, &dst_left, p->blob.data, &src_left);
                        fmt = value->append_ascii(base64, dst_size - dst_left);
                        ::free(base64);
                    }
                    else
                        fmt = true;

                    if (fmt)
                        *flags  = config::SF_TYPE_BLOB | config::SF_QUOTED;
                    break;
                }
                default:
                    break;
            }

            if (!fmt)
            {
                lsp_warn("Error formatting parameter %s", pname);
                continue;
            }

            // All is OK
            return STATUS_OK;
        }

        return STATUS_NO_DATA;
    }

    plugin_ui::ConfigSink::~ConfigSink()
    {
        unbind();
    }

    void plugin_ui::ConfigSink::unbind()
    {
        if (pUI != NULL)
            pUI->pConfigSink = NULL;
        pUI = NULL;
    }

    status_t plugin_ui::ConfigSink::on_complete(status_t code, const LSPString *data)
    {
        return ((code == STATUS_OK) && (pUI != NULL)) ? pUI->paste_from_clipboard(data) : STATUS_OK;
    }


    //--------------------------------------------------------------------------------------------------------

    const port_t plugin_ui::vConfigMetadata[] =
    {
        SWITCH(UI_MOUNT_STUD_PORT_ID, "Visibility of mount studs in the UI", 1.0f),
        PATH(UI_LAST_VERSION_PORT_ID, "Last version of the product installed"),
        PATH(UI_DLG_SAMPLE_PATH_ID, "Dialog path for selecting sample files"),
        PATH(UI_DLG_IR_PATH_ID, "Dialog path for selecting impulse response files"),
        PATH(UI_DLG_CONFIG_PATH_ID, "Dialog path for saving/loading configuration files"),
        PATH(UI_DLG_REW_PATH_ID, "Dialog path for importing REW settings files"),
        PATH(UI_DLG_MODEL3D_PATH_ID, "Dialog for saving/loading 3D model files"),
        PATH(UI_DLG_DEFAULT_PATH_ID, "Dialog default path for other files"),
        PATH(UI_R3D_BACKEND_PORT_ID, "Identifier of selected backend for 3D rendering"),
        PATH(UI_LANGUAGE_PORT_ID, "Selected language identifier for the UI interface"),
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
        pRootCtl        = NULL;
        pRootWidget     = root_widget;
        pConfigSink     = NULL;
    }

    plugin_ui::~plugin_ui()
    {
        destroy();
    }

    void plugin_ui::destroy()
    {
        // Unbind sink
        if (pConfigSink != NULL)
        {
            pConfigSink->unbind();
            pConfigSink = NULL;
        }

        // Destroy registry
        CtlRegistry::destroy();

        // Destroy widgets
        for (size_t i=0, n=vWidgets.size(); i<n; ++i)
        {
            LSPWidget *widget = vWidgets.at(i);
            if (widget != NULL)
            {
                widget->destroy();
                delete widget;
            }
        }

        vWidgets.flush();
        pRoot       = NULL;
        pRootCtl    = NULL;

        // Destroy switched ports
        for (size_t i=0, n=vSwitched.size(); i<n; ++i)
        {
            CtlSwitchedPort *p = vSwitched.at(i);
            if (p != NULL)
            {
                lsp_trace("Destroy switched port id=%s", p->id());
                delete p;
            }
        }

        // Destroy config ports
        for (size_t i=0, n=vConfigPorts.size(); i<n; ++i)
        {
            CtlPort *p = vConfigPorts.at(i);
            if (p != NULL)
            {
                lsp_trace("Destroy configuration port id=%s", p->metadata()->id);
                delete p;
            }
        }

        // Destroy time ports
        for (size_t i=0, n=vTimePorts.size(); i<n; ++i)
        {
            CtlPort *p = vTimePorts.at(i);
            if (p != NULL)
            {
                lsp_trace("Destroy timing port id=%s", p->metadata()->id);
                delete p;
            }
        }

        // Destroy custom ports
        for (size_t i=0, n=vCustomPorts.size(); i<n; ++i)
        {
            CtlPort *p = vCustomPorts.at(i);
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
        vCustomPorts.clear();
        vSwitched.clear();
        vAliases.clear(); // Aliases will be destroyed as controllers
        vKvtListeners.flush(); // Destroy references to KVT listeners

        // Destroy display
        sDisplay.destroy();

        // Destroy list of presets
        destroy_presets();
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
            pRoot->title()->set(title);
    }

    CtlWidget *plugin_ui::build_widget(widget_ctl_t w_class)
    {
        switch (w_class)
        {
            // Main plugin window
            case WC_PLUGIN:
            {
                if (pRoot == NULL)
                {
                    pRoot = new LSPWindow(&sDisplay, pRootWidget);
                    pRoot->init();
                    vWidgets.add(pRoot);
                }
                if (pRootCtl == NULL)
                    pRootCtl = new CtlPluginWindow(this, pRoot);
                return pRootCtl;
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
            case WC_HSBOX:
            {
                LSPScrollBox *box = new LSPScrollBox(&sDisplay, true);
                box->init();
                vWidgets.add(box);
                return new CtlScrollBox(this, box, O_HORIZONTAL);
            }
            case WC_VSBOX:
            {
                LSPScrollBox *box = new LSPScrollBox(&sDisplay, false);
                box->init();
                vWidgets.add(box);
                return new CtlScrollBox(this, box, O_VERTICAL);
            }
            case WC_SBOX:
            {
                LSPScrollBox *box = new LSPScrollBox(&sDisplay);
                box->init();
                vWidgets.add(box);
                return new CtlScrollBox(this, box);
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
                return new CtlCell(this);
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
            case WC_THREADCOMBO:
            {
                LSPComboBox *cbox = new LSPComboBox(&sDisplay);
                cbox->init();
                vWidgets.add(cbox);
                return new CtlThreadComboBox(this, cbox);
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
            case WC_STATUS:
            {
                LSPLabel *lbl = new LSPLabel(&sDisplay);
                lbl->init();
                vWidgets.add(lbl);
                return new CtlLabel(this, lbl, CTL_STATUS_CODE);
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
            case WC_PROGRESS:
            {
                LSPProgressBar *bar = new LSPProgressBar(&sDisplay);
                bar->init();
                vWidgets.add(bar);
                return new CtlProgressBar(this, bar);
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
            case WC_VOID:
            {
                LSPVoid *v = new LSPVoid(&sDisplay);
                v->init();
                vWidgets.add(v);
                return new CtlVoid(this, v);
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
            case WC_LOAD:
            {
                LSPLoadFile *load = new LSPLoadFile(&sDisplay);
                load->init();
                vWidgets.add(load);
                return new CtlLoadFile(this, load);
            }
            case WC_SAMPLE:
            {
                LSPAudioSample *sample = new LSPAudioSample(&sDisplay);
                sample->init();
                vWidgets.add(sample);
                return new CtlAudioSample(this, sample);
            }

            // 3D viewer object
            case WC_VIEWER3D:
            {
                LSPArea3D *a = new LSPArea3D(&sDisplay);
                a->init();
                vWidgets.add(a);
                return new CtlViewer3D(this, a);
            }
            case WC_CAPTURE3D:
            {
                LSPCapture3D *c = new LSPCapture3D(&sDisplay);
                c->init();
                vWidgets.add(c);
                return new CtlCapture3D(this, c);
            }
            case WC_SOURCE3D:
            {
                LSPMesh3D *m = new LSPMesh3D(&sDisplay);
                m->init();
                vWidgets.add(m);
                return new CtlSource3D(this, m);
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
        // Store pointer to wrapper
        pWrapper    = wrapper;

        // Initialize display
        status_t result = sDisplay.init(argc, argv);
        if (result != STATUS_OK)
            return result;

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

        result = scan_presets();
        if (result != STATUS_OK)
            return result;

        // Return successful status
        return STATUS_OK;
    }

    status_t plugin_ui::slot_preset_select(LSPWidget *sender, void *ptr, void *data)
    {
        plugin_ui *_this = reinterpret_cast<plugin_ui *>(ptr);
        if (_this == NULL)
            return STATUS_BAD_STATE;

        for (size_t i=0, n=_this->vPresets.size(); i<n; ++i)
        {
            preset_t *p     = _this->vPresets.at(i);
            if ((p == NULL) || (p->item != sender))
                continue;

            return _this->import_settings(p->path, true);
        }

        return STATUS_OK;
    }

    status_t plugin_ui::scan_presets()
    {
        char base[PATH_MAX + 1];

#ifdef LSP_BUILTIN_RESOURCES
        ::snprintf(base, PATH_MAX, "presets/%s/", pMetadata->ui_presets);

        base[PATH_MAX] = '\0';
        size_t prefix_len = ::strlen(base);

        for (const resource::resource_t *r = resource::all(); (r->id != NULL); ++r)
        {
            // Check that resource matches
            if (r->type != resource::RESOURCE_PRESET)
                continue;
            if (::strstr(r->id, base) != r->id)
                continue;

            // Add preset
            preset_t *p = vPresets.add();
            if (p == NULL)
            {
                destroy_presets();
                return STATUS_NO_MEM;
            }
            p->name     = NULL;
            p->path     = NULL;
            p->item     = NULL;

            int xres    = ::asprintf(&p->path, "builtin://%s", r->id);
            if ((xres <= 0) || (p->path == NULL))
            {
                destroy_presets();
                return STATUS_NO_MEM;
            }

            if ((p->name = ::strdup(&r->id[prefix_len])) == NULL)
            {
                destroy_presets();
                return STATUS_NO_MEM;
            }

            // Remove '.preset' extension from name
            size_t len = ::strlen(p->name);
            if ((len >= 7) && (::strcasecmp(&p->name[len-7], ".preset") == 0))
                p->name[len-7]   = '\0';
        }
#else
        ::snprintf(base, PATH_MAX, "res" FILE_SEPARATOR_S "presets" FILE_SEPARATOR_S "%s", pMetadata->ui_presets);

        io::Dir dir;
        io::Path entry;
        io::fattr_t fattr;
        LSPString extension, tmp;

        if (!extension.set_ascii(".preset"))
            return STATUS_NO_MEM;

        status_t res = dir.open(base);
        if (res != STATUS_OK)
            return STATUS_OK;

        while (true)
        {
            // Read entry
            res = dir.reads(&entry, &fattr, true);
            if (res == STATUS_EOF)
                break;
            else if (res != STATUS_OK)
            {
                destroy_presets();
                dir.close();
                return res;
            }
            if (fattr.type != io::fattr_t::FT_REGULAR)
                continue;
            if (!entry.as_string()->ends_with_nocase(&extension))
                continue;

            // Add preset
            preset_t *p = vPresets.add();
            if (p == NULL)
            {
                destroy_presets();
                return STATUS_NO_MEM;
            }
            p->name     = NULL;
            p->path     = NULL;
            p->item     = NULL;

            if ((p->path = ::strdup(entry.as_string()->get_utf8())) == NULL)
            {
                destroy_presets();
                return STATUS_NO_MEM;
            }

            res = entry.get_last(&tmp);
            if (res != STATUS_OK)
            {
                destroy_presets();
                return res;
            }

            ssize_t len = tmp.length() - extension.length();
            if (len >= 0)
                tmp.set_length(len);

            if ((p->name = ::strdup(tmp.get_utf8())) == NULL)
            {
                destroy_presets();
                return STATUS_NO_MEM;
            }
        }

        dir.close();
#endif

        // Sort presets in alphabetical order
        if (vPresets.size() > 0)
        {
            for (ssize_t i=0, n=vPresets.size(); i<n-1; ++i)
            {
                preset_t *a = vPresets.at(i);
                for (ssize_t j=i+1; j<n; ++j)
                {
                    preset_t *b = vPresets.at(j);
                    if (strcmp(a->name, b->name) > 0)
                    {
                        swap(a->path, b->path);
                        swap(a->name, b->name);
                        swap(a->item, b->item);
                    }
                }
            }
        }

        return STATUS_OK;
    }

    void plugin_ui::destroy_presets()
    {
        for (size_t i=0, n=vPresets.size(); i<n; ++i)
        {
            preset_t *p = vPresets.at(i);
            if (p->name != NULL)
                ::free(p->name);
            if (p->path != NULL)
                ::free(p->path);
            p->item = NULL;
        }
        vPresets.flush();
    }

    status_t plugin_ui::build()
    {
        status_t result;
        LSPString path;

        // Load theme
        LSPTheme *theme = sDisplay.theme();
        if (theme == NULL)
            return STATUS_UNKNOWN_ERR;

        lsp_trace("Loading theme");
        result = load_theme(theme, "ui/theme.xml");
        if (result != STATUS_OK)
            return result;

        lsp_trace("Loading dictionary");
        IDictionary *dict = sDisplay.dictionary();
        #ifdef LSP_BUILTIN_RESOURCES
            result = dict->init(LSP_BUILTIN_PREFIX "i18n");
        #else
            result = dict->init(LSP_RESOURCE_PATH "/i18n");
        #endif
        if (result != STATUS_OK)
            return result;

        // Read global configuration
        result = load_global_config();
        if (result != STATUS_OK)
            lsp_error("Error while loading global configuration file");

        // Generate path to UI schema
        ui_builder bld(this);
        if (!path.fmt_utf8("ui/%s", pMetadata->ui_resource))
            return STATUS_NO_MEM;
        lsp_trace("Generating UI from URI %s", path.get_utf8());
        if ((result = bld.build(&path)) != STATUS_OK)
        {
            lsp_error("Could not build UI from URI %s", path.get_utf8());
            return result;
        }
        lsp_trace("UI has been generated");

        // Fetch main menu
        LSPMenu *menu       = widget_cast<LSPMenu>(resolve(WUID_MAIN_MENU));
        if (menu == NULL)
            return STATUS_NO_MEM;

        // Add presets if they are present
        if ((menu != NULL) && (vPresets.size() > 0))
        {
            // Get display
            LSPDisplay *dpy     = menu->display();

            // Create submenu item
            LSPMenuItem *item   = new LSPMenuItem(dpy);
            if (item == NULL)
                return STATUS_NO_MEM;
            vWidgets.add(item);
            result = item->init();
            if (result != STATUS_OK)
                return result;

            item->text()->set("actions.load_preset");
            menu->add(item);

            // Create submenu
            LSPMenu *submenu    = new LSPMenu(dpy);
            if (submenu == NULL)
                return STATUS_NO_MEM;
            vWidgets.add(submenu);
            result = submenu->init();
            if (result != STATUS_OK)
                return result;
            item->set_submenu(submenu);

            // Iterate all presets
            for (size_t i=0, n=vPresets.size(); i<n; ++i)
            {
                preset_t *p     = vPresets.at(i);
                if (p == NULL)
                    continue;

                // Create menu item and bind handler
                item        = new LSPMenuItem(dpy);
                if (item == NULL)
                    return STATUS_NO_MEM;
                vWidgets.add(item);
                result = item->init();
                if (result != STATUS_OK)
                    return result;
                item->text()->set_raw(p->name);
                p->item     = item;

                item->slots()->bind(LSPSLOT_SUBMIT, slot_preset_select, this);
                submenu->add(item);
            }
        }

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

    void plugin_ui::kvt_write(KVTStorage *storage, const char *id, const kvt_param_t *value)
    {
        for (size_t i=0, n=vKvtListeners.size(); i<n; ++i)
        {
            CtlKvtListener *l = vKvtListeners.at(i);
            if (l != NULL)
                l->changed(storage, id, value);
        }
    }

    KVTStorage *plugin_ui::kvt_lock()
    {
        return (pWrapper != NULL) ? pWrapper->kvt_lock() : NULL;
    }

    KVTStorage *plugin_ui::kvt_trylock()
    {
        return (pWrapper != NULL) ? pWrapper->kvt_trylock() : NULL;
    }

    void plugin_ui::kvt_release()
    {
        if (pWrapper != NULL)
            pWrapper->kvt_release();
    }

    status_t plugin_ui::add_port(CtlPort *port)
    {
        if (!vPorts.add(port))
            return STATUS_NO_MEM;

        lsp_trace("added port id=%s", port->metadata()->id);
        return STATUS_OK;
    }

    status_t plugin_ui::add_custom_port(CtlPort *port)
    {
        if (!vCustomPorts.add(port))
            return STATUS_NO_MEM;

        lsp_trace("added custom port id=%s", port->metadata()->id);
        return STATUS_OK;
    }

    status_t plugin_ui::add_kvt_listener(CtlKvtListener *listener)
    {
        if (!vKvtListeners.add(listener))
            return STATUS_NO_MEM;
        lsp_trace("added KVT listener id=%s", listener->name());
        return STATUS_OK;
    }

    io::File *plugin_ui::open_config_file(bool write)
    {
        io::Path cfg;
        status_t res = system::get_home_directory(&cfg);
        if (res == STATUS_OK)
            res = cfg.append_child(".config");
        if (res == STATUS_OK)
            res = cfg.append_child(LSP_ARTIFACT_ID);
        if (res == STATUS_OK)
            res = cfg.mkdir(true);
        if (res == STATUS_OK)
            res = cfg.append_child(LSP_ARTIFACT_ID ".cfg");

        if (res != STATUS_OK)
            return NULL;

        io::NativeFile *fd = new io::NativeFile();
        if (fd == NULL)
            return NULL;
        res = fd->open(&cfg, (write) ?
                io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE :
                io::File::FM_READ);

        if (res == STATUS_OK)
            return fd;

        fd->close();
        delete fd;
        return NULL;
    }

    void plugin_ui::build_config_header(LSPString &c)
    {
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
    }

    status_t plugin_ui::export_settings(const char *filename)
    {
        LSPString c;
        build_config_header(c);

        // Serialize data to file
        KVTStorage *kvt = kvt_lock();
        ConfigSource cfg(this, vPorts, kvt, &c);
        status_t res = config::save(filename, &cfg, true);
        kvt->gc();
        kvt_release();

        return res;
    }

    status_t plugin_ui::export_settings_to_clipboard()
    {
        LSPString c, data;
        build_config_header(c);

        // Serialize data to string
        KVTStorage *kvt = kvt_lock();
        ConfigSource cfg(this, vPorts, kvt, &c);
        status_t res = config::serialize(&data, &cfg, true);
        kvt->gc();
        kvt_release();

        if (res != STATUS_OK)
            return res;

        // Put data to clipboard
        LSPTextDataSource *ds = new LSPTextDataSource();
        if (ds == NULL)
            return STATUS_NO_MEM;
        ds->acquire();
        res = ds->set_text(&data);
        if (res == STATUS_OK)
            res = sDisplay.set_clipboard(CBUF_CLIPBOARD, ds);
        ds->release();

        return res;
    }

    status_t plugin_ui::import_settings_from_clipboard()
    {
        // Unbind previous
        ConfigSink *ds = new ConfigSink(this);
        if (ds == NULL)
            return STATUS_NO_MEM;

        if (pConfigSink != NULL)
            pConfigSink->unbind();
        pConfigSink = ds;

        // Request clipboard data
        ds->acquire();
        status_t res = sDisplay.get_clipboard(CBUF_CLIPBOARD, pConfigSink);
        ds->release();

        return res;
    }

    status_t plugin_ui::paste_from_clipboard(const LSPString *data)
    {
        // Deserialize configuration data
        KVTStorage *kvt = kvt_lock();
        ConfigHandler handler(this, vPorts, kvt, false);
        status_t res = config::deserialize(data, &handler);
        handler.notify_all();
        if (kvt != NULL)
        {
            kvt->gc();
            kvt_release();
        }

        return res;
    }

    status_t plugin_ui::import_settings(const char *filename, bool preset)
    {
        // Load configuration data
        KVTStorage *kvt = kvt_lock();
        ConfigHandler handler(this, vPorts, kvt, preset);
        status_t res = config::load(filename, &handler);
        handler.notify_all();
        if (kvt != NULL)
        {
            kvt->gc();
            kvt_release();
        }

        return res;
    }

    status_t plugin_ui::save_global_config()
    {
        io::File *fd    = open_config_file(true);
        if (fd == NULL)
            return STATUS_UNKNOWN_ERR;

        LSPString c;

        c.append_utf8       ("This file contains global configuration of plugins.\n");
        c.append            ('\n');
        c.append_utf8       ("(C) " LSP_FULL_NAME " \n");
        c.append_utf8       ("  " LSP_BASE_URI " \n");

        ConfigSource cfg(this, vConfigPorts, NULL, &c);

        status_t status = config::save(fd, &cfg, true);

        // Close file
        fd->close();
        delete fd;

        return status;
    }

    status_t plugin_ui::load_global_config()
    {
        io::File *fd    = open_config_file(false);
        if (fd == NULL)
            return STATUS_UNKNOWN_ERR;

        ConfigHandler handler(this, vConfigPorts, NULL, false);
        status_t status = config::load(fd, &handler);

        // Close file
        fd->close();
        delete fd;

        return status;
    }

    bool plugin_ui::apply_changes(const char *key, const char *value, cvector<CtlPort> &ports, bool preset)
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
            if (!::strcmp(meta->id, key))
                return set_port_value(p, value, (preset) ? PF_PRESET_IMPORT : PF_STATE_IMPORT);
        }
        return false;
    }

    static int port_cmp(const void *va, const void *vb)
    {
        const CtlPort *const *a = reinterpret_cast<const CtlPort *const *>(va);
        const CtlPort *const *b = reinterpret_cast<const CtlPort *const *>(vb);

        const port_t *am    = (*a)->metadata();
        const port_t *bm    = (*b)->metadata();

        return ::strcmp(am->id, bm->id);
    }

    size_t plugin_ui::rebuild_sorted_ports()
    {
        size_t count = vPorts.size();
        vSortedPorts.clear();

        for (size_t i=0; i<count; ++i)
            vSortedPorts.add(vPorts.at(i));

        if (count <= 1)
            return count;

        count           = vSortedPorts.size();
        CtlPort **vp    = vSortedPorts.get_array();

        // Sort by port's ID
        ::qsort(vp, count, sizeof(CtlPort *), port_cmp);

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

        // Check that port name contains "time:" prefix
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

        // Look to custom ports
        for (size_t i=0, n=vCustomPorts.size(); i<n; ++i)
        {
            CtlPort *p = vCustomPorts.get(i);
            const port_t *ctl = (p != NULL) ? p->metadata() : NULL;
            if ((ctl != NULL) && (!strcmp(ctl->id, name)))
                return p;
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
