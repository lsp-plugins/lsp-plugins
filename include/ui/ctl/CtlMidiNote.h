/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 дек. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UI_CTL_CTLMIDINOTE_H_
#define UI_CTL_CTLMIDINOTE_H_

namespace lsp
{
    namespace ctl
    {
        class CtlMidiNote: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                class PopupWindow: public LSPWindow
                {
                    private:
                        friend class CtlMidiNote;

                    protected:
                        CtlMidiNote    *pNote;
                        LSPBox          sBox;
                        LSPEdit         sValue;
                        LSPLabel        sUnits;
                        LSPButton       sApply;
                        LSPButton       sCancel;

                    public:
                        explicit PopupWindow(CtlMidiNote *note, LSPDisplay *dpy);
                        virtual ~PopupWindow();

                        virtual status_t init();
                        virtual void destroy();
                };

            protected:
                size_t          nNote;
                CtlPort        *pNote;
                CtlPort        *pOctave;
                CtlPort        *pValue;
                PopupWindow    *pPopup;
                CtlColor        sColor;
                CtlColor        sTextColor;

            protected:
                static status_t slot_submit_value(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_change_value(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_cancel_value(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_dbl_click(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_key_up(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_button(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_scroll(LSPWidget *sender, void *ptr, void *data);

            protected:
                void            do_destroy();
                void            commit_value(float value);
                bool            apply_value(const LSPString *value);
                void            apply_value(ssize_t value);

            public:
                explicit CtlMidiNote(CtlRegistry *src, LSPIndicator *widget);
                virtual ~CtlMidiNote();

                virtual void    destroy();

            public:
                virtual void    init();

                virtual void    end();

                virtual void    set(widget_attribute_t att, const char *value);

                virtual void    notify(CtlPort *port);

        };

    } /* namespace ctl */
} /* namespace lsp */



#endif /* UI_CTL_CTLMIDINOTE_H_ */
