/*
 * IDisplay.h
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#ifndef UI_WS_IDISPLAY_H_
#define UI_WS_IDISPLAY_H_

#include <data/cstorage.h>

namespace lsp
{
    namespace ws
    {
        class INativeWindow;

        /** Display
         *
         */
        class IDisplay
        {
            protected:
                typedef struct dtask_t
                {
                    taskid_t        nID;
                    timestamp_t     nTime;
                    task_handler_t  pHandler;
                    void           *pArg;
                } dtask_t;

            protected:
                taskid_t            nTaskID;
                cstorage<dtask_t>   sTasks;

            protected:
                bool                taskid_exists(taskid_t id);

            public:
                IDisplay();
                virtual ~IDisplay();

            public:
                virtual int init(int argc, const char **argv);
                virtual void destroy();

                virtual int main();
                virtual int main_iteration();
                virtual void quit_main();

                virtual size_t screens();
                virtual size_t default_screen();

                virtual status_t screen_size(size_t screen, ssize_t *w, ssize_t *h);

            public:
                /** Create native window
                 *
                 * @return native window
                 */
                virtual INativeWindow *createWindow();

                /** Create window at the specified screen
                 *
                 * @param screen screen
                 * @return status native window
                 */
                virtual INativeWindow *createWindow(size_t screen);

                /** Create native window from specified window handle
                 *
                 * @return native window from specified window handle
                 */
                virtual INativeWindow *createWindow(void *handle);

                /** Create surface for drawing
                 *
                 * @param width surface width
                 * @param height surface height
                 * @return surface or NULL on error
                 */
                virtual ISurface *createSurface(size_t width, size_t height);

                /** Submit task for execution
                 *
                 * @param time time when the task should be triggered (timestamp in milliseconds)
                 * @param handler task handler
                 * @param arg task handler argument
                 * @return submitted task identifier or negative error code
                 */
                virtual taskid_t submitTask(timestamp_t time, task_handler_t handler, void *arg);

                /** Cancel submitted task
                 *
                 * @param id task identifier to cancel
                 * @return status of operation
                 */
                virtual status_t cancelTask(taskid_t id);

                /** Request clipboard data by it's identifier
                 *
                 * @param id clipboard identifier
                 * @param ctype requested content type
                 * @param charset requested character set
                 * @param handler callback handler when clipboard data is ready
                 * @param arg argument to pass to the callback handler
                 * @return pointer to clipboard data object or NULL if not present
                 */
                virtual status_t fetchClipboard(size_t id, const char *ctype, clipboard_handler_t handler, void *arg = NULL);

                /** Get clipboard by it's identifier
                 *
                 * @param wnd the window that is owner of the clipboard
                 * @param id clipboard identifier
                 * @param c the clipboard data holder object
                 * @return pointer to clipboard or NULL if not present
                 */
//                virtual status_t writeClipboard(INativeWindow *wnd, size_t id, IClipboard *c);
                virtual status_t writeClipboard(size_t id, IClipboard *c);
        };

    } /* namespace ws */
} /* namespace lsp */

#endif /* UI_IDISPLAY_H_ */
