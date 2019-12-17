/*
 * nonlinear_convolver.cpp
 *
 *  Created on: 14 Apr 2018
 *      Author: crocoduck
 */

#include <plugins/nonlinear_convolver.h>

#define LOAD_EXT        ".lspc"         // Loaded file extension
#define TMP_BUF_SIZE    1024
#define PRC_BUF_SIZE    (12 * 1024)     // Multiple of 3, 4 and 8
#define FFT_MAX_RANK    16

namespace lsp
{
    nonlinear_convolver_mono::Loader::Loader(nonlinear_convolver_mono *base)
    {
        pCore = base;
    }

    nonlinear_convolver_mono::Loader::~Loader()
    {
        pCore = NULL;
    }

    status_t nonlinear_convolver_mono::Loader::run()
    {
        pCore->bDataLoaded = false;

        path_t *path = pCore->pFile->getBuffer<path_t>();

        if ((path == NULL) || (!path->accepted()))
        {
            pCore->nStatus = STATUS_BAD_ARGUMENTS;
            pCore->pStatus->setValue(pCore->nStatus);
            return pCore->nStatus;
        }
        else
        {
            path->commit();
        }

        status_t status = pCore->sSyncChirpProcessor.load_from_lspc(path->get_path());

//        if (status != STATUS_OK)
//            return status;
//
//        status = pCore->sSyncChirpProcessor.postprocess_nonlinear_convolution(
//                    pCore->nModelOrder,
//                    pCore->calculate_rank(pCore->nWindowSize),
//                    pCore->nWindowSize / 2
//                    );
//
        if (status == STATUS_OK)
            pCore->bDataLoaded = true;

        return status;
    }

    nonlinear_convolver_mono::Preparator::Preparator(nonlinear_convolver_mono *base)
    {
        pCore = base;
    }

    nonlinear_convolver_mono::Preparator::~Preparator()
    {
        pCore = NULL;
    }

