/*
 * IWidgetCore.h
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#ifndef INCLUDE_UI_IWIDGETCORE_H_
#define INCLUDE_UI_IWIDGETCORE_H_

namespace lsp
{
    class IWidgetCore
    {
        protected:
            ssize_t     nLeft;
            ssize_t     nTop;
            size_t      nWidth;
            size_t      nHeight;
            bool        bVisible;

        public:
            IWidgetCore();
            virtual ~IWidgetCore();

        public:
            /** Get widget class
             *
             * @return widget class
             */
            virtual widget_t getClass() const;

            /** Render widget
             *
             * @param surface surface to perform rendering
             */
            virtual void render(ISurface *s);

            /** Realize widget
             *
             * @param r widget realization parameters
             */
            virtual void realize(const ui_realize_t *r);

            /** Request for size
             *
             * @param r minium and maximum dimensions of the widget
             */
            virtual void size_request(ui_size_request_t *r);

            /** Handle UI event
             *
             * @param e UI event
             */
            virtual void handle_event(const ui_event_t *e);

        public:
            /** Handle button press event
             * @param e event
             */
            virtual void on_mouse_down(const ui_event_t *e);

            /** Handle button release event
             * @param e event
             */
            virtual void on_mouse_up(const ui_event_t *e);

            /** Handle key press event
             * @param e event
             */
            virtual void on_key_down(const ui_event_t *e);

            /** Handle key release event
             * @param e event
             */
            virtual void on_key_up(const ui_event_t *e);

            /** Handle mouse motion event
             * @param e event
             */
            virtual void on_motion(const ui_event_t *e);



    };

} /* namespace lsp */

#endif /* INCLUDE_UI_IWIDGETCORE_H_ */
