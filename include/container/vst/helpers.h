/*
 * defs.h
 *
 *  Created on: 30 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_HELPERS_H_
#define _CONTAINER_VST_HELPERS_H_

// Some helper functions
namespace lsp
{
    VstInt32 vst_cconst(const char *vst_id)
    {
        if (strlen(vst_id) != 4)
        {
            lsp_error("Invalid cconst: %s", vst_id);
            return 0;
        }
        return CCONST(vst_id[0], vst_id[1], vst_id[2], vst_id[3]);
    }
}

#endif /* _CONTAINER_VST_HELPERS_H_ */
