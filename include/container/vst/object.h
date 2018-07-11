/*
 * object.h
 *
 *  Created on: 08 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTANIER_VST_OBJECT_H_
#define CONTAINER_VST_OBJECT_H_

#include <container/vst/defs.h>

namespace lsp
{
    class VSTAudioPort;
    class VSTParameterPort;

    typedef struct vst_object_t
    {
        uint32_t                    magic;
        plugin_t                   *plugin;
        const plugin_metadata_t    *metadata;
        cvector<VSTAudioPort>       inputs;
        cvector<VSTAudioPort>       outputs;
        cvector<VSTParameterPort>   params;
        audioMasterCallback         master;
    } vst_user_t;
}

#endif /* CONTAINER_VST_OBJECT_H_ */
