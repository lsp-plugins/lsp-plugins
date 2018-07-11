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
        nWidth      = 1;
        nCenter     = 0;

        sColor.init(this, C_GRAPH_MESH, A_COLOR, -1, -1, -1, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
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
        if ((pPort == NULL) || (!bVisible))
            return;

#ifdef LSP_HOST_SIMULATION
        // THIS IS TEST CODE
        mesh_t *mesh    = reinterpret_cast<mesh_t *>(alloca(sizeof(mesh_t) + sizeof(float *) * 2));
        mesh->nBuffers  = 2;
        mesh->nItems    = 1000;
        mesh->pvData[0] = reinterpret_cast<float *>(alloca(mesh->nItems * sizeof(float)));
        mesh->pvData[1] = reinterpret_cast<float *>(alloca(mesh->nItems * sizeof(float)));

        for (size_t i=0; i<mesh->nItems; ++i)
        {
            mesh->pvData[0][i] = float(i) * 24000 / (mesh->nItems - 1);
            mesh->pvData[1][i] = cosf(4.0 * M_PI * mesh->pvData[0][i] / 6000);
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

        float cx = 0.0f, cy = 0.0f;
        cv->center(nCenter, &cx, &cy);

        // Initialize dimensions as zeros
        float *x_vec        = new float[mesh->nItems];
        if (x_vec == NULL)
            return;
        float *y_vec        = new float[mesh->nItems];
        if (y_vec == NULL)
        {
            delete [] x_vec;
            return;
        }
        for (size_t i=0; i<mesh->nItems; ++i)
        {
            x_vec[i]    = cx;
            y_vec[i]    = cy;
        }

        // Calculate dot coordinates
        if (nBasis > 0)
        {
            for (size_t i=0; i<nBasis; ++i)
            {
                // Try to get new axis
                Axis *axis  = pGraph->basisAxis(vBasis[i]);
                if (axis == NULL)
                {
                    delete [] x_vec;
                    delete [] y_vec;
                    return;
                }

                // Try to apply axis
                if (!axis->apply(cv, x_vec, y_vec, mesh->pvData[i], mesh->nItems))
                {
                    delete [] x_vec;
                    delete [] y_vec;
                    return;
                }
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
                if (!axis->apply(cv, x_vec, y_vec, mesh->pvData[i], mesh->nItems))
                {
                    delete [] x_vec;
                    delete [] y_vec;
                    return;
                }
            }
        }

        // Now we have dots in x_vec[] and y_vec[]
        cv->set_color(sColor.color());
        cv->set_line_width(nWidth);

        for (size_t i=0; i < mesh->nItems; ++i)
        {
            if (i == 0)
            {
                cv->move_to(x_vec[i], y_vec[i]);
                if (mesh->nItems < 2)
                    cv->line_to(x_vec[i], y_vec[i]);
            }
            else
                cv->line_to(x_vec[i], y_vec[i]);
        }
        cv->stroke();

        delete [] x_vec;
        delete [] y_vec;
    }

    void Mesh::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                BIND_PORT(pUI, pPort, value);
                break;
            case A_WIDTH:
                PARSE_INT(value, nWidth = __);
                break;
            case A_CENTER:
                PARSE_INT(value, nCenter = __);
                break;
            default:
                if (sColor.set(att, value))
                    break;
                IGraphObject::set(att, value);
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

    void Mesh::notify(IUIPort *port)
    {
        sColor.notify(port);
        IGraphObject::notify(port);
    }

} /* namespace lsp */
