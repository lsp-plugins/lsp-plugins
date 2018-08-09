/*
 * nonlinear_convolver.h
 *
 *  Created on: 14 Apr 2018
 *      Author: crocoduck
 */

#ifndef PLUGINS_NONLINEAR_CONVOLVER_H_
#define PLUGINS_NONLINEAR_CONVOLVER_H_

#include <core/plugin.h>
#include <metadata/plugins.h>
#include <core/util/Bypass.h>
#include <core/util/SyncChirpProcessor.h>
#include <core/util/Convolver.h>

namespace lsp
{
    class nonlinear_convolver_mono: public plugin_t, public nonlinear_convolver_mono_metadata
    {
        protected:
            // Class to handle loading of nonlinear measurements
            class Loader: public ITask
            {
                private:
                    nonlinear_convolver_mono *pCore;

                public:
                    Loader(nonlinear_convolver_mono *base);
                    virtual ~Loader();

                public:
                    virtual int run();
            };

            // Class to prepare the modelling DSP structure from measurement data
            class Preparator: public ITask
            {
                private:
                    nonlinear_convolver_mono *pCore;

                public:
                    Preparator (nonlinear_convolver_mono *base);
                    virtual ~Preparator();

                public:
                    virtual int run();
            };

            // Object state descriptor
            enum state_t
            {
                IDLE,
                LOADING,        // <- Not Realtime: ITask
                PREPARE,        // <- Not Realtime: ITask
                PROCESSING
            };

            // DSP Topology
            enum dsp_t
            {
                HAMMERSTEIN_FIR,
                WIENER_HAMMERSTEIN_FIR,
                HAMMERSTEIN_IIR_BIQUADS,
                WIENER_HAMMERSTEIN_IIR_BIQUADS
            };

            protected:
                typedef struct misoFIR_t
                {
                    Convolver         **FIRConvolvers;
                    size_t              nBranches;
                    size_t              nTaps;
                } misoFIR_t;

            protected:
                state_t                 nState;
                dsp_t                   nDSP;

                IExecutor              *pExecutor;
                Loader                 *pLoader;
                Preparator             *pPreparator;

                Bypass                  sBypass;
                SyncChirpProcessor      sSyncChirpProcessor;
                Oversampler             sOverPrepare;
                Oversampler             sOverProcess;

                size_t                  nSampleRate;
                size_t                  nStatus;
                float                   fOutGain;
                size_t                  nModelOrder;
                size_t                  nModelOrder_Previous;
                size_t                  nWindowSize;
                size_t                  nWindowSize_Previous;

                misoFIR_t               misoFIRs;

                bool                    bBypass;
                bool                    bIsFirstAllocation;
                bool                    bReAllocate;
                bool                    bDataLoaded;
                bool                    bDSP_Valid;
                bool                    bDSP_Prepare_Trigger;
                bool                    bSwitch2Loading;
                bool                    bSwitch2Prepare;

                float                  *mDSP;
                uint8_t                *pDSPData;

                float                  *vBuffer;
                float                  *vProcessBufIn;
                float                  *vProcessBufTmp;
                float                  *vProcessBufOut;
                uint8_t                *pData;

                IPort                  *pIn;
                IPort                  *pOut;

                IPort                  *pBypass;

                IPort                  *pFile;
                IPort                  *pStatus;
                IPort                  *pOutGain;
                IPort                  *pOrder;
                IPort                  *pWindowSize;
                IPort                  *pDSP_Prepare_Trigger;
                IPort                  *pKernelsMesh;

            protected:
                float fastIntPow(float base, size_t exponent);
                void apply_fastIntPow(float *dst, float *src, size_t exponent, size_t count);
                size_t calculate_rank(size_t taps);
                void process_hammerstein_fir(float *dst, float *src, size_t count);

            protected:
                static size_t get_model_order(size_t order);
                static size_t get_window_size(size_t windowSize);

            public:
                nonlinear_convolver_mono();
                virtual ~nonlinear_convolver_mono();

            public:
                virtual void init(IWrapper *wrapper);
                virtual void process(size_t samples);
                virtual void update_settings();
                virtual void update_sample_rate(long sr);
                virtual void destroy();
    };
}

#endif /* PLUGINS_NONLINEAR_CONVOLVER_H_ */
