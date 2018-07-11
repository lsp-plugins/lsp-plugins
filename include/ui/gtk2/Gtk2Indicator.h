/*
 * Gtk2Indicator.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2INDICATOR_H_
#define _UI_GTK2_GTK2INDICATOR_H_

namespace lsp
{
    class Gtk2Indicator: public Gtk2CustomWidget
    {
        private:
            static const size_t DIGITS_DFL          = 5;
            static const size_t ITEMS_MAX           = 16;

            enum format_t
            {
                F_UNKNOWN,
                F_FLOAT,
                F_INT,
                F_TIME
            };

            enum flags_t
            {
                F_SIGN          = 1 << 0,
                F_PLUS          = 1 << 1,
                F_PAD_ZERO      = 1 << 2,
                F_FIXED_PREC    = 1 << 3,
                F_NO_ZERO          = 1 << 4,
                F_DOT           = 1 << 5,
                F_TOLERANCE     = 1 << 6
            };

            typedef struct item_t
            {
                char        type;
                size_t      digits;
                ssize_t     precision;
            } item_t;

            typedef struct buffer_t
            {
                char       *data;
                size_t      offset;
                size_t      len;

            } buffer_t;

            Color           sColor;
            Color           sTextColor;
            Color           sBgColor;
            IUIPort   *pPort;
            float           fValue;

            // Format
            format_t        nFormat;
            size_t          sDigits;
            size_t          nItems;
            size_t          nFlags;
            item_t          vItems[ITEMS_MAX];

        private:
            bool parseFormat(const char *format);
            void drawDigit(cairo_t *cr, int x, int y, char ch, char mod);

            bool formatTimeInterval(buffer_t &buf, item_t *descr, double value);
            bool formatFloat(buffer_t &buf, item_t *descr, double value);
            bool formatInt(buffer_t &buf, item_t *descr, ssize_t value);
            bool format(double value, char *buf, size_t len);

            static bool parseLong(char *p, char **ret, long *value);

            static bool append_buf(buffer_t &buf, char c);
            static bool append_buf(buffer_t &buf, char *s);
            static bool append_buf(buffer_t &buf, char *s, size_t count);

        protected:
            virtual void    draw(cairo_t *cr);

        public:
            Gtk2Indicator(plugin_ui *ui);
            virtual ~Gtk2Indicator();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void end();


            virtual void resize(size_t &w, size_t &h);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2INDICATOR_H_ */
