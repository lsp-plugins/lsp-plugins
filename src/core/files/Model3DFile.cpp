/*
 * Model3DFile.cpp
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#include <core/debug.h>
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
                point3d_t  *p;
                vector3d_t *n;
                ssize_t     ip;
                ssize_t     in;
            } vertex_t;

        protected:
            Scene3D        *pScene;
            Object3D       *pObject;
            cstorage<vertex_t> sVertex;

        public:
            FileHandler3D(Scene3D *scene)
            {
                pScene      = scene;
                pObject     = NULL;
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
                    pScene->destroy(true);

                if (pObject != NULL)
                {
                    pObject->destroy();
                    delete pObject;
                    pObject = NULL;
                }
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

                pObject = new Object3D();
                if (pObject == NULL)
                    return STATUS_NO_MEM;

                return (pObject->set_name(name)) ? STATUS_OK : STATUS_NO_MEM;
            }

            virtual status_t end_object(size_t id)
            {
                if (pObject == NULL)
                    return STATUS_BAD_STATE;

                if (pScene->add_object(pObject))
                {
                    pObject = NULL;
                    return STATUS_OK;
                }

                pObject->destroy();
                delete pObject;
                pObject = NULL;
                return STATUS_NO_MEM;
            }

            virtual status_t add_vertex(const point3d_t *p)
            {
                if (pObject == NULL)
                    return STATUS_BAD_STATE;
                ssize_t idx = pObject->add_vertex(p);
                return (idx < 0)? status_t(-idx) : STATUS_OK;
            }

            virtual status_t add_normal(const vector3d_t *v)
            {
                if (pObject == NULL)
                    return STATUS_BAD_STATE;
                ssize_t idx = pObject->add_normal(v);
                return (idx < 0)? status_t(-idx) : STATUS_OK;
            }

            virtual status_t add_face(const ssize_t *vv, const ssize_t *vn, const ssize_t *vt, size_t n)
            {
                if (n < 3)
                    return STATUS_BAD_STATE;

                // Prepare structure
                sVertex.clear();
                for (size_t i=0; i<n; ++i)
                {
                    vertex_t *vx        = sVertex.append();
                    if (vx == NULL)
                        return STATUS_NO_MEM;

                    vx->ip              = *(vv++);
                    vx->p               = (vx->ip >= 0) ? pObject->get_vertex(vx->ip) : NULL;
                    if (vx->p == NULL)
                        return STATUS_BAD_STATE;
                    vx->in              = *(vn++);
                    vx->n               = (vx->in >= 0) ? pObject->get_normal(vx->in) : NULL;
                    if (vx->n == NULL)
                        return STATUS_BAD_STATE;
                }

                // Triangulation algorithm
                vertex_t *v1, *v2, *v3;
                size_t index = 0;
                float ck = 0.0f;

                while (n > 3)
                {
                    v1 = sVertex.at(index % n);
                    v2 = sVertex.at((index+1) % n);
                    v3 = sVertex.at((index+2) % n);

                    lsp_trace(
                        "analyzing triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
                        v1->p->x, v1->p->y, v1->p->z,
                        v2->p->x, v2->p->y, v2->p->z,
                        v3->p->x, v3->p->y, v3->p->z
                    );

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
                        if (!sVertex.remove((index + remove) % n))
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
                        vertex_t *vx = sVertex.at(i);
                        if ((vx->ip == v1->ip) || (vx->ip == v2->ip) || (vx->ip == v3->ip))
                            continue;

                        ck  = dsp::check_point3d_on_triangle_p3p(v1->p, v2->p, v3->p, vx->p);
                        if (ck >= 0.0f)
                        {
                            lsp_trace("point (%8.3f, %8.3f, %8.3f) has failed", vx->p->x, vx->p->y, vx->p->z);
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
                    lsp_trace(
                        "emit triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
                        v1->p->x, v1->p->y, v1->p->z,
                        v2->p->x, v2->p->y, v2->p->z,
                        v3->p->x, v3->p->y, v3->p->z
                    );
                    status_t result = pObject->add_triangle(v1->ip, v2->ip, v3->ip, v1->in, v2->in, v3->in);
                    if (result != STATUS_OK)
                        return result;

                    // Remove the middle point
                    if (!sVertex.remove((index + 1) % n))
                        return STATUS_BAD_STATE;

                    if (index >= (--n))
                        index = 0;
                }

                // Add last triangle
                v1 = sVertex.at(0);
                v2 = sVertex.at(1);
                v3 = sVertex.at(2);

                ck = dsp::check_triplet3d_p3n(v1->p, v2->p, v3->p, v1->n);
                if (ck != 0.0f)
                {
                    lsp_trace(
                        "emit triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
                        v1->p->x, v1->p->y, v1->p->z,
                        v2->p->x, v2->p->y, v2->p->z,
                        v3->p->x, v3->p->y, v3->p->z
                    );
                    status_t result = (ck < 0.0f) ?
                        pObject->add_triangle(v1->ip, v3->ip, v2->ip, v1->in, v2->in, v3->in) :
                        pObject->add_triangle(v1->ip, v2->ip, v3->ip, v1->in, v2->in, v3->in);
                    if (result != STATUS_OK)
                        return result;
                }

                return STATUS_OK;
            }

//            virtual status_t face_vertex(size_t id, const ssize_t *iv, const point3d_t **v, size_t n)
//            {
//                if (nIndex != size_t(-1))
//                    return STATUS_BAD_STATE;
//                if (n < 3)
//                    return STATUS_CORRUPTED_FILE;
//
//                vertex_t vx;
//                vx.n.dx     = 0.0f;
//                vx.n.dy     = 0.0f;
//                vx.n.dz     = 0.0f;
//                vx.n.dw     = 0.0f;
//                vx.in       = -1;
//
//                // Store vertexes
//                sVertex.clear();
//
//                while (n--)
//                {
//                    const point3d_t *p  = *(v++);
//                    size_t idx          = *(iv++);
//                    if (p != NULL)
//                    {
//                        vx.p        = *p;
//                        vx.ip       = idx;
//                        if (!sVertex.add(&vx))
//                            return STATUS_NO_MEM;
//                    }
//                }
//
//                nIndex      = id;
//
//                return STATUS_OK;
//            }

//            virtual status_t face_normal(size_t id, const ssize_t *iv, const vector3d_t **v, size_t n)
//            {
//                if (nIndex != id)
//                    return STATUS_BAD_STATE;
//                if (n != sVertex.size())
//                    return STATUS_BAD_STATE;
//
//                // Update normals
//                for (size_t i=0; i<n; ++i)
//                {
//                    const vector3d_t *nx = *(v++);
//                    size_t idx           = *(iv++);
//                    if (nx == NULL)
//                        continue;
//
//                    vertex_t *vx        = sVertex.get(i);
//                    if (vx == NULL)
//                        return STATUS_BAD_STATE;
//
//                    vx->n       = *nx;
//                    vx->in      = idx;
//                }
//
//                // Triangulation algorithm
//                vertex_t *v1, *v2, *v3;
//                size_t index = 0;
//                float ck = 0.0f;
//
//                while (n > 3)
//                {
//                    v1 = sVertex.at(index % n);
//                    v2 = sVertex.at((index+1) % n);
//                    v3 = sVertex.at((index+2) % n);
//
//                    lsp_trace(
//                        "analyzing triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
//                        v1->p.x, v1->p.y, v1->p.z,
//                        v2->p.x, v2->p.y, v2->p.z,
//                        v3->p.x, v3->p.y, v3->p.z
//                    );
//
//                    // Check that it is an ear
//                    ck = dsp::check_triplet3d_p3n(&v1->p, &v2->p, &v3->p, &v1->n);
//                    if (ck < 0.0f)
//                    {
//                        index = (index + 1) % n;
//                        continue;
//                    }
//                    else if (ck == 0.0f)
//                    {
//                        size_t longest = dsp::longest_edge3d_p3(&v1->p, &v2->p, &v3->p);
//                        size_t remove = (longest + 2) % 3;
//
//                        // Need to eliminate point that lies on the line
//                        if (!sVertex.remove((index + remove) % n))
//                            return STATUS_BAD_STATE;
//
//                        // Rollback index and decrement counter
//                        n--;
//                        index = (index > 0) ? index - 1 : n-1;
//                        continue;
//                    }
//
//                    // Now ensure that there are no other points inside the triangle
//                    int found = 0;
//                    for (size_t i=0; i<n; ++i)
//                    {
//                        vertex_t *vx = sVertex.at(i);
//                        if ((vx->ip == v1->ip) || (vx->ip == v2->ip) || (vx->ip == v3->ip))
//                            continue;
//
////                        ck  = dsp::check_point3d_location_p3p(&v1->p, &v2->p, &v3->p, &vx->p);
////                        if (ck == 0.0f)
////                        {
////                            ck  = dsp::check_point3d_on_edge_p2p(&v1->p, &v3->p, &vx->p);
////                            if ((ck >= 0.0f) && ((vx->ip == v1->ip) || (vx->ip == v3->ip)))
////                                ck = -1.0f;
////                        }
//                        ck  = dsp::check_point3d_on_triangle_p3p(&v1->p, &v2->p, &v3->p, &vx->p);
//                        if (ck >= 0.0f)
//                        {
//                            lsp_trace("point (%8.3f, %8.3f, %8.3f) has failed", vx->p.x, vx->p.y, vx->p.z);
//                            found ++;
//                            break;
//                        }
//                    }
//
//                    if (found)
//                    {
//                        index = (index + 1) % n;
//                        continue;
//                    }
//
//                    // It's an ear, there are no points inside, can emit triangle and remove the middle point
//                    lsp_trace(
//                        "emit triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
//                        v1->p.x, v1->p.y, v1->p.z,
//                        v2->p.x, v2->p.y, v2->p.z,
//                        v3->p.x, v3->p.y, v3->p.z
//                    );
//                    if (!pObject->add_triangle(&v1->p, &v2->p, &v3->p))
//                        return STATUS_NO_MEM;
//
//                    // Remove the middle point
//                    if (!sVertex.remove((index + 1) % n))
//                        return STATUS_BAD_STATE;
//
//                    if (index >= (--n))
//                        index = 0;
//                }
//
//                // Add last triangle
//                v1 = sVertex.at(0);
//                v2 = sVertex.at(1);
//                v3 = sVertex.at(2);
//
//                ck = dsp::check_triplet3d_p3n(&v1->p, &v2->p, &v3->p, &v1->n);
//                if (ck != 0.0f)
//                {
//                    lsp_trace(
//                        "emit triangle (%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f):(%8.3f, %8.3f, %8.3f)",
//                        v1->p.x, v1->p.y, v1->p.z,
//                        v2->p.x, v2->p.y, v2->p.z,
//                        v3->p.x, v3->p.y, v3->p.z
//                    );
//                    bool result = (ck < 0.0f) ?
//                        pObject->add_triangle(&v1->p, &v3->p, &v2->p) :
//                        pObject->add_triangle(&v1->p, &v2->p, &v3->p);
//                    if (!result)
//                        return STATUS_NO_MEM;
//                }
//                nIndex  = -1;
//
//                return STATUS_OK;
//            }
    };


    Model3DFile::Model3DFile()
    {
    }

    Model3DFile::~Model3DFile()
    {
    }

    status_t Model3DFile::load(Scene3D **scene, const char *path)
    {
        Scene3D *s = new Scene3D();
        if (s == NULL)
            return STATUS_NO_MEM;

        status_t status = load(s, path, false);
        if (status != STATUS_OK)
        {
            s->destroy(true);
            delete s;
            return status;
        }

        *scene = s;
        return STATUS_OK;
    }

    status_t Model3DFile::load(Scene3D *scene, const char *path, bool clear)
    {
        if (clear)
            scene->clear();

        FileHandler3D fh(scene);

        // Try to parse as obj file
        status_t status = ObjFileParser::parse(path, &fh);
        if (status == STATUS_OK)
            return fh.complete();

        fh.reset_state();
        return status;
    }
} /* namespace lsp */
