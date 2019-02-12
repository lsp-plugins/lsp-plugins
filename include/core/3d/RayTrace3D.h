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
#include <core/3d/rt_context.h>

namespace lsp
{
    enum rt_audio_source_t
    {
        RT_AS_SPOT,
        RT_AS_SPEAKER,
        RT_AS_OMNI
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
            typedef struct Material: public rt_material_t
            {
            } Material;

            typedef struct source_t
            {
                ray3d_t             position;
                float               size;
                rt_audio_source_t   type;
                float               volume;
            } source_t;

            typedef struct capture_t
            {
                ray3d_t             position;
                Material            material;
                float               size;
                rt_audio_capture_t  type;
                Sample             *sample;
                size_t              channel;
            } capture_t;

        private:
            cvector<rt_context_t>       vTasks;
            cstorage<Material>          vMaterials;
            cstorage<source_t>          vSources;
            cstorage<capture_t>         vCaptures;
            Scene3D                    *pScene;
            Scene3D                     sPrimitives;
            ssize_t                     nSphereId;
            rt_progress_t              *pProgress;
            void                       *pProgressData;
            size_t                      nSampleRate;

        protected:
            void        destroy_tasks();
            void        remove_scene(bool destroy);
            status_t    resize_materials(size_t objects);

            status_t    build_sphere();

            ssize_t     get_icosphere();

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
            status_t set_progress_callback(rt_progress_t *callback, void *data);

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
             * @param position audio source position and direction
             * @param volume audio source volume
             * @return status of operation
             */
            status_t add_source(const ray3d_t *position, float size, rt_audio_source_t type, float volume);

            /**
             * Add audio capture
             * @param type audio capture type
             * @param position audio capture position and direction
             * @param sample sample object to store captured data
             * @param channel the sample channel to store captured data
             * @return status of operation
             */
            status_t add_capture(const ray3d_t *position, float size, rt_audio_capture_t type, Sample *sample, size_t channel);

            /** Remove all audio sources
             *
             */
            inline void         clear_sources()     {   vSources.flush();       };

            /** Remove all audio captures
             *
             */
            inline void         clear_captures()    {   vCaptures.flush();      };

            /**
             * Perform processing
             * @return status of operation
             */
            status_t            process();
    };

} /* namespace lsp */

#endif /* CORE_3D_RAYTRACE3D_H_ */
