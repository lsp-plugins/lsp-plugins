/*
 * LSPDisplay.cpp
 *
 *  Created on: 19 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>
#include <ui/tk/tk.h>

#include <core/i18n/Dictionary.h>

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
            pDictionary     = NULL;
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
                if (ptr == NULL)
                    continue;

                ptr->id         = NULL;
                if (ptr->widget != NULL)
                {
                    ptr->widget->destroy();
                    delete ptr->widget;
                    ptr->widget = NULL;
                }
                ::free(ptr);
            }
            sWidgets.flush();

            // Execute slot
            sSlots.execute(LSPSLOT_DESTROY, NULL);
            sSlots.destroy();

            // Destroy atoms
            for (size_t i=0, n=vAtoms.size(); i<n; ++i)
            {
                char *ptr = vAtoms.at(i);
                if (ptr != NULL)
                    ::free(ptr);
            }
            vAtoms.flush();

            // Destroy display
            if (pDisplay != NULL)
            {
                pDisplay->destroy();
                delete pDisplay;
                pDisplay = NULL;
            }

            // Destroy dictionary
            if (pDictionary != NULL)
            {
                delete pDictionary;
                pDictionary = NULL;
            }
        }

        status_t LSPDisplay::main_task_handler(ws::timestamp_t time, void *arg)
        {
            LSPDisplay *_this   = reinterpret_cast<LSPDisplay *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;

            for (size_t i=0, n=_this->vGarbage.size(); i<n; ++i)
            {
                // Get widget
                LSPWidget *w = _this->vGarbage.at(i);
                if (w == NULL)
                    continue;

                // Widget is registered?
                for (size_t j=0, m=_this->sWidgets.size(); j<m; )
                {
                    item_t *item = _this->sWidgets.at(j);
                    if (item->widget == w)
                    {
                        // Free the binding
                        _this->sWidgets.remove(j, true);
                        item->id        = NULL;
                        item->widget    = NULL;
                        ::free(item);
                    }
                    else
                        ++j;
                }

                // Destroy widget
                w->destroy();
                delete w;
            }

            // Cleanup garbage
            _this->vGarbage.flush();

            return STATUS_OK;
        }

        status_t LSPDisplay::init(int argc, const char **argv)
        {
            IDisplay *dpy = NULL;

            // Create display dependent on the platform
            #ifdef USE_X11_DISPLAY
                dpy         = new x11::X11Display();
            #else
                #error "Unknown windowing system configuration"
            #endif /* USE_X11_DISPLAY */

            // Analyze display pointer
            if (dpy == NULL)
                return STATUS_NO_MEM;

            status_t res = dpy->init(argc, argv);
            if (res == STATUS_OK)
                res = init(dpy, argc, argv);

            if (res != STATUS_OK)
            {
                dpy->destroy();
                delete dpy;
            }

            return res;
        }

        status_t LSPDisplay::init(IDisplay *dpy, int argc, const char **argv)
        {
            // Should be non-null
            if (dpy == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Initialize dictionary
            Dictionary *dict = new Dictionary();
            if (dict == NULL)
                return STATUS_NO_MEM;

            // Initialize display
            pDisplay        = dpy;
            pDisplay->set_main_callback(main_task_handler, this);

            // Create slots
            LSPSlot *slot   = sSlots.add(LSPSLOT_DESTROY);
            if (slot == NULL)
            {
                delete dict;
                return STATUS_NO_MEM;
            }
            slot = sSlots.add(LSPSLOT_RESIZE);
            if (slot == NULL)
            {
                delete dict;
                return STATUS_NO_MEM;
            }

            // Initialize theme
            sTheme.init(this);
            pDictionary     = dict;

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
            return (pDisplay != NULL) ? pDisplay->create_surface(width, height) : NULL;
        }

        void LSPDisplay::sync()
        {
            if (pDisplay != NULL)
                pDisplay->sync();
        }

        status_t LSPDisplay::add(LSPWidget *widget, const char *id)
        {
            LSPWidget **w = add(id);
            if (w == NULL)
                return STATUS_NO_MEM;

            *w = widget;
            return STATUS_OK;
        }

        LSPWidget **LSPDisplay::add(const char *id)
        {
            // Prevent from duplicates
            if (id != NULL)
            {
                // Check that widget does not exist
                LSPWidget *widget = get(id);
                if (widget != NULL)
                    return NULL;
            }

            // Allocate memory
            size_t slen     = (id != NULL) ? (::strlen(id) + 1) * sizeof(char) : 0;
            size_t to_alloc = ALIGN_SIZE(sizeof(item_t) + slen, DEFAULT_ALIGN);

            // Append widget
            item_t *w   = reinterpret_cast<item_t *>(::malloc(to_alloc));
            if (w == NULL)
                return NULL;
            else if (!sWidgets.add(w))
            {
                ::free(w);
                return NULL;
            }

            // Initialize widget
            w->widget       = NULL;
            w->id           = NULL;
            if (id != NULL)
            {
                w->id           = reinterpret_cast<char *>(&w[1]);
                ::memcpy(w->id, id, slen);
            }

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

        ui_atom_t LSPDisplay::atom_id(const char *name)
        {
            if (name == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Find existing atom
            size_t last = vAtoms.size();
            for (size_t i=0; i<last; ++i)
            {
                const char *aname = vAtoms.at(i);
                if (!::strcmp(aname, name))
                    return i;
            }

            // Allocate new atom name
            char *aname         = ::strdup(name);
            if (aname == NULL)
                return -STATUS_NO_MEM;

            // Insert atom name to the found position
            if (!vAtoms.add(aname))
            {
                ::free(aname);
                return -STATUS_NO_MEM;
            }

            return last;
        }

        const char *LSPDisplay::atom_name(ui_atom_t id)
        {
            if (id < 0)
                return NULL;
            return vAtoms.get(id);
        }

        status_t LSPDisplay::get_clipboard(size_t id, IDataSink *sink)
        {
            return pDisplay->get_clipboard(id, sink);
        }

        status_t LSPDisplay::set_clipboard(size_t id, IDataSource *src)
        {
            return pDisplay->set_clipboard(id, src);
        }

        status_t LSPDisplay::reject_drag()
        {
            return pDisplay->reject_drag();
        }

        status_t LSPDisplay::accept_drag(IDataSink *sink, drag_t action, bool internal, const realize_t *r)
        {
            return pDisplay->accept_drag(sink, action, internal, r);
        }

        const char * const *LSPDisplay::get_drag_mime_types()
        {
            return pDisplay->get_drag_ctypes();
        }

        status_t LSPDisplay::queue_destroy(LSPWidget *widget)
        {
            return vGarbage.add(widget) ? STATUS_OK : STATUS_NO_MEM;
        }
    }

} /* namespace lsp */
