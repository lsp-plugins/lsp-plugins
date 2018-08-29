/*
 * task.h
 *
 *  Created on: 29 мар. 2017 г.
 *      Author: sadko
 */

#ifndef TEST_3D_TASK_H_
#define TEST_3D_TASK_H_

#define TEST_SAMPLE_RATE 48000

//#define SS_COORD    -2.0f, 4.0f, 0.0f
//#define C1_COORD    0.5f, -1.0f, -1.0f
//#define C2_COORD    -0.5f, -1.0f, -1.0f
//#define FILENAME    "auditorium-opened-10x6x4.obj"

//#define SS_COORD    -7.0f, 0.0f, -1.0f
//#define C1_COORD    7.0f, 0.5f, 0.0f
//#define C2_COORD    7.0f, -0.5f, 0.0f
//#define FILENAME    "empty-corridor-4x16x3.obj"

//#define SS_COORD    0.0f, -7.0f, -2.0f
//#define C1_COORD    0.5f, 7.0f, -2.0f
//#define C2_COORD    -0.5f, 7.0f, -2.0f
//#define FILENAME    "four-columns-hall.obj"

#define SS_COORD    0.0f, -17.0f, -3.0f
#define C1_COORD    0.5f, 17.0f, -3.0f
#define C2_COORD    -0.5f, 17.0f, -3.0f
#define FILENAME    "hall-40x40x10.obj"

//#define STRESS_TEST     65536 /*2048*/

