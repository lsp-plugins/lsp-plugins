/*
 * experimental.h
 *
 *  Created on: 11 янв. 2017 г.
 *      Author: sadko
 */

#ifndef PLUGINS_EXPERIMENTAL_H_
#define PLUGINS_EXPERIMENTAL_H_

#include <metadata/metadata.h>
#include <metadata/plugins.h>

#include <core/plugin.h>

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    class test_plugin: public plugin_t, public test_plugin_metadata
    {
        protected:
            IPort      *pIn[2];
            IPort      *pOut[2];
            IPort      *pMesh;
            IPort      *pGain;
            IPort      *pFileName;
            IPort      *pHeadCut;
            IPort      *pTailCut;
            IPort      *pFadeIn;
            IPort      *pFadeOut;
            IPort      *pStatus;
            IPort      *pLength;
            IPort      *pData;

            IPort      *pOutFile;
            IPort      *pOutCmd;
            IPort      *pOutStatus;
            IPort      *pOutProgress;

            float       fGain;
            bool        bFileSet;
            size_t      nPhase;
            size_t      nColorID;
            size_t      nStatus;
            size_t      nProgCurr;
            size_t      nProgLast;

        public:
            test_plugin();
            virtual ~test_plugin();

        public:
            virtual void init(IWrapper *wrapper);

            virtual void process(size_t samples);

            virtual void update_settings();

            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };
#endif
}

#endif /* INCLUDE_PLUGINS_EXPERIMENTAL_H_ */
