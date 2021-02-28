/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 7 февр. 2021 г.
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
        const ctl_class_t CtlStream::metadata = { "CtlStream", &CtlWidget::metadata };

        CtlStream::CtlStream(CtlRegistry *src, LSPMesh *mesh): CtlWidget(src, mesh)
        {
            pClass          = &metadata;
            pPort           = NULL;
            fTransparency   = 0.0f;
            pMesh           = NULL;
        }

        CtlStream::~CtlStream()
        {
            if (pMesh != NULL)
            {
                mesh_t::destroy(pMesh);
                pMesh   = NULL;
            }
        }

        void CtlStream::init()
        {
            CtlWidget::init();

            LSPMesh *mesh       = widget_cast<LSPMesh>(pWidget);
            if (mesh == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, mesh, mesh->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

            // Initialize expressions
            sStrobes.init(pRegistry, this);
        }

        void CtlStream::set(widget_attribute_t att, const char *value)
        {
            LSPMesh *mesh = widget_cast<LSPMesh>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_WIDTH:
                    if (mesh != NULL)
                        PARSE_INT(value, mesh->set_line_width(__));
                    break;
                case A_CENTER:
                    if (mesh != NULL)
                        PARSE_INT(value, mesh->set_center_id(__));
                    break;
                case A_FILL:
                    PARSE_FLOAT(value, fTransparency = __);
                    break;
                case A_X_INDEX:
                    if (mesh != NULL)
                        PARSE_INT(value, mesh->set_x_index(__));
                    break;
                case A_Y_INDEX:
                    if (mesh != NULL)
                        PARSE_INT(value, mesh->set_y_index(__));
                    break;
                case A_S_INDEX:
                    if (mesh != NULL)
                        PARSE_INT(value, mesh->set_s_index(__));
                    break;
                case A_STROBE:
                    if (mesh != NULL)
                        PARSE_BOOL(value, mesh->set_strobes(__));
                    break;
                case A_STROBES:
                    BIND_EXPR(sStrobes, value);
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlStream::end()
        {
            sColor.set_alpha(fTransparency);
            CtlWidget::end();

            trigger_expr();
        }

        void CtlStream::trigger_expr()
        {
            LSPMesh *mesh = widget_cast<LSPMesh>(pWidget);
            if (mesh == NULL)
                return;

            if (sStrobes.valid())
            {
                ssize_t value = sStrobes.evaluate();
                mesh->set_num_strobes(value);
            }
        }

        void CtlStream::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            // Trigger expressions
            trigger_expr();

            // Commit stream data
            if ((port != NULL) && (port == pPort))
                commit_data();
        }

        void CtlStream::commit_data()
        {
            LSPMesh *mesh = widget_cast<LSPMesh>(pWidget);
            if (mesh == NULL)
                return;

            const port_t *mdata = pPort->metadata();
            if ((mdata == NULL) || (mdata->role != R_STREAM))
                return;

            stream_t *stream  = pPort->get_buffer<stream_t>();
            if (stream == NULL)
                return;

            // Need to create mesh?
            if (pMesh == NULL)
            {
                pMesh   = mesh_t::create(stream->channels(), stream->capacity());
                if (pMesh == NULL)
                    return;
                pMesh->nBuffers = stream->channels();
            }

            // Perform read from stream to mesh
            size_t last     = stream->frame_id();
            ssize_t length  = stream->get_length(last);

            for (size_t i=0, n=stream->channels(); i<n; ++i)
                stream->read(i, pMesh->pvData[i], 0, length);

            // Set data to mesh
            mesh->set_data(pMesh->nBuffers, length, const_cast<const float **>(pMesh->pvData));
        }
    } /* namespace ctl */
} /* namespace lsp */


