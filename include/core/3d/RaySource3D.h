/*
 * RaySource3D.h
 *
 *  Created on: 20 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_3D_RAYSOURCE3D_H_
#define CORE_3D_RAYSOURCE3D_H_

#include <dsp/dsp.h>
#include <core/3d/RayTrace3D.h>
#include <core/util/Randomizer.h>

namespace lsp
{
    enum ray_source_type
    {
        RS3DT_RANDOM,
        RS3DT_CONIC,
        RS3DT_CYLINDRIC,
        RS3DT_OMNI,
        RS3DT_SPHERIC
    };

    class RaySource3D
    {
        protected:
            matrix3d_t      sMatrix;
            ray_source_type enType;
            Randomizer      sRandomizer;

            float           fRadius1;
            float           fRadius2;
            float           fHeight;
            bool            bEnabled;
            uint32_t        nSeed;

        public:
            RaySource3D();
            ~RaySource3D();

        protected:
            void build_conic_rays(RayTrace3D *rt, size_t count);
            void build_cylindric_rays(RayTrace3D *rt, size_t count);
            void build_omni_rays(RayTrace3D *rt, size_t count);
            void build_spheric_rays(RayTrace3D *rt, size_t count);
            void build_random_rays(RayTrace3D *rt, size_t count);

        public:
            /** Initialize ray source
             *
             * @param seed random seed
             */
            void init(uint32_t seed);

            /** Reset state of the ray source;
             *
             */
            void reset();

            /** Set ray source type
             *
             * @param type ray source type
             */
            inline void set_type(ray_source_type type) {  enType = type;  }

            /** Generate set of rays for raytracing
             *
             * @param rt raytracing stack
             * @param count number of rays to generate
             */
            void generate(RayTrace3D *rt, size_t count);

            /** Get ray source matrix
             *
             * @return ray source matrix
             */
            inline matrix3d_t *get_matrix() { return &sMatrix; }

            /** Set first radius
             *
             * @param radius first radius
             */
            void set_radius1(float radius)
            {
                fRadius1 = radius;
            }

            /** Set second radius
             *
             * @param radius second radius
             */
            void set_radius2(float radius)
            {
                fRadius2 = radius;
            }

            void set_height(float height)
            {
                fHeight = height;
            }
            
            /** Check that ray source is enabled
             *
             * @return true if ray source is enabled
             */
            bool is_enabled() const
            {
                return bEnabled;
            }
            
            /** Check set enabled state for ray source
             *
             * @param enabled enabled state
             */
            void set_enabled(bool enabled)
            {
                bEnabled = enabled;
            }

            inline float get_radius1() const { return fRadius1; }

            inline float get_radius2() const { return fRadius2; }
    };

} /* namespace lsp */

#endif /* CORE_3D_RAYSOURCE3D_H_ */
