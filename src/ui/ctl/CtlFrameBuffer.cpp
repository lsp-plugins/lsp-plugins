/*
 * CtlFrame.cpp
 *
 *  Created on: 13 нояб. 2018 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlFrameBuffer::CtlFrameBuffer(CtlRegistry *src, LSPFrameBuffer *fb): CtlWidget(src, fb)
        {
            pPort       = NULL;
            nRowID      = 0;
        }
        
        CtlFrameBuffer::~CtlFrameBuffer()
        {
        }

        void CtlFrameBuffer::set(widget_attribute_t att, const char *value)
        {
            LSPFrameBuffer *fb = widget_cast<LSPFrameBuffer>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_WIDTH:
                    if (fb != NULL)
                        PARSE_FLOAT(value, fb->set_width(__));
                    break;
                case A_HEIGHT:
                    if (fb != NULL)
                        PARSE_FLOAT(value, fb->set_height(__));
                    break;
                case A_HPOS:
                    if (fb != NULL)
                        PARSE_FLOAT(value, fb->set_hpos(__));
                    break;
                case A_VPOS:
                    if (fb != NULL)
                        PARSE_FLOAT(value, fb->set_vpos(__));
                    break;
                case A_ANGLE:
                    if (fb != NULL)
                        PARSE_INT(value, fb->set_angle(__));
                    break;
                case A_OPACITY:
                    if (fb != NULL)
                        PARSE_FLOAT(value, fb->set_opacity(__));
                    break;
                case A_TRANSPARENCY:
                    if (fb != NULL)
                        PARSE_FLOAT(value, fb->set_transparency(__));
                    break;
                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlFrameBuffer::end()
        {
            LSPFrameBuffer *fb = widget_cast<LSPFrameBuffer>(pWidget);
            if (fb == NULL)
                return;

            if (pPort != NULL)
            {
                const port_t *mdata = pPort->metadata();
                if ((mdata != NULL) && (mdata->role == R_FBUFFER))
                    fb->set_size(mdata->start, mdata->step);
            }
        }

        void CtlFrameBuffer::notify(CtlPort *port)
        {
            CtlWidget::notify(pPort);

            LSPFrameBuffer *fb = widget_cast<LSPFrameBuffer>(pWidget);
            if (fb == NULL)
                return;

            // Commit changes if there are
            if ((pPort == port) && (pPort != NULL))
            {
                const port_t *mdata = pPort->metadata();
                if ((mdata != NULL) && (mdata->role == R_FBUFFER))
                {
                    frame_buffer_t *data    = pPort->get_buffer<frame_buffer_t>();
                    size_t rowid            = data->next_rowid();
                    size_t delta            = rowid - nRowID;
                    if (delta > fb->get_rows())
                        nRowID                  = rowid - fb->get_rows();

                    while (nRowID != rowid)
                    {
                        float *row = data->get_row(nRowID++);
                        if (row != NULL)
                            fb->append_data(row);
                    }
                }
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
