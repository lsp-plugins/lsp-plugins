/*
 * LSPFileDialog.h
 *
 *  Created on: 29 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_DIALOGS_LSPFILEDIALOG_H_
#define UI_TK_WIDGETS_DIALOGS_LSPFILEDIALOG_H_

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
                    F_ISOTHER   = 1 << 2,
                    F_ISINVALID = 1 << 3,
                    F_DOTDOT    = 1 << 4,
                    F_ISHIDDEN  = 1 << 5
                };

                typedef struct file_entry_t
                {
                    LSPString       sName;
                    size_t          nFlags;
                } file_entry_t;

                class LSPFileDialogFilter: public LSPFileFilter
                {
                    protected:
                        LSPFileDialog      *pDialog;

                    public:
                        LSPFileDialogFilter(LSPFileDialog *dlg);
                        virtual ~LSPFileDialogFilter();

                    protected:
                        virtual status_t item_updated(size_t idx, filter_t *flt);

                        virtual status_t item_removed(size_t idx, filter_t *flt);

                        virtual status_t item_added(size_t idx, filter_t *flt);

                        virtual void default_updated(ssize_t idx);
                };

            protected:
                LSPEdit             sWPath;
                LSPEdit             sWSearch;
                LSPComboBox         sWFilter;
                LSPListBox          sWFiles;
                LSPButton           sWAction;
                LSPButton           sWCancel;
                LSPBox              sVBox;
                LSPBox              sHBox;
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

                LSPString           sConfirm;       // Confirmation message
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

                virtual status_t    on_dlg_action(void *data);
                virtual status_t    on_dlg_confirm(void *data);
                virtual status_t    on_dlg_cancel(void *data);
                virtual status_t    on_dlg_search(void *data);
                virtual status_t    on_dlg_mouse_dbl_click(void *data);
                virtual status_t    on_dlg_list_change(void *data);
                virtual status_t    on_dlg_go(void *data);
                virtual status_t    on_dlg_up(void *data);

                void                do_destroy();
                status_t            refresh_current_path();
                ssize_t             default_index(ssize_t val);
                status_t            add_label(LSPWidgetContainer *c, const char *text, LSPLabel **label = NULL);
                status_t            add_ext_button(LSPWidgetContainer *c, const char *text);
                status_t            add_file_entry(cvector<file_entry_t> *dst, const char *name, size_t flags);
                void                destroy_file_entries(cvector<file_entry_t> *dst);
                status_t            apply_filters();
                bool                check_mask_match(const LSPString *mask, const LSPString *item);
                bool                check_multimask_match();
                static int          cmp(const file_entry_t *a, const file_entry_t *b);

                void                sync_mode();
                status_t            build_full_path(LSPString *dst, const LSPString *fname);
                status_t            show_message(const char *heading, const char *main, const char *message);
                file_entry_t       *selected_entry();

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

                inline status_t get_action_title(LSPString *dst) const { return sWAction.get_title(dst); };
                inline const char *action_title() const { return sWAction.title(); };

                inline status_t get_cancel_title(LSPString *dst) const { return sWAction.get_title(dst); };
                inline const char *cancel_title() const { return sWAction.title(); };

                inline status_t get_confirmation(LSPString *dst) const { return (dst->set(&sConfirm)) ? STATUS_OK : STATUS_NO_MEM; };
                inline const char *confirmation() const { return sConfirm.get_native(); };

                inline LSPFileFilter *filter() { return &sFilter; }

//                inline size_t filters() const { return vFilters.size(); }
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

                status_t set_confirmation(const LSPString *value);
                status_t set_confirmation(const char *value);

                inline status_t set_action_title(const LSPString *value) { return sWAction.set_title(value); };
                inline status_t set_action_title(const char *value) { return sWAction.set_title(value); };

                inline status_t set_cancel_title(const LSPString *value) { return sWCancel.set_title(value); };
                inline status_t set_cancel_title(const char *value) { return sWCancel.set_title(value); };

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