    status_t nonlinear_convolver_mono::Preparator::run()
    {
        pCore->bDSP_Valid = false;

        if (!pCore->bDataLoaded)
            return STATUS_NO_DATA;

        status_t status = STATUS_OK;

//        status = pCore->sSyncChirpProcessor.postprocess_nonlinear_convolution(
//                            pCore->nModelOrder,
//                            false,
//                            10,
//                            10,
//                            windows::HANN,
//                            pCore->calculate_rank(pCore->nWindowSize)
//                            );

        if (status != STATUS_OK)
            return status;

        // Handle allocation.
        if (pCore->bReAllocate || pCore->bIsFirstAllocation)
        {
            // DSP Data
            free_aligned(pCore->pDSPData);
            pCore->mDSP = NULL;

            // Convolvers
            for (size_t n = 0; n < pCore->misoFIRs.nBranches; ++n)
            {
                if (pCore->misoFIRs.FIRConvolvers[n] != NULL)
                {
                    pCore->misoFIRs.FIRConvolvers[n]->destroy();
                    delete pCore->misoFIRs.FIRConvolvers[n];
                    pCore->misoFIRs.FIRConvolvers[n] = NULL;
                }
            }
            delete [] pCore->misoFIRs.FIRConvolvers;
            pCore->misoFIRs.nBranches   = 0;
            pCore->misoFIRs.nTaps       = 0;

            // Set up oversampler
            switch (pCore->nModelOrder)
            {
                case 2:
                {
                    pCore->sOverPrepare.set_mode(OM_LANCZOS_2X2);
                    pCore->sOverProcess.set_mode(OM_LANCZOS_2X2);
                }
                break;
                case 3:
                {
                    pCore->sOverPrepare.set_mode(OM_LANCZOS_3X2);
                    pCore->sOverProcess.set_mode(OM_LANCZOS_3X2);
                }
                break;
                case 4:
                {
                    pCore->sOverPrepare.set_mode(OM_LANCZOS_4X2);
                    pCore->sOverProcess.set_mode(OM_LANCZOS_4X2);
                }
                break;
                case 6:
                {
                    pCore->sOverPrepare.set_mode(OM_LANCZOS_6X2);
                    pCore->sOverProcess.set_mode(OM_LANCZOS_6X2);
                }
                break;
                case 8:
                {
                    pCore->sOverPrepare.set_mode(OM_LANCZOS_8X2);
                    pCore->sOverProcess.set_mode(OM_LANCZOS_8X2);
                }
                break;
            }

            switch (pCore->nDSP)
            {
                case HAMMERSTEIN_FIR:
                {
                    // DSP Data as order by window size matrix of FIR taps, but
                    // oversampled by order
                    size_t samples  = pCore->nModelOrder * pCore->nModelOrder * pCore->nWindowSize;

                    float *ptr      = alloc_aligned<float>(pCore->pDSPData, samples);
                    if (ptr == NULL)
                        return STATUS_NO_MEM;

                    lsp_guard_assert(float *save = ptr);
                    pCore->mDSP     = ptr;
                    ptr            += samples;

                    lsp_assert(ptr <= &save[samples]);

                    // Create the required convolvers.
                    pCore->misoFIRs.nBranches   = pCore->nModelOrder;
                    pCore->misoFIRs.nTaps       = pCore->nModelOrder * pCore->nWindowSize;

                    pCore->misoFIRs.FIRConvolvers = new Convolver*[pCore->misoFIRs.nBranches]();
                    Convolver *tmpPtr;

                    for (size_t n = 0; n < pCore->misoFIRs.nBranches; ++n)
                    {
                        tmpPtr = new Convolver();

                        if (tmpPtr == NULL)
                        {
                            return STATUS_NO_MEM;
                        }
                        else
                        {
                            pCore->misoFIRs.FIRConvolvers[n] = tmpPtr;
                        }
                    }
                }
                break;

                default:
                    return STATUS_NOT_IMPLEMENTED;
            }

            pCore->bIsFirstAllocation = false;

            pCore->nModelOrder_Previous = pCore->nModelOrder;
            pCore->nWindowSize_Previous = pCore->nWindowSize;
        }

        if (status != STATUS_OK)
            return status;

        // Assign all DSP structures.
        switch (pCore->nDSP)
        {
            case HAMMERSTEIN_FIR:
            {
                // Randomize phase of the convolver
                uint32_t phase  = seed_addr(this);
                phase           = ((phase << 16) | (phase >> 16)) & 0x7fffffff;
                uint32_t step   = 0x80000000 / (pCore->misoFIRs.nBranches + 1);

                for (size_t n = 0; n < pCore->misoFIRs.nBranches; ++n)
                {
                    status = pCore->sSyncChirpProcessor.get_kernel_fir(
                            &pCore->mDSP[n * pCore->misoFIRs.nTaps], n + 1
                            );

                    if (status != STATUS_OK)
                        return status;

                    pCore->sOverPrepare.upsample(
                            &pCore->mDSP[n * pCore->misoFIRs.nTaps],
                            &pCore->mDSP[n * pCore->misoFIRs.nTaps],
                            pCore->nWindowSize
                            );

                    if(
                            !pCore->misoFIRs.FIRConvolvers[n]->init(
                                   &pCore->mDSP[n * pCore->misoFIRs.nTaps],
                                    pCore->misoFIRs.nTaps,
                                    FFT_MAX_RANK,
                                    float((phase + n * step) & 0x7fffffff) / float(0x80000000))
                            )
                        return STATUS_NO_MEM;
                }
            }
            break;

            default:
                return STATUS_NOT_IMPLEMENTED;

        }

        if (status == STATUS_OK)
            pCore->bDSP_Valid = true;

        return status;
    }

