/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 июл. 2017 г.
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

#include <ui/tk/tk.h>
#include <dsp/dsp.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMesh::metadata = { "LSPMesh", &LSPGraphItem::metadata };

        LSPMesh::LSPMesh(LSPDisplay *dpy): LSPGraphItem(dpy)
        {
            nWidth      = 1;
            nCenter     = 0;
            nDimensions = 0;
            nPoints     = 0;
            nXIndex     = -1;
            nYIndex     = -1;
            nSIndex     = -1;
            nStrobes    = 0;
            bStrobe     = false;
            vBuffer     = NULL;
            nBufSize    = 0;

            pClass      = &metadata;
        }

        LSPMesh::~LSPMesh()
        {
            drop_data();
        }

        status_t LSPMesh::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GRAPH_MESH, &sColor);
            return STATUS_OK;
        }

        void LSPMesh::drop_data()
        {
            if (vBuffer != NULL)
            {
                free(vBuffer);
                vBuffer     = NULL;
            }
            nBufSize    = 0;
        }

        void LSPMesh::destroy()
        {
            drop_data();
            LSPGraphItem::destroy();
        }

        void LSPMesh::set_line_width(size_t value)
        {
            if (nWidth == value)
                return;
            nWidth = value;
            query_draw();
        }

        void LSPMesh::set_x_index(ssize_t value)
        {
            if (nXIndex == value)
                return;
            nXIndex = value;
            query_draw();
        }

        void LSPMesh::set_y_index(ssize_t value)
        {
            if (nYIndex == value)
                return;
            nYIndex = value;
            query_draw();
        }

        void LSPMesh::set_s_index(ssize_t value)
        {
            if (nSIndex == value)
                return;
            nSIndex = value;
            query_draw();
        }

        void LSPMesh::set_num_strobes(ssize_t value)
        {
            size_t num = lsp_max(0, value);
            if (nStrobes == num)
                return;
            nStrobes = num;
            query_draw();
        }

        void LSPMesh::set_strobes(bool set)
        {
            if (bStrobe == set)
                return;
            bStrobe = set;
            query_draw();
        }

        void LSPMesh::set_center_id(size_t value)
        {
            if (nCenter == value)
                return;
            nCenter = value;
            query_draw();
        }

        status_t LSPMesh::set_data(size_t dimensions, size_t points, const float **data)
        {
//            lsp_trace("dimensions = %d, points=%d, data=%p", int(dimensions), int(points), data);
            ssize_t k=0, i=0;
            if (nXIndex >= 0)
                ++k;
            if (nYIndex >= 0)
                ++k;
            if (nSIndex >= 0)
                ++k;

            ssize_t n       = (k > 0) ? k : dimensions;

            // Allocate data
            size_t vec_size = ALIGN_SIZE(points, DEFAULT_ALIGN);
            size_t new_size = (n + 2) * vec_size;

            if (new_size > nBufSize)
            {
                if (vBuffer != NULL)
                {
                    float *ptr  = reinterpret_cast<float *>(::realloc(vBuffer, new_size * sizeof(float)));
                    if (ptr == NULL)
                        return STATUS_NO_MEM;
                    vBuffer     = ptr;
                }
                else
                {
                    vBuffer     = reinterpret_cast<float *>(::malloc(new_size * sizeof(float)));
                    if (vBuffer == NULL)
                        return STATUS_NO_MEM;
                }

                nBufSize    = new_size;
            }

            // Now we are ready to store data
            float *dst      = vBuffer;
            if (k > 0)
            {
                // Copy just specific data
                if (nXIndex >= 0)
                {
                    if (nXIndex < ssize_t(dimensions))
                        dsp::copy(dst, data[nXIndex], points);
                    else
                        dsp::fill_zero(dst, points);
                    dst            += vec_size;
                }
                if (nYIndex >= 0)
                {
                    if (nYIndex < ssize_t(dimensions))
                        dsp::copy(dst, data[nYIndex], points);
                    else
                        dsp::fill_zero(dst, points);
                    dst            += vec_size;
                }
            }

            // Strobe is used?
            if (nSIndex >= 0)
                --n;

            // Copy all data
            for (; i<n; ++i)
            {
                // Skip matching items
                if ((i == nXIndex) || (i == nYIndex) || (i == nSIndex))
                    continue;
                dsp::copy(dst, data[i], points);
                dst            += vec_size;
            }

            // Copy strobe data as last in the list
            if (nSIndex >= 0)
            {
                if (nSIndex < ssize_t(dimensions))
                    dsp::copy(dst, data[nSIndex], points);
                else
                    dsp::fill_zero(dst, points);
                dst            += vec_size;
            }

            // Store dimensions
            nDimensions     = n;
            nPoints         = points;

            query_draw();
            return STATUS_OK;
        }

        size_t LSPMesh::find_offset(size_t *found, const float *v, size_t count, size_t strobes)
        {
            size_t nf       = 0;    // No strobes found
            while (count > 0)
            {
                float s         = v[--count];
                if (s < 0.5f)
                    continue;
                if ((strobes--) <= 0)   // We found start point?
                    break;
                ++nf;                   // Remember that we found the strobe
            }

            if (found != NULL)
                *found = nf;

            return count;
        }

        size_t LSPMesh::get_length(const float *v, size_t off, size_t count)
        {
            size_t start = off;
            while (true)
            {
                if ((++off) >= count)
                    return count - start;
                if (v[off] >= 0.5f)
                    break;
            }

            return off - start;
        }

        void LSPMesh::render(ISurface *s, bool force)
        {
            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
            {
                lsp_trace("cv == null");
                return;
            }

            // Prepare palette
            Color color(sColor);
            color.scale_lightness(brightness());

            // Determine number of dimensions
            ssize_t dimensions  = (bStrobe) ? nDimensions - 1 : nDimensions;
            ssize_t basis       = cv->basis_axes();
            ssize_t n_vecs      = (dimensions > basis) ? basis : dimensions;
            if (n_vecs <= 0)
                return;

            float cx = 0.0f, cy = 0.0f;
            cv->center(nCenter, &cx, &cy);

            // Ensure that we have enough buffer size
            size_t vec_size     = ALIGN_SIZE(nPoints, DEFAULT_ALIGN);
            size_t found        = 0;
            size_t strobes      = nStrobes;
            size_t off          = (bStrobe) ? find_offset(&found, &vBuffer[dimensions*vec_size], nPoints, strobes) : 0;

            // Initialize dimensions as zeros
            float *x_vec        = &vBuffer[nDimensions * vec_size];
            float *y_vec        = &x_vec[vec_size];
            dsp::fill(x_vec, cx, nPoints);
            dsp::fill(y_vec, cy, nPoints);

            // Calculate dot coordinates
            const float *src = vBuffer;
            for (ssize_t i=0; i<n_vecs; ++i)
            {
                // Try to get new axis
                LSPAxis *axis   = cv->basis_axis(i);
                if (axis == NULL) // There is no axis, leave cycle
                {
                    lsp_trace("axis == null");
                    break;
                }

                // Try to apply axis
                if (!axis->apply(x_vec, y_vec, src, nPoints))
                {
                    lsp_trace("!axis->apply");
                    return;
                }
                src += vec_size;
            }

            // Now we have dots in x_vec[] and y_vec[], draw them
            bool aa = s->set_antialiasing(bSmooth);

            if (bStrobe)
            {
                const float *svec   = &vBuffer[dimensions*vec_size];

                // Draw mesh using strobes
                ssize_t op  = strobes - found + 1;      // Initial opacity coefficient
                float kop   = 1.0f / (strobes + 1.0f);  // Opacity coefficient
                Color line, wire;

                while (off < nPoints)
                {
                    size_t length       = get_length(svec, off, nPoints);
                    float ka            = (op++) * kop;

                    line.copy(color);
                    line.alpha(1.0f - (1.0f - line.alpha()) * ka);

                    // Draw part of mesh
                    if (sColor.alpha() <= 0.0f)
                        s->wire_poly(&x_vec[off], &y_vec[off], length, nWidth, line);
                    else
                    {
                        wire.copy(sColor, 0.0f);
                        wire.alpha(1.0f - (1.0f - wire.alpha()) * ka);
                        s->draw_poly(&x_vec[off], &y_vec[off], length, nWidth, color, wire);
                    }

                    // Update offset
                    off                += length;
                }
            }
            else
            {
                // Draw mesh as usual
                if (sColor.alpha() <= 0.0f)
                    s->wire_poly(x_vec, y_vec, nPoints, nWidth, color);
                else
                {
                    Color wire(sColor, 0.0f);
                    s->draw_poly(x_vec, y_vec, nPoints, nWidth, color, wire);
                }
            }
            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */
