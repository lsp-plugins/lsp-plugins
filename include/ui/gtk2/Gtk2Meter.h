/*
 * Gtk2Meter.h
 *
 *  Created on: 30 мая 2016 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2METER_H_
#define UI_GTK2_GTK2METER_H_

#include <gtk/gtk.h>

namespace lsp
{
    
    class Gtk2Meter: public Gtk2CustomWidget
    {
        protected:
            enum type_t
            {
                MT_PEAK,
                MT_VU
            };

            enum conversion_t
            {
                MC_NONE,
                MC_LOG,
                MC_DB10,
                MC_DB20
            };

            enum flags_t
            {
                MF_MIN      = 1 << 0,
                MF_MAX      = 1 << 1,
                MF_LOG      = 1 << 2,
                MF_LOG_SET  = 1 << 3,
                MF_REV      = 1 << 4,
                MF_VALUE    = 1 << 5,
                MF_INACTIVE = 1 << 6
            };

        protected:
            IUIPort    *pPort;
            IUIPort    *pActivity;  // Activity flag
            float       fMin;       // Minimum displayed value
            float       fMax;       // Maximum displayed value
            float       fValue;     // Current value
            size_t      nAngle;     // Angle 0..3
            size_t      nMWidth;    // Width
            size_t      nMHeight;   // Height
            size_t      nFlags;     // Flags
            type_t      nType;      // Type of meter
            size_t      nConversion;// Data conversion
            size_t      nBorder;    // Border;
            Padding     sPadding;   // Padding
            Color       sBgColor;   // Background color
            Color       sIndColor;  // Indication color
            ColorHolder sColor;     // Color holder
            guint       hFunction;

        protected:
            float           normalized_value();
            float           get_value(size_t seg, size_t nseg) const;
            void            get_color(float rs, float re, Color &cl);
//            void            set_value(float value, bool force);
            static gboolean redraw_meter(gpointer ptr);
            void            format_meter(float value, char *buf, size_t n) const;

        public:
            Gtk2Meter(plugin_ui *ui);
            virtual ~Gtk2Meter();

        public:
            virtual void render();

            virtual void set(widget_attribute_t att, const char *value);

            virtual void end();

            virtual void resize(size_t &w, size_t &h);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2METER_H_ */