    nonlinear_convolver_mono::nonlinear_convolver_mono(): plugin_t(metadata)
    {
        nState                  = IDLE;
        nDSP                    = HAMMERSTEIN_FIR;

        pExecutor               = NULL;
        pLoader                 = NULL;
        pPreparator             = NULL;

        nSampleRate             = 0;
        nStatus                 = STATUS_UNSPECIFIED;
        fOutGain                = 0;
        nModelOrder             = 0;
        nModelOrder_Previous    = 0;
        nWindowSize             = 0;
        nWindowSize_Previous    = 0;

        misoFIRs.FIRConvolvers  = NULL;
        misoFIRs.nBranches      = 0;
        misoFIRs.nTaps          = 0;

        bBypass                 = true;
        bIsFirstAllocation      = true;
        bReAllocate             = true;
        bDataLoaded             = false;
        bDSP_Valid              = false;
        bDSP_Prepare_Trigger    = false;
        bSwitch2Loading         = false;
        bSwitch2Prepare         = false;

        mDSP                    = NULL;
        pDSPData                = NULL;

        vBuffer                 = NULL;
        vProcessBufIn           = NULL;
        vProcessBufTmp          = NULL;
        vProcessBufOut          = NULL;
        pData                   = NULL;

        pIn                     = NULL;
        pOut                    = NULL;

        pBypass                 = NULL;

        pFile                   = NULL;
        pStatus                 = NULL;
        pOutGain                = NULL;
        pOrder                  = NULL;
        pWindowSize             = NULL;
        pDSP_Prepare_Trigger    = NULL;
        pKernelsMesh            = NULL;
    }

    nonlinear_convolver_mono::~nonlinear_convolver_mono()
    {

    }

    void nonlinear_convolver_mono::destroy()
    {
        if (pLoader != NULL)
        {
            delete pLoader;
            pLoader     = NULL;
        }

        if (pPreparator !=NULL)
        {
            delete pPreparator;
            pPreparator = NULL;
        }

        free_aligned(pDSPData);
        mDSP            = NULL;

        free_aligned(pData);
        vBuffer         = NULL;
        vProcessBufIn   = NULL;
        vProcessBufTmp  = NULL;
        vProcessBufOut  = NULL;
    }

    size_t nonlinear_convolver_mono::get_model_order(size_t order)
    {
        switch (order)
        {
            case nonlinear_convolver_mono_metadata::MODEL_ORDER_2:
                return 2;
            case nonlinear_convolver_mono_metadata::MODEL_ORDER_3:
                return 3;
            case nonlinear_convolver_mono_metadata::MODEL_ORDER_4:
                return 4;
            case nonlinear_convolver_mono_metadata::MODEL_ORDER_6:
                return 6;
            case nonlinear_convolver_mono_metadata::MODEL_ORDER_8:
                return 8;
            default:
                return 0;
        }
    }

    size_t nonlinear_convolver_mono::get_window_size(size_t windowSize)
    {
        switch (windowSize)
        {
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_512:
                return 512;
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_1024:
                return 1024;
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_2048:
                return 2048;
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_4096:
                return 4096;
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_8192:
                return 8192;
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_16384:
                return 16384;
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_32768:
                return 32768;
            case nonlinear_convolver_mono_metadata::WSIZE_ORDER_65536:
                return 65536;
            default:
                return 0;
        }
    }

    void nonlinear_convolver_mono::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        pExecutor               = wrapper->get_executor();

        pLoader                 = new Loader(this);
        pPreparator             = new Preparator(this);

        sSyncChirpProcessor.init();
        sOverPrepare.init();
        sOverProcess.init();

        // 1X temporary DSP buffer + 3X main process buffers
        size_t samples          = TMP_BUF_SIZE + 3 * PRC_BUF_SIZE;

        float *ptr              = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return;

        lsp_guard_assert(float *save = ptr);
        vBuffer                 = ptr;
        ptr                    += TMP_BUF_SIZE;
        vProcessBufIn           = ptr;
        ptr                    += PRC_BUF_SIZE;
        vProcessBufTmp          = ptr;
        ptr                    += PRC_BUF_SIZE;
        vProcessBufOut          = ptr;
        ptr                    += PRC_BUF_SIZE;

