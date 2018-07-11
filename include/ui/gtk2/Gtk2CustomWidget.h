/*
 * Gtk2CustomWidget.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2CUSTOMWIDGET_H_
#define UI_GTK2_GTK2CUSTOMWIDGET_H_

namespace lsp
{
    class Gtk2CustomWidget;

    struct _Gtk2Custom
    {
        GtkWidget           widget;
        Gtk2CustomWidget   *pImpl;
    };

    struct _Gtk2CustomClass
    {
        GtkWidgetClass parent_class;
        void           (*destroy)  (GtkObject *object);
    };

    typedef struct _Gtk2Custom Gtk2Custom;
    typedef struct _Gtk2CustomClass Gtk2CustomClass;

    class Gtk2CustomWidget : public Gtk2Widget
    {
        protected:
            static const GTypeInfo type_info;

            friend struct _Gtk2Custom;

            ssize_t     nLeft;
            ssize_t     nTop;
            size_t      nWidth;
            size_t      nHeight;

        private:
            static void class_init(Gtk2CustomClass *klass);
            static void destroy(GtkObject *object);

            static void init(GtkWidget *widget);
            static void realize(GtkWidget *widget);
            static gboolean expose(GtkWidget *widget, GdkEventExpose *event);
            static void size_request(GtkWidget *widget, GtkRequisition *requisition);
            static void size_allocate(GtkWidget *widget, GtkAllocation *allocation);
            static gboolean button_press(GtkWidget *widget, GdkEventButton *event);
            static gboolean motion_notify(GtkWidget *widget, GdkEventMotion *event);
            static gboolean enter_notify(GtkWidget *widget, GdkEventCrossing *event);
            static gboolean button_release(GtkWidget *widget, GdkEventButton *event);
            static gboolean scroll(GtkWidget *widget, GdkEventScroll *event);

        public:
            Gtk2CustomWidget(plugin_ui *ui, widget_t w_class, GType basic_class = GTK_TYPE_WIDGET);
            virtual ~Gtk2CustomWidget();

        public:
            virtual void render();

            virtual void resize(size_t &w, size_t &h);

            virtual void locate(ssize_t x, ssize_t y, size_t w, size_t h);

            virtual void button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_double_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_triple_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void motion(ssize_t x, ssize_t y, size_t state);

            virtual void scroll(ssize_t x, ssize_t y, size_t state, size_t direction);

            virtual void destroy();

            virtual void markRedraw();
    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2CUSTOMWIDGET_H_ */
