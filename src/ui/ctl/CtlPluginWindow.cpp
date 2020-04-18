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
        const ctl_class_t CtlPluginWindow::metadata = { "CtlPluginWindow", &CtlWidget::metadata };
        
        CtlPluginWindow::CtlPluginWindow(plugin_ui *src, LSPWindow *wnd):
            CtlWidget(src, wnd)
        {
            pClass          = &metadata;
            pWnd            = wnd;
            pMessage        = NULL;
            bResizable      = false;
            nVisible        = 1;
            pUI             = src;
            pBox            = NULL;
            pMenu           = NULL;
            pImport         = NULL;
            pExport         = NULL;
            pPMStud         = NULL;
            pPVersion       = NULL;
            pPBypass        = NULL;
            pPath           = NULL;
            pR3DBackend     = NULL;
            pLanguage       = NULL;
            nVisible        = 0;
        }
        
        CtlPluginWindow::~CtlPluginWindow()
        {
            for (size_t i=0, n=vWidgets.size(); i<n; ++i)
            {
                LSPWidget *w = vWidgets.at(i);
                if (w == NULL)
                    continue;
                w->destroy();
                delete w;
            }

            for (size_t i=0, n=vLangSel.size(); i<n; ++i)
            {
                lang_sel_t *s = vLangSel.at(i);
                if (s != NULL)
                    delete s;
            }

            vWidgets.flush();
            vBackendSel.flush();
            vLangSel.flush();
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
            BIND_PORT(pRegistry, pR3DBackend, R3D_BACKEND_PORT);
            BIND_PORT(pRegistry, pLanguage, LANGUAGE_PORT);

            const plugin_metadata_t *meta   = pUI->metadata();

            // Initialize window
            LSPDisplay *dpy = pWnd->display();

            pWnd->set_class(meta->lv2_uid, LSP_ARTIFACT_ID);
            pWnd->set_role("audio-plugin");
            pWnd->title()->set_raw(meta->name);

            if (!pWnd->nested())
                pWnd->actions()->deny_actions(WA_RESIZE);

            {
                // Initialize menu
                pMenu = new LSPMenu(dpy);
                pMenu->set_unique_id(WUID_MAIN_MENU);
                vWidgets.add(pMenu);
                pMenu->init();

                // Initialize menu items
                {
                    // Create export menu
                    LSPMenu *submenu = new LSPMenu(dpy);
                    vWidgets.add(submenu);
                    submenu->init();
                    submenu->set_unique_id(WUID_EXPORT_MENU);

                    LSPMenuItem *itm = new LSPMenuItem(dpy);
                    vWidgets.add(itm);
                    itm->init();
                    itm->text()->set("actions.export");
                    itm->set_submenu(submenu);
                    pMenu->add(itm);

                    // Create export menu items
                    {
                        LSPMenuItem *child = new LSPMenuItem(dpy);
                        vWidgets.add(child);
                        child->init();
                        child->text()->set("actions.export_settings_to_file");
                        child->slots()->bind(LSPSLOT_SUBMIT, slot_export_settings_to_file, this);
                        submenu->add(child);

                        child = new LSPMenuItem(dpy);
                        vWidgets.add(child);
                        child->init();
                        child->text()->set("actions.export_settings_to_clipboard");
                        child->slots()->bind(LSPSLOT_SUBMIT, slot_export_settings_to_clipboard, this);
                        submenu->add(child);
                    }

                    // Create import menu
                    submenu = new LSPMenu(dpy);
                    vWidgets.add(submenu);
                    submenu->init();
                    submenu->set_unique_id(WUID_IMPORT_MENU);

                    itm = new LSPMenuItem(dpy);
                    vWidgets.add(itm);
                    itm->init();
                    itm->text()->set("actions.import");
                    itm->set_submenu(submenu);
                    pMenu->add(itm);

                    // Create import menu items
                    {
                        LSPMenuItem *child = new LSPMenuItem(dpy);
                        vWidgets.add(child);
                        child->init();
                        child->text()->set("actions.import_settings_from_file");
                        child->slots()->bind(LSPSLOT_SUBMIT, slot_import_settings_from_file, this);
                        submenu->add(child);

                        child = new LSPMenuItem(dpy);
                        vWidgets.add(child);
                        child->init();
                        child->text()->set("actions.import_settings_from_clipboard");
                        child->slots()->bind(LSPSLOT_SUBMIT, slot_import_settings_from_clipboard, this);
                        submenu->add(child);
                    }

                    // Add separator
                    itm     = new LSPMenuItem(dpy);
                    vWidgets.add(itm);
                    itm->init();
                    itm->set_separator(true);
                    pMenu->add(itm);

                    // Create 'Toggle rack mount' menu item
                    itm     = new LSPMenuItem(dpy);
                    vWidgets.add(itm);
                    itm->init();
                    itm->text()->set("actions.toggle_rack_mount");
                    itm->slots()->bind(LSPSLOT_SUBMIT, slot_toggle_rack_mount, this);
                    pMenu->add(itm);

                    // Create language selection menu
                    init_i18n_support(pMenu);

                    // Add support of 3D rendering backend switch
                    if (meta->extensions & E_3D_BACKEND)
                        init_r3d_support(pMenu);
                }

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
                        lbl->text()->set("labels.bypass");
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

        status_t CtlPluginWindow::init_i18n_support(LSPMenu *menu)
        {
            if (menu == NULL)
                return STATUS_OK;

            LSPDisplay *dpy   = menu->display();
            if (dpy == NULL)
                return STATUS_OK;

            IDictionary *dict = dpy->dictionary();
            if (dict == NULL)
                return STATUS_OK;

            // Perform lookup before loading list of languages
            status_t res = dict->lookup("default.lang.target", &dict);
            if (res != STATUS_OK)
                return res;

            // Create submenu item
            LSPMenuItem *root       = new LSPMenuItem(menu->display());
            if (root == NULL)
                return STATUS_NO_MEM;
            if ((res = root->init()) != STATUS_OK)
            {
                delete root;
                return res;
            }
            if (!vWidgets.add(root))
            {
                root->destroy();
                delete root;
                return STATUS_NO_MEM;
            }
            root->text()->set("actions.select_language");
            if ((res = menu->add(root)) != STATUS_OK)
                return res;

            // Create submenu
            menu                = new LSPMenu(menu->display());
            if (menu == NULL)
                return STATUS_NO_MEM;
            if ((res = menu->init()) != STATUS_OK)
            {
                menu->destroy();
                delete menu;
                return res;
            }
            if (!vWidgets.add(menu))
            {
                menu->destroy();
                delete menu;
                return STATUS_NO_MEM;
            }
            root->set_submenu(menu);

            // Iterate all children and add language keys
            LSPString key, value;
            lang_sel_t *lang;
            size_t added = 0;
            for (size_t i=0, n=dict->size(); i<n; ++i)
            {
                // Fetch placeholder for language selection key
                if ((res = dict->get_value(i, &key, &value)) != STATUS_OK)
                {
                    // Skip nested dictionaries
                    if (res == STATUS_BAD_TYPE)
                        continue;
                    return res;
                }
                if ((lang = new lang_sel_t()) == NULL)
                    return STATUS_NO_MEM;
                if (!lang->lang.set(&key))
                {
                    delete lang;
                    return STATUS_NO_MEM;
                }
                if (!vLangSel.add(lang))
                {
                    delete lang;
                    return STATUS_NO_MEM;
                }
                lang->ctl   = this;

                // Create menu item
                LSPMenuItem *item = new LSPMenuItem(menu->display());
                if (item == NULL)
                    continue;
                if ((res = item->init()) != STATUS_OK)
                {
                    item->destroy();
                    delete item;
                    continue;
                }
                if (!vWidgets.add(item))
                {
                    item->destroy();
                    delete item;
                    continue;
                }

                item->text()->set_raw(&value);
                menu->add(item);

                // Create closure and bind
                item->slots()->bind(LSPSLOT_SUBMIT, slot_select_language, lang);

                ++added;
            }

            // Set menu item visible only if there are available languages
            root->set_visible(added > 0);
            if (pLanguage != NULL)
            {
                const char *lang = pLanguage->get_buffer<char>();
                ui_atom_t atom = dpy->atom_id("language");

                if ((lang != NULL) && (strlen(lang) > 0) && (atom >= 0))
                {
                    LSPTheme *theme = dpy->theme();
                    LSPStyle *style = (theme != NULL) ? theme->root() : NULL;
                    if (style != NULL)
                    {
                        lsp_trace("System language set to: %s", lang);
                        style->set_string(atom, lang);
                    }
                }
            }

            return STATUS_OK;
        }

        status_t CtlPluginWindow::init_r3d_support(LSPMenu *menu)
        {
            if (menu == NULL)
                return STATUS_OK;

            IDisplay *dpy   = menu->display()->display();
            if (dpy == NULL)
                return STATUS_OK;

            status_t res;

            // Create submenu item
            LSPMenuItem *item       = new LSPMenuItem(menu->display());
            if (item == NULL)
                return STATUS_NO_MEM;
            if ((res = item->init()) != STATUS_OK)
            {
                delete item;
                return res;
            }
            if (!vWidgets.add(item))
            {
                item->destroy();
                delete item;
                return STATUS_NO_MEM;
            }

            // Add item to the main menu
            item->text()->set("actions.3d_rendering");
            menu->add(item);

            // Get backend port
            const char *backend = (pR3DBackend != NULL) ? pR3DBackend->get_buffer<char>() : NULL;

            // Create submenu
            menu                = new LSPMenu(menu->display());
            if (menu == NULL)
                return STATUS_NO_MEM;
            if ((res = menu->init()) != STATUS_OK)
            {
                menu->destroy();
                delete menu;
                return res;
            }
            if (!vWidgets.add(menu))
            {
                menu->destroy();
                delete menu;
                return STATUS_NO_MEM;
            }
            item->set_submenu(menu);

            for (size_t id=0; ; ++id)
            {
                // Enumerate next backend information
                const R3DBackendInfo *info = dpy->enumBackend(id);
                if (info == NULL)
                    break;

                // Create menu item
                item       = new LSPMenuItem(menu->display());
                if (item == NULL)
                    continue;
                if ((res = item->init()) != STATUS_OK)
                {
                    item->destroy();
                    delete item;
                    continue;
                }
                if (!vWidgets.add(item))
                {
                    item->destroy();
                    delete item;
                    continue;
                }

                item->text()->set_raw(&info->display);
                menu->add(item);

                // Create closure and bind
                backend_sel_t *sel = vBackendSel.add();
                if (sel != NULL)
                {
                    sel->ctl    = this;
                    sel->item   = item;
                    sel->id     = id;
                    item->slots()->bind(LSPSLOT_SUBMIT, slot_select_backend, sel);
                }

                // Backend identifier matches?
                if ((backend == NULL) || (!info->uid.equals_ascii(backend)))
                {
                    slot_select_backend(item, sel, NULL);
                    if (backend == NULL)
                        backend     = info->uid.get_ascii();
                }
            }

            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_select_backend(LSPWidget *sender, void *ptr, void *data)
        {
            backend_sel_t *sel = reinterpret_cast<backend_sel_t *>(ptr);
            if ((sender == NULL) || (sel == NULL) || (sel->ctl == NULL))
                return STATUS_BAD_ARGUMENTS;

            IDisplay *dpy = sender->display()->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            const R3DBackendInfo *info = dpy->enumBackend(sel->id);
            if (info == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Mark backend as selected
            dpy->selectBackendId(sel->id);

            // Need to commit backend identifier to config file?
            const char *value = info->uid.get_ascii();
            if (value == NULL)
                return STATUS_NO_MEM;

            if (sel->ctl->pR3DBackend != NULL)
            {
                const char *backend = sel->ctl->pR3DBackend->get_buffer<char>();
                if ((backend == NULL) || (strcmp(backend, value)))
                {
                    sel->ctl->pR3DBackend->write(value, strlen(value));
                    sel->ctl->pR3DBackend->notify_all();
                }
            }

            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_select_language(LSPWidget *sender, void *ptr, void *data)
        {
            lang_sel_t *sel = reinterpret_cast<lang_sel_t *>(ptr);
            lsp_trace("sender=%p, sel=%p", sender, sel);
            if ((sender == NULL) || (sel == NULL) || (sel->ctl == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPDisplay *dpy = sender->display();
            lsp_trace("dpy = %p", dpy);
            if (dpy == NULL)
                return STATUS_BAD_STATE;
            ui_atom_t atom = dpy->atom_id("language");
            lsp_trace("atom = %d", int(atom));
            if (atom < 0)
                return STATUS_BAD_STATE;

            LSPTheme *theme = dpy->theme();
            lsp_trace("theme = %p", theme);
            if (theme == NULL)
                return STATUS_BAD_STATE;
            LSPStyle *style = theme->root();
            lsp_trace("style = %p", style);
            if (style == NULL)
                return STATUS_BAD_STATE;

            const char *dlang = sel->lang.get_utf8();
            lsp_trace("Select language: \"%s\"", dlang);
            status_t res = style->set_string(atom, &sel->lang);
            lsp_trace("Updated style: %d", int(res));
            if ((res == STATUS_OK) && (sel->ctl->pLanguage != NULL))
            {
                const char *slang = sel->ctl->pLanguage->get_buffer<char>();
                lsp_trace("Current language: \"%s\"", slang);
                if ((slang == NULL) || (strcmp(slang, dlang)))
                {
                    lsp_trace("Write and notify: \"%s\"", dlang);
                    sel->ctl->pLanguage->write(dlang, strlen(dlang));
                    sel->ctl->pLanguage->notify_all();
                }
            }

            lsp_trace("Language has been selected");

            return STATUS_OK;
        }

        void CtlPluginWindow::end()
        {
            // Check widget pointer
            if (pWidget != NULL)
            {
                // Update window geometry
                LSPWindow *wnd  = widget_cast<LSPWindow>(pWidget);
//                wnd->set_min_size(nMinWidth, nMinHeight);
                wnd->set_border_style((bResizable) ? BS_SIZABLE : BS_SINGLE);
                wnd->actions()->set_resizable(bResizable);
                wnd->actions()->set_maximizable(bResizable);
            }

            if (pPMStud != NULL)
                notify(pPMStud);

            pWnd->set_policy((bResizable) ? WP_NORMAL : WP_GREEDY);

//            if (!pWnd->nested())
//            {
//                size_request_t r;
//                pWnd->size_request(&r);
//                pWnd->resize(r.nMinWidth, r.nMinHeight);
//
//                LSPDisplay *dpy = pWnd->display();
//                if (dpy != NULL)
//                {
//                    ssize_t w, h;
//                    if (dpy->screen_size(pWnd->screen(), &w, &h) == STATUS_OK)
//                    {
//                        w = (w - r.nMinWidth) >> 1;
//                        h = (h - r.nMinHeight) >> 1;
//                        pWnd->move(w, h);
//                    }
//                }
//            }


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

        LSPWidget *CtlPluginWindow::resolve(const char *uid)
        {
            for (size_t i=0,n=vWidgets.size(); i<n; ++i)
            {
                LSPWidget *widget = vWidgets.get(i);
                if (widget == NULL)
                    continue;
                const char *wuid = widget->unique_id();
                if ((wuid != NULL) && (!strcmp(wuid, uid)))
                    return widget;
            }

            return CtlWidget::resolve(uid);
        }

        status_t CtlPluginWindow::add(CtlWidget *child)
        {
            // Check widget pointer
            if (pBox == NULL)
                return STATUS_BAD_STATE;

            return pBox->add(child->widget());
        }

        status_t CtlPluginWindow::slot_export_settings_to_file(LSPWidget *sender, void *ptr, void *data)
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
                dlg->title()->set("titles.export_settings");
                dlg->action_title()->set("actions.save");
                dlg->set_use_confirm(true);
                dlg->confirm()->set("messages.file.confirm_overwrite");

                LSPFileFilter *f = dlg->filter();
                {
                    LSPFileFilterItem ffi;

                    ffi.pattern()->set("*.cfg");
                    ffi.title()->set("files.config.lsp");
                    ffi.set_extension(".cfg");
                    f->add(&ffi);

                    ffi.pattern()->set("*");
                    ffi.title()->set("files.all");
                    ffi.set_extension("");
                    f->add(&ffi);
                }
                dlg->bind_action(slot_call_export_settings_to_file, ptr);
                dlg->slots()->bind(LSPSLOT_SHOW, slot_fetch_path, __this);
                dlg->slots()->bind(LSPSLOT_HIDE, slot_commit_path, __this);
            }

            return dlg->show(__this->pWnd);
        }

        status_t CtlPluginWindow::slot_import_settings_from_file(LSPWidget *sender, void *ptr, void *data)
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
                dlg->title()->set("titles.import_settings");
                dlg->action_title()->set("actions.open");

                LSPFileFilter *f = dlg->filter();
                {
                    LSPFileFilterItem ffi;

                    ffi.pattern()->set("*.cfg");
                    ffi.title()->set("files.config.lsp");
                    ffi.set_extension(".cfg");
                    f->add(&ffi);

                    ffi.pattern()->set("*");
                    ffi.title()->set("files.all");
                    ffi.set_extension("");
                    f->add(&ffi);
                }
                dlg->bind_action(slot_call_import_settings_to_file, ptr);
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

        status_t CtlPluginWindow::slot_call_export_settings_to_file(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            __this->pUI->export_settings(__this->pExport->selected_file());
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_call_import_settings_to_file(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            __this->pUI->import_settings(__this->pImport->selected_file(), false);
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_export_settings_to_clipboard(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            __this->pUI->export_settings_to_clipboard();
            return STATUS_OK;
        }

        status_t CtlPluginWindow::slot_import_settings_from_clipboard(LSPWidget *sender, void *ptr, void *data)
        {
            CtlPluginWindow *__this = static_cast<CtlPluginWindow *>(ptr);
            __this->pUI->import_settings_from_clipboard();
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

        LSPLabel *CtlPluginWindow::create_label(LSPWidgetContainer *dst, const char *key, float halign)
        {
            LSPLabel *lbl = new LSPLabel(pUI->display());
            lbl->init();
            vWidgets.add(lbl);
            dst->add(lbl);
            lbl->text()->set(key);
            lbl->set_fill(true);
            lbl->set_align(halign, 0.5f);
            return lbl;
        }

        LSPLabel *CtlPluginWindow::create_plabel(LSPWidgetContainer *dst, const char *key, const calc::Parameters *params, const float halign)
        {
            LSPLabel *lbl = new LSPLabel(pUI->display());
            lbl->init();
            vWidgets.add(lbl);
            dst->add(lbl);
            lbl->text()->set(key, params);
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
            hlink->text()->set_raw(text);
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
                pMessage->title()->set("titles.update_notification");
                pMessage->actions()->deny_all();
                pMessage->actions()->set_closeable(true);
                pMessage->padding()->set_all(16);

                LSPBox *vbox = new LSPBox(pUI->display(), false);
                vbox->init();
                vbox->set_spacing(8);
                vWidgets.add(vbox);
                pMessage->add(vbox);

                calc::Parameters p;

                LSPLabel *lbl  = create_label(vbox, "headings.greetings");
                lbl->font()->set_size(24);
                lbl->font()->set_bold();

                p.clear();
                p.set_cstring("version", LSP_MAIN_VERSION);
                lbl  = create_plabel(vbox, "messages.greetings.0", &p);
                lbl->font()->set_bold();

                p.clear();
                p.set_cstring("project", LSP_FULL_NAME);
                lbl  = create_plabel(vbox, "messages.greetings.1", &p);
                lbl  = create_label(vbox, "messages.greetings.2");
                create_hlink(vbox, LSP_DONATION_URI1, 0.02);
                create_hlink(vbox, LSP_DONATION_URI2, 0.02);

                lbl  = create_label(vbox, "messages.greetings.3");
                lbl  = create_label(vbox, "messages.greetings.4");

                lbl  = create_label(vbox, "messages.greetings.5", 1.0f);
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
                btn->title()->set("actions.close");

                // Bind slots
                btn->slots()->bind(LSPSLOT_SUBMIT, slot_message_close, this);
                pMessage->slots()->bind(LSPSLOT_CLOSE, slot_message_close, this);
            }

            return pMessage->show(pWnd);
        }
    
    } /* namespace ctl */
} /* namespace lsp */
