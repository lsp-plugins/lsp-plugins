/*
 * X11CairoGradient.cpp
 *
 *  Created on: 19 дек. 2016 г.
 *      Author: sadko
 */

#include <ui/ws/x11/ws.h>

#ifdef USE_X11_DISPLAY
namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            X11CairoGradient::X11CairoGradient()
            {
                pCP = NULL;
            }

            X11CairoGradient::~X11CairoGradient()
            {
                if (pCP != NULL)
                {
                    cairo_pattern_destroy(pCP);
                    pCP = NULL;
                }
            }

            void X11CairoGradient::add_color(float offset, float r, float g, float b, float a)
            {
                if (pCP == NULL)
                    return;

                cairo_pattern_add_color_stop_rgba(pCP, offset, r, g, b, 1.0f - a);
            }

            void X11CairoGradient::apply(cairo_t *cr)
            {
                if (pCP == NULL)
                    return;
                cairo_set_source(cr, pCP);
            }

            X11CairoLinearGradient::~X11CairoLinearGradient()
            {
            }

            X11CairoRadialGradient::~X11CairoRadialGradient()
            {
            }
        }
    }
} /* namespace lsp */

#endif /* USE_X11_DISPLAY */
