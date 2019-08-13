/*
 * scene3d.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UTILS_RESOURCE_GEN_SCENE3D_H_
#define UTILS_RESOURCE_GEN_SCENE3D_H_

namespace lsp
{
    namespace resgen
    {
        static int preprocess_3d_scene(scan_resource_t *resource, cvector<xml_word_t> *wdict, cstorage<dict_float_t> *dict)
        {
            Scene3D *pscene = NULL;
            Model3DFile file;

            // Load scene
            LSPString path;
            if (!path.set_native(resource->path))
                return STATUS_NO_MEM;

            status_t res = file.load(&pscene, &path);
            if (res == STATUS_OK)
            {
                // Vertices
                for (size_t i=0, n=pscene->num_vertexes(); i<n; ++i)
                {
                    obj_vertex_t *v = pscene->vertex(i);
                    if (!float_dict_add(dict, v->x))
                        return STATUS_NO_MEM;
                    if (!float_dict_add(dict, v->y))
                        return STATUS_NO_MEM;
                    if (!float_dict_add(dict, v->z))
                        return STATUS_NO_MEM;
                }

                // Normals
                for (size_t i=0, n=pscene->num_normals(); i<n; ++i)
                {
                    obj_normal_t *v = pscene->normal(i);
                    if (!float_dict_add(dict, v->dx))
                        return STATUS_NO_MEM;
                    if (!float_dict_add(dict, v->dy))
                        return STATUS_NO_MEM;
                    if (!float_dict_add(dict, v->dz))
                        return STATUS_NO_MEM;
                }

                for (size_t i=0, n=pscene->num_objects(); i<n; ++i)
                {
                    Object3D *o = pscene->object(i);
                    if (o == NULL)
                        continue;

                    if (!res_dict_add(wdict, o->get_name()))
                        return STATUS_NO_MEM;
                }

                resource->scene     = pscene;
            }

            return res;
        }

        static int serialize_3d_scene(FILE *out, const scan_resource_t *resource, cvector<xml_word_t> *wdict, cstorage<dict_float_t> *fdict)
        {
            // Output resource descriptor
            fprintf(out,    "\t// Contents of file %s\n", resource->id);
            fprintf(out,    "\tstatic const char *builtin_resource%s =", resource->hex);

            dict_float_t *f;
            Scene3D *s      = resource->scene;

            // Encode vertices
            fprintf(out,    "\n\t\t");
            encode_value(out, s->num_vertexes());
            fprintf(out,    " // Vertex count");
            fprintf(out,    "\n\t\t\t// Vertex data { x, y, z }");
            for (size_t i=0, n=s->num_vertexes(); i<n; ++i)
            {
                obj_vertex_t *v = s->vertex(i);
                fprintf(out, "\n\t\t\t");

                f = float_dict_get(fdict, v->x);
                encode_value(out, f->index);
                fprintf(out, " ");

                f = float_dict_get(fdict, v->y);
                encode_value(out, f->index);
                fprintf(out, " ");

                f = float_dict_get(fdict, v->z);
                encode_value(out, f->index);
            }

            // Encode normals
            fprintf(out,    "\n\t\t");
            encode_value(out, s->num_normals());
            fprintf(out,    " // Normal count");
            fprintf(out,    "\n\t\t\t// Normal data { dx, dy, dz }");
            for (size_t i=0, n=s->num_normals(); i<n; ++i)
            {
                obj_normal_t *v = s->normal(i);
                fprintf(out, "\n\t\t\t");

                f = float_dict_get(fdict, v->dx);
                encode_value(out, f->index);
                fprintf(out, " ");

                f = float_dict_get(fdict, v->dy);
                encode_value(out, f->index);
                fprintf(out, " ");

                f = float_dict_get(fdict, v->dz);
                encode_value(out, f->index);
            }

            // Encode objects
            fprintf(out,    "\n\t\t");
            encode_value(out, s->num_objects());
            fprintf(out,    " // Objects count");
            for (size_t i=0, n=s->num_objects(); i<n; ++i)
            {
                Object3D *o = s->object(i);

                fprintf(out, "\n\t\t\t");
                encode_word(out, wdict, o->get_name());
                fprintf(out, " // Object name");

                fprintf(out, "\n\t\t\t");
                encode_value(out, o->num_triangles());
                fprintf(out,    " // Object triangles { face_id, v1, v2, v3, n1, n2, n3 }");

                for (size_t j=0, m=o->num_triangles(); j<m; ++j)
                {
                    obj_triangle_t *t = o->triangle(j);
                    fprintf(out, "\n\t\t\t\t");

                    encode_value(out, t->face);
                    fprintf(out, " ");

                    encode_value(out, t->v[0]->id);
                    fprintf(out, " ");

                    encode_value(out, t->v[1]->id);
                    fprintf(out, " ");

                    encode_value(out, t->v[2]->id);
                    fprintf(out, " ");

                    encode_value(out, t->n[0]->id);
                    fprintf(out, " ");

                    encode_value(out, t->n[1]->id);
                    fprintf(out, " ");

                    encode_value(out, t->n[2]->id);
                    fprintf(out, " ");
                }
            }

            fprintf(out,    "\n\t\t;\n\n");

            return STATUS_OK;
        }
    }
}

#endif /* UTILS_RESOURCE_GEN_SCENE3D_H_ */
