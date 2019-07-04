/*
 * raytrace.cpp
 *
 *  Created on: 21 февр. 2019 г.
 *      Author: sadko
 */


#include <test/mtest.h>
#include <core/3d/rt_context.h>
#include <core/3d/RayTrace3D.h>
#include <core/files/Model3DFile.h>
#include <core/sampling/Sample.h>
#include <core/files/AudioFile.h>

using namespace lsp;

//-----------------------------------------------------------------------------
// Performance test for equalizer module
MTEST_BEGIN("core.3d", raytrace)

    MTEST_MAIN
    {
        ssize_t nthreads = 1;
        LSPString path;

        // Parse arguments
        MTEST_ASSERT(path.fmt_utf8("tmp/utest-raytrace", this->full_name()));
        for (int i=0; i<argc; )
        {
            const char *arg = argv[i++];
            if (!strcmp(arg, "--threads"))
            {
                MTEST_ASSERT(i < argc);
                nthreads = atoi(argv[i++]);
                MTEST_ASSERT(nthreads >= 0);
            }
            else if (!strcmp(arg, "--outfile"))
            {
                MTEST_ASSERT(i < argc);
                MTEST_ASSERT(path.set_native(argv[i++]));
            }
            else
            {
                MTEST_FAIL_MSG("Unknown argument: %s", arg);
            }
        }

        // Perform assertions
        MTEST_ASSERT_MSG(!(sizeof(rtm_vertex_t) & 0x0f), "sizeof(rt_vertex_t) = 0x%x", int(sizeof(rtm_vertex_t)));
        MTEST_ASSERT_MSG(!(sizeof(rtm_edge_t) & 0x0f), "sizeof(rt_edge_t) = 0x%x", int(sizeof(rtm_edge_t)));
        MTEST_ASSERT_MSG(!(sizeof(rtm_triangle_t) & 0x0f), "sizeof(rt_triangle_t) = 0x%x", int(sizeof(rtm_triangle_t)));

        // Load scene
        Scene3D scene;
//        MTEST_ASSERT(Model3DFile::load(&scene, "res/test/3d/empty-room-4x4x3.obj", true) == STATUS_OK);
//        MTEST_ASSERT(Model3DFile::load(&scene, "res/test/3d/devel-room.obj", true) == STATUS_OK);
        MTEST_ASSERT(Model3DFile::load(&scene, "res/test/3d/simple-4-columns.obj", true) == STATUS_OK);
//        MTEST_ASSERT(Model3DFile::load(&scene, "res/test/3d/boolean/crossing-cubes.obj", true) == STATUS_OK);
//        MTEST_ASSERT(Model3DFile::load(&scene, "res/test/3d/plane-8x.obj", true) == STATUS_OK);

        // Initialize sample
        Sample direct, early, late, all, indirect;
        MTEST_ASSERT(direct.init(2, 512, 0));
        MTEST_ASSERT(early.init(2, 512, 0));
        MTEST_ASSERT(late.init(2, 512, 0));
        MTEST_ASSERT(all.init(2, 512, 0));
        MTEST_ASSERT(indirect.init(2, 512, 0));

        // Prepare raytracing
        RayTrace3D trace;
        MTEST_ASSERT(trace.init() == STATUS_OK);
        trace.set_sample_rate(48000);

//        trace.set_energy_threshold(1e-6f);
        trace.set_energy_threshold(1e-5f);

        trace.set_tolerance(1e-5f);
//        trace.set_tolerance(1e-10f);

        trace.set_detalization(1e-9f);
//        trace.set_detalization(1e-10f);

//        trace.set_energy_threshold(1e-4f);
//        trace.set_tolerance(1e-4f);
        MTEST_ASSERT(trace.set_scene(&scene, true) == STATUS_OK);

        // Add source
        rt_source_settings_t src;
        dsp::init_matrix3d_identity(&src.pos);
        src.type        = RT_AS_ICOSPHERE;
        src.size        = 0.3048f; // 12" speaker source
        src.height      = 0.3048f;
        src.angle       = 0.0f;
        src.curvature   = 0.0f;
        src.amplitude   = 1.0f;

        MTEST_ASSERT(trace.add_source(&src) == STATUS_OK);

        // Add capture
        ray3d_t cap_l, cap_r;
        dsp::init_point_xyz(&cap_l.z, 1.0f, -0.06f, 0.0f);
        dsp::init_point_xyz(&cap_r.z, 1.0f, 0.06f, 0.0f);
        dsp::init_vector_dxyz(&cap_l.v, -M_SQRT2, +M_SQRT2, 0.0f); // 2" microphone diaphragm
        dsp::init_vector_dxyz(&cap_r.v, -M_SQRT2, -M_SQRT2, 0.0f); // 2" microphone diaphragm

        rt_capture_settings_t cs_l, cs_r;
        dsp::calc_matrix3d_transform_r1(&cs_l.pos, &cap_l);
        dsp::calc_matrix3d_transform_r1(&cs_r.pos, &cap_r);
        cs_l.radius = 0.0254 * 2; // 2" diaphragm
        cs_r.radius = 0.0254 * 2; // 2" diaphragm
        cs_l.type   = RT_AC_CARDIO;
        cs_r.type   = RT_AC_CARDIO;

        MTEST_ASSERT(trace.add_capture(&cs_l) == 0);
        MTEST_ASSERT(trace.add_capture(&cs_r) == 1);

        // Left audio channel
        trace.bind_capture(0, &direct, 0, 0, 0);    // Direct reflections
        trace.bind_capture(0, &early, 0, 1, 3);     // Early reflections
        trace.bind_capture(0, &late, 0, 4, -1);     // Late reflections
        trace.bind_capture(0, &all, 0, -1, -1);     // All reflections
        trace.bind_capture(0, &indirect, 0, 1, -1); // Indirect (all except direct) reflections

        // Right audio channel
        trace.bind_capture(1, &direct, 1, 0, 0);    // Direct reflections
        trace.bind_capture(1, &early, 1, 1, 3);     // Early reflections
        trace.bind_capture(1, &late, 1, 4, -1);     // Late reflections
        trace.bind_capture(1, &all, 1, -1, -1);     // All reflections
        trace.bind_capture(1, &indirect, 1, 1, -1); // Indirect (all except direct) reflections

        // Perform raytracing
        MTEST_ASSERT(trace.process(nthreads, 1.0f) == STATUS_OK);
        lsp_trace("Sample parameters: channels=%d, length=%d", int(early.channels()), int(early.length()));

        // Save sample to file
        AudioFile af;
        io::Path outfile;

        MTEST_ASSERT(outfile.set(&path) == STATUS_OK);
        MTEST_ASSERT(outfile.concat("-direct.wav") == STATUS_OK);
        MTEST_ASSERT(af.create(&direct, trace.get_sample_rate()) == STATUS_OK);
        MTEST_ASSERT(af.store(&outfile) == STATUS_OK);

        MTEST_ASSERT(outfile.set(&path) == STATUS_OK);
        MTEST_ASSERT(outfile.concat("-early.wav") == STATUS_OK);
        MTEST_ASSERT(af.create(&early, trace.get_sample_rate()) == STATUS_OK);
        MTEST_ASSERT(af.store(&outfile) == STATUS_OK);

        MTEST_ASSERT(outfile.set(&path) == STATUS_OK);
        MTEST_ASSERT(outfile.concat("-late.wav") == STATUS_OK);
        MTEST_ASSERT(af.create(&late, trace.get_sample_rate()) == STATUS_OK);
        MTEST_ASSERT(af.store(&outfile) == STATUS_OK);\

        MTEST_ASSERT(outfile.set(&path) == STATUS_OK);
        MTEST_ASSERT(outfile.concat("-all.wav") == STATUS_OK);
        MTEST_ASSERT(af.create(&all, trace.get_sample_rate()) == STATUS_OK);
        MTEST_ASSERT(af.store(&outfile) == STATUS_OK);

        MTEST_ASSERT(outfile.set(&path) == STATUS_OK);
        MTEST_ASSERT(outfile.concat("-indirect.wav") == STATUS_OK);
        MTEST_ASSERT(af.create(&indirect, trace.get_sample_rate()) == STATUS_OK);
        MTEST_ASSERT(af.store(&outfile) == STATUS_OK);

        // Destroy objects
        trace.destroy(false);
        scene.destroy();
    }
MTEST_END

