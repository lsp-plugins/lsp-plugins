/*
 * scene.h
 *
 *  Created on: 29 мар. 2017 г.
 *      Author: sadko
 */

#ifndef TEST_3D_VIEW_H_
#define TEST_3D_VIEW_H_

#define MAX_3D_TRIANGLES    8192
#define MAX_3D_POINTS       65536
#define MAX_3D_RAYS         8192
#define MAX_3D_SEGMENTS     65536

namespace geometry3d_test
{
    typedef struct view_t
    {
        bool            bWireframe;
        bool            bRotate;
        bool            bLight;
        bool            bInvert;
        bool            bCullFace;
        bool            bDrawRays;
        bool            bDrawSegments;
        bool            bDrawTriangles;
        bool            bDrawPoints;
        bool            bDrawNormals;
        bool            bDrawCapture;
        bool            bDrawSource;

        float           fAngleX;
        float           fAngleY;
        float           fAngleZ;
        float           fScale;

        float           fAngleDX;
        float           fAngleDY;
        float           fAngleDZ;
        float           fDeltaScale;

        Scene3D        *pScene;
    } scene_t;

    bool is_supported(const char *set, const char *ext)
    {
        size_t ck_len = strlen(ext);

        while (set != NULL)
        {
            char *sep = strchr(const_cast<char *>(set), ' ');
            if (sep == NULL)
                sep = strchr(const_cast<char *>(set), '\0');

            size_t len = sep - set;
            if (len == ck_len)
            {
                if (!strncasecmp(set, ext, len))
                    return true;
            }

            set = (*sep == '\0') ? NULL : sep + 1;
        }

        return false;
    }

    void init_view(view_t *v)
    {
        v->bWireframe       = false;
        v->bRotate          = true;
        v->bLight           = true;
        v->bInvert          = false;
        v->bCullFace        = true;

        v->bDrawRays        = true;
        v->bDrawTriangles   = true;
        v->bDrawPoints      = true;
        v->bDrawNormals     = true;
        v->bDrawSegments    = true;
        v->bDrawCapture     = true;
        v->bDrawSource      = true;

        v->fAngleX          = 0.0f;
        v->fAngleY          = 0.0f;
        v->fAngleZ          = 0.0f;
        v->fScale           = 1.0f;

        v->fAngleDX         = 0.0f;
        v->fAngleDY         = 0.0f;
        v->fAngleDZ         = 0.0f;
        v->fDeltaScale      = 0.0f;

        const char* extensions = (const char *)glGetString(GL_EXTENSIONS);
        if (extensions != NULL)
        {
            lsp_trace("OpenGL extension list: %s", extensions);
            lsp_trace("GL_ARB_arrays_of_arrays supported: %s", is_supported(extensions, "gl_arb_arrays_of_arrays") ? "true" : "false");
            lsp_trace("GL_SUN_slice_accum supported: %s", is_supported(extensions, "gl_sun_slice_accum") ? "true" : "false");
            lsp_trace("INVALID_EXTENSION supported: %s", is_supported(extensions, "invalid_extension") ? "true" : "false");
            // TODO: detect several set of extensions
        }

        v->pScene           = NULL;
    }

    void destroy_view(view_t *v)
    {
        if (v->pScene != NULL)
        {
            v->pScene->destroy();
            v->pScene = NULL;
        }
    }

    void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
    {
        GLdouble fW, fH;

        fH = tan( fovY * M_PI / 360.0f) * zNear;
        fW = fH * aspect;

        glFrustum( -fW, fW, -fH, fH, zNear, zFar );
    }

    float light_pos[] = { 0.0, 0.0, 3.0f };

    void render_view(view_t *v, int width, int height)
    {
        glEnable(GL_DEPTH_TEST);
        if (v->bCullFace)
        {
            glEnable(GL_CULL_FACE);
            glCullFace((v->bInvert) ? GL_FRONT : GL_BACK);
        }
        glEnable(GL_COLOR_MATERIAL);

        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        perspectiveGL(90.0f, float(width)/float(height), 0.1f, 100.0f);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0.0f, 0.0f, -6.0f);

        float scale = v->fScale + v->fDeltaScale;
        glScalef(scale, scale, scale); // Scale

