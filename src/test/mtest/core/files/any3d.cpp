/*
 * any3d.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <core/files/Model3DFile.h>

using namespace lsp;

static const char *FILE_NAME = "res/test/3d/quad.obj";

using namespace lsp;

MTEST_BEGIN("core.files", any3d)

    MTEST_MAIN
    {
        const char *path = FILE_NAME;
        if (argc > 0)
            path = argv[0];

        Scene3D *scene = NULL;

        status_t status = Model3DFile::load(&scene, path);
        MTEST_ASSERT_MSG(status == STATUS_OK, "Could not load file %s, status=%d", path, int(status));

        printf("Scene statistics: vertexes=%d, normals=%d, edges=%d, triangles=%d\n",
                int(scene->num_vertexes()),
                int(scene->num_normals()),
                int(scene->num_edges()),
                int(scene->num_triangles())
            );

        printf("Dumping vertex list:\n");
        for (size_t i=0, n=scene->num_vertexes(); i<n; ++i)
        {
            obj_vertex_t *v = scene->vertex(i);
            printf("(%.3f, %.3f, %.3f): ",
                    v->x, v->y, v->z);
            obj_edge_t *e = v->ve;
            while (e != NULL)
            {
                printf("edge[%p]{(%.3f, %.3f, %.3f)-(%.3f, %.3f, %.3f)} ",
                    e,
                    e->v[0]->x, e->v[0]->y, e->v[0]->z,
                    e->v[1]->x, e->v[1]->y, e->v[1]->z
                    );
                e = (e->v[0] == v) ? e->vlnk[0] : e->vlnk[1];
            }
            printf("\n");
        }

        size_t n_objects = scene->num_objects();
        printf("Loaded scene num_objects = %d\n", int(n_objects));

        for (size_t i=0; i<n_objects; ++i)
        {
            Object3D *obj = scene->get_object(i);
            if (obj == NULL)
                continue;

            size_t t_count = obj->num_triangles();
            printf("Object #%d id=\"%s\" triangles=%d\n", int(i), obj->get_name(), int(t_count));

            for (size_t j=0; j<t_count; ++j)
            {
                obj_triangle_t *t = obj->triangle(j);
                printf("  Triangle %d: ", int(j));
                for (size_t k = 0; k<3; ++k)
                {
                    if (k != 0)
                        printf(":");
                    printf("(%8.3f, %8.3f, %8.3f)/(%8.3f, %8.3f, %8.3f)",
                            t->v[k]->x, t->v[k]->y, t->v[k]->z,
                            t->n[k]->dx, t->n[k]->dy, t->n[k]->dz);
                }

                printf("\n");
            }
            printf("\n");
        }

        scene->destroy();
    }

MTEST_END



