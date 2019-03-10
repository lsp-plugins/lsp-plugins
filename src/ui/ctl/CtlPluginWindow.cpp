/*
 * CtlPluginWindow.cpp
 *
 *  Created on: 27 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

#include <ui/ui.h>
#include <ui/plugin_ui.h>
#include <metadata/ports.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlPluginWindow::CtlPluginWindow(plugin_ui *src, LSPWindow *wnd):
            CtlWidget(src, wnd)
        {
            pWnd        = wnd;
            pMessage    = NULL;
            bResizable  = true;
            nVisible    = 1;
            pUI         = src;
            pBox        = NULL;
            pMenu       = NULL;
            pImport     = NULL;
            pExport     = NULL;
            pPMStud     = NULL;
            pPVersion   = NULL;
            pPBypass    = NULL;
            pPath       = NULL;
        }
        
        CtlPluginWindow::~CtlPluginWindow()
        {
            size_t n    = vWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPWidget *w = vWidgets.at(i);
                if (w == NULL)
                    continue;
                w->destroy();
                delete w;
            }
            vWidgets.flush();
        }

        status_t CtlPluginWindow::slot_window_close(LSPWidget *sender, void *ptr, void *data)
        {
            LSPDisplay *dpy = static_cast<LSPDisplay *>(ptr);
            dpy->quit_main();
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_window_show(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            __this->show_notification();
            return STATUS_OK;
        }

        void CtlPluginWindow::set(widget_attribute_t att, const char *value)
        {
            switch (att)
            {
                case A_RESIZABLE:
                    PARSE_BOOL(value, bResizable = __);
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }

        void CtlPluginWindow::init()
        {
            CtlWidget::init();

            // Bind ports
            BIND_PORT(pRegistry, pPMStud, MSTUD_PORT);
            BIND_PORT(pRegistry, pPVersion, VERSION_PORT);
            BIND_PORT(pRegistry, pPath, CONFIG_PATH_PORT);
            BIND_PORT(pRegistry, pPBypass, PORT_NAME_BYPASS);

            const plugin_metadata_t *meta   = pUI->metadata();

            // Initialize window
            LSPDisplay *dpy = pWnd->display();

            pWnd->set_title(meta->name);
            pWnd->set_policy(WP_GREEDY);
            if (!pWnd->nested())
                pWnd->actions()->deny_actions(WA_RESIZE);

            {
                // Initialize menu
                pMenu = new LSPMenu(dpy);
                vWidgets.add(pMenu);
                pMenu->init();

                    // Initialize menu items
                    LSPMenuItem *itm = new LSPMenuItem(dpy);
                    vWidgets.add(itm);
                    itm->init();
                    itm->set_text("Export settings...");
                    itm->slots()->bind(LSPSLOT_SUBMIT, slot_export_settings, this);
                    pMenu->add(itm);

                    itm     = new LSPMenuItem(dpy);
                    vWidgets.add(itm);
                    itm->init();
                    itm->set_text("Import settings...");
                    itm->slots()->bind(LSPSLOT_SUBMIT, slot_import_settings, this);
                    pMenu->add(itm);

                    itm     = new LSPMenuItem(dpy);
                    vWidgets.add(itm);
                    itm->init();
                    itm->set_text("Toggle rack mount");
                    itm->slots()->bind(LSPSLOT_SUBMIT, slot_toggle_rack_mount, this);
                    pMenu->add(itm);

                // Initialize main grid
                LSPGrid *grd = new LSPGrid(dpy);
                vWidgets.add(grd);
                pWnd->add(grd);
                grd->init();
                grd->set_rows(2);
                grd->set_columns((pPBypass != NULL) ? 4 : 3);

                    // Initialize mount studs
                    char buf[80];
                    sprintf(buf, "%s  %s", LSP_ACRONYM, meta->acronym);

                    LSPMountStud *mstud = new LSPMountStud(dpy);
                    vMStud[0] = mstud;
                    vWidgets.add(mstud);
                    mstud->init();
                    mstud->set_angle(2);
                    mstud->set_text(buf);
                    mstud->font()->set_size(12);
                    mstud->slots()->bind(LSPSLOT_SUBMIT, slot_show_menu_top, this);
                    grd->add(mstud, 1, (pPBypass != NULL) ? 4 : 3);

                    mstud   = new LSPMountStud(dpy);
                    vMStud[1] = mstud;
                    vWidgets.add(mstud);
                    mstud->init();
                    mstud->set_angle(1);
                    mstud->set_text(LSP_ACRONYM);
                    mstud->font()->set_size(16);
                    mstud->slots()->bind(LSPSLOT_SUBMIT, slot_show_menu_left, this);
                    grd->add(mstud);

                    if (pPBypass != NULL)
                    {
                        LSPBox     *box = new LSPBox(dpy, false);
                        vWidgets.add(box);
                        box->init();
                        box->set_fill(false);
                        box->padding()->set_all(4);
                        grd->add(box);

                        LSPLabel   *lbl = new LSPLabel(dpy);
                        vWidgets.add(lbl);
                        lbl->init();
                        lbl->set_text("Bypass");
                        box->add(lbl);

                        LSPSwitch  *sw  = new LSPSwitch(dpy);
                        vWidgets.add(sw);
                        sw->init();
                        sw->set_size(24);
                        sw->set_angle(1);
                        box->add(sw);

                        LSPLed *led = new LSPLed(dpy);
                        vWidgets.add(led);
                        led->init();
                        init_color(C_RED, led->color());
                        led->set_size(8);
                        box->add(led);

                        CtlWidget *ctl = new CtlSwitch(pRegistry, sw);
                        ctl->init();
                        ctl->set("id", pPBypass->metadata()->id);
                        ctl->begin();
                        ctl->end();
                        pRegistry->add_widget(ctl);

                        ctl = new CtlLed(pRegistry, led);
                        ctl->init();
                        ctl->set("id", pPBypass->metadata()->id);
                        ctl->begin();
                        ctl->end();
                        pRegistry->add_widget(ctl);
                    }

                    pBox    = new LSPBox(dpy);
                    vWidgets.add(pBox);
                    pBox->init();
                    pBox->set_expand(true);
                    pBox->set_fill(true);
                    pBox->padding()->set_all(2);
                    grd->add(pBox);

                    mstud   = new LSPMountStud(dpy);
                    vMStud[2] = mstud;
                    vWidgets.add(mstud);
                    mstud->init();
                    mstud->set_angle(0);
                    mstud->set_text(meta->acronym);
                    mstud->font()->set_size(16);
                    mstud->slots()->bind(LSPSLOT_SUBMIT, slot_show_menu_right, this);
                    grd->add(mstud);
            }

            // Bind close handler
            pWnd->slots()->bind(LSPSLOT_CLOSE, slot_window_close, pWidget->display());
            pWnd->slots()->bind(LSPSLOT_SHOW, slot_window_show, this);
        }

        void CtlPluginWindow::end()
        {
            // Check widget pointer
            if (pWidget != NULL)
            {
                // Update window geometry
                LSPWindow *wnd  = static_cast<LSPWindow *>(pWidget);
                wnd->set_min_size(nMinWidth, nMinHeight);
                wnd->set_border_style((bResizable) ? BS_SIZABLE : BS_SINGLE);
            }

            if (pPMStud != NULL)
                notify(pPMStud);

            if (!pWnd->nested())
            {
                size_request_t r;
                pWnd->size_request(&r);

                LSPDisplay *dpy = pWnd->display();
                if (dpy != NULL)
                {
                    ssize_t w, h;
                    if (dpy->screen_size(pWnd->screen(), &w, &h) == STATUS_OK)
                    {
                        w = (w - r.nMinWidth) >> 1;
                        h = (h - r.nMinHeight) >> 1;
                        pWnd->move(w, h);
                    }
                }
            }


            // Call for parent class method
            CtlWidget::end();
        }

        void CtlPluginWindow::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (port == pPMStud)
            {
                bool top    = pPMStud->get_value() < 0.5f;
                vMStud[0]->set_visible(top);
                vMStud[1]->set_visible(!top);
                vMStud[2]->set_visible(!top);
            }
        }

        status_t CtlPluginWindow::add(LSPWidget *child)
        {
            // Check widget pointer
            if (pBox == NULL)
                return STATUS_BAD_STATE;

            return pBox->add(child);
        }

        status_t CtlPluginWindow::slot_export_settings(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            LSPFileDialog *dlg = __this->pExport;
            if (dlg == NULL)
            {
                dlg = new LSPFileDialog(__this->pWnd->display());
                __this->vWidgets.add(dlg);
                __this->pExport = dlg;

                dlg->init();
                dlg->set_mode(FDM_SAVE_FILE);
                dlg->set_title("Export settings");
                dlg->set_action_title("Save");
                dlg->set_confirmation("The selected file already exists. Overwrite?");

                LSPFileFilter *f = dlg->filter();
                f->add("*.cfg", "LSP plugin configuration file (*.cfg)", ".cfg");
                f->add("*", "All files (*.*)", "");
                dlg->bind_action(slot_call_export_settings, ptr);
                dlg->slots()->bind(LSPSLOT_SHOW, slot_fetch_path, __this);
                dlg->slots()->bind(LSPSLOT_HIDE, slot_commit_path, __this);
            }

            return dlg->show(__this->pWnd);
        }

        status_t CtlPluginWindow::slot_import_settings(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            LSPFileDialog *dlg = __this->pImport;
            if (dlg == NULL)
            {
                dlg = new LSPFileDialog(__this->pWnd->display());
                __this->vWidgets.add(dlg);
                __this->pImport = dlg;

                dlg->init();
                dlg->set_mode(FDM_OPEN_FILE);
                dlg->set_title("Import settings");
                dlg->set_action_title("Open");

                LSPFileFilter *f = dlg->filter();
                f->add("*.cfg", "Configuration file (*.cfg)", ".cfg");
                f->add("*", "All files (*.*)", "");
                dlg->bind_action(slot_call_import_settings, ptr);
                dlg->slots()->bind(LSPSLOT_SHOW, slot_fetch_path, __this);
                dlg->slots()->bind(LSPSLOT_HIDE, slot_commit_path, __this);
            }

            return dlg->show(__this->pWnd);
        }

        status_t CtlPluginWindow::slot_toggle_rack_mount(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            CtlPort *mstud = __this->pPMStud;
            if (mstud != NULL)
            {
                bool x = mstud->get_value() >= 0.5f;
                mstud->set_value((x) ? 0.0f : 1.0f);
                mstud->notify_all();
            }

            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_show_menu_top(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            return __this->show_menu(0, data);
        }

        status_t CtlPluginWindow::slot_show_menu_left(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            return __this->show_menu(1, data);
        }

        status_t CtlPluginWindow::slot_show_menu_right(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            return __this->show_menu(2, data);
        }

        status_t CtlPluginWindow::show_menu(size_t actor_id, void *data)
        {
            LSPWidget *actor = vMStud[actor_id];
            if (data != NULL)
            {
                ws_event_t *ev = static_cast<ws_event_t *>(data);
                return pMenu->show(actor, ev);
            }

            return pMenu->show(actor);
        }

        status_t CtlPluginWindow::slot_call_export_settings(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            __this->pUI->export_settings(__this->pExport->selected_file());
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_call_import_settings(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            __this->pUI->import_settings(__this->pImport->selected_file());
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_message_close(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            if (__this->pMessage != NULL)
                __this->pMessage->hide();
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_fetch_path(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *_this = static_cast<CtlPluginWindow *>(ptr);
            if ((_this == NULL) || (_this->pPath == NULL))
                return STATUS_BAD_STATE;

            LSPFileDialog *dlg = widget_cast<LSPFileDialog>(sender);
            if (dlg == NULL)
                return STATUS_OK;

            dlg->set_path(_this->pPath->get_buffer<char>());
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_commit_path(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *_this = static_cast<CtlPluginWindow *>(ptr);
            if ((_this == NULL) || (_this->pPath == NULL))
                return STATUS_BAD_STATE;

            LSPFileDialog *dlg = widget_cast<LSPFileDialog>(sender);
            if (dlg == NULL)
                return STATUS_OK;

            const char *path = dlg->path();
            if (path != NULL)
            {
                _this->pPath->write(path, strlen(path));
                _this->pPath->notify_all();
            }

            return STATUS_OK;
        }

        LSPLabel *CtlPluginWindow::create_label(LSPWidgetContainer *dst, const char *text, float halign)
        {
            LSPLabel *lbl = new LSPLabel(pUI->display());
            lbl->init();
            vWidgets.add(lbl);
            dst->add(lbl);
            lbl->set_text(text);
            lbl->set_fill(true);
            lbl->set_align(halign, 0.5f);
            return lbl;
        }

        LSPHyperlink *CtlPluginWindow::create_hlink(LSPWidgetContainer *dst, const char *text, float halign)
        {
            LSPAlign *algn = new LSPAlign(pUI->display());
            algn->init();
            vWidgets.add(algn);
            algn->set_hpos(halign);
//            algn->set_border(0);
            dst->add(algn);

            LSPHyperlink *hlink = new LSPHyperlink(pUI->display());
            hlink->init();
            vWidgets.add(hlink);
            algn->add(hlink);
            hlink->set_url(text);
            hlink->set_text(text);
            return hlink;
        }

        status_t CtlPluginWindow::show_notification()
        {
            // Check that we really need to show notification window
            if (pPVersion != NULL)
            {
                const char *v = pPVersion->get_buffer<char>();
                if ((v != NULL) && (strcmp(LSP_MAIN_VERSION, v) == 0))
                    return STATUS_OK;

                pPVersion->write(LSP_MAIN_VERSION, strlen(LSP_MAIN_VERSION));
                pPVersion->notify_all();
            }

            lsp_trace("Showing notification dialog");

            if (pMessage == NULL)
            {
                pMessage = new LSPWindow(pUI->display());
                if (pMessage == NULL)
                    return STATUS_NO_MEM;
                vWidgets.add(pMessage);
                pMessage->init();
                pMessage->set_border_style(BS_DIALOG);
                pMessage->set_title("Update notification");
                pMessage->actions()->deny_all();
                pMessage->actions()->set_closeable(true);
                pMessage->padding()->set_all(16);

                LSPBox *vbox = new LSPBox(pUI->display(), false);
                vbox->init();
                vbox->set_spacing(8);
                vWidgets.add(vbox);
                pMessage->add(vbox);

                LSPLabel *lbl  = create_label(vbox, "Greetings!");
//                lbl->bg_color()->set_rgb(1.0f, 0.0f, 0.0f);
                lbl->font()->set_size(24);
                lbl->font()->set_bold();

                lbl  = create_label(vbox, "You've just updated plugins to version " LSP_MAIN_VERSION "!");
                lbl->font()->set_bold();

                lbl  = create_label(vbox, "The " LSP_FULL_NAME " is non-commercial project and needs financial support for the further development.");
                lbl  = create_label(vbox, "You may help all plugins become open source by visiting the following link and submitting donations to the project:");
                create_hlink(vbox, LSP_DONATION_URI, 0.02);

                lbl  = create_label(vbox, "You can find more information about policy of publishing source code by visiting the following link:");
                create_hlink(vbox, LSP_DOWNLOAD_URI, 0.02);

                lbl  = create_label(vbox, "Remember that subscription and regular small donations will give more benefits to the project than one-time donations.");
                lbl  = create_label(vbox, "To not to be very annoying, this dialog will be shown only after each version update of plugins.");

                lbl  = create_label(vbox, "Thanks in advance", 1.0f);
                lbl  = create_label(vbox, LSP_FULL_NAME, 1.0f);
                create_hlink(vbox, LSP_BASE_URI, 1.0f);

                LSPAlign *algn = new LSPAlign(pUI->display());
                algn->init();
                algn->set_fill(true);
                vWidgets.add(algn);
                vbox->add(algn);

                LSPButton *btn = new LSPButton(pUI->display());
                btn->init();
                vWidgets.add(btn);
                algn->add(btn);
                btn->set_min_width(96);
                btn->set_title("Close");

                // Bind slots
                btn->slots()->bind(LSPSLOT_SUBMIT, slot_message_close, this);
                pMessage->slots()->bind(LSPSLOT_CLOSE, slot_message_close, this);
            }

            return pMessage->show(pWnd);
        }
    
    } /* namespace ctl */
} /* namespace lsp */
