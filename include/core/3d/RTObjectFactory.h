/*
 * RTObjectFactory.h
 *
 *  Created on: 12 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_RTOBJECTFACTORY_H_
#define INCLUDE_CORE_3D_RTOBJECTFACTORY_H_

#include <core/3d/Scene3D.h>
#include <data/cstorage.h>

namespace lsp
{
    class RTObjectFactory
    {
        protected:
            enum object_type_t
            {
                OT_ICOSPHERE
            };

            typedef struct f_object_t
            {
                object_type_t type;
                size_t  obj_id;
                union
                {
                    struct
                    {
                        size_t level;
                    } icosphere;
                };
            } f_object_t;

        protected:
            Scene3D                 sScene;
            cstorage<f_object_t>    sObjects;

        protected:
            Object3D    *generateIcosphere(size_t level);

        public:
            explicit RTObjectFactory();
            ~RTObjectFactory();

        public:
            void        clear();

            Object3D    *buildIcosphere(size_t level);
    };

} /* namespace mtest */

#endif /* INCLUDE_CORE_3D_RTOBJECTFACTORY_H_ */
