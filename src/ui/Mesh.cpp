/*
 * Mesh.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>

#include <math.h>


namespace lsp
{
    Mesh::Mesh(plugin_ui *ui): IGraphObject(ui, W_MESH)
    {
        nBasis      = 0;
        nCapacity   = 0;
        vBasis      = NULL;
        pPort       = NULL;
        sColor.set(ui->theme(), C_GRAPH_MESH);
        nWidth      = 1;
    }

    Mesh::~Mesh()
    {
        if (vBasis != NULL)
        {
            delete [] vBasis;
            vBasis      = NULL;
        }
        nBasis      = 0;
        nCapacity   = 0;
    }

    void Mesh::draw(IGraphCanvas *cv)
    {
        if (pPort == NULL)
            return;

#ifdef LSP_HOST_SIMULATION
        // THIS IS TEST CODE
        mesh_t *mesh    = reinterpret_cast<mesh_t *>(alloca(sizeof(mesh_t) + sizeof(float *) * 2));
        mesh->nBuffers  = 2;
        mesh->nItems    = 200;
        mesh->pvData[0] = reinterpret_cast<float *>(alloca(mesh->nItems * sizeof(float)));
        mesh->pvData[1] = reinterpret_cast<float *>(alloca(mesh->nItems * sizeof(float)));

        for (size_t i=0; i<mesh->nItems; ++i)
        {
            mesh->pvData[0][i] = float(i) / (mesh->nItems - 1);
            mesh->pvData[1][i] = cosf(4.0 * M_PI * mesh->pvData[0][i]);
        }
#else
        mesh_t *mesh    = reinterpret_cast<mesh_t *>(pPort->getBuffer());
#endif /* HOST_SIMULATION */

        if ((mesh == NULL) || (mesh->nBuffers <= 0) || (mesh->nItems <= 0))
            return;

        // Determine number of dimensions
        size_t basis    = (nBasis > 0) ? nBasis : pGraph->basisAxises();
        size_t n_vecs   = (mesh->nBuffers > basis) ? basis : mesh->nBuffers;
        if (n_vecs <= 0)
            return;

        // Initialize dimensions as zeros
        point_t *vecs   = reinterpret_cast<point_t *>(alloca(n_vecs * sizeof(point_t)));
        for (size_t i=0; i<n_vecs; ++i)
        {
            vecs[i].x   = 0;
            vecs[i].y   = 0;
        }

        // Fill dimensions with values
        if (nBasis > 0)
        {
            for (size_t i=0; i<nBasis; ++i)
            {
                // Try to get new axis
                Axis *axis  = pGraph->basisAxis(vBasis[i]);
                if (axis == NULL)
                    return;

                // Try to apply axis
                if (!axis->apply(cv, vecs[i].x, vecs[i].y, 1.0))
                    return;
            }
        }
        else
        {
            for (size_t i=0; i<n_vecs; ++i)
            {
                // Try to get new axis
                Axis *axis  = pGraph->basisAxis(i);
                if (axis == NULL) // There is no axis, leave cycle
                    break;

                // Try to apply axis
                if (!axis->apply(cv, vecs[i].x, vecs[i].y, 1.0))
                    return;
            }
        }

        // Now we have set of normals in vecs[]
        cv->set_color(sColor);
        cv->set_line_width(nWidth);

        for (size_t i=0; i < mesh->nItems; ++i)
        {
            // Calculate dot coordinates
            float x = 0.0f, y = 0.0f;
            for (size_t j=0; j<n_vecs; ++j)
            {
                x   += mesh->pvData[j][i] * vecs[j].x;
                y   += mesh->pvData[j][i] * vecs[j].y;
            }

            //
            if (i == 0)
            {
                cv->move_to(x, y);
                if (mesh->nItems < 2)
                    cv->line_to(x, y);
            }
            else
                cv->line_to(x, y);
        }
        cv->stroke();
    }

    void Mesh::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                pPort       = pUI->port(value);
                if (pPort != NULL)
                    pPort->bind(this);
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_WIDTH:
                PARSE_INT(value, nWidth = __);
                break;
            default:
                IWidget::set(att, value);
                break;
        }
    }

    void Mesh::add(IWidget *widget)
    {
        if (widget->getClass() != W_BASIS)
            return IWidget::add(widget);

        Basis *basis = static_cast<Basis *>(widget);

        // Add basis to list
        if (nBasis >= nCapacity)
        {
            ssize_t *alloc  = new ssize_t[nCapacity + 16];
            if (alloc == NULL)
                return;

            // Copy objects
            for (size_t i=0; i<nBasis; ++i)
                alloc[i]    = vBasis[i];

            delete [] vBasis;
            vBasis          = alloc;
            nCapacity      += 16;
        }

        // Add to list
        vBasis[nBasis++]        = basis->getID();
    }

} /* namespace lsp */
