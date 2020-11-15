/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 нояб. 2018 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <ui/ctl/ctl.h>
#include <core/port_data.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlFrameBuffer::metadata = { "CtlFrameBuffer", &CtlWidget::metadata };
        
        CtlFrameBuffer::CtlFrameBuffer(CtlRegistry *src, LSPFrameBuffer *fb): CtlWidget(src, fb)
        {
            pClass          = &metadata;
            pPort           = NULL;
            nRowID          = 0;
        }
        
        CtlFrameBuffer::~CtlFrameBuffer()
        {
            destroy();
        }

        void CtlFrameBuffer::init()
        {
            CtlWidget::init();

            LSPFrameBuffer *fb      = widget_cast<LSPFrameBuffer>(pWidget);
            if (fb == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, fb, fb->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sMode.init(pRegistry, this);
        }

        void CtlFrameBuffer::destroy()
        {
            sMode.destroy();
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
                case A_MODE:
                    BIND_EXPR(sMode, value);
                    break;
                default:
                {
                    sColor.set(att, value);
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
            if (sMode.valid())
            {
                size_t mode = sMode.evaluate();
                fb->set_palette(mode);
            }
        }

        void CtlFrameBuffer::notify(CtlPort *port)
        {
            CtlWidget::notify(pPort);

            LSPFrameBuffer *fb = widget_cast<LSPFrameBuffer>(pWidget);
            if (fb == NULL)
                return;

            if (sMode.valid())
            {
                size_t mode = sMode.evaluate();
                fb->set_palette(mode);
            }

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
                            fb->append_data(nRowID, row);
                    }
                }
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
