/*
 * CtlLabel.h
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLLABEL_H_
#define UI_CTL_CTLLABEL_H_

namespace lsp
{
    namespace ctl
    {
        enum ctl_label_type_t
        {
            CTL_LABEL_TEXT,
            CTL_LABEL_VALUE,
            CTL_LABEL_PARAM,
            CTL_STATUS_CODE
        };


        class CtlLabel: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                class PopupWindow: public LSPWindow
                {
                    private:
                        friend class CtlLabel;

                    protected:
                        CtlLabel   *pLabel;
                        LSPBox      sBox;
                        LSPEdit     sValue;
                        LSPLabel    sUnits;
                        LSPButton   sApply;
                        LSPButton   sCancel;

                    public:
                        explicit PopupWindow(CtlLabel *label, LSPDisplay *dpy);
                        virtual ~PopupWindow();

                        virtual status_t init();
                        virtual void destroy();
                };

                class Listener: public IStyleListener
                {
                    private:
                        friend class CtlLabel;

                    protected:
                        CtlLabel   *pLabel;

                    public:
                        inline explicit Listener(CtlLabel *lbl) { pLabel = lbl; }

                        virtual void notify(ui_atom_t property);
                };

                void do_destroy();

            protected:
                CtlColor            sColor;
                Listener            sListener;
                CtlPort            *pPort;
                ctl_label_type_t    enType;
                float               fValue;
                bool                bDetailed;
                bool                bSameLine;
                ssize_t             nUnits;
                ssize_t             nPrecision;
                ssize_t             nAtomID;
                PopupWindow        *pPopup;

            protected:
                static status_t slot_submit_value(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_change_value(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_cancel_value(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_dbl_click(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_key_up(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_button(LSPWidget *sender, void *ptr, void *data);

            protected:
                void            commit_value();
                bool            apply_value(const LSPString *value);

            public:
                explicit CtlLabel(CtlRegistry *src, LSPLabel *widget, ctl_label_type_t type);
                virtual ~CtlLabel();

                virtual void destroy();

            public:
                virtual void init();

                virtual void set(const char *name, const char *value);

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLLABEL_H_ */
