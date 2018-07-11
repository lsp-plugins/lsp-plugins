/*
 * Gtk2Label.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2LABEL_H_
#define _UI_GTK2_GTK2LABEL_H_

namespace lsp
{
    enum label_type_t
    {
        LT_TEXT,
        LT_VALUE,
        LT_PARAM
    };

    class Gtk2Label: public Gtk2Widget
    {
        private:
            char           *sText;
            Color           sColor;
            Color           sBgColor;
            IUIPort        *pPort;
            label_type_t    enType;
            float           fValue;
            float           fVAlign;
            float           fHAlign;
            bool            bDetailed;
            ssize_t         nFontSize;
            ssize_t         nUnits;

        private:
            void updateText();

        public:
            Gtk2Label(plugin_ui *ui, label_type_t type = LT_TEXT);
            virtual ~Gtk2Label();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void end();

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2LABEL_H_ */
