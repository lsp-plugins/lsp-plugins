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
                cvector<item_t>         sWidgets;
                cvector<LSPWidget>      vGarbage;
                cvector<char>           vAtoms;
                LSPSlotSet              sSlots;
                LSPTheme                sTheme;
                IDictionary            *pDictionary;
                IDisplay               *pDisplay;

            protected:
                void    do_destroy();

            protected:
                static status_t     main_task_handler(ws::timestamp_t time, void *arg);

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
                 * @param argc number of additional arguments
                 * @param argv list of additional arguments
                 * @return status of operation
                 */
                status_t init(int argc, const char **argv);

                /** Initialize display
                 *
                 * @param dpy underlying display object
                 * @param argc number of additional arguments
                 * @param argv list of additional arguments
                 * @return status of operation
                 */
                status_t init(ws::IDisplay *dpy, int argc, const char **argv);

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

                /**
                 * Get the dictionary that contains localization data
                 * @return dictionary object
                 */
                inline IDictionary *dictionary()            { return pDictionary; }

                /** Get current display theme
                 *
                 * @return current display theme
                 */
                inline LSPTheme *theme()                    { return &sTheme; }

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                ui_atom_t atom_id(const char *name);

                /**
                 * Get atom name by identifier
                 * @param name atom name or NULL
                 * @return atom identifier
                 */
                const char *atom_name(ui_atom_t id);

                /**
                 * Get clipboard data
                 * @param id clipboard identifier
                 * @param sink data sink
                 * @return status of operation
                 */
                status_t get_clipboard(size_t id, IDataSink *sink);

                /**
                 * Set clipboard data
                 * @param id clipboard identifier
                 * @param src data source
                 * @return status of operation
                 */
                status_t set_clipboard(size_t id, IDataSource *src);

                /**
                 * Reject drag event
                 * @return status of operation
                 */
                status_t reject_drag();

                /**
                 * Accept drag request
                 * @param sink the sink that will handle data transfer
                 * @param action drag action
                 * @param internal true if we want to receive notifications inside of the drag rectangle
                 * @param r parameters of the drag rectangle, can be NULL
                 * @return status of operation
                 */
                status_t accept_drag(IDataSink *sink, drag_t action, bool internal, const realize_t *r);

                /**
                 * Get NULL-terminated list of provided MIME types for a drag
                 * @return NULL-terminated list of strings
                 */
                const char * const *get_drag_mime_types();

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

                /**
                 * Queue widget for removal. Because all widget operations are done in the
                 * main event loop, it's unsafe to destroy widget immediately in callback
                 * handlers. This method allows to put the widget to the garbage queue that
                 * will be recycled at the end of the event loop iteration.
                 *
                 * @param widget widget to be queued for destroy
                 * @return status of operation
                 */
                status_t queue_destroy(LSPWidget *widget);
        };
    }

} /* namespace lsp */

#endif /* UI_TK_LSPDISPLAY_H_ */
