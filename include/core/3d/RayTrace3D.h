/*
 * RayTrace3D.hpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_3D_RAYTRACE3D_H_
#define CORE_3D_RAYTRACE3D_H_

#include <dsp/dsp.h>
#include <data/cstorage.h>

namespace lsp
{
    /** Ray tracing storage implemented as a stack
     *
     */
    class RayTrace3D
    {
        private:
            cstorage<raytrace3d_t>  sRays;

        public:
            /** Default constructor
             *
             */
            RayTrace3D();

            /** Destructor
             *
             */
            ~RayTrace3D();

        public:
            /** Destroy the raytrace storage
             *
             */
            void destroy();

            /** Add ray to raytrace storage
             *
             * @param r ray to add
             * @return true if ray was added, false if there is not enough memory
             */
            bool push(const ray3d_t *r);

            /** Add ray to raytrace storage
             *
             * @param r ray to add
             * @param ix last intersection of the ray
             * @return true if ray was added, false if there is not enough memory
             */
            bool push(const ray3d_t *r, const intersection3d_t *ix);

            /** Add raytrace to raytrace storage
             *
             * @param r raytrace to add
             * @return true if ray was added, false if there is not enough memory
             */
            bool push(const raytrace3d_t *r);

            /** Extract last raytrace from stack
             *
             * @param r pointer to store the extracted raytrace copy
             * @return true if raytrace was extracted from stack
             */
            bool pop(raytrace3d_t *r);

            /** Get number of elements in the raytrace stack
             *
             * @return number of elements in the raytrace stack
             */
            inline size_t size() const { return sRays.size();  }

            /** Get current capacity of the raytrace stack
             *
             * @return current capacity of the raytrace stack
             */
            inline size_t capacity() const { return sRays.capacity();  }
    };

} /* namespace lsp */

#endif /* CORE_3D_RAYTRACE3D_H_ */