        if (v->bLight)
        {
            glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHTING);
            glEnable(GL_RESCALE_NORMAL);
        }

        glRotatef(v->fAngleX + v->fAngleDX, 1.0f, 0.0f, 0.0f);
        glRotatef(v->fAngleY + v->fAngleDY, 0.0f, 1.0f, 0.0f);
        glRotatef(v->fAngleZ + v->fAngleDZ, 0.0f, 0.0f, 1.0f);

        glPolygonOffset(-1, -1);
        glEnable(GL_POLYGON_OFFSET_POINT);
        glPointSize(5.0f);

        // Draw triangles
        if (v->bDrawTriangles)
        {
            matrix3d_t mv, m;
            glGetFloatv(GL_MODELVIEW_MATRIX, mv.m);

            Object3D **objs = v->pScene->get_objects();
            TraceCapture3D **capts = v->pScene->get_captures();

            for (size_t i = v->pScene->num_objects(); i>0; --i)
            {
                Object3D *obj        = *(objs++);
                TraceCapture3D *capt = *(capts++);
                if ((obj == NULL) || (capt != NULL) || (!obj->is_visible()))
                    continue;

                dsp::apply_matrix3d_mm2(&m, &mv, obj->get_matrix());
                glLoadMatrixf(m.m);

                point3d_t *tr       = obj->get_vertexes();
                vector3d_t *tn      = obj->get_normals();
                vertex_index_t *vvx = obj->get_vertex_indexes();
                vertex_index_t *vnx = obj->get_normal_indexes();

                for (ssize_t i=obj->get_triangles_count(); i > 0; --i)
                {
                    glBegin((v->bWireframe) ? GL_LINE_LOOP : GL_TRIANGLES);

                        glColor3f(1.0f, 0.0f, 0.0f);
                        glNormal3fv(&tn[*(vnx++)].dx);
                        glVertex3fv(&tr[*(vvx++)].x);

                        glColor3f(0.0f, 1.0f, 0.0f);
                        glNormal3fv(&tn[*(vnx++)].dx);
                        glVertex3fv(&tr[*(vvx++)].x);

                        glColor3f(0.0f, 0.0f, 1.0f);
                        glNormal3fv(&tn[*(vnx++)].dx);
                        glVertex3fv(&tr[*(vvx++)].x);

                    glEnd();
                }

                if (v->bDrawNormals)
                {
    //                tr = obj->get_triangles();
                    tr      = obj->get_vertexes();
                    tn      = obj->get_normals();
                    vvx     = obj->get_vertex_indexes();
                    vnx     = obj->get_normal_indexes();

                    for (ssize_t i=obj->get_triangles_count(); i > 0; --i)
                    {
                        glColor3f(1.0f, 1.0f, 0.0f);
                        glBegin(GL_LINES);
                            for (int j=0; j<3; ++j)
                            {
                                point3d_t *xp   = &tr[*(vvx++)];
                                vector3d_t *xv  = &tn[*(vnx++)];

                                glVertex3fv(&xp->x);
                                glVertex3f(xp->x + xv->dx, xp->y + xv->dy, xp->z + xv->dz);
                            }
                        glEnd();
                    }
                }
            }
            glLoadMatrixf(mv.m);
        }

        if (v->bDrawSource)
        {
            matrix3d_t mv, m;
            glGetFloatv(GL_MODELVIEW_MATRIX, mv.m);

            RaySource3D **rs = v->pScene->get_sources();

            for (size_t i = v->pScene->num_sources(); i>0; --i)
            {
                RaySource3D *rrs    = *(rs++);
                if (rrs == NULL)
                    continue;

                dsp::apply_matrix3d_mm2(&m, &mv, rrs->get_matrix());
                glLoadMatrixf(m.m);

                float r = (rrs->get_radius1() + rrs->get_radius2()) * 0.5f;

                glColor3f(1.0f, 0.5f, 0.0f);
                glBegin(GL_LINE_LOOP);
                    glVertex3f(r, r, r);
                    glVertex3f(r, -r, r);
                    glVertex3f(-r, -r, r);
                    glVertex3f(-r, r, r);
                glEnd();

                glBegin(GL_LINE_LOOP);
                    glVertex3f(r, r, -r);
                    glVertex3f(r, -r, -r);
                    glVertex3f(-r, -r, -r);
                    glVertex3f(-r, r, -r);
                glEnd();

                glBegin(GL_LINES);
                    glVertex3f(r, r, -r);
                    glVertex3f(r, r, r);

                    glVertex3f(r, -r, -r);
                    glVertex3f(r, -r, r);

                    glVertex3f(-r, -r, -r);
                    glVertex3f(-r, -r, r);

                    glVertex3f(-r, r, -r);
                    glVertex3f(-r, r, r);
                glEnd();

                glBegin(GL_POINTS);
                    glVertex3f(0.0f, 0.0f, 0.0f);
                glEnd();

    //            r *= 2.0f;
    //
    //            glBegin(GL_LINES);
    //                glVertex3f(0.0f, 0.0f, 0.0f);
    //                glVertex3f(r, 0.0f, 0.0f);
    //                glVertex3f(r, 0.0f, 0.0f);
    //                glVertex3f(r * 0.8f, r * 0.05f, 0.0f);
    //                glVertex3f(r, 0.0f, 0.0f);
    //                glVertex3f(r * 0.8f, r * -0.05f, 0.0f);
    //                glVertex3f(r, 0.0f, 0.0f);
    //                glVertex3f(r * 0.8f, 0.0f, r * 0.05f);
    //                glVertex3f(r, 0.0f, 0.0f);
    //                glVertex3f(r * 0.8f, 0.0f, r * -0.05f);
    //            glEnd();
            }

            glLoadMatrixf(mv.m);
        }

        if (v->bDrawCapture)
        {
            matrix3d_t mv, m;
            glGetFloatv(GL_MODELVIEW_MATRIX, mv.m);

            Object3D **objs = v->pScene->get_objects();
            TraceCapture3D **capts = v->pScene->get_captures();

            for (size_t i = v->pScene->num_objects(); i>0; --i)
            {
                Object3D *obj        = *(objs++);
                TraceCapture3D *capt = *(capts++);
                if ((obj == NULL) || (capt == NULL) || (!obj->is_visible()))
                    continue;

                dsp::apply_matrix3d_mm2(&m, &mv, obj->get_matrix());
                glLoadMatrixf(m.m);

                point3d_t *tr       = obj->get_vertexes();
                vector3d_t *tn      = obj->get_normals();
                vertex_index_t *vvx = obj->get_vertex_indexes();
                vertex_index_t *vnx = obj->get_normal_indexes();

                for (ssize_t i=obj->get_triangles_count(); i > 0; --i)
                {
                    glBegin((v->bWireframe) ? GL_LINE_LOOP : GL_TRIANGLES);

                        glColor3f(1.0f, 0.0f, 0.0f);
                        glNormal3fv(&tn[*(vnx++)].dx);
                        glVertex3fv(&tr[*(vvx++)].x);

                        glColor3f(0.0f, 1.0f, 0.0f);
                        glNormal3fv(&tn[*(vnx++)].dx);
                        glVertex3fv(&tr[*(vvx++)].x);

                        glColor3f(0.0f, 0.0f, 1.0f);
                        glNormal3fv(&tn[*(vnx++)].dx);
                        glVertex3fv(&tr[*(vvx++)].x);

                    glEnd();
                }

                float r = capt->get_radius();

                glColor3f(1.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
                    glVertex3f(r, r, r);
                    glVertex3f(r, -r, r);
                    glVertex3f(-r, -r, r);
                    glVertex3f(-r, r, r);
                glEnd();

                glBegin(GL_LINE_LOOP);
                    glVertex3f(r, r, -r);
                    glVertex3f(r, -r, -r);
                    glVertex3f(-r, -r, -r);
                    glVertex3f(-r, r, -r);
                glEnd();

                glBegin(GL_LINES);
                    glVertex3f(r, r, -r);
                    glVertex3f(r, r, r);

                    glVertex3f(r, -r, -r);
                    glVertex3f(r, -r, r);

                    glVertex3f(-r, -r, -r);
                    glVertex3f(-r, -r, r);

                    glVertex3f(-r, r, -r);
                    glVertex3f(-r, r, r);
                glEnd();

                glBegin(GL_POINTS);
                    glVertex3f(0.0f, 0.0f, 0.0f);
                glEnd();

                r *= 2.0f;

                glBegin(GL_LINES);
                    glVertex3f(0.0f, 0.0f, 0.0f);
                    glVertex3f(r, 0.0f, 0.0f);
                    glVertex3f(r, 0.0f, 0.0f);
                    glVertex3f(r * 0.8f, r * 0.05f, 0.0f);
                    glVertex3f(r, 0.0f, 0.0f);
                    glVertex3f(r * 0.8f, r * -0.05f, 0.0f);
                    glVertex3f(r, 0.0f, 0.0f);
                    glVertex3f(r * 0.8f, 0.0f, r * 0.05f);
                    glVertex3f(r, 0.0f, 0.0f);
                    glVertex3f(r * 0.8f, 0.0f, r * -0.05f);
                glEnd();
            }
            glLoadMatrixf(mv.m);
        }

        if (v->bLight)
        {
            glDisable(GL_RESCALE_NORMAL);
            glDisable(GL_LIGHTING);
        }

        // Draw segments
        if (v->bDrawSegments)
        {
            segment3d_t *s = v->pScene->get_segments();

            for (size_t i=v->pScene->num_segments(); i>0; --i, ++s)
            {
                glColor3f(0.0f, 1.0f, 0.0f);
                glBegin(GL_POINTS);
                    glVertex3fv(&s->p[0].x);
                    glVertex3fv(&s->p[1].x);
                glEnd();

                glBegin(GL_LINES);
                    glVertex3fv(&s->p[0].x);
                    glVertex3fv(&s->p[1].x);
                glEnd();
            }
        }

        // Draw rays
        if (v->bDrawRays)
        {
            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            ray3d_t *r = v->pScene->get_rays();
            glPointSize(5.0f);

            for (size_t i=v->pScene->num_rays(); i>0; --i, ++r)
            {
                if (r->v.dw < 0.0f)
                    continue;

                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                glBegin(GL_POINTS);
                    glVertex3fv(&r->z.x);
                glEnd();

                glBegin(GL_LINES);
                    glVertex3fv(&r->z.x);
                    glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
                    glVertex3f(r->z.x + r->v.dx*4.0f, r->z.y + r->v.dy*4.0f, r->z.z + r->v.dz*4.0f);
                glEnd();
            }

            glDisable(GL_BLEND);
        }

        // Draw points
        if (v->bDrawPoints)
        {
            point3d_t *p = v->pScene->get_points();

            glColor3f(0.0f, 1.0f, 1.0f);
            glBegin(GL_POINTS);
                for (size_t i=v->pScene->num_points(); i>0; --i, ++p)
                    glVertex3fv(&p->x);
            glEnd();
        }

        if (v->bLight)
            glEnable(GL_LIGHTING);

        glDisable(GL_POLYGON_OFFSET_POINT);

        if (v->bLight)
            glDisable(GL_LIGHTING);

        // Draw axis coordinates
        glDisable(GL_DEPTH_TEST);

        glLoadIdentity();
        glTranslatef(-0.8f * float(width)/float(height), -0.8f /* float(height)/float(width) */, -0.9f);
        glScalef(0.2, 0.2, 0.2); // Scale

        glRotatef(v->fAngleX + v->fAngleDX, 1.0f, 0.0f, 0.0f);
        glRotatef(v->fAngleY + v->fAngleDY, 0.0f, 1.0f, 0.0f);
        glRotatef(v->fAngleZ + v->fAngleDZ, 0.0f, 0.0f, 1.0f);

        glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, 0.05f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, -0.05f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, 0.0f, 0.05f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, 0.0f, -0.05f);

            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.8f, 0.05f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.8f, -0.05f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.05f, 0.8f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-0.05f, 0.8f, 0.0f);

            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 1.0f);

            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.05f, 0.0f, 0.8f);
            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-0.05f, 0.0f, 0.8f);
            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.05f, 0.8f);
            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, -0.05f, 0.8f);
        glEnd();

        if (v->bCullFace)
            glDisable(GL_CULL_FACE);

        // Rotate scene if it is possible
        if (v->bRotate)
        {
            v->fAngleX -= 0.3f;
            v->fAngleZ -= 0.4f;
        }
    }
}

#endif /* TEST_3D_VIEW_H_ */
