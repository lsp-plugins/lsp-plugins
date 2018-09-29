/*
 * any3d.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <core/files/Model3DFile.h>

using namespace lsp;

static const char *FILE_NAME = "test_data/3d/double_ring.obj";

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

        size_t n_objects = scene->num_objects();
        printf("Loaded scene num_objects = %d\n", int(n_objects));

        for (size_t i=0; i<n_objects; ++i)
        {
            Object3D *obj = scene->get_object(i);
            if (obj == NULL)
                continue;

            size_t t_count = obj->get_triangles_count();
            printf("Object #%d id=\"%s\" triangles=%d\n", int(i), obj->get_name(), int(t_count));

            vertex_index_t *vi = obj->get_vertex_indexes();
            vertex_index_t *ni = obj->get_normal_indexes();

            for (size_t j=0; j<t_count; ++j)
            {
                printf("  Triangle %d: ", int(j));
                for (size_t k = 0; k<3; ++k)
                {
                    if (k != 0)
                        printf(":");
                    point3d_t *p = obj->get_vertex(*(vi++));
                    vector3d_t *n = obj->get_normal(*(ni++));
                    printf("(%8.3f, %8.3f, %8.3f)/(%8.3f, %8.3f, %8.3f)",
                            p->x, p->y, p->z, n->dx, n->dy, n->dz);
                }

                printf("\n");
            }
            printf("\n");
        }

        scene->destroy(true);
    }

MTEST_END