        lsp_assert(ptr <= &save[samples]);

        size_t port_id          = 0;

        pIn                     = vPorts[port_id++];
        pOut                    = vPorts[port_id++];

        pBypass                 = vPorts[port_id++];

        pFile                   = vPorts[port_id++];
        pStatus                 = vPorts[port_id++];
        pOutGain                = vPorts[port_id++];
        pOrder                  = vPorts[port_id++];
        pWindowSize             = vPorts[port_id++];
        pDSP_Prepare_Trigger    = vPorts[port_id++];
        pKernelsMesh            = vPorts[port_id++];
    }

    void nonlinear_convolver_mono::update_sample_rate(long sr)
    {
        nSampleRate = sr;

        sBypass.init(sr);
        sSyncChirpProcessor.set_sample_rate(sr);
        sOverPrepare.set_sample_rate(sr);
        sOverProcess.set_sample_rate(sr);
    }

    float nonlinear_convolver_mono::fastIntPow(float base, size_t exponent)
    {
        return powf(base, exponent);
//        if( exponent == 0)
//           return 1;
//
//        float temp = fastIntPow(base, exponent / 2);
//
//        if (exponent % 2 == 0)
//        {
//            return temp * temp;
//        }
//        else
//        {
//            if(exponent > 0)
//                return base * temp * temp;
//            else
//                return (temp * temp) / base;
//        }
    }

    void nonlinear_convolver_mono::apply_fastIntPow(float *dst, float *src, size_t exponent, size_t count)
    {
        for (size_t n = 0; n < count; ++n)
        {
            dst[n] = fastIntPow(src[n], exponent);
        }
    }

    size_t nonlinear_convolver_mono::calculate_rank(size_t taps)
    {
        size_t rank         = 0;
        size_t rankSamples  = 1;

        while (rankSamples < taps)
        {
            rankSamples <<= 1;
            rank += 1;
        }

        return rank;
    }

    void nonlinear_convolver_mono::process_hammerstein_fir(float *dst, float *src, size_t count)
    {
        size_t nOversampling    = sOverProcess.get_oversampling();
        size_t max_to_do        = PRC_BUF_SIZE / sOverProcess.get_oversampling();

        while (count > 0)
        {
            size_t to_do = (count < max_to_do) ? count : max_to_do;

            sOverProcess.upsample(vProcessBufIn, src, to_do);

            dsp::fill_zero(vProcessBufOut, to_do * nOversampling);

            for (size_t b = 1; b <= misoFIRs.nBranches; ++b)
            {
                apply_fastIntPow(vProcessBufTmp, vProcessBufIn, b, to_do * nOversampling);
                misoFIRs.FIRConvolvers[b - 1]->process(vProcessBufTmp, vProcessBufTmp, to_do * nOversampling);
                dsp::add2(vProcessBufOut, vProcessBufTmp, to_do * nOversampling);
            }

            sOverProcess.downsample(dst, vProcessBufOut, to_do);

            count   -= to_do;
            src     += to_do;
            dst     += to_do;
        }
    }

    void nonlinear_convolver_mono::process(size_t samples)
    {
        float *in = pIn->getBuffer<float>();
        if (in == NULL)
            return;

        float *out = pOut->getBuffer<float>();
        if (out == NULL)
            return;

        if (bSwitch2Loading)
        {
            pLoader->reset();
            pPreparator->reset();

            nState = LOADING;

            bSwitch2Loading = false;
        }

        if (bSwitch2Prepare)
        {
            pLoader->reset();
            pPreparator->reset();

            nState = PREPARE;

            bSwitch2Prepare = false;
        }

        while (samples > 0)
        {
            size_t to_do = (samples > TMP_BUF_SIZE) ? TMP_BUF_SIZE : samples;

            switch (nState)
            {
                case LOADING:
                {
                    if (pLoader->idle())
                        pExecutor->submit(pLoader);

                    if (pLoader->completed())
                    {
                        if (pLoader->successful())
                            nState = PREPARE;
                        else
                            nState = IDLE;

                        pLoader->reset();
                    }

                    dsp::fill_zero(vBuffer, to_do);
                }
                break;

                case PREPARE:
                {
                    if (pPreparator->idle())
                        pExecutor->submit(pPreparator);

                    if (pPreparator->completed())
                    {
                        if (pPreparator->successful())
                            nState = PROCESSING;
                        else
                            nState = IDLE;

                        pPreparator->reset();
                    }

                    dsp::fill_zero(vBuffer, to_do);
                }
                break;

                case PROCESSING:
                {
                    switch (nDSP)
                    {
                        case HAMMERSTEIN_FIR:
                            // Replace with cool function:
                            process_hammerstein_fir(vBuffer, in, to_do);
                            break;
                        case WIENER_HAMMERSTEIN_FIR:
                            // Replace with cool function:
                            dsp::fill_zero(vBuffer, to_do);
                            break;
                        case HAMMERSTEIN_IIR_BIQUADS:
                            // Replace with cool function:
                            dsp::fill_zero(vBuffer, to_do);
                            break;
                        case WIENER_HAMMERSTEIN_IIR_BIQUADS:
                            // Replace with cool function:
                            dsp::fill_zero(vBuffer, to_do);
                            break;
                        default:
                            dsp::fill_zero(vBuffer, to_do);
                    }

                }
                break;

                case IDLE:
                default:
                {
                    dsp::fill_zero(vBuffer, to_do);
                }
            }

            dsp::mul_k2(vBuffer, fOutGain, to_do);

            sBypass.process(out, in, vBuffer, to_do);

            in         += to_do;
            out        += to_do;
            samples    -= to_do;
        }
    }

    void nonlinear_convolver_mono::update_settings()
    {
        bBypass                 = pBypass->getValue() >= 0.5f;
        sBypass.set_bypass(bBypass);

        // File extension check
        path_t *path            = pFile->getBuffer<path_t>();

        if ((path != NULL) && (path->pending()))
        {
            const char *fname   = path->get_path();
            size_t len          = strlen(fname);

            const char *ext     = LOAD_EXT;
            size_t extlen       = strlen(ext);

            if (len < extlen)
                nStatus         = (len > 0) ? STATUS_BAD_ARGUMENTS : STATUS_UNSPECIFIED;
            else
            {
                nStatus         = STATUS_OK;

                for (size_t n   = 0; n < extlen; ++n)
                {
                    if (fname[len - extlen + n] != ext[n]) // This is case sensitive
                    {
                        nStatus = STATUS_BAD_ARGUMENTS;
                        break;
                    }
                }

            }

            path->accept();
            bSwitch2Loading     = true;
        }

        pStatus->setValue(nStatus);

        fOutGain                = pOutGain->getValue();

//        size_t previousOrder    = nModelOrder;
        nModelOrder             = get_model_order(pOrder->getValue());

//        size_t previousWSize    = nWindowSize;
        nWindowSize             = get_window_size(pWindowSize->getValue());

        bReAllocate             =
                                  (nModelOrder_Previous != nModelOrder) ||
                                  (nWindowSize_Previous != nWindowSize);

        bool previousTrigger    = bDSP_Prepare_Trigger;
        bDSP_Prepare_Trigger    = pDSP_Prepare_Trigger->getValue() >= 0.5f;

        // Was the trigger pressed (are we passing from false to true?). Also,
        // was a new file loaded? In the case, we cannot jump to prepare, but
        // we will go to loading first.
        if (!previousTrigger && bDSP_Prepare_Trigger && !bSwitch2Loading)
            bSwitch2Prepare     = true;
        else
            bSwitch2Prepare     = false;
    }
}
