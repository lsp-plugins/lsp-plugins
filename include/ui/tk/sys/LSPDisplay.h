/*
 * LSPDisplay.h
 *
 *  Created on: 19 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPDISPLAY_H_
#define UI_TK_LSPDISPLAY_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;
        class LSPSlotSet;
        class LSPTheme;

        /** Main display
         *
         */
        class LSPDisplay
        {
            protected:
                typedef struct item_t
                {
                    LSPWidget      *widget;
                    char           *id;
                } item_t;

            protected:
                cstorage<item_t>        sWidgets;
                LSPSlotSet              sSlots;
                LSPTheme                sTheme;
                IDisplay               *pDisplay;

            protected:
                void    do_destroy();

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                /** Constructor
                 *
                 */
                explicit LSPDisplay();

                /** Destructor
                 *
                 */
                virtual ~LSPDisplay();

                /** Initialize display
                 *
                 * @param argc
                 * @param argv
                 * @return status of operation
                 */
                status_t init(int argc, const char **argv);

                /** Destroy display
                 *
                 */
                void destroy();

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Execute main loop function
                 *
                 * @return status of operation
                 */
                status_t main();

                /** Execute one main loop iteration
                 *
                 * @return status of operation
                 */
                status_t main_iteration();

                /** Interrupt main loop function
                 *
                 */
                void quit_main();

                /**
                 * Synchronize display
                 */
                void sync();

                /** Register widget, it will be automatically destroyed
                 *
                 * @param widget widget to queue
                 * @param id widget identifier
                 * @return status of operation
                 */
                status_t add(LSPWidget *widget, const char *id = NULL);

                /** Add widget and return pointer to write to
                 *
                 * @param id widget identifier
                 * @return pointer or NULL
                 */
                LSPWidget **add(const char *id);

                /** Get widget by it's identifier
                 *
                 * @param id widget identifier
                 * @return pointer to widget
                 */
                LSPWidget *get(const char *id);

                /** Deregister widget by it's identifier
                 *
                 * @param id widget identifier
                 * @return pointer to widget or NULL
                 */
                LSPWidget *remove(const char *id);

                /** Deregister widget by it's pointer
                 *
                 * @param widget widget to remove from queue
                 * @return status of operation
                 */
                bool remove(LSPWidget *widget);

                /** Check that widget exists
                 *
                 * @param id widget pointer
                 * @return true on success
                 */
                bool exists(LSPWidget *widget);

                /** Check that widget exists
                 *
                 * @param id widget identifier
                 * @return true on success
                 */
                inline bool exists(const char *id) { return get(id) != NULL; }

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /** Get slots
                 *
                 * @return slots
                 */
                inline LSPSlotSet *slots()                  { return &sSlots; }

                /** Get slot
                 *
                 * @param id slot identifier
                 * @return pointer to slot or NULL
                 */
                inline LSPSlot *slot(ui_slot_t id)          { return sSlots.slot(id); }

                /** Return native display handle
                 *
                 * @return native display handle
                 */
                inline IDisplay *display()                  { return pDisplay; }

                /** Get current display theme
                 *
                 * @return current display theme
                 */
                inline LSPTheme *theme()                    { return &sTheme; }

                /** Fetch clipboard object
                 *
                 * @param id clipboard identifier
                 * @param ctype content type
                 * @param handler callback handler
                 * @param arg argument for callback handler
                 * @return
                 */
                virtual status_t fetch_clipboard(size_t id, const char *ctype, clipboard_handler_t handler, void *arg = NULL);

                /** Get clipboard by it's identifier
                 *
                 * @param wnd the window that is owner of the clipboard
                 * @param id clipboard identifier
                 * @param c the clipboard data holder object
                 * @return pointer to clipboard or NULL if not present
                 */
                virtual status_t write_clipboard(size_t id, IClipboard *c);

                /**
                 * Get clipboard data
                 * @param id clipboard identifier
                 * @param sink data sink
                 * @return status of operation
                 */
                virtual status_t get_clipboard(size_t id, IDataSink *sink);

                /** Get screen size
                 *
                 * @param screen screen identifier
                 * @param w screen width
                 * @param h screen height
                 * @return status of operation
                 */
                inline status_t screen_size(size_t screen, ssize_t *w, ssize_t *h) { return pDisplay->screen_size(screen, w, h); }

                /** Create surface
                 *
                 * @param width surface width
                 * @param height surface height
                 * @return pointer to created surface or NULL
                 */
                ISurface *create_surface(size_t width, size_t height);
        };
    }

} /* namespace lsp */

#endif /* UI_TK_LSPDISPLAY_H_ */
