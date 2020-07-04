/*
 * pop_destroyer.h
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#ifndef METADATA_POP_DESTROYER_H_
#define METADATA_POP_DESTROYER_H_

namespace lsp
{
    struct pop_destroyer_base_metadata
    {
        static const float THRESH_MIN       = 0.0f;
        static const float THRESH_MAX       = GAIN_AMP_M_24_DB;
        static const float THRESH_DFL       = GAIN_AMP_M_72_DB;
        static const float THRESH_STEP      = 1e-04f;

        static const float ATTACK_MIN       = 0.0f;
        static const float ATTACK_MAX       = 5.0f;
        static const float ATTACK_DFL       = 10.0f;
        static const float ATTACK_STEP      = 1e-6f;

        static const float RELEASE_MIN      = 0.0f;
        static const float RELEASE_MAX      = 200.0f;
        static const float RELEASE_DFL      = 10.0f;
        static const float RELEASE_STEP     = 1e-4f;

        static const float FADE_MIN         = 1.0f;
        static const float FADE_MAX         = 200.0f;
        static const float FADE_DFL         = 50.0f;
        static const float FADE_STEP        = 0.1f;

        static const size_t MESH_POINTS     = 640;
        static const float MESH_TIME        = 5.0f;
    };

    struct pop_destroyer_mono_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct pop_destroyer_stereo_metadata
    {
        static const plugin_metadata_t metadata;
    };
}


#endif /* METADATA_POP_DESTROYER_H_ */
