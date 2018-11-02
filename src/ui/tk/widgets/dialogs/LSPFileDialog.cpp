/*
 * LSPFileDialog.cpp
 *
 *  Created on: 29 сент. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

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

        status_t LSPFileDialog::LSPFileDialogFilter::item_updated(size_t idx, filter_t *flt)
        {
            return pDialog->sWFilter.items()->set_text(idx, &flt->sTitle);
        }

        status_t LSPFileDialog::LSPFileDialogFilter::item_removed(size_t idx, filter_t *flt)
        {
            return pDialog->sWFilter.items()->remove(idx);
        }

        status_t LSPFileDialog::LSPFileDialogFilter::item_added(size_t idx, filter_t *flt)
        {
            return pDialog->sWFilter.items()->insert(idx, &flt->sTitle);
        }

        void LSPFileDialog::LSPFileDialogFilter::default_updated(ssize_t idx)
        {
            pDialog->sWFilter.set_selected(idx);
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
            sVBox(dpy),
            sHBox(dpy),
            sAppendExt(dpy),
            wAutoExt(dpy),
            wGo(dpy),
            wUp(dpy),
            wPathBox(dpy),
            sWWarning(dpy),
            sFilter(this)
        {
            pWConfirm       = NULL;
            nDefaultFilter  = 0;
            nUIDGen         = 0;
            pWSearch        = NULL;
            pWMessage       = NULL;
            pClass          = &metadata;
            enMode          = FDM_OPEN_FILE;
        }

        LSPFileDialog::~LSPFileDialog()
        {
            do_destroy();
        }

        status_t LSPFileDialog::add_label(LSPWidgetContainer *c, const char *text, LSPLabel **label)
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
            algn->set_hpos(0.0f);
            if (result == STATUS_OK)
                result = lbl->set_text(text);

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
                result = lbl->set_text(text);
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
            LSP_STATUS_ASSERT(sWCancel.set_title("Cancel"));
            sWCancel.set_min_width(96);
            sWCancel.set_min_height(24);
            LSP_STATUS_ASSERT(sWWarning.init());
            sWWarning.set_visible(false);

            LSP_STATUS_ASSERT(wGo.init());
            LSP_STATUS_ASSERT(wGo.set_title("Go"));
            wGo.set_min_width(32);
            LSP_STATUS_ASSERT(wUp.init());
            LSP_STATUS_ASSERT(wUp.set_title("Up"));
            wUp.set_min_width(32);
            LSP_STATUS_ASSERT(wPathBox.init());
            wPathBox.set_horizontal();
            wPathBox.set_spacing(2);
            wPathBox.set_fill(true);

            init_color(C_YELLOW, sWWarning.font()->color());

            // Initialize boxes
            LSP_STATUS_ASSERT(sVBox.init());
            sVBox.set_vertical();
            sVBox.set_spacing(4);

            LSP_STATUS_ASSERT(sHBox.init());
            sHBox.set_horizontal();
            sHBox.set_spacing(8);

            // Initialize structure
            LSP_STATUS_ASSERT(add_label(&sVBox, "Location"));
            LSP_STATUS_ASSERT(wPathBox.add(&sWPath));
            LSP_STATUS_ASSERT(wPathBox.add(&wGo));
            LSP_STATUS_ASSERT(wPathBox.add(&wUp));
            LSP_STATUS_ASSERT(sVBox.add(&wPathBox));
            LSP_STATUS_ASSERT(sVBox.add(&sWWarning));
            LSP_STATUS_ASSERT(add_label(&sVBox, "Files"));
            LSP_STATUS_ASSERT(sVBox.add(&sWFiles));
            LSP_STATUS_ASSERT(add_label(&sVBox, "File name", &pWSearch));
            LSP_STATUS_ASSERT(sVBox.add(&sWSearch));
            LSP_STATUS_ASSERT(add_ext_button(&sVBox, "Automatic extension"));
            LSP_STATUS_ASSERT(add_label(&sVBox, "Filter"));
            LSP_STATUS_ASSERT(sVBox.add(&sWFilter));
            LSP_STATUS_ASSERT(sVBox.add(&sHBox));
            LSP_STATUS_ASSERT(sHBox.add(&sWAction));
            LSP_STATUS_ASSERT(sHBox.add(&sWCancel));

            init_color(C_YELLOW, wAutoExt.color());
            wAutoExt.set_led(true);
            wAutoExt.set_toggle();
            wAutoExt.set_down(true);

            // Add child
            LSP_STATUS_ASSERT(this->add(&sVBox));

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
            sVBox.destroy();
            sHBox.destroy();
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

        void LSPFileDialog::sync_mode()
        {
            if (enMode == FDM_OPEN_FILE)
            {
                if (pWSearch != NULL)
                    pWSearch->set_text("Search");
                sAppendExt.set_visible(false);
            }
            else if (enMode == FDM_SAVE_FILE)
            {
                if (pWSearch != NULL)
                    pWSearch->set_text("File name");
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

        status_t LSPFileDialog::set_confirmation(const LSPString *value)
        {
            if (!sConfirm.set(value))
                return STATUS_NO_MEM;
            if ((sConfirm.length() > 0) || (pWConfirm == NULL))
                return STATUS_OK;
            if ((pWConfirm != NULL) && (pWConfirm->hidden()))
            {
                pWConfirm->destroy();
                delete pWConfirm;
                pWConfirm = NULL;
            }
            return STATUS_OK;
        };

        status_t LSPFileDialog::set_confirmation(const char *value)
        {
            if (!sConfirm.set_native(value))
                return STATUS_NO_MEM;
            if ((sConfirm.length() > 0) || (pWConfirm == NULL))
                return STATUS_OK;
            if ((pWConfirm != NULL) && (pWConfirm->hidden()))
            {
                pWConfirm->destroy();
                delete pWConfirm;
                pWConfirm = NULL;
            }
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
            struct stat st;
            status_t xres;
            char srcpath[PATH_MAX], dstpath[PATH_MAX];

            // Store current path to variable
            const char *cpath = sWPath.text();
            if ((cpath == NULL) || (strlen(cpath) <= 0))
            {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                    sWPath.set_text(cwd);
                cpath = sWPath.text();
                if (cpath == NULL)
                    return STATUS_NO_MEM;
            }

            // Try to open directory
            errno       = 0;
            DIR *fd     = opendir(cpath);
            int errcode = errno;

            // Add dotdot entry
            if (strcmp(cpath, FILE_SEPARATOR_S) != 0)
            {
                if ((xres = add_file_entry(&scanned, "..", F_DOTDOT)) != STATUS_OK)
                {
                    destroy_file_entries(&scanned);
                    return xres;
                }
            }

            // If directory is opened, scan directory
            if (fd != NULL)
            {
                sWWarning.hide();

                // Read directory
                dirent *item;

                while (true)
                {
                    // Read next entry
                    if (!(item = readdir(fd)))
                        break;

                    // Reject dot and dotdot from search
                    if (!strcmp(item->d_name, "."))
                        continue;
                    else if (!strcmp(item->d_name, ".."))
                        continue;

                    // Analyze file flags
                    size_t nflags = 0;
                    if (item->d_name[0] == '.')
                        nflags      |= F_ISHIDDEN;

                    if (item->d_type == DT_DIR) // Directory?
                        nflags      |= F_ISDIR;
                    else if (item->d_type == DT_LNK) // Symbolic link?
                    {
                        nflags      |= F_ISLINK;

                        snprintf(srcpath, sizeof(srcpath)/sizeof(char), "%s%s%s", cpath, FILE_SEPARATOR_S, item->d_name);

                        do
                        {
                            // Try to read link
                            ssize_t len = readlink(srcpath, dstpath, sizeof(srcpath)/sizeof(char) - 1);

                            // Analyze file
                            dstpath[len] = '\0';
                            if (len < 0)
                            {
                                nflags |= F_ISINVALID;
                                break;
                            }
                            if (stat(dstpath, &st) != 0)
                            {
                                nflags |= F_ISINVALID;
                                break;
                            }
                        } while (S_ISLNK(st.st_mode));

                        if (!(nFlags & F_ISINVALID))
                        {
                            if (S_ISDIR(st.st_mode))
                                nflags      |= F_ISDIR;
                            else if (!S_ISREG(st.st_mode))
                                nflags      |= F_ISOTHER;
                        }
                    }
                    else if (item->d_type != DT_REG)
                        nflags      |= F_ISOTHER;

                    // Add entry to list of found files
                    if ((xres = add_file_entry(&scanned, item->d_name, nflags)) != STATUS_OK)
                    {
                        closedir(fd);
                        destroy_file_entries(&scanned);
                        return xres;
                    }
                }

                // Close directory
                if (closedir(fd) != 0)
                {
                    destroy_file_entries(&scanned);
                    return STATUS_IO_ERROR;
                }

                // Readdir on successful end of directory should return result = NULL
                if (item != NULL)
                {
                    destroy_file_entries(&scanned);
                    return STATUS_IO_ERROR;
                }
            }
            else // Analyze errcode
            {
                const char *text = "unknown I/O error";
                switch (errcode)
                {
                    case EACCES:    text = "permission denied"; break;
                    case EBADF:     text = "not a valid file descriptor"; break;
                    case EMFILE:    text = "too many files opened"; break;
                    case ENFILE:    text = "too many files opened by system"; break;
                    case ENOENT:    text = "directory does not exist"; break;
                    case ENOMEM:    text = "not enough memory"; break;
                    case ENOTDIR:   text = "not a directory"; break;
                    default: break;
                }

                str.set_native("Access error: ");
                path.set_native(text);
                str.append(&path);
                sWWarning.set_text(&str);
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

            return STATUS_OK;
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
                fmask = sFilter.get_mask((sel < 0) ? 0 : sel);
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
                    if (ent->nFlags & (F_ISLINK | F_ISINVALID))
                        ok = ok && psrc->prepend((nFlags & F_ISINVALID) ? '!' : '~');
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
                if ((xres = lst->add(psrc, i)) != STATUS_OK)
                {
                    lst->clear();
                    return xres;
                }

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

        status_t LSPFileDialog::show_message(const char *heading, const char *title, const char *message)
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

                LSP_STATUS_ASSERT(pWMessage->add_button("OK"));
            }
            LSP_STATUS_ASSERT(pWMessage->set_heading(heading));
            LSP_STATUS_ASSERT(pWMessage->set_title(title));
            LSP_STATUS_ASSERT(pWMessage->set_message(message));

            return pWMessage->show(this);
        }

        LSPFileDialog::file_entry_t *LSPFileDialog::selected_entry()
        {
            ssize_t index = sWFiles.selection()->value();
            if (index < 0)
                return NULL;
            index = sWFiles.items()->value(index);
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
                    if (sFilter.get_extension((sel < 0) ? 0 : sel, &ext) == STATUS_OK)
                    {
                        lsp_trace("fname = %s, ext = %s", fname.get_native(), ext.get_native());
                        if (!fname.ends_with_nocase(&ext))
                            fname.append(&ext);
                        lsp_trace("fname = %s", fname.get_native());
                    }
                }

                if (LSPFileMask::is_dots(&fname) || (!LSPFileMask::valid_file_name(&fname)))
                    return show_message("Attention", "Attention", "The entered file name is not valid");

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
                    return show_message("Attention", "Attention", "The file name is not specified");

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
            struct stat st;
            int stat_result = stat(sSelected.get_native(), &st);

            if (enMode == FDM_SAVE_FILE)
            {
                if (sConfirm.length() <= 0)
                    return on_dlg_confirm(data);

                // Check that file exists and avoid confirmation if it doesn't
                lsp_trace("Checking file: %s", sSelected.get_native());
                if (stat_result != 0)
                    return on_dlg_confirm(data);
            }
            else
            {
                if (stat_result != 0)
                    return show_message("Attention", "Attention", "The selected file does not exist");

                if (sConfirm.length() <= 0)
                    return on_dlg_confirm(data);
            }

            if (pWConfirm == NULL)
            {
                // Create dialog object
                pWConfirm = new LSPMessageBox(pDisplay);
                if (pWConfirm == NULL)
                    return STATUS_NO_MEM;
                pWConfirm->init();

                pWConfirm->set_heading("Confirmation");
                pWConfirm->set_title("Confirmation");
                pWConfirm->add_button("Yes", slot_on_confirm, self());
                pWConfirm->add_button("No");
            }
            pWConfirm->set_message(&sConfirm);
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
            refresh_current_path();
            return STATUS_OK;
        }

        status_t LSPFileDialog::on_close(const ws_event_t *e)
        {
            ws_event_t ev = *e;
            return on_dlg_cancel(&ev);
        }

    } /* namespace ctl */
} /* namespace lsp */
