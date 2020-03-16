/*
 * LSPFileDialog.cpp
 *
 *  Created on: 29 сент. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <core/io/Dir.h>
#include <core/system.h>
#include <unistd.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPFileDialog::metadata = { "LSPFileDialog", &LSPWindow::metadata };

        //---------------------------------------------------------------------
        LSPFileDialog::LSPFileDialogFilter::LSPFileDialogFilter(LSPFileDialog *dlg)
        {
            pDialog     = dlg;
        }

        LSPFileDialog::LSPFileDialogFilter::~LSPFileDialogFilter()
        {
        }

        status_t LSPFileDialog::LSPFileDialogFilter::item_updated(size_t idx, LSPFileFilterItem *flt)
        {
            LSPItem *item = pDialog->sWFilter.items()->get(idx);
            return (item != NULL) ? item->text()->set(flt->title()) : STATUS_NOT_FOUND;
        }

        status_t LSPFileDialog::LSPFileDialogFilter::item_removed(size_t idx, LSPFileFilterItem *flt)
        {
            return pDialog->sWFilter.items()->remove(idx);
        }

        status_t LSPFileDialog::LSPFileDialogFilter::item_added(size_t idx, LSPFileFilterItem *flt)
        {
            LSPItem *item = NULL;
            status_t res = pDialog->sWFilter.items()->insert(idx, &item);
            if (res == STATUS_OK)
                res = item->text()->set(flt->title());
            return res;
        }

        void LSPFileDialog::LSPFileDialogFilter::default_updated(ssize_t idx)
        {
            pDialog->sWFilter.set_selected(idx);
        }

        void LSPFileDialog::ConfirmMsg::sync()
        {
            // Propagate message to the confirm dialog
            LSPFileDialog *dlg = widget_cast<LSPFileDialog>(pWidget);
            if ((dlg != NULL) && (dlg->pWConfirm != NULL))
                dlg->pWConfirm->message()->set(this);
        }

        //---------------------------------------------------------------------
        LSPFileDialog::LSPFileDialog(LSPDisplay *dpy):
            LSPWindow(dpy),
            sWPath(dpy),
            sWSearch(dpy),
            sWFilter(dpy),
            sWFiles(dpy),
            sWAction(dpy),
            sWCancel(dpy),
            sMainGrid(dpy),
            sSBBookmarks(dpy),
            sSBAlign(dpy),
            sBookmarks(dpy),
            sBMPopup(dpy),
            sBMAdd(dpy),
            sHBox(dpy),
            sWarnBox(dpy),
            sAppendExt(dpy),
            wAutoExt(dpy),
            wGo(dpy),
            wUp(dpy),
            wPathBox(dpy),
            sWWarning(dpy),
            sConfirm(this),
            sFilter(this)
        {
            pSelBookmark    = NULL;
            pPopupBookmark  = NULL;
            pWConfirm       = NULL;
            nDefaultFilter  = 0;
            nUIDGen         = 0;
            pWSearch        = NULL;
            pWMessage       = NULL;
            pClass          = &metadata;
            enMode          = FDM_OPEN_FILE;
            bUseConfirm     = false;
        }

        LSPFileDialog::~LSPFileDialog()
        {
            do_destroy();
        }

        status_t LSPFileDialog::add_label(LSPWidgetContainer *c, const char *key, float align, LSPLabel **label)
        {
            LSPAlign *algn = new LSPAlign(pDisplay);
            if (algn == NULL)
                return STATUS_NO_MEM;

            LSPLabel *lbl = new LSPLabel(pDisplay);
            if (lbl == NULL)
            {
                delete algn;
                return STATUS_NO_MEM;
            }

            status_t result = (vWidgets.add(lbl)) ? STATUS_OK : STATUS_NO_MEM;
            if (result == STATUS_OK)
                result = (vWidgets.add(algn)) ? STATUS_OK : STATUS_NO_MEM;

            if (result == STATUS_OK)
                result = lbl->init();
            if (result == STATUS_OK)
                result = algn->init();
            algn->set_hpos(align);
            if (result == STATUS_OK)
                result = lbl->text()->set(key);

            if (result == STATUS_OK)
                result = algn->add(lbl);
            if (result == STATUS_OK)
                result = c->add(algn);

            if (result != STATUS_OK)
            {
                vWidgets.remove(lbl);
                vWidgets.remove(algn);
                lbl->destroy();
                delete lbl;
                algn->destroy();
                delete algn;
            }

            if (label != NULL)
                *label = lbl;

            return result;
        }

        status_t LSPFileDialog::add_menu_item(LSPMenu *m, const char *key, ui_event_handler_t handler)
        {
            LSPMenuItem *mi = new LSPMenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            if (!vWidgets.add(mi))
            {
                mi->destroy();
                delete mi;
                return STATUS_NO_MEM;
            }

            LSP_STATUS_ASSERT(mi->init());
            if (key != NULL)
            {
                LSP_STATUS_ASSERT(mi->text()->set(key));
                ui_handler_id_t id = mi->slots()->bind(LSPSLOT_SUBMIT, handler, self());
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;
            }
            else
                mi->set_separator(true);

            LSP_STATUS_ASSERT(m->add(mi));

            return STATUS_OK;
        }

        status_t LSPFileDialog::add_ext_button(LSPWidgetContainer *c, const char *text)
        {
            LSP_STATUS_ASSERT(sAppendExt.init());
            LSP_STATUS_ASSERT(wAutoExt.init());

            LSPLabel *lbl = new LSPLabel(pDisplay);
            if (lbl == NULL)
                return STATUS_NO_MEM;

            LSPBox *box = new LSPBox(pDisplay, true);
            if (box == NULL)
            {
                delete lbl;
                return STATUS_NO_MEM;
            }

            status_t result = (vWidgets.add(lbl)) ? STATUS_OK : STATUS_NO_MEM;
            if (result == STATUS_OK)
                result = (vWidgets.add(box)) ? STATUS_OK : STATUS_NO_MEM;

            if (result == STATUS_OK)
                result = lbl->init();
            if (result == STATUS_OK)
                result = box->init();

            box->set_spacing(4);
            sAppendExt.set_hpos(0.0f);

            if (result == STATUS_OK)
                result = lbl->text()->set_raw(text);
            if (result == STATUS_OK)
                result = sAppendExt.add(box);
            if (result == STATUS_OK)
                result = box->add(&wAutoExt);
            if (result == STATUS_OK)
                result = box->add(lbl);
            if (result == STATUS_OK)
                result = c->add(&sAppendExt);

            if (result != STATUS_OK)
            {
                vWidgets.remove(lbl);
                vWidgets.remove(box);
                lbl->destroy();
                delete lbl;
                box->destroy();
                delete box;
            }

            return result;
        }

        status_t LSPFileDialog::init()
        {
            // Initialize bindings
            sConfirm.bind();

            // Initialize labels
            LSP_STATUS_ASSERT(LSPWindow::init());
            LSP_STATUS_ASSERT(sWPath.init());
            sWPath.set_expand(true);
            LSP_STATUS_ASSERT(sWSearch.init());
            LSP_STATUS_ASSERT(sWFilter.init());
            sWFilter.set_fill(true);
            LSP_STATUS_ASSERT(sWFiles.init());
            sWFiles.constraints()->set_min(400, 320);
            sWFiles.set_expand(true);
            LSP_STATUS_ASSERT(sWAction.init());
            sWAction.set_min_width(96);
            sWAction.set_min_height(24);
            LSP_STATUS_ASSERT(sWCancel.init());
            LSP_STATUS_ASSERT(sWCancel.title()->set("actions.cancel"));
            sWCancel.set_min_width(96);
            sWCancel.set_min_height(24);
            LSP_STATUS_ASSERT(sWWarning.init());
            sWWarning.set_visible(false);
            sWWarning.set_expand(true);
            sWWarning.set_align(1.0f, 0.5f);

            LSP_STATUS_ASSERT(wGo.init());
            LSP_STATUS_ASSERT(wGo.title()->set("actions.nav.go"));
            wGo.set_min_width(32);
            LSP_STATUS_ASSERT(wUp.init());
            LSP_STATUS_ASSERT(wUp.title()->set("actions.nav.up"));
            wUp.set_min_width(32);
            LSP_STATUS_ASSERT(sBMAdd.init());
            LSP_STATUS_ASSERT(sBMAdd.title()->set("actions.to_bookmarks"));
            sBMAdd.set_min_width(32);

            LSP_STATUS_ASSERT(wPathBox.init());
            wPathBox.set_horizontal();
            wPathBox.set_spacing(2);
            wPathBox.set_fill(true);

            LSP_STATUS_ASSERT(sMainGrid.init());
            sMainGrid.set_rows(7);
            sMainGrid.set_columns(2);
            sMainGrid.set_spacing(4, 4);

            LSP_STATUS_ASSERT(sHBox.init());
            sHBox.set_horizontal();
            sHBox.set_spacing(8);

            LSP_STATUS_ASSERT(sWarnBox.init());
            sWarnBox.set_horizontal();
            sWarnBox.set_spacing(8);

            LSP_STATUS_ASSERT(sSBBookmarks.init());
            sSBBookmarks.set_vertical();
            sSBBookmarks.set_spacing(4);
            sSBBookmarks.set_expand(true);
            sSBBookmarks.constraints()->set_min_width(192);
            sSBBookmarks.set_vscroll(SCROLL_OPTIONAL);
            sSBBookmarks.set_vscroll_bypass(false);
            sSBBookmarks.set_hscroll(SCROLL_NONE);
            sSBBookmarks.set_hscroll_bypass(false);

            LSP_STATUS_ASSERT(sSBAlign.init());
            sSBAlign.set_pos(0.0f, -1.0f); // Middle, Top
            sSBAlign.set_scale(1.0f, 0.0f); // Maximum width, minimum height
            LSP_STATUS_ASSERT(sSBBookmarks.add(&sSBAlign));

            LSP_STATUS_ASSERT(sBookmarks.init());
            sBookmarks.set_vertical();
            sBookmarks.set_spacing(4);
            sBookmarks.set_expand(true);
            LSP_STATUS_ASSERT(sSBAlign.add(&sBookmarks));

            LSP_STATUS_ASSERT(init_bm_popup_menu());

            init_color(C_YELLOW, sWWarning.font()->color());

            // Initialize supplementary elements
            // Path box
            sBMAdd.set_fill(true);
            LSP_STATUS_ASSERT(wPathBox.add(&sBMAdd));
            LSP_STATUS_ASSERT(wPathBox.add(&wUp));
            LSP_STATUS_ASSERT(wPathBox.add(&wGo));
            LSP_STATUS_ASSERT(add_label(&wPathBox, "labels.location", 1.0f));
            // Button box
            LSP_STATUS_ASSERT(sHBox.add(&sWAction));
            LSP_STATUS_ASSERT(sHBox.add(&sWCancel));
            // Warning box
            LSP_STATUS_ASSERT(add_label(&sWarnBox, "labels.file_list"));
            LSP_STATUS_ASSERT(sWarnBox.add(&sWWarning));

            // Initialize grid
            // Row 1
            LSP_STATUS_ASSERT(sMainGrid.add(&wPathBox));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWPath));
            // Row 2
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.bookmark_list"));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWarnBox));
            // Row 3
            LSP_STATUS_ASSERT(sMainGrid.add(&sSBBookmarks));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWFiles));
            // Row 4
            LSP_STATUS_ASSERT(sMainGrid.add(NULL));
            LSP_STATUS_ASSERT(add_ext_button(&sMainGrid, "labels.automatic_extension"));
            // Row 5
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.file_name", 1.0f, &pWSearch));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWSearch));
            // Row 6
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.filter", 1.0f));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWFilter));
            // Row 7
            LSP_STATUS_ASSERT(sMainGrid.add(NULL));
            LSP_STATUS_ASSERT(sMainGrid.add(&sHBox));

            // Initialize structure
            init_color(C_YELLOW, wAutoExt.color());
            wAutoExt.set_led(true);
            wAutoExt.set_toggle();
            wAutoExt.set_down(true);

            LSP_STATUS_ASSERT(this->add(&sMainGrid));

            // Bind events
            status_t result = sWAction.slots()->bind(LSPSLOT_SUBMIT, slot_on_action, self());
            if (result < 0)
                return -result;
            result = sWCancel.slots()->bind(LSPSLOT_SUBMIT, slot_on_cancel, self());
            if (result < 0)
                return -result;
            result = sWSearch.slots()->bind(LSPSLOT_CHANGE, slot_on_search, self());
            if (result < 0)
                return -result;
            result = sWFilter.slots()->bind(LSPSLOT_SUBMIT, slot_on_search, self());
            if (result < 0)
                return -result;
            result = sWFiles.slots()->bind(LSPSLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, self());
            if (result < 0)
                return -result;
            result = sWFiles.slots()->bind(LSPSLOT_CHANGE, slot_list_change, self());
            if (result < 0)
                return -result;
            result = wGo.slots()->bind(LSPSLOT_SUBMIT, slot_on_go, self());
            if (result < 0)
                return -result;
            result = wUp.slots()->bind(LSPSLOT_SUBMIT, slot_on_up, self());
            if (result < 0)
                return -result;
            result = sBMAdd.slots()->bind(LSPSLOT_SUBMIT, slot_on_bm_add, self());
            if (result < 0)
                return -result;
            result = sWPath.slots()->bind(LSPSLOT_KEY_UP, slot_on_path_key_up, self());
            if (result < 0)
                return -result;

            padding()->set_all(8);
            set_border_style(BS_DIALOG);
            actions()->set_actions(WA_DIALOG | WA_RESIZE | WA_CLOSE);

            sync_mode();

            return STATUS_OK;
        }

        void LSPFileDialog::destroy()
        {
            do_destroy();
            LSPWindow::destroy();
        }

        void LSPFileDialog::do_destroy()
        {
            drop_bookmarks();
            destroy_file_entries(&vFiles);

            // Clear labels
            size_t n = vWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPWidget *w    = vWidgets.at(i);
                if (w == NULL)
                    continue;
                w->destroy();
                delete w;
            }
            vWidgets.clear();

            sWPath.destroy();
            sWSearch.destroy();
            sWFilter.destroy();
            sWFiles.destroy();
            sWAction.destroy();
            sWCancel.destroy();
            sHBox.destroy();
            sWarnBox.destroy();
            sSBBookmarks.destroy();
            sSBAlign.destroy();
            sBookmarks.destroy();
            sBMPopup.destroy();
            sBMAdd.destroy();
            sMainGrid.destroy();
            sWWarning.destroy();
            sAppendExt.destroy();
            wAutoExt.destroy();
            wGo.destroy();
            wUp.destroy();
            wPathBox.destroy();

            pWSearch = NULL;

            if (pWConfirm != NULL)
            {
                pWConfirm->destroy();
                delete pWConfirm;
                pWConfirm = NULL;
            }

            if (pWMessage != NULL)
            {
                pWMessage->destroy();
                delete pWMessage;
                pWMessage = NULL;
            }
        }

        status_t LSPFileDialog::init_bm_popup_menu()
        {
            LSP_STATUS_ASSERT(sBMPopup.init());
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.open", slot_on_bm_menu_open));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.link.follow", slot_on_bm_menu_follow));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.link.copy", slot_on_bm_menu_copy));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.delete", slot_on_bm_menu_delete));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, NULL, NULL));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_first", slot_on_bm_menu_first));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_up", slot_on_bm_menu_up));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_down", slot_on_bm_menu_down));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_last", slot_on_bm_menu_last));

            return STATUS_OK;
        }

        void LSPFileDialog::sync_mode()
        {
            if (enMode == FDM_OPEN_FILE)
            {
                if (pWSearch != NULL)
                    pWSearch->text()->set("labels.search");
                sAppendExt.set_visible(false);
            }
            else if (enMode == FDM_SAVE_FILE)
            {
                if (pWSearch != NULL)
                    pWSearch->text()->set("labels.file_name");
                sAppendExt.set_visible(true);
            }
        }

        status_t LSPFileDialog::set_mode(file_dialog_mode_t mode)
        {
            if (mode == enMode)
                return STATUS_OK;
            enMode = mode;
            sync_mode();
            return STATUS_OK;
        }

        status_t LSPFileDialog::set_path(const LSPString *value)
        {
            LSP_STATUS_ASSERT(sWPath.set_text(value));
            if (is_visible())
                refresh_current_path();
            return STATUS_OK;
        }

        status_t LSPFileDialog::set_path(const char *value)
        {
            LSP_STATUS_ASSERT(sWPath.set_text(value));
            if (is_visible())
                refresh_current_path();
            return STATUS_OK;
        }

        status_t LSPFileDialog::set_search(const LSPString *value)
        {
            LSP_STATUS_ASSERT(sWSearch.set_text(value));
            if (is_visible())
                apply_filters();
            return STATUS_OK;
        }

        status_t LSPFileDialog::set_search(const char *value)
        {
            LSP_STATUS_ASSERT(sWSearch.set_text(value));
            if (is_visible())
                apply_filters();
            return STATUS_OK;
        }

        status_t LSPFileDialog::set_use_confirm(bool set)
        {
            if (bUseConfirm == set)
                return STATUS_OK;
            if ((!set) && (pWConfirm != NULL) && (pWConfirm->hidden()))
            {
                pWConfirm->destroy();
                delete pWConfirm;
                pWConfirm = NULL;
            }
            bUseConfirm = set;
            return STATUS_OK;
        };

        status_t LSPFileDialog::add_file_entry(cvector<file_entry_t> *dst, const char *name, size_t flags)
        {
            file_entry_t *ent = new file_entry_t();
            if (ent == NULL)
                return STATUS_NO_MEM;
            if (!ent->sName.set_native(name))
            {
                delete ent;
                return STATUS_NO_MEM;
            }
            ent->nFlags     = flags;

            if (!dst->add(ent))
            {
                delete ent;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        void LSPFileDialog::destroy_file_entries(cvector<file_entry_t> *dst)
        {
            size_t n = dst->size();
            for (size_t i=0; i<n; ++i)
            {
                file_entry_t *fd = dst->at(i);
                if (fd == NULL)
                    continue;
                delete fd;
            }
            dst->clear();
        }

        status_t LSPFileDialog::refresh_current_path()
        {
            cvector<file_entry_t> scanned;
            LSPString str, path;
//            struct stat st;
            status_t xres;
//            char srcpath[PATH_MAX], dstpath[PATH_MAX];

            // Obtain the path to working directory
            io::Path xpath;
            xres = sWPath.get_text(&path);
            if ((xres == STATUS_OK) && (path.length() > 0))
                xres = xpath.set(&path); // Directory is specified, use it
            else
            {
                xres = xpath.current(); // Directory is not specified, use curren
                if (xres == STATUS_OK)
                    sWPath.set_text(xpath.as_string());
            }
            if ((xres == STATUS_OK) && (!xpath.is_root())) // Need to add dotdot entry?
                xres = add_file_entry(&scanned, "..", F_DOTDOT);

            if (xres != STATUS_OK) // Check result
            {
                destroy_file_entries(&scanned);
                return xres;
            }

            // Open directory for reading
            io::Dir dir;
            xres = dir.open(&xpath);
            if (xres == STATUS_OK)
            {
                sWWarning.hide();

                // Read directory
                io::fattr_t fattr;
                io::Path fname;

                while (dir.reads(&fname, &fattr, false) == STATUS_OK)
                {
                    // Reject dot and dotdot from search
                    if ((fname.is_dot()) || (fname.is_dotdot()))
                        continue;

                    // Analyze file flags
                    size_t nflags = 0;
                    if (fname.as_string()->first() == '.')
                        nflags      |= F_ISHIDDEN;

                    if (fattr.type == io::fattr_t::FT_DIRECTORY) // Directory?
                        nflags      |= F_ISDIR;
                    else if (fattr.type == io::fattr_t::FT_SYMLINK) // Symbolic link?
                        nflags      |= F_ISLINK;
                    else if (fattr.type == io::fattr_t::FT_REGULAR)
                        nflags      |= F_ISREG;
                    else
                        nflags      |= F_ISOTHER;

                    if (nflags & F_ISLINK)
                    {
                        // Stat a file associated with symbolic link
                        xres = dir.sym_stat(&fname, &fattr);

                        if (xres != STATUS_OK)
                            nflags      |= F_ISINVALID;
                        else if (fattr.type == io::fattr_t::FT_DIRECTORY) // Directory?
                            nflags      |= F_ISDIR;
                        else if (fattr.type == io::fattr_t::FT_SYMLINK) // Symbolic link?
                            nflags      |= F_ISLINK;
                        else if (fattr.type == io::fattr_t::FT_REGULAR)
                            nflags      |= F_ISREG;
                        else
                            nflags      |= F_ISOTHER;
                    }

                    // Add entry to list of found files
                    if ((xres = add_file_entry(&scanned, fname.as_native(), nflags)) != STATUS_OK)
                    {
                        dir.close();
                        destroy_file_entries(&scanned);
                        return xres;
                    }
                }

                // Close directory
                if (dir.close() != STATUS_OK)
                {
                    destroy_file_entries(&scanned);
                    return STATUS_IO_ERROR;
                }
            }
            else // Analyze errcode
            {
                const char *text = "unknown I/O error";
                switch (xres)
                {
                    case STATUS_PERMISSION_DENIED:    text = "permission denied"; break;
                    case STATUS_NOT_FOUND:    text = "directory does not exist"; break;
                    case STATUS_NO_MEM:    text = "not enough memory"; break;
                    default: break;
                }

                str.set_native("Access error: ");
                path.set_native(text);
                str.append(&path);
                sWWarning.text()->set_raw(&str);
                sWWarning.show();
            }

            // Now we have the complete list of files, need to reorder them
            size_t n = scanned.size();
            if (n > 2)
            {
                for (size_t i=0; i<(n-1); ++i)
                    for (size_t j=i+1; j<n; ++j)
                        if (cmp(scanned.at(i), scanned.at(j)) > 0)
                            scanned.swap_unsafe(i, j);
            }

            // Alright, now we can swap contents and delete previous contents
            vFiles.swap_data(&scanned);
            destroy_file_entries(&scanned);

            apply_filters();

            return select_current_bookmark();
        }

        status_t LSPFileDialog::apply_filters()
        {
            status_t xres;
            LSPString tmp, xfname, *psrc = NULL;
            LSPFileMask *fmask = NULL, smask;

            // Initialize masks
            if (enMode == FDM_OPEN_FILE) // Additional filtering is available only when opening file
            {
                LSP_STATUS_ASSERT(sWSearch.get_text(&tmp));

                if (tmp.length() > 0)
                {
                    if (!tmp.prepend('*'))
                        return STATUS_NO_MEM;
                    if (!tmp.append('*'))
                        return STATUS_NO_MEM;
                    xres = smask.parse(&tmp);
                    if (xres != STATUS_OK)
                        return xres;
                }
            }
            else
            {
                sWFiles.selection()->clear();
                LSP_STATUS_ASSERT(sWSearch.get_text(&xfname));
            }

            if (sWFilter.items()->size() > 0)
            {
                ssize_t sel = sWFilter.selected();
                LSPFileFilterItem *fi = sFilter.get((sel < 0) ? 0 : sel);
                fmask = (fi != NULL) ? fi->pattern() : NULL;
            }

            // Now we need to fill data
            LSPItemList *lst = sWFiles.items();
            float xs = sWFiles.hscroll(), ys = sWFiles.vscroll();
            lst->clear();

            // Process files
            size_t n = vFiles.size();
            for (size_t i=0; i<n; ++i)
            {
                file_entry_t *ent = vFiles.at(i);
                psrc = &ent->sName;

                // Pass entry name through filter
                if (!(ent->nFlags & (F_ISDIR | F_DOTDOT)))
                {
                    // Process with masks
                    if ((fmask != NULL) && (!fmask->matched(psrc)))
                        continue;
                    if (!smask.matched(psrc))
                        continue;
                }

                // Add some special characters
                if (ent->nFlags & (F_ISOTHER | F_ISDIR | F_ISLINK | F_ISINVALID))
                {
                    if (!tmp.set(psrc))
                    {
                        lst->clear();
                        return STATUS_NO_MEM;
                    }
                    psrc = &tmp;

                    // Modify the name of the item
                    bool ok = true;
                    if (ent->nFlags & F_ISOTHER)
                        ok = ok && psrc->prepend('*');
                    else if (ent->nFlags & (F_ISLINK | F_ISINVALID))
                        ok = ok && psrc->prepend((ent->nFlags & F_ISINVALID) ? '!' : '~');

                    if (ent->nFlags & F_ISDIR)
                    {
                        ok = ok && psrc->prepend('[');
                        ok = ok && psrc->append(']');
                    }

                    if (!ok)
                    {
                        lst->clear();
                        return STATUS_NO_MEM;
                    }
                }

                // Add item
                LSPItem *item = NULL;
                if ((xres = lst->add(&item)) != STATUS_OK)
                {
                    lst->clear();
                    return xres;
                }
                item->text()->set_raw(psrc);
                item->set_value(i);

                // Check if is equal
                if ((!(ent->nFlags & (F_ISDIR | F_DOTDOT))) && (xfname.length() > 0))
                {
                    lsp_trace("  %s <-> %s", ent->sName.get_native(), xfname.get_native());
                    #ifdef PLATFORM_UNIX_COMPATIBLE
                    if (ent->sName.equals(&xfname))
                        sWFiles.selection()->set_value(lst->size() - 1);
                    #endif /* PLATFORM_UNIX_COMPATIBLE */

                    #ifdef PLATFORM_WINDOWS
                    if (ent->sName.equals_nocase(&xfname))
                        sWFiles.selection()->set_value(lst->size() - 1);
                    #endif /* PLATFORM_WINDOWS */
                }
            }

            sWFiles.set_hscroll(xs);
            sWFiles.set_vscroll(ys);

            return STATUS_OK;
        }

        int LSPFileDialog::cmp(const file_entry_t *a, const file_entry_t *b)
        {
            ssize_t delta = ((b->nFlags & F_DOTDOT) - (a->nFlags & F_DOTDOT));
            if (delta != 0)
                return delta;
            delta = ((b->nFlags & F_ISDIR) - (a->nFlags & F_ISDIR));
            if (delta != 0)
                return delta;
            return a->sName.compare_to(&b->sName);
        }

        status_t LSPFileDialog::slot_on_action(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_action(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_on_confirm(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_confirm(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_on_cancel(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_cancel(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_on_search(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_search(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_mouse_dbl_click(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_mouse_dbl_click(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_list_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_list_change(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_on_go(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_go(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_on_up(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_up(data) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_on_bm_add(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            if (dlg != NULL)
                dlg->add_new_bookmark();
            return STATUS_OK;
        }

        status_t LSPFileDialog::slot_on_path_key_up(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *dlg = widget_ptrcast<LSPFileDialog>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return (dlg != NULL) ? dlg->on_path_key_up(ev) : STATUS_BAD_STATE;
        }

        status_t LSPFileDialog::slot_on_bm_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            return (_this != NULL) ? _this->on_bm_submit(sender) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPFileDialog::slot_on_bm_popup(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            _this->pPopupBookmark = _this->find_bookmark(sender);
            return STATUS_OK;;
        }

        status_t LSPFileDialog::slot_on_bm_menu_open(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
                _this->set_path(&_this->pPopupBookmark->sBookmark.path) :
                STATUS_OK;
        }

        status_t LSPFileDialog::slot_on_bm_menu_follow(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
                _this->pPopupBookmark->sHlink.follow_url() :
                STATUS_OK;
        }

        status_t LSPFileDialog::slot_on_bm_menu_copy(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
                _this->pPopupBookmark->sHlink.copy_url(CBUF_CLIPBOARD) :
                STATUS_OK;
        }

        status_t LSPFileDialog::slot_on_bm_menu_delete(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
                _this->remove_bookmark(_this->pPopupBookmark) :
                STATUS_OK;
        }

        status_t LSPFileDialog::slot_on_bm_menu_up(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            ssize_t idx = (_this->pPopupBookmark != NULL) ?
                    _this->vBookmarks.index_of(_this->pPopupBookmark) : -1;

            // Find previous visible bookmark
            ssize_t prev = idx - 1;
            for ( ; prev >= 0; --prev)
            {
                bm_entry_t *ent = _this->vBookmarks.at(prev);
                if ((ent != NULL) && (ent->sBookmark.origin & bookmarks::BM_LSP))
                    break;
            }

            if (prev < 0)
                return STATUS_OK;

            return (_this->vBookmarks.swap(prev, idx)) ?  _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
        }

        status_t LSPFileDialog::slot_on_bm_menu_down(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            ssize_t items = _this->vBookmarks.size();
            ssize_t idx = (_this->pPopupBookmark != NULL) ?
                    _this->vBookmarks.index_of(_this->pPopupBookmark) : items;

            // Find next visible bookmark
            ssize_t next = idx + 1;
            for ( ; next < items; ++next)
            {
                bm_entry_t *ent = _this->vBookmarks.at(next);
                if ((ent != NULL) && (ent->sBookmark.origin & bookmarks::BM_LSP))
                    break;
            }

            if (next >= items)
                return STATUS_OK;

            return (_this->vBookmarks.swap(idx, next)) ?  _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
        }

        status_t LSPFileDialog::slot_on_bm_menu_first(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            ssize_t idx = (_this->pPopupBookmark != NULL) ?
                    _this->vBookmarks.index_of(_this->pPopupBookmark) : -1;
            if (idx <= 0)
                return STATUS_OK;
            return (_this->vBookmarks.move(idx, 0)) ? _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
        }

        status_t LSPFileDialog::slot_on_bm_menu_last(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFileDialog *_this = widget_ptrcast<LSPFileDialog>(ptr);
            ssize_t last = _this->vBookmarks.size() - 1;
            ssize_t idx = (_this->pPopupBookmark != NULL) ?
                    _this->vBookmarks.index_of(_this->pPopupBookmark) : last;
            if (idx >= last)
                return STATUS_OK;
            return (_this->vBookmarks.move(idx, last)) ? _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
        }

        status_t LSPFileDialog::on_dlg_go(void *data)
        {
            LSPString path;
            LSP_STATUS_ASSERT(sWPath.get_text(&path));
            return set_path(&path);
        }

        status_t LSPFileDialog::on_dlg_up(void *data)
        {
            LSPString path;
            LSP_STATUS_ASSERT(sWPath.get_text(&path));
            ssize_t pos = path.rindex_of(FILE_SEPARATOR_C);
            if (pos < 0)
                return STATUS_OK;
            path.truncate(pos);
            if (path.length() <= 0)
                path.append(FILE_SEPARATOR_C);
            return set_path(&path);
        }

        status_t LSPFileDialog::on_path_key_up(ws_event_t *e)
        {
            lsp_trace("Path key code released=%x, modifiers=%x", int(e->nCode), int(e->nState));
            ws_code_t key = LSPKeyboardHandler::translate_keypad(e->nCode);
            if (key == WSK_RETURN)
                return on_dlg_go(e);
            return STATUS_OK;
        }

        status_t LSPFileDialog::on_dlg_mouse_dbl_click(void *data)
        {
            file_entry_t *ent = selected_entry();
            if (ent == NULL)
                return STATUS_OK;

            // Analyze what to do
            LSPString path;
            if (ent->nFlags & F_DOTDOT)
                return on_dlg_up(NULL);
            else if (ent->nFlags & F_ISDIR)
            {
                LSP_STATUS_ASSERT(sWPath.get_text(&path));
                LSP_STATUS_ASSERT(LSPFileMask::append_path(&path, &ent->sName));
                return set_path(&path);
            }
            else
                return on_dlg_action(data);

            return STATUS_OK;
        }

        status_t LSPFileDialog::on_dlg_list_change(void *data)
        {
            if (enMode != FDM_SAVE_FILE)
                return STATUS_OK;

            file_entry_t *ent = selected_entry();
            if (ent == NULL)
                return STATUS_OK;

            // Analyze what to do
            if ((ent->nFlags & F_DOTDOT) || (ent->nFlags & F_ISDIR))
                return STATUS_OK;

            return sWSearch.set_text(&ent->sName);
        }

        status_t LSPFileDialog::on_dlg_search(void *data)
        {
            if (invisible())
                return STATUS_OK;

            return apply_filters();
        }

        status_t LSPFileDialog::build_full_path(LSPString *dst, const LSPString *fname)
        {
            LSPString path;
            LSP_STATUS_ASSERT(sWPath.get_text(&path));
            return LSPFileMask::append_path(dst, &path, fname);
        }

        status_t LSPFileDialog::show_message(const char *title, const char *heading, const char *message)
        {
            if (pWMessage == NULL)
            {
                pWMessage       = new LSPMessageBox(pDisplay);
                status_t res    = pWMessage->init();
                if (res != STATUS_OK)
                {
                    delete pWMessage;
                    return res;
                }

                LSP_STATUS_ASSERT(pWMessage->add_button("actions.ok"));
            }
            LSP_STATUS_ASSERT(pWMessage->title()->set(title));
            LSP_STATUS_ASSERT(pWMessage->heading()->set(heading));
            LSP_STATUS_ASSERT(pWMessage->message()->set(message));

            return pWMessage->show(this);
        }

        LSPFileDialog::file_entry_t *LSPFileDialog::selected_entry()
        {
            ssize_t index = sWFiles.selection()->value();
            if (index < 0)
                return NULL;
            LSPItem *item = sWFiles.items()->get(index);
            if (item == NULL)
                return NULL;
            index = item->value();
            if (index < 0)
                return NULL;
            return vFiles.get(index);
        }

        status_t LSPFileDialog::on_dlg_action(void *data)
        {
            bool committed = false;

            if (enMode == FDM_SAVE_FILE) // Use 'File name' field
            {
                LSPString fname;
                LSP_STATUS_ASSERT(sWSearch.get_text(&fname));

                if (wAutoExt.is_down())
                {
                    LSPString ext;
                    ssize_t sel = sWFilter.selected();

                    LSPFileFilterItem *item  = sFilter.get((sel < 0) ? 0 : sel);
                    status_t res = (item != NULL) ? item->get_extension(&ext) : STATUS_NOT_FOUND;

                    if (res == STATUS_OK)
                    {
                        lsp_trace("fname = %s, ext = %s", fname.get_native(), ext.get_native());
                        if (!fname.ends_with_nocase(&ext))
                            fname.append(&ext);
                        lsp_trace("fname = %s", fname.get_native());
                    }
                }

                if (LSPFileMask::is_dots(&fname) || (!LSPFileMask::valid_file_name(&fname)))
                    return show_message("titles.attention", "headings.attention", "messages.file.invalid_name");

                LSP_STATUS_ASSERT(build_full_path(&sSelected, &fname));
                committed = true;
            }
            else
            {
                LSPString fname;
                LSP_STATUS_ASSERT(sWSearch.get_text(&fname));
                if ((!LSPFileMask::is_dots(&fname)) && (LSPFileMask::valid_file_name(&fname)))
                {
                    LSP_STATUS_ASSERT(build_full_path(&sSelected, &fname));
                    committed = true;
                }
            }

            // Use selection
            if (!committed)
            {
                file_entry_t *ent = selected_entry();
                if (ent == NULL)
                    return show_message("titles.attention", "headings.attention", "messages.file.not_specified");

                // Analyze what to do
                if (ent->nFlags & F_DOTDOT)
                    return on_dlg_up(NULL);
                else if (ent->nFlags & F_ISDIR)
                {
                    LSPString path;
                    LSP_STATUS_ASSERT(sWPath.get_text(&path));
                    LSP_STATUS_ASSERT(LSPFileMask::append_path(&path, &ent->sName));
                    return set_path(&path);
                }
                else
                {
                    LSPString path;
                    LSP_STATUS_ASSERT(sWPath.get_text(&path));
                    LSP_STATUS_ASSERT(LSPFileMask::append_path(&sSelected, &path, &ent->sName));
                }
            }

            // Special case for saving file
            io::fattr_t fattr;
            status_t stat_result = io::File::sym_stat(&sSelected, &fattr);

            if (enMode == FDM_SAVE_FILE)
            {
                if (!bUseConfirm)
                    return on_dlg_confirm(data);

                // Check that file exists and avoid confirmation if it doesn't
                lsp_trace("Checking file: %s", sSelected.get_native());
                if (stat_result != STATUS_OK)
                    return on_dlg_confirm(data);
            }
            else
            {
                if (stat_result != 0)
                    return show_message("titles.attention", "headings.attention", "messages.file.not_exists");

                if (!bUseConfirm)
                    return on_dlg_confirm(data);
            }

            if (pWConfirm == NULL)
            {
                // Create dialog object
                pWConfirm = new LSPMessageBox(pDisplay);
                if (pWConfirm == NULL)
                    return STATUS_NO_MEM;
                pWConfirm->init();

                pWConfirm->title()->set("titles.confirmation");
                pWConfirm->heading()->set("headings.confirmation");
                pWConfirm->add_button("actions.confirm.yes", slot_on_confirm, self());
                pWConfirm->add_button("actions.confirm.no");
            }
            pWConfirm->message()->set(&sConfirm);
            pWConfirm->show(this);

            return STATUS_OK;
        }

        status_t LSPFileDialog::on_dlg_confirm(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
            hide();
            destroy_file_entries(&vFiles);

            // Execute slots
            return sAction.execute(this, data);
        }

        status_t LSPFileDialog::on_dlg_cancel(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
            hide();
            destroy_file_entries(&vFiles);

            // Execute slots
            return sCancel.execute(this, data);
        }

        status_t LSPFileDialog::on_show()
        {
            ssize_t idx = sFilter.get_default();
            if ((idx < 0) && (sFilter.size() > 0))
                idx = 0;
            sWFilter.set_selected(idx);
            refresh_bookmarks();
            refresh_current_path();
            return STATUS_OK;
        }

        status_t LSPFileDialog::on_close(const ws_event_t *e)
        {
            ws_event_t ev = *e;
            return on_dlg_cancel(&ev);
        }

        status_t LSPFileDialog::read_lsp_bookmarks(cvector<bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_user_config_path(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(LSP_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks(&vbm, &path);
        }

        status_t LSPFileDialog::read_gtk2_bookmarks(cvector<bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(GTK2_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks_gtk2(&vbm, &path);
        }

        status_t LSPFileDialog::read_gtk3_bookmarks(cvector<bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(GTK3_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks_gtk3(&vbm, &path);
        }

        status_t LSPFileDialog::read_qt5_bookmarks(cvector<bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(QT5_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks_qt5(&vbm, &path);
        }

        status_t LSPFileDialog::save_bookmarks(cvector<bookmark_t> *vbm)
        {
            io::Path path, parent;
            cvector<bookmark_t> tmp;

            status_t res = system::get_user_config_path(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(LSP_BOOKMARK_PATH)) != STATUS_OK)
                return res;
            if ((res = path.get_parent(&parent)) != STATUS_OK)
                return res;
            if ((res = parent.mkdir(true)) != STATUS_OK)
                return res;

            if (vbm == NULL)
            {
                // Build list of bookmarks
                for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
                {
                    bm_entry_t *ent = vBookmarks.at(i);
                    if ((ent != NULL) && (!tmp.add(&ent->sBookmark)))
                    {
                        tmp.flush();
                        return STATUS_NO_MEM;
                    }
                }

                vbm = &tmp;
            }

            return bookmarks::save_bookmarks(vbm, &path);
        }

        void LSPFileDialog::drop_bookmarks()
        {
            // Deactivate currently selected bookmark
            sBookmarks.remove_all();
            pSelBookmark    = NULL;
            pPopupBookmark  = NULL;

            // Destroy bookmarks storage
            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.at(i);
                if (ent != NULL)
                {
                    ent->sHlink.destroy();
                    delete ent;
                }
            }
            vBookmarks.flush();
        }

        status_t LSPFileDialog::select_current_bookmark()
        {
            status_t res;
            LSPString spath;
            io::Path path;

            // Get path and canonicalize
            if ((res = sWPath.get_text(&spath)) != STATUS_OK)
                return res;
            if ((res = path.set(&spath)) != STATUS_OK)
                return res;
            if ((res = path.canonicalize()) != STATUS_OK)
                return res;

            bm_entry_t *found = NULL;
            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.at(i);
                if ((ent != NULL) && (ent->sPath.equals(&path)))
                {
                    found = ent;
                    break;
                }
            }

            // Check state
            if (found == pSelBookmark)
                return STATUS_OK;

            // Deactivate selected bookmark
            if (pSelBookmark != NULL)
                pSelBookmark->sHlink.bg_color()->set_default();
            pSelBookmark = found;
            if (found != NULL)
                init_color(C_BACKGROUND2, found->sHlink.bg_color());

            return STATUS_OK;
        }

        status_t LSPFileDialog::sync_bookmarks()
        {
            status_t res;
            if ((res = sBookmarks.remove_all()) != STATUS_OK)
                return res;

            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.at(i);
                if ((ent == NULL) || (!(ent->sBookmark.origin & bookmarks::BM_LSP)))
                    continue;
                if ((res = sBookmarks.add(&ent->sHlink)) != STATUS_OK)
                    break;
            }

            if (res != STATUS_OK)
                sBookmarks.remove_all();

            return (res == STATUS_OK) ? save_bookmarks(NULL) : res;
        }

        status_t LSPFileDialog::refresh_bookmarks()
        {
            drop_bookmarks();
            LSPString url;

            // Read LSP bookmarks
            cvector<bookmark_t> bm, tmp;
            status_t res, xres;
            size_t changes = 0;

            // Read bookmarks from different sources and merge
            xres = read_lsp_bookmarks(bm);
            if ((res = read_gtk2_bookmarks(tmp)) == STATUS_OK)
                bookmarks::merge_bookmarks(&bm, &changes, &tmp, bookmarks::BM_GTK2);
            if ((res = read_gtk3_bookmarks(tmp)) == STATUS_OK)
                bookmarks::merge_bookmarks(&bm, &changes, &tmp, bookmarks::BM_GTK3);
            if ((res = read_qt5_bookmarks(tmp)) == STATUS_OK)
                bookmarks::merge_bookmarks(&bm, &changes, &tmp, bookmarks::BM_QT5);
            bookmarks::destroy_bookmarks(&tmp);

            // Check if we need to store bookmarks
            if ((changes > 0) || (xres != STATUS_OK))
                save_bookmarks(&bm);

            // Create widgets
            bm_entry_t *ent = NULL;
            for (size_t i=0, n=bm.size(); i<n; ++i)
            {
                bookmarks::bookmark_t *b = bm.at(i);
                if (b == NULL)
                    continue;

                // Allocate entry
                if ((ent = new bm_entry_t(pDisplay)) == NULL)
                {
                    res = STATUS_NO_MEM;
                    break;
                }

                // Initialize data
                if ((res = ent->sPath.set(&b->path)) != STATUS_OK)
                    break;
                if ((res = ent->sPath.canonicalize()) != STATUS_OK)
                    break;
                if ((res = ent->sHlink.init()) != STATUS_OK)
                    break;
                if ((res = ent->sHlink.text()->set_raw(&b->name)) != STATUS_OK)
                    break;
                res = (url.set_ascii("file://")) ? STATUS_OK : STATUS_NO_MEM;
                if (res == STATUS_OK)
                    res = (url.append(&b->path)) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    break;

                ent->sHlink.set_halign(0.0f);
                ent->sHlink.set_follow(false);
                ent->sHlink.set_url(&url);
                ent->sHlink.padding()->set_horizontal(8, 8);
                ent->sHlink.slots()->bind(LSPSLOT_SUBMIT, slot_on_bm_submit, self());
                ent->sHlink.slots()->bind(LSPSLOT_BEFORE_POPUP, slot_on_bm_popup, self());
                ent->sHlink.set_popup(&sBMPopup);
                if (b->origin & bookmarks::BM_LSP)
                {
                    if ((res = sBookmarks.add(&ent->sHlink)) != STATUS_OK)
                        break;
                }
                ent->sBookmark.path.swap(&b->path);
                ent->sBookmark.name.swap(&b->name);
                ent->sBookmark.origin = b->origin;

                // Commit to list if
                res = (vBookmarks.add(ent)) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    break;
            }

            bookmarks::destroy_bookmarks(&bm);

            if (res != STATUS_OK)
            {
                drop_bookmarks();
                if (ent != NULL)
                {
                    ent->sHlink.destroy();
                    delete ent;
                }
                return res;
            }

            return select_current_bookmark();
        }

        LSPFileDialog::bm_entry_t *LSPFileDialog::find_bookmark(LSPWidget *sender)
        {
            LSPHyperlink *hlink = widget_cast<LSPHyperlink>(sender);
            if (hlink == NULL)
                return NULL;

            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.at(i);
                if ((ent != NULL) && (hlink == &ent->sHlink))
                    return ent;
            }
            return NULL;
        }

        status_t LSPFileDialog::on_bm_submit(LSPWidget *sender)
        {
            bm_entry_t *ent = find_bookmark(sender);
            return (ent != NULL) ? set_path(&ent->sBookmark.path) : STATUS_OK;
        }

        status_t LSPFileDialog::remove_bookmark(bm_entry_t *entry)
        {
            // Make invisible for LSP toolkit
            entry->sBookmark.origin &= ~bookmarks::BM_LSP;
            sBookmarks.remove(&entry->sHlink);
            if (pSelBookmark == entry)
                pSelBookmark = NULL;
            if (pPopupBookmark == entry)
                pPopupBookmark = NULL;

            // Drop entry if origin is emtpy
            if (!entry->sBookmark.origin)
            {
                vBookmarks.remove(entry);
                entry->sHlink.destroy();
                delete entry;
            }

            // Synchronize bookmarks and save state
            return sync_bookmarks();
        }

        status_t LSPFileDialog::add_new_bookmark()
        {
            status_t res;
            LSPString spath;
            io::Path path;

            // Get current path
            if ((res = sWPath.get_text(&spath)) != STATUS_OK)
                return res;
            if ((res = path.set(&spath)) != STATUS_OK)
                return res;

            // Get selected entry
            file_entry_t *fent = selected_entry();
            if ((fent != NULL) && ((fent->nFlags & (F_ISDIR | F_DOTDOT)) == F_ISDIR))
            {
                if ((res = path.append_child(&fent->sName)) != STATUS_OK)
                    return res;
            }

            lsp_trace("Add bookmark path=%s", path.as_native());

            // Canonicalize path
            if ((res = path.canonicalize()) != STATUS_OK)
                return res;
            if (!path.is_dir())
                return STATUS_NOT_DIRECTORY;

            // Now seek that item is already present
            bm_entry_t *ent;
            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                ent = vBookmarks.at(i);
                if ((ent != NULL) && (ent->sPath.equals(&path)))
                {
                    if (ent->sBookmark.origin & bookmarks::BM_LSP)
                        return STATUS_ALREADY_EXISTS;
                    if ((res = path.get_last(&ent->sBookmark.name)) != STATUS_OK)
                        return res;
                    ent->sBookmark.origin |= bookmarks::BM_LSP;
                    return sync_bookmarks();
                }
            }

            // Create new bookmark
            if ((ent = new bm_entry_t(pDisplay)) == NULL)
                return STATUS_NO_MEM;
            if (!vBookmarks.add(ent))
            {
                delete ent;
                return STATUS_NO_MEM;
            }

            if ((res = init_entry(ent, &path)) != STATUS_OK)
            {
                vBookmarks.remove(ent);
                ent->sHlink.destroy();
                delete ent;
                return STATUS_NO_MEM;
            }

            // Synchronize bookmarks and exit
            return sync_bookmarks();
        }

        status_t LSPFileDialog::init_entry(bm_entry_t *ent, const io::Path *path)
        {
            status_t res;
            LSPString url;

            ent->sBookmark.origin   = bookmarks::BM_LSP;
            if ((res = path->get_last(&ent->sBookmark.name)) != STATUS_OK)
                return res;
            if ((res = path->get(&ent->sBookmark.path)) != STATUS_OK)
                return res;

            // Initialize data
            if ((res = ent->sPath.set(path)) != STATUS_OK)
                return res;
            if ((res = ent->sHlink.init()) != STATUS_OK)
                return res;
            if ((res = ent->sHlink.text()->set_raw(&ent->sBookmark.name)) != STATUS_OK)
                return res;
            if ((res = path->get(&url)) != STATUS_OK)
                return res;
            if (!url.prepend_ascii("file://"))
                return STATUS_NO_MEM;

            ent->sHlink.set_halign(0.0f);
            ent->sHlink.set_follow(false);
            ent->sHlink.set_url(&url);
            ent->sHlink.padding()->set_horizontal(8, 8);
            ent->sHlink.slots()->bind(LSPSLOT_SUBMIT, slot_on_bm_submit, self());
            ent->sHlink.slots()->bind(LSPSLOT_BEFORE_POPUP, slot_on_bm_popup, self());
            ent->sHlink.set_popup(&sBMPopup);

            return STATUS_OK;
        }

    } /* namespace ctl */
} /* namespace lsp */
