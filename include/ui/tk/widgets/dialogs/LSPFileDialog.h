/*
 * LSPFileDialog.h
 *
 *  Created on: 29 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_DIALOGS_LSPFILEDIALOG_H_
#define UI_TK_WIDGETS_DIALOGS_LSPFILEDIALOG_H_

#include <core/files/bookmarks.h>

namespace lsp
{
    namespace tk
    {

        enum file_dialog_mode_t
        {
            FDM_OPEN_FILE,
            FDM_SAVE_FILE
        };

        class LSPFileDialog: public LSPWindow
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum
                {
                    F_ISDIR     = 1 << 0,
                    F_ISLINK    = 1 << 1,
                    F_ISREG     = 1 << 2,
                    F_ISOTHER   = 1 << 3,
                    F_ISINVALID = 1 << 4,
                    F_DOTDOT    = 1 << 5,
                    F_ISHIDDEN  = 1 << 6
                };

                typedef lsp::bookmarks::bookmark_t  bookmark_t;

                typedef struct file_entry_t
                {
                    LSPString       sName;
                    size_t          nFlags;
                } file_entry_t;

                typedef struct bm_entry_t
                {
                    LSPHyperlink            sHlink;
                    io::Path                sPath;
                    bookmarks::bookmark_t   sBookmark;

                    inline bm_entry_t(LSPDisplay *dpy): sHlink(dpy) {}
                } bm_entry_t;

                class LSPFileDialogFilter: public LSPFileFilter
                {
                    protected:
                        LSPFileDialog      *pDialog;

                    public:
                        LSPFileDialogFilter(LSPFileDialog *dlg);
                        virtual ~LSPFileDialogFilter();

                    protected:
                        virtual status_t item_updated(size_t idx, LSPFileFilterItem *flt);

                        virtual status_t item_removed(size_t idx, LSPFileFilterItem *flt);

                        virtual status_t item_added(size_t idx, LSPFileFilterItem *flt);

                        virtual void default_updated(ssize_t idx);
                };

                class ConfirmMsg: public LSPLocalString
                {
                    public:
                        inline ConfirmMsg(LSPFileDialog *dlg): LSPLocalString(dlg) {}

                        virtual void sync();
                };

            protected:
                LSPEdit             sWPath;
                LSPEdit             sWSearch;
                LSPComboBox         sWFilter;
                LSPListBox          sWFiles;
                LSPButton           sWAction;
                LSPButton           sWCancel;
                LSPGrid             sMainGrid;
                LSPScrollBox        sSBBookmarks;
                LSPAlign            sSBAlign;
                LSPBox              sBookmarks;
                LSPMenu             sBMPopup;
                LSPButton           sBMAdd;
                LSPBox              sHBox;
                LSPBox              sWarnBox;
                LSPAlign            sAppendExt;
                LSPButton           wAutoExt;
                LSPButton           wGo;
                LSPButton           wUp;
                LSPBox              wPathBox;
                LSPMessageBox      *pWConfirm;
                LSPMessageBox      *pWMessage;
                LSPSlot             sAction;
                LSPSlot             sCancel;
                LSPLabel            sWWarning;
                LSPLabel           *pWSearch;
                file_dialog_mode_t  enMode;
                cvector<LSPWidget>  vWidgets;
                cvector<file_entry_t> vFiles;
                cvector<bm_entry_t> vBookmarks;
                bm_entry_t         *pSelBookmark;
                bm_entry_t         *pPopupBookmark;
                bool                bUseConfirm;

                ConfirmMsg          sConfirm;       // Confirmation message
                LSPString           sSelected;
                LSPFileDialogFilter sFilter;
                size_t              nDefaultFilter;
                size_t              nUIDGen;

            protected:
                static status_t     slot_on_action(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_confirm(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_cancel(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_search(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_mouse_dbl_click(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_list_change(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_go(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_up(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_path_key_up(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_add(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_popup(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_open(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_follow(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_copy(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_delete(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_up(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_down(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_first(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_bm_menu_last(LSPWidget *sender, void *ptr, void *data);

                virtual status_t    on_dlg_action(void *data);
                virtual status_t    on_dlg_confirm(void *data);
                virtual status_t    on_dlg_cancel(void *data);
                virtual status_t    on_dlg_search(void *data);
                virtual status_t    on_dlg_mouse_dbl_click(void *data);
                virtual status_t    on_dlg_list_change(void *data);
                virtual status_t    on_dlg_go(void *data);
                virtual status_t    on_dlg_up(void *data);
                virtual status_t    on_path_key_up(ws_event_t *e);

                status_t            on_bm_submit(LSPWidget *sender);

                void                do_destroy();
                status_t            refresh_current_path();
                ssize_t             default_index(ssize_t val);
                status_t            add_label(LSPWidgetContainer *c, const char *text, float align = 0.0f, LSPLabel **label = NULL);
                status_t            add_menu_item(LSPMenu *m, const char *text, ui_event_handler_t handler);
                status_t            add_ext_button(LSPWidgetContainer *c, const char *text);
                status_t            add_file_entry(cvector<file_entry_t> *dst, const char *name, size_t flags);
                void                destroy_file_entries(cvector<file_entry_t> *dst);
                status_t            apply_filters();
                bool                check_mask_match(const LSPString *mask, const LSPString *item);
                bool                check_multimask_match();
                static int          cmp(const file_entry_t *a, const file_entry_t *b);

                void                sync_mode();
                status_t            build_full_path(LSPString *dst, const LSPString *fname);
                status_t            show_message(const char *title, const char *heading, const char *message);
                file_entry_t       *selected_entry();

                void                drop_bookmarks();
                static status_t     read_lsp_bookmarks(cvector<bookmark_t> &vbm);
                static status_t     read_gtk2_bookmarks(cvector<bookmark_t> &vbm);
                static status_t     read_gtk3_bookmarks(cvector<bookmark_t> &vbm);
                static status_t     read_qt5_bookmarks(cvector<bookmark_t> &vbm);
                status_t            save_bookmarks(cvector<bookmark_t> *vbm);
                status_t            sync_bookmarks();
                status_t            refresh_bookmarks();
                status_t            select_current_bookmark();
                status_t            remove_bookmark(bm_entry_t *entry);
                status_t            init_bm_popup_menu();
                status_t            add_new_bookmark();
                status_t            init_entry(bm_entry_t *ent, const io::Path *path);
                bm_entry_t         *find_bookmark(LSPWidget *sender);

            public:
                explicit LSPFileDialog(LSPDisplay *dpy);
                virtual ~LSPFileDialog();

                virtual status_t init();
                virtual void destroy();

            public:
                inline status_t get_path(LSPString *dst) const { return sWPath.get_text(dst); };
                inline const char *path() const { return sWPath.text(); };

                inline status_t get_search(LSPString *dst) const { return sWSearch.get_text(dst); };
                inline const char *search() const { return sWSearch.text(); };

                inline LSPLocalString *action_title() { return sWAction.title(); };
                inline const LSPLocalString *action_title() const { return sWAction.title(); };

                inline LSPLocalString *cancel_title() { return sWCancel.title(); };
                inline const LSPLocalString *cancel_title() const { return sWCancel.title(); };

                inline bool         use_confirm() const { return bUseConfirm; }

                inline LSPLocalString *confirm() { return &sConfirm; }
                inline const LSPLocalString *confirm() const { return &sConfirm; }

                inline LSPFileFilter *filter() { return &sFilter; }

                status_t get_filter(size_t idx, LSPString *pattern, LSPString *title);
                inline size_t default_filter() const { return nDefaultFilter; }

                inline const char *selected_file() const { return sSelected.get_native(); };
                inline status_t get_selected_file(LSPString *dst) { return (dst->set(&sSelected)) ? STATUS_OK : STATUS_NO_MEM; };

                inline file_dialog_mode_t mode() const { return enMode; }

                inline bool auto_extension() const { return wAutoExt.is_down(); }

            public:
                status_t set_mode(file_dialog_mode_t mode);

                status_t set_path(const LSPString *value);
                status_t set_path(const char *value);

                status_t set_search(const LSPString *value);
                status_t set_search(const char *value);

                status_t set_use_confirm(bool use);

                inline status_t    bind_action(ui_event_handler_t handler, void *arg = NULL) { return sAction.bind(handler, arg); };
                inline status_t    bind_cancel(ui_event_handler_t handler, void *arg = NULL) { return sCancel.bind(handler, arg); };

                inline void set_auto_extension(bool set = true) { wAutoExt.set_down(set); }

            public:
                virtual status_t on_show();

                virtual status_t on_close(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_DIALOGS_LSPFILEDIALOG_H_ */
