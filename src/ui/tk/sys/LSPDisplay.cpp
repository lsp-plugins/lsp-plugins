/*
 * LSPDisplay.cpp
 *
 *  Created on: 19 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>
#include <ui/tk/tk.h>

#if defined(USE_X11_DISPLAY)
    #include <ui/ws/x11/ws.h>
#else
    #error "Unsupported platform"
#endif

namespace lsp
{
    namespace tk
    {
        LSPDisplay::LSPDisplay()
        {
            pDisplay        = NULL;
        }

        LSPDisplay::~LSPDisplay()
        {
            do_destroy();
        }
        
        void LSPDisplay::do_destroy()
        {
            // Auto-destruct widgets
            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.at(i);
                if (ptr->id != NULL)
                {
                    free(ptr->id);
                    ptr->id     = NULL;
                }
                if (ptr->widget != NULL)
                {
                    ptr->widget->destroy();
                    delete ptr->widget;
                    ptr->widget = NULL;
                }
            }
            sWidgets.flush();

            // Execute slot
            sSlots.execute(LSPSLOT_DESTROY, NULL);
            sSlots.destroy();

            // Destroy display
            if (pDisplay != NULL)
            {
                pDisplay->destroy();
                delete pDisplay;
                pDisplay = NULL;
            }
        }

        status_t LSPDisplay::init(int argc, const char **argv)
        {
            // Create display dependent on the platform
            #ifdef USE_X11_DISPLAY
                pDisplay        = new x11::X11Display();
            #else
                #error "Unknown windowing system configuration"
            #endif /* USE_X11_DISPLAY */

            // Analyze display pointer
            if (pDisplay == NULL)
                return STATUS_NO_MEM;

            // Initialize display
            status_t result = pDisplay->init(argc, argv);
            if (result != STATUS_OK)
                return result;

            // Create slots
            LSPSlot *slot = sSlots.add(LSPSLOT_DESTROY);
            if (slot == NULL)
                return STATUS_NO_MEM;
            slot = sSlots.add(LSPSLOT_RESIZE);
            if (slot == NULL)
                return STATUS_NO_MEM;

            // Initialize theme
            sTheme.init(this);

            return STATUS_OK;
        }

        void LSPDisplay::destroy()
        {
            do_destroy();
        }

        status_t LSPDisplay::main()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->main();
        }

        status_t LSPDisplay::main_iteration()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->main_iteration();
        }

        void LSPDisplay::quit_main()
        {
            if (pDisplay != NULL)
                pDisplay->quit_main();
        }

        ISurface *LSPDisplay::create_surface(size_t width, size_t height)
        {
            return (pDisplay != NULL) ? pDisplay->createSurface(width, height) : NULL;
        }

        void LSPDisplay::sync()
        {
            if (pDisplay != NULL)
                pDisplay->sync();
        }

        status_t LSPDisplay::add(LSPWidget *widget, const char *id)
        {
            if (id != NULL)
            {
                // Check that widget does not exist
                LSPWidget *widget = get(id);
                if (widget != NULL)
                    return STATUS_ALREADY_EXISTS;
            }

            // Append widget
            item_t *w   = sWidgets.append();
            w->widget   = widget;
            if (id != NULL)
            {
                w->id       = strdup(id);
                if (w->id == NULL)
                {
                    sWidgets.remove_last();
                    return STATUS_NO_MEM;
                }
            }
            else
                w->id       = NULL;

            return STATUS_OK;
        }

        LSPWidget **LSPDisplay::add(const char *id)
        {
            if (id != NULL)
            {
                // Check that widget does not exist
                LSPWidget *widget = get(id);
                if (widget != NULL)
                    return NULL;
            }

            // Append widget
            item_t *w   = sWidgets.append();
            if (id != NULL)
            {
                w->id       = strdup(id);
                if (w->id == NULL)
                {
                    sWidgets.remove_last();
                    return NULL;
                }
            }
            else
                w->id       = NULL;

            return &w->widget;
        }

        LSPWidget *LSPDisplay::get(const char *id)
        {
            if (id == NULL)
                return NULL;

            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.at(i);
                if (ptr->id == NULL)
                    continue;
                if (!strcmp(ptr->id, id))
                    return ptr->widget;
            }

            return NULL;
        }

        LSPWidget *LSPDisplay::remove(const char *id)
        {
            if (id == NULL)
                return NULL;

            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.at(i);
                if (ptr->id == NULL)
                    continue;
                if (!strcmp(ptr->id, id))
                {
                    LSPWidget *result = ptr->widget;
                    sWidgets.remove(i);
                    return result;
                }
            }

            return NULL;
        }

        bool LSPDisplay::remove(LSPWidget *widget)
        {
            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.at(i);
                if (ptr->widget == widget)
                {
                    sWidgets.remove(i);
                    return true;
                }
            }

            return false;
        }

        bool LSPDisplay::exists(LSPWidget *widget)
        {
            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.at(i);
                if (ptr->widget == widget)
                    return true;
            }

            return false;
        }

        status_t LSPDisplay::get_clipboard(size_t id, IDataSink *sink)
        {
            return pDisplay->getClipboard(id, sink);
        }

        status_t LSPDisplay::set_clipboard(size_t id, IDataSource *src)
        {
            return pDisplay->setClipboard(id, src);
        }

        status_t LSPDisplay::reject_drag()
        {
            return pDisplay->rejectDrag();
        }

        status_t LSPDisplay::accept_drag(IDataSink *sink, drag_t action, bool internal, const realize_t *r)
        {
            return pDisplay->acceptDrag(sink, action, internal, r);
        }

        const char * const *LSPDisplay::get_drag_mime_types()
        {
            return pDisplay->getDragContentTypes();
        }
    }

} /* namespace lsp */
