/*
 * Model3DFile.cpp
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/resource.h>
#include <core/files/Model3DFile.h>
#include <core/files/3d/ObjFileParser.h>
#include <core/files/3d/IFileHandler3D.h>

namespace lsp
{
    class FileHandler3D: public IFileHandler3D
    {
        protected:
            typedef struct vertex_t
            {
                obj_vertex_t   *p;
                obj_normal_t   *n;
                ssize_t         ip;
                ssize_t         in;
            } vertex_t;

        protected:
            Scene3D                *pScene;
            Object3D               *pObject;
            ssize_t                 nFaceID;

        public:
            explicit FileHandler3D(Scene3D *scene)
            {
                pScene      = scene;
                pObject     = NULL;
                nFaceID     = 0;
                reset_state();
            }

            virtual ~FileHandler3D()
            {
                reset_state();
            }

        public:
            /** Reset state
             *
             */
            void reset_state()
            {
                if (pScene != NULL)
                    pScene->destroy();
                pObject     = NULL;
                nFaceID     = 0;
            }

            status_t complete()
            {
                pScene      = NULL;
                pObject     = NULL;
                return STATUS_OK;
            }

        public:
            virtual status_t begin_object(size_t id, const char *name)
            {
                if (pObject != NULL)
                    return STATUS_BAD_STATE;

                LSPString sname;
                if (!sname.set_utf8(name))
                    return STATUS_NO_MEM;

                pObject = pScene->add_object(&sname);
                return (pObject != NULL) ? STATUS_OK : STATUS_NO_MEM;
            }

            virtual status_t end_object(size_t id)
            {
                if (pObject == NULL)
                    return STATUS_BAD_STATE;

                pObject->post_load();
                pObject = NULL;
                return STATUS_OK;
            }

            virtual status_t end_of_data()
            {
                if (pScene == NULL)
                    return STATUS_BAD_STATE;
                pScene->postprocess_after_loading();
                return STATUS_OK;
            }

            virtual ssize_t add_vertex(const point3d_t *p)
            {
                return pScene->add_vertex(p);
            }

            virtual ssize_t add_normal(const vector3d_t *v)
            {
                return pScene->add_normal(v);
            }

            virtual status_t add_face(const ssize_t *vv, const ssize_t *vn, const ssize_t *vt, size_t n)
            {
                if ((pObject == NULL) || (n < 3))
                    return STATUS_BAD_STATE;

                cstorage<vertex_t> vertex;
                vertex_t *vx = vertex.append_n(n);
                if (vx == NULL)
                    return STATUS_NO_MEM;

                // Prepare structure
                for (size_t i=0; i<n; ++i)
                {
                    vx[i].ip            = vv[i];
                    vx[i].p             = (vx[i].ip >= 0) ? pScene->vertex(vx[i].ip) : NULL;
                    if (vx[i].p == NULL)
                        return STATUS_BAD_STATE;
                    vx[i].in            = vn[i];
                    vx[i].n             = (vx[i].in >= 0) ? pScene->normal(vx[i].in) : NULL;
                }

                ssize_t face_id     = nFaceID++;

                // Calc default normals for vertexes without normals
                vertex_t *v1, *v2, *v3;
                obj_normal_t on;
                v1 = vertex.at(0);
                v2 = vertex.at(1);
                v3 = vertex.at(2);

                dsp::calc_normal3d_p3(&on, v1->p, v2->p, v3->p);
                for (size_t i=0; i<n; ++i)
                {
                    v1 = &vx[i];
                    if (v1->n == NULL)
                        v1->n = &on;
                }

                // Triangulation algorithm
                size_t index = 0;
                float ck = 0.0f;

                while (n > 3)
                {
                    v1 = vertex.at(index % n);
                    v2 = vertex.at((index+1) % n);
                    v3 = vertex.at((index+2) % n);

//                    lsp_trace(
//                        "analyzing triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
//                        v1->p->x, v1->p->y, v1->p->z,
//                        v2->p->x, v2->p->y, v2->p->z,
//                        v3->p->x, v3->p->y, v3->p->z
//                    );

                    // Check that it is an ear
                    ck = dsp::check_triplet3d_p3n(v1->p, v2->p, v3->p, v1->n);
                    if (ck < 0.0f)
                    {
                        index = (index + 1) % n;
                        continue;
                    }
                    else if (ck == 0.0f)
                    {
                        size_t longest = dsp::longest_edge3d_p3(v1->p, v2->p, v3->p);
                        size_t remove = (longest + 2) % 3;

                        // Need to eliminate point that lies on the line
                        if (!vertex.remove((index + remove) % n))
                            return STATUS_BAD_STATE;

                        // Rollback index and decrement counter
                        n--;
                        index = (index > 0) ? index - 1 : n-1;
                        continue;
                    }

                    // Now ensure that there are no other points inside the triangle
                    int found = 0;
                    for (size_t i=0; i<n; ++i)
                    {
                        vertex_t *vx = vertex.at(i);
                        if ((vx->ip == v1->ip) || (vx->ip == v2->ip) || (vx->ip == v3->ip))
                            continue;

                        ck  = dsp::check_point3d_on_triangle_p3p(v1->p, v2->p, v3->p, vx->p);
                        if (ck >= 0.0f)
                        {
//                            lsp_trace("point (%8.3f, %8.3f, %8.3f) has failed", vx->p->x, vx->p->y, vx->p->z);
                            found ++;
                            break;
                        }
                    }

                    if (found)
                    {
                        index = (index + 1) % n;
                        continue;
                    }

                    // It's an ear, there are no points inside, can emit triangle and remove the middle point
//                    lsp_trace(
//                        "emit triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
//                        v1->p->x, v1->p->y, v1->p->z,
//                        v2->p->x, v2->p->y, v2->p->z,
//                        v3->p->x, v3->p->y, v3->p->z
//                    );
                    status_t result = pObject->add_triangle(face_id, v1->ip, v2->ip, v3->ip, v1->in, v2->in, v3->in);
                    if (result != STATUS_OK)
                        return result;

                    // Remove the middle point
                    if (!vertex.remove((index + 1) % n))
                        return STATUS_BAD_STATE;

                    if (index >= (--n))
                        index = 0;
                }

                // Add last triangle
                v1 = vertex.at(0);
                v2 = vertex.at(1);
                v3 = vertex.at(2);

                ck = dsp::check_triplet3d_p3n(v1->p, v2->p, v3->p, v1->n);
                if (ck != 0.0f)
                {
//                    lsp_trace(
//                        "emit triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
//                        v1->p->x, v1->p->y, v1->p->z,
//                        v2->p->x, v2->p->y, v2->p->z,
//                        v3->p->x, v3->p->y, v3->p->z
//                    );
                    status_t result = (ck < 0.0f) ?
                        pObject->add_triangle(face_id, v1->ip, v3->ip, v2->ip, v1->in, v3->in, v2->in) :
                        pObject->add_triangle(face_id, v1->ip, v2->ip, v3->ip, v1->in, v2->in, v3->in);
                    if (result != STATUS_OK)
                        return result;
                }

                return STATUS_OK;
            }
    };


    Model3DFile::Model3DFile()
    {
    }

    Model3DFile::~Model3DFile()
    {
    }

    status_t Model3DFile::load(Scene3D **scene, const char *path)
    {
        if ((path == NULL) || (scene == NULL))
            return STATUS_BAD_ARGUMENTS;
        LSPString spath;
        if (!spath.set_utf8(path))
            return STATUS_NO_MEM;

        return load(scene, &spath);
    }

    status_t Model3DFile::load(Scene3D *scene, const char *path, bool clear)
    {
        if ((path == NULL) || (scene == NULL))
            return STATUS_BAD_ARGUMENTS;
        LSPString spath;
        if (!spath.set_utf8(path))
            return STATUS_NO_MEM;

        return load(scene, &spath, clear);
    }

    status_t Model3DFile::load(Scene3D **scene, const LSPString *path)
    {
        Scene3D *s = new Scene3D();
        if (s == NULL)
            return STATUS_NO_MEM;

        status_t status = load(s, path, false);
        if (status != STATUS_OK)
        {
            s->destroy();
            delete s;
            return status;
        }

        *scene = s;
        return STATUS_OK;
    }

    status_t Model3DFile::load_from_resource(Scene3D *scene, const void *data)
    {
        size_t iv = scene->num_vertexes();
        size_t in = scene->num_normals();

        // Fetch vertexes
        size_t nv = resource::fetch_number(&data);
        for (size_t i=0; i<nv; ++i)
        {
            point3d_t p;
            p.x     = resource::fetch_dfloat(&data);
            p.y     = resource::fetch_dfloat(&data);
            p.z     = resource::fetch_dfloat(&data);
            p.w     = 1.0f;

            ssize_t res = scene->add_vertex(&p);
            if (res < 0)
                return -res;
        }

        // Fetch normals
        size_t nn = resource::fetch_number(&data);
        for (size_t i=0; i<nn; ++i)
        {
            vector3d_t v;
            v.dx    = resource::fetch_dfloat(&data);
            v.dy    = resource::fetch_dfloat(&data);
            v.dz    = resource::fetch_dfloat(&data);
            v.dw    = 0.0f;

            ssize_t res = scene->add_normal(&v);
            if (res < 0)
                return -res;
        }

        // Fetch objects
        size_t no = resource::fetch_number(&data);
        for (size_t i=0; i<no; ++i)
        {
            const char *name = resource::fetch_dstring(&data);
            Object3D *obj = scene->add_object(name);
            if (obj == NULL)
                return STATUS_NO_MEM;

            size_t triangles= resource::fetch_number(&data);
            for (size_t j=0; j<triangles; ++j)
            {
                size_t face_id  = resource::fetch_number(&data);
                size_t v0       = resource::fetch_number(&data) + iv;
                size_t v1       = resource::fetch_number(&data) + iv;
                size_t v2       = resource::fetch_number(&data) + iv;
                size_t n0       = resource::fetch_number(&data) + in;
                size_t n1       = resource::fetch_number(&data) + in;
                size_t n2       = resource::fetch_number(&data) + in;

                ssize_t res     = obj->add_triangle(face_id, v0, v1, v2, n0, n1, n2);
                if (res < 0)
                    return -res;
            }
        }

        return STATUS_OK;
    }

    status_t Model3DFile::load(Scene3D *scene, const LSPString *path, bool clear)
    {
        if (clear)
            scene->clear();

        // Check builtin prefix
        status_t status = STATUS_OK;

        if (path->starts_with_ascii(LSP_BUILTIN_PREFIX))
        {
        #ifdef LSP_BUILTIN_RESOURCES
            const resource::resource_t *r = resource::get(path->get_utf8(10), resource::RESOURCE_3D_SCENE);
            if (r == NULL)
                return STATUS_NOT_FOUND;

            return load_from_resource(scene, r->data);
        #else
            LSPString tmp;
            if (!tmp.append_ascii("res/"))
                return STATUS_NO_MEM;
            if (!tmp.append(path, 10))
                return STATUS_NO_MEM;

            // Try to parse as obj file
            FileHandler3D fh(scene);
            status = ObjFileParser::parse(&tmp, &fh);
            if (status == STATUS_OK)
                return fh.complete();

            fh.reset_state();
        #endif
        }
        else
        {
            // Try to parse as obj file
            FileHandler3D fh(scene);
            status = ObjFileParser::parse(path, &fh);
            if (status == STATUS_OK)
                return fh.complete();

            fh.reset_state();
        }

        return status;
    }

    status_t Model3DFile::load(Scene3D **scene, const io::Path *path)
    {
        if ((path == NULL) || (scene == NULL))
            return STATUS_BAD_ARGUMENTS;
        return load(scene, path->as_string());
    }

    status_t Model3DFile::load(Scene3D *scene, const io::Path *path, bool clear)
    {
        if ((path == NULL) || (scene == NULL))
            return STATUS_BAD_ARGUMENTS;
        return load(scene, path->as_string(), clear);
    }
} /* namespace lsp */
