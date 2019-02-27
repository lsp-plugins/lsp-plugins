/*
 * RayTrace3D.hpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_3D_RAYTRACE3D_H_
#define CORE_3D_RAYTRACE3D_H_

#include <dsp/dsp.h>
#include <data/cvector.h>
#include <data/cstorage.h>
#include <core/sampling/Sample.h>
#include <core/3d/common.h>
#include <core/3d/rt_mesh.h>
#include <core/3d/rt_context.h>
#include <core/3d/RTObjectFactory.h>
#include <core/ipc/Thread.h>
#include <core/ipc/Mutex.h>

namespace lsp
{
    enum rt_audio_source_t
    {
        RT_AS_SPOT,
        RT_AS_SPEAKER,
        RT_AS_OMNI,
        RT_AS_TRIANGLE
    };

    enum rt_audio_capture_t
    {
        RT_AC_CARDIOID,
        RT_AC_SUPERCARDIOID,
        RT_AC_HYPERCARDIOID,
        RT_AC_BIDIRECTIONAL,
        RT_AC_OMNIDIRECTIONAL
    };

    /** Ray tracing storage implemented as a stack
     *
     */
    class RayTrace3D
    {
        protected:
            typedef struct source_t
            {
                ray3d_t             position;
                rt_audio_source_t   type;
                float               volume;
            } source_t;

            typedef struct sample_t
            {
                Sample             *sample;
                size_t              channel;
                ssize_t             r_min;
                ssize_t             r_max;
            } sample_t;

            typedef struct capture_t
            {
                matrix3d_t          matrix;
                ray3d_t             position;
                rt_material_t       material;
                rt_audio_capture_t  type;
                cstorage<sample_t>  bindings;
                float               gain;
                float               volume;
            } capture_t;

            typedef struct stats_t
            {
                uint64_t            tasks_stolen;
                uint64_t            calls_scan;
                uint64_t            calls_cull;
                uint64_t            calls_split;
                uint64_t            calls_cullback;
                uint64_t            calls_reflect;
                uint64_t            calls_capture;
            } stats_t;

        protected:
            class TaskThread: public ipc::Thread
            {
                private:
                    RayTrace3D             *trace;
                    stats_t                 stats;
                    cvector<rt_context_t>   tasks;
                    rt_mesh_t               root;

                protected:
                    status_t    main_loop(cvector<rt_context_t> *tasks, stats_t *stats);
                    status_t    process_context(cvector<rt_context_t> *tasks, stats_t *stats, rt_context_t *ctx);

                    status_t    scan_objects(cvector<rt_context_t> *tasks, rt_context_t *ctx);
                    status_t    cull_view(cvector<rt_context_t> *tasks, rt_context_t *ctx);
                    status_t    split_view(cvector<rt_context_t> *tasks, rt_context_t *ctx);
                    status_t    cullback_view(cvector<rt_context_t> *tasks, rt_context_t *ctx);
                    status_t    reflect_view(cvector<rt_context_t> *tasks, stats_t *stats, rt_context_t *ctx);
                    status_t    capture(capture_t *capture, const rt_view_t *v, View3D *trace);

                public:
                    TaskThread(RayTrace3D *trace);
                    virtual ~TaskThread();

                public:
                    virtual status_t run();

                    inline stats_t *get_stats() { return &stats; }
            };

        private:
            cstorage<rt_material_t>     vMaterials;
            cstorage<source_t>          vSources;
            cvector<capture_t>          vCaptures;
            Scene3D                    *pScene;
            RTObjectFactory             sFactory;
            rt_progress_t               pProgress;
            void                       *pProgressData;
            size_t                      nSampleRate;
            float                       fEnergyThresh;
            float                       fTolerance;
            bool                        bNormalize;
            volatile bool               bCancelled;
            volatile bool               bFailed;

            rt_debug_t                 *pDebug;

            cvector<rt_context_t>       vTasks;
            size_t                      nProgressPoints;
            size_t                      nProgressMax;
            ipc::Mutex                  lkTasks;
            ipc::Mutex                  lkCapture;

        protected:
            static void destroy_tasks(cvector<rt_context_t> *tasks);
            static void clear_stats(stats_t *stats);
            static void dump_stats(const char *label, const stats_t *stats);
            static void merge_stats(stats_t *dst, const stats_t *src);

            void        remove_scene(bool destroy);
            status_t    resize_materials(size_t objects);

            status_t    report_progress(float progress);

            // Main ray-tracing routines
            status_t    generate_root_mesh();
            status_t    generate_tasks(cvector<rt_context_t> *tasks, float initial);
            status_t    check_object(rt_context_t *ctx, Object3D *obj, const matrix3d_t *m);

            void        normalize_output();
            bool        is_already_passed(const sample_t *bind);

            status_t    prepare_main_loop(float initial, stats_t *stats);

        public:
            /** Default constructor
             *
             */
            explicit RayTrace3D();

            /** Destructor
             *
             */
            ~RayTrace3D();

        public:
            /**
             * Initialize raytrace object
             */
            status_t init();

            /** Destroy the ray tracing processor state
             * @param recursive destroy the currently used scene object
             */
            void destroy(bool recursive=false);

            /**
             * Set scene object
             * @param scene scene object to set
             * @param destroy destroy previously used scene
             * @return status of operation
             */
            status_t set_scene(Scene3D *scene, bool destroy=true);

            /**
             * Set/clear progress callback
             * @param callback callback routine to report progress
             * @param data data that will be passed to callback routine
             * @return status of operation
             */
            status_t set_progress_callback(rt_progress_t callback, void *data);

            /**
             * Clear progress callback
             * @return status of operation
             */
            status_t clear_progress_callback();

            /**
             * Get the material for the corresponding object
             * @param idx the index of the corresponding object
             * @return pointer to material or NULL
             */
            inline rt_material_t *material(size_t idx)  { return vMaterials.get(idx); }

            /**
             * Get the scene object
             * @param idx scene object
             * @return object pointer or NULL
             */
            inline Object3D *object(size_t idx) { return (pScene != NULL) ? pScene->get_object(idx) : NULL; }

            /**
             * Set sample rate
             * @param sr sample rate
             */
            inline void set_sample_rate(size_t sr) { nSampleRate = sr; }

            /**
             * Get sample rate
             * @return sample rate
             */
            inline size_t get_sample_rate() const { return nSampleRate; }

            /**
             * Add audio source
             * @param type audio source type
             * @param position audio source position, direction and size
             * @param volume audio source volume
             * @return status of operation
             */
            status_t add_source(const ray3d_t *position, rt_audio_source_t type, float volume);

            /**
             * Add audio capture
             * @param type audio capture type
             * @param position audio capture position, direction and size
             * @param sample sample object to store captured data
             * @param channel the sample channel to store captured data
             * @param gain capture gain
             * @return non-negative capture identifier or negative error status code
             */
            ssize_t add_capture(const ray3d_t *position, rt_audio_capture_t type, float gain);

            /**
             * Bind audio sample to capture
             * @param id capture identifier
             * @param sample audio sample to bind
             * @param channel number of channel of the sample that will be modified by capture
             * @param r_min the minimum reflection index, negative value for any
             * @param r_max the maximum reflection index, negative value for any
             * @return status of operation
             */
            status_t    bind_capture(size_t id, Sample *sample, size_t channel, ssize_t r_min, ssize_t r_max);

            /** Remove all audio sources
             *
             */
            inline void         clear_sources()     {   vSources.flush();       };

            /** Remove all audio captures
             *
             */
            inline void         clear_captures()    {   vCaptures.flush();      };

            /**
             * Set debug context for debugging purposes
             * @param shared shared context for debugging
             */
            inline void         set_debug_context(rt_debug_t *debug) { pDebug  = debug; }

            inline float        get_energy_threshold() const { return fEnergyThresh; }

            void                set_energy_threshold(float thresh) { fEnergyThresh = thresh; }

            inline float        get_tolerance() const { return fTolerance; }

            void                set_tolerance(float tolerance) { fTolerance = tolerance; }

            inline bool         get_normalize() const { return bNormalize; };
            inline void         set_normalize(bool normalize) { bNormalize = normalize; };

            /**
             * This method indicates that the cancel request was sent to
             * the processor, RT-safe method
             * @return true if cancel request was sent to processor
             */
            inline bool         cancelled() const { return bCancelled; }

            /**
             * This method allows to cancel the execution of process() method by
             * another thread, RT-safe method
             */
            void                cancel() { if (!bCancelled) bCancelled = true; }

            /**
             * Perform processing, non-RT-safe, should be launched in a thread
             * @param threads number of threads used for processing
             * @param initial initial energy of the signal
             * @return status of operation
             */
            status_t            process(size_t threads, float initial);
    };

} /* namespace lsp */

#endif /* CORE_3D_RAYTRACE3D_H_ */
