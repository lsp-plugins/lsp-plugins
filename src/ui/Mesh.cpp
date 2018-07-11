/*
 * Mesh.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>
#include <core/dsp.h>

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
        bSmooth     = false;
        vBuffer     = NULL;
        nBufSize    = 0;

        sColor.init(this, C_GRAPH_MESH, A_COLOR, -1, -1, -1, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
    }

    Mesh::~Mesh()
    {
        if (vBasis != NULL)
        {
            delete [] vBasis;
            vBasis      = NULL;
        }
        if (vBuffer != NULL)
        {
            delete [] vBuffer;
            vBuffer     = NULL;
        }

        nBasis      = 0;
        nCapacity   = 0;
        nBufSize    = 0;
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

        // Ensure that we have enough buffer size
        size_t vec_size     = ALIGN_SIZE(mesh->nItems, DEFAULT_ALIGN);
        size_t buf_size     = vec_size * 2;

        if (vec_size > nBufSize)
        {
            if (vBuffer != NULL)
            {
                delete [] vBuffer;
                vBuffer     = NULL;
                nBufSize    = 0;
            }

            vBuffer     = new float[buf_size];
            if (vBuffer == NULL)
                return;
            nBufSize    = buf_size;
        }

        // Initialize dimensions as zeros
        float *x_vec        = vBuffer;
        float *y_vec        = &vBuffer[vec_size];
        dsp::fill(x_vec, cx, mesh->nItems);
        dsp::fill(y_vec, cy, mesh->nItems);

        // Calculate dot coordinates
        if (nBasis > 0)
        {
            for (size_t i=0; i<nBasis; ++i)
            {
                // Try to get new axis
                Axis *axis  = pGraph->basisAxis(vBasis[i]);
                if (axis == NULL)
                    return;

                // Try to apply axis
                if (!axis->apply(cv, x_vec, y_vec, mesh->pvData[i], mesh->nItems))
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
                if (!axis->apply(cv, x_vec, y_vec, mesh->pvData[i], mesh->nItems))
                    return;
            }
        }

        // Now we have dots in x_vec[] and y_vec[]
        bool aa = cv->set_anti_aliasing(bSmooth);
        cv->set_line_width(nWidth);
        if (sColor.alpha() <= 0.0)
        {
            cv->set_color(sColor);
            cv->draw_lines(x_vec, y_vec, mesh->nItems);
            cv->stroke();
        }
        else
        {
            Color line(sColor);
            line.alpha(0);
            cv->draw_poly(x_vec, y_vec, mesh->nItems, line, sColor);
        }
        cv->set_anti_aliasing(aa);
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
            case A_FILL:
                PARSE_FLOAT(value, sColor.alpha(__));
                break;
            case A_SMOOTH:
                PARSE_BOOL(value, bSmooth = __);
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
