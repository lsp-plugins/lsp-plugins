/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>

#include <core/types.h>
#include <core/debug.h>
#include <stdlib.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <sys/poll.h>

using namespace lsp;

static const color3d_t C_RED    = { 1.0f, 0.0f, 0.0f, 0.0f };
static const color3d_t C_GREEN  = { 0.0f, 1.0f, 0.0f, 0.0f };
static const color3d_t C_BLUE   = { 0.0f, 0.0f, 1.0f, 0.0f };

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D     *pScene;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                update_view();
            }

            virtual ~Renderer()
            {
            }

        public:
            virtual void on_key_press(const XKeyEvent &ev, KeySym key)
            {
                switch (key)
                {
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                    {
                        Object3D *obj = pScene->get_object(key - '0');
                        if (obj != NULL)
                        {
                            obj->set_visible(!obj->is_visible());
                            update_view();
                        }
                        break;
                    }
                    default:
                        X11Renderer::on_key_press(ev, key);
                        break;
                }
            }

        protected:
            void    update_view()
            {
                // Clear view state
                pView->clear_all();

                // Generate visible triangles
                v_vertex3d_t v[3];

                for (size_t i=0, n=pScene->num_objects(); i<n; ++i)
                {
                    Object3D *obj   = pScene->get_object(i);
                    if ((obj == NULL) || (!obj->is_visible()))
                        continue;
                    matrix3d_t *om  = obj->get_matrix();

                    point3d_t *tr       = obj->get_vertexes();
                    vector3d_t *tn      = obj->get_normals();
                    vertex_index_t *vvx = obj->get_vertex_indexes();
                    vertex_index_t *vnx = obj->get_normal_indexes();

                    for (ssize_t j=0, m=obj->get_triangles_count(); j < m; ++j)
                    {
                        // Initialize points
                        v[0].p              = tr[*(vvx++)];
                        v[0].n              = tn[*(vnx++)];
                        v[0].c              = C_RED;

                        v[1].p              = tr[*(vvx++)];
                        v[1].n              = tn[*(vnx++)];
                        v[1].c              = C_GREEN;

                        v[2].p              = tr[*(vvx++)];
                        v[2].n              = tn[*(vnx++)];
                        v[2].c              = C_BLUE;

                        // Apply matrix modifications
                        dsp::apply_matrix3d_mp1(&v[0].p, om);
                        dsp::apply_matrix3d_mp1(&v[1].p, om);
                        dsp::apply_matrix3d_mp1(&v[2].p, om);

                        dsp::apply_matrix3d_mv1(&v[0].n, om);
                        dsp::apply_matrix3d_mv1(&v[1].n, om);
                        dsp::apply_matrix3d_mv1(&v[2].n, om);

                        pView->add_triangle(v);
                    }
                }
            }
    };

    MTEST_MAIN
    {
        const char *scene_file = (argc < 1) ? "res/test/3d/cross.obj" : argv[0];

        // Load scene
        Scene3D s;
        View3D v;
        status_t res = Model3DFile::load(&s, scene_file, true);
        MTEST_ASSERT_MSG(res == STATUS_OK, "Error loading scene from file %s", scene_file);

        // Initialize renderer
        Renderer r(&s, &v);
        MTEST_ASSERT_MSG(r.init() == STATUS_OK, "Error initializing renderer");
        r.run();
        r.destroy();

        // Destroy scene
        s.destroy();
    }

MTEST_END



