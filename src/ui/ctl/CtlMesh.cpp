/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 июл. 2017 г.
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
        const ctl_class_t CtlMesh::metadata = { "CtlMesh", &CtlWidget::metadata };

        CtlMesh::CtlMesh(CtlRegistry *src, LSPMesh *mesh): CtlWidget(src, mesh)
        {
            pClass          = &metadata;
            pPort           = NULL;
            fTransparency   = 0.0f;
        }

        CtlMesh::~CtlMesh()
        {
        }

        void CtlMesh::init()
        {
            CtlWidget::init();

            LSPMesh *mesh       = widget_cast<LSPMesh>(pWidget);
            if (mesh == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, mesh, mesh->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }

        void CtlMesh::set(widget_attribute_t att, const char *value)
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
                default:
                {
                    bool set = sColor.set(att, value);
                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlMesh::end()
        {
            sColor.set_alpha(fTransparency);
            CtlWidget::end();
        }

        void CtlMesh::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            LSPMesh *mesh = widget_cast<LSPMesh>(pWidget);
            if (mesh == NULL)
                return;

            if ((pPort == port) && (pPort != NULL))
            {
                const port_t *mdata = pPort->metadata();
                if ((mdata != NULL) && (mdata->role == R_MESH))
                {
                    mesh_t *data  = pPort->get_buffer<mesh_t>();
                    if (data != NULL)
                        mesh->set_data(data->nBuffers, data->nItems, const_cast<const float **>(data->pvData));
                }
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
