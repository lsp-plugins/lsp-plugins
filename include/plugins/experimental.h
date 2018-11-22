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
            typedef struct osc_t
            {
                float       A0; // Initial amplitude
                float       X0; // Initial location
                float       W0; // Frequency
                float       P0; // Initial phase
                float       R0; // Reduction/Decay
            } osc_t;

        protected:
            IPort      *pIn[2];
            IPort      *pOut[2];
            IPort      *pMesh;
            IPort      *pFB;
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

            size_t      nOscPhase;
            size_t      nOscLeft;
            size_t      nRows;
            float       vBuffer[FRM_BUFFER_SIZE];

            osc_t       vOsc[3];

        protected:
            void        oscillate(float *dst, const osc_t *osc, float t, ssize_t n);

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
