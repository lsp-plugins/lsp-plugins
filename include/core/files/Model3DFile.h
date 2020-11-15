/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 апр. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CORE_FILES_MODEL3DFILE_H_
#define CORE_FILES_MODEL3DFILE_H_

#include <core/types.h>
#include <core/status.h>
#include <core/3d/Scene3D.h>
#include <core/io/Path.h>

namespace lsp
{
    /** Model file used to load 3D objects
     *
     */
    class Model3DFile
    {
        private:
            Model3DFile & operator = (const Model3DFile &);

        protected:
            static status_t load_from_resource(Scene3D *scene, const void *data);

        public:
            explicit Model3DFile();
            ~Model3DFile();

        public:
            /** Create new scene and load file contents to the scene
             *
             * @param scene pointer to store loaded scene
             * @param path location of the file
             * @return status of the operation
             */
            static status_t load(Scene3D **scene, const char *path);

            /** Load file to the passed scene
             *
             * @param scene scene to store contents
             * @param path location of the file
             * @param clear issue clear() on the scene
             * @return status of the operation
             */
            static status_t load(Scene3D *scene, const char *path, bool clear);

            /** Create new scene and load file contents to the scene
             *
             * @param scene pointer to store loaded scene
             * @param path location of the file
             * @return status of the operation
             */
            static status_t load(Scene3D **scene, const LSPString *path);

            /** Load file to the passed scene
             *
             * @param scene scene to store contents
             * @param path location of the file
             * @param clear issue clear() on the scene
             * @return status of the operation
             */
            static status_t load(Scene3D *scene, const LSPString *path, bool clear);

            /** Create new scene and load file contents to the scene
             *
             * @param scene pointer to store loaded scene
             * @param path location of the file
             * @return status of the operation
             */
            static status_t load(Scene3D **scene, const io::Path *path);

            /** Load file to the passed scene
             *
             * @param scene scene to store contents
             * @param path location of the file
             * @param clear issue clear() on the scene
             * @return status of the operation
             */
            static status_t load(Scene3D *scene, const io::Path *path, bool clear);
    };

} /* namespace lsp */

#endif /* CORE_FILES_MODEL3DFILE_H_ */
