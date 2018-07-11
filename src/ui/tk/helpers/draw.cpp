/*
 * draw.cpp
 *
 *  Created on: 29 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ui/tk/helpers/draw.h>

namespace lsp
{
    namespace tk
    {
        void draw_border(ISurface *s, ssize_t left, ssize_t top, ssize_t width, ssize_t height, ssize_t thick, size_t iradius, size_t mask, const Color &c)
        {
            // Draw border
            bool aa = s->set_antialiasing(true);

            float pr = sqrtf(float(width*width) + float(height*height));

            for (ssize_t i=0; i < thick; ++i)
            {
                float bright = float(thick - i) / thick;
                Color l(1.0f, 1.0f, 1.0f);
                l.blend(c, bright);

                IGradient *g = s->radial_gradient(left, top + height, i, left, top + height, pr * 1.5f);
                g->add_color(0.0f, l);
                g->add_color(1.0f, c);
                s->wire_round_rect(left + i + 0.5f, top + i + 0.5f, width - (i << 1) - 1, height - (i << 1) - 1, iradius - i, mask, 1.0f, g);
                delete g;
            }

            s->fill_round_rect(left + thick + 0.5f, top + thick + 0.5f, width - (thick << 1) - 1, height - (thick << 1) - 1, iradius - thick, mask, c);
            s->set_antialiasing(aa);
        }

        ISurface *create_glass(ISurface *s, ISurface **g, size_t width, size_t height, size_t radius, size_t mask)
        {
            // Check surface
            if (*g != NULL)
            {
                if ((width != (*g)->width()) || (height != (*g)->height()))
                {
                    (*g)->destroy();
                    delete *g;
                    (*g)        = NULL;
                }
            }

            // Create new surface if needed
            if ((*g) != NULL)
                return *g;

            if (s == NULL)
                return NULL;
            *g          = s->create(width, height);
            if ((*g) == NULL)
                return NULL;

            // Draw glass effect
            size_t pr = sqrtf(float(width)*float(width) + float(height)*float(height));

            IGradient *gr = (*g)->radial_gradient(width, 0, 1, width, 0, pr);
            gr->add_color(0.0f, 1.0f, 1.0f, 1.0f, 0.85f);
//            gr->add_color(0.0f, 1.0f, 1.0f, 1.0f, 0.5f);
            gr->add_color(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

            bool aa = (*g)->set_antialiasing(true);
            (*g)->fill_round_rect(0, 0, width, height, radius, mask, gr);
            (*g)->set_antialiasing(aa);
            delete gr;

            return *g;
        }

        ISurface * create_border_glass(
            ISurface *s, ISurface **g,
            size_t width, size_t height,
            ssize_t thick, size_t radius,
            size_t mask, const Color &c)
        {
            // Check surface
            if (*g != NULL)
            {
                if ((width != (*g)->width()) || (height != (*g)->height()))
                {
                    (*g)->destroy();
                    delete *g;
                    (*g)        = NULL;
                }
            }

            // Create new surface if needed
            if ((*g) != NULL)
                return *g;

            if (s == NULL)
                return NULL;
            *g          = s->create(width, height);
            if ((*g) == NULL)
                return NULL;

            // Pre-calculate params
            IGradient *gr = NULL;
            bool aa = (*g)->set_antialiasing(true);
            float pr = sqrtf(float(width)*float(width) + float(height)*float(height));

            // Draw border
            for (ssize_t i=0; i < thick; ++i)
            {
                float bright = float(thick - i) / thick;
                Color l(1.0f, 1.0f, 1.0f);
                l.blend(c, bright);

                gr = (*g)->radial_gradient(0, height, i, 0, height, pr * 1.5f);
                gr->add_color(0.0f, l);
                gr->add_color(1.0f, c);
                (*g)->wire_round_rect(i + 0.5f, i + 0.5f, width - (i << 1) - 1, height - (i << 1) - 1, radius - i, mask, 1.0f, gr);
                delete gr;
            }

            s->wire_round_rect(thick + 0.5f, thick + 0.5f, width - (thick << 1) - 1, height - (thick << 1) - 1, radius - thick, mask, 1.0f, c);

            // Draw glass effect
            gr = (*g)->radial_gradient(width, 0, 1, width, 0, pr);
            gr->add_color(0.0f, 1.0f, 1.0f, 1.0f, 0.85f);
//            gr->add_color(0.0f, 1.0f, 1.0f, 1.0f, 0.5f);
            gr->add_color(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

            (*g)->fill_round_rect(thick, thick, width - (thick << 1), height - (thick << 1), radius - thick, mask, gr);
            (*g)->set_antialiasing(aa);
            delete gr;

            s->set_antialiasing(aa);

            return *g;
        }

        void draw_border_back(
            ISurface *s,
            ssize_t left, ssize_t top,
            size_t width, size_t height,
            ssize_t thick, size_t radius,
            size_t mask, const Color &c
        )
        {
            // Draw border
            bool aa = s->set_antialiasing(true);
//            float pr = sqrtf(float(width*width) + float(height*height));
            s->fill_round_rect(left + thick + 0.5f, top + thick + 0.5f, width - (thick << 1) - 1, height - (thick << 1) - 1, radius - thick, mask, c);
            s->set_antialiasing(aa);
        }
    }
}