namespace geometry3d_test
{
    void build_scene(Scene3D *s)
    {
        dsp::context_t ctx;
        dsp::start(&ctx);

    //    Model3DFile::load(s, "test_data/3d/model01.obj", true);
    //    Model3DFile::load(s, "test_data/3d/empty-room-4x4x3.obj", true);

        Model3DFile::load(s, "test_data/3d/" FILENAME, true);

    //    Model3DFile::load(s, "test_data/3d/double_ring.obj", true);

    //    for (size_t i=0; i < s->num_objects(); ++i)
    //    {
    //        Object3D *obj = s->get_object(i);
    //        lsp_trace("Object #%d name=%s", int(i), obj->get_name());
    //        obj->set_traceable(false);
    //        obj->set_visible(false);
    //    }
    //
    //    s->get_object(1)->set_visible(true);
    //    s->get_object(1)->set_traceable(true);
    //
    //    s->get_object(9)->set_visible(true);
    //    s->get_object(9)->set_traceable(true);

    //    Model3DFile::load(s, "test_data/3d/double_ring.obj", true);

    //    {
    //        Object3D *obj = new Object3D();
    //        obj->add_triangle(-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f); // Bottom
    //        obj->add_triangle(1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f);
    //        obj->add_triangle(-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f); // Top
    //        obj->add_triangle(1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f);
    //        obj->add_triangle(-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f); // Left
    //        obj->add_triangle(-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f);
    //        obj->add_triangle(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f); // Right
    //        obj->add_triangle(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
    //        obj->add_triangle(-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f); // Front
    //        obj->add_triangle(-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    //        obj->add_triangle(-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f); // Back
    //        obj->add_triangle(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);
    //
    //        obj->add_triangle(0.05f, -1.0f, -1.0f, 0.05f, -1.0f, 1.0f, 0.05f, 1.0f, 1.0f); // Middle
    //        obj->add_triangle(0.05f, -1.0f, -1.0f, 0.05f, 1.0f, 1.0f, 0.05f, 1.0f, -1.0f);
    //        obj->add_triangle(-0.05f, -1.0f, -1.0f, -0.05f, 1.0f, 1.0f, -0.05f, -1.0f, 1.0f);
    //        obj->add_triangle(-0.05f, -1.0f, -1.0f, -0.05f, 1.0f, -1.0f, -0.05f, 1.0f, 1.0f);
    //        s->add_object(obj);
    //    }

        material3d_t mat;
        mat.speed       = 1.0f;
        mat.damping     = 0.0f; //logf(0.5f) * 0.05f; // * 0.01f;
        mat.absorption  = 0.5f;
        mat.transparency= 0.3f;
        mat.reflection  = 1.0f;
        mat.refraction  = 1.1f;
        mat.diffuse     = 0.0f;

        for (size_t i=0; i<s->num_objects(); ++i)
        {
            Object3D *obj = s->get_object(i);
            obj->set_material(&mat);
    //        dsp::init_matrix3d_scale(obj->get_matrix(), 2.0f, 2.0f, 2.0f);
        }

        // Sample for capturing
        Sample sample;
        sample.init(2, TEST_SAMPLE_RATE * 4, 0);

        // Create ray source
        RaySource3D *rgen = new RaySource3D();
        rgen->init(134321123);
        rgen->set_type(RS3DT_SPHERIC);
        rgen->set_radius1(0.1f);
        rgen->set_radius2(0.2f);
        rgen->set_height(0.2f);
        dsp::init_matrix3d_translate(rgen->get_matrix(), SS_COORD);
        s->add_source(rgen);

        // Add Capture 1
        TraceCapture3D *capture = new TraceCapture3D();
        capture->init(&sample, 0);
        capture->set_sample_rate(TEST_SAMPLE_RATE);
        capture->set_radius(0.25f);

        Object3D *co = capture->build_surface(2);
        dsp::init_matrix3d_translate(co->get_matrix(), C1_COORD);
        s->add_object(co, capture);

        // Add Capture 2
        capture = new TraceCapture3D();
        capture->init(&sample, 1);
        capture->set_sample_rate(TEST_SAMPLE_RATE);
        capture->set_radius(0.25f);

        co = capture->build_surface(2);
        dsp::init_matrix3d_translate(co->get_matrix(), C2_COORD);
        s->add_object(co, capture);

        // Build trace map
        TraceMap3D *tm = s->build_trace_map();

    #ifdef STRESS_TEST
        tm->raytrace(STRESS_TEST, NULL);
    #else
        tm->raytrace(16, s);
    #endif
    //    tm->raytrace(65536, s);
    //    tm->raytrace(16, s);

        tm->destroy();

        lsp_trace("Summary info: objects=%d, triangles=%d, rays=%d, segments=%d, points=%d",
                int(s->num_objects()), int(s->num_triangles()), int(s->num_rays()), int(s->num_segments()), int(s->num_points()));

        lsp_trace("Captured sample length=%d", int(sample.length()));

        // Calculate maximum
        float max = 0.0f;
        for (size_t i=0; i<sample.channels(); ++i)
        {
            float mx = dsp::abs_max(sample.getBuffer(i), sample.length());
            if (mx > max)
                max = mx;
        }

        // Normalize sample
        for (size_t i=0; i<sample.channels(); ++i)
            dsp::scale2(sample.getBuffer(i), 1.0f/max, sample.length());

    #ifdef STRESS_TEST
        // Output responses
        float *c[2];
        c[0] = sample.getBuffer(0);
        c[1] = sample.getBuffer(1);
        size_t len = sample.length();

        FILE *fd = fopen("response.csv", "w");
    //    fprintf(fd, "i;Left;Right;\n");
        for (size_t i=0; i<len; ++i)
            fprintf(fd, "%d;%.8f;%.8f\n", int(i), *(c[0]++), *(c[1]++));
        fclose(fd);

        AudioFile af;
        af.create(sample.channels(), TEST_SAMPLE_RATE, samples_to_seconds(TEST_SAMPLE_RATE, sample.length()));
        size_t sample_length = af.samples();
        if (sample_length > sample.length())
            sample_length   = sample.length();
        for (size_t i=0; i<sample.channels(); ++i)
            dsp::copy(af.channel(i), sample.getBuffer(i), sample_length);

        af.store("response.wav");
        af.destroy();

        dsp::finish(&ctx);


        exit(0);
    #endif
    }

    #ifdef STRESS_TEST
        #undef STRESS_TEST
    #endif

    #undef TEST_SAMPLE_RATE
}

#endif /* TEST_3D_TASK_H_ */
