/*
 * SpectralProcessor.cpp
 *
 *  Created on: 1 июл. 2020 г.
 *      Author: sadko
 */

#include <core/util/SpectralProcessor.h>
#include <dsp/dsp.h>

namespace lsp
{
    
    SpectralProcessor::SpectralProcessor()
    {
        nRank           = 0;
        nMaxRank        = 0;
        fPreGain        = 0.0f;
        fPhase          = 0.0f;
        bUpdate         = true;

        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;
    }
    
    SpectralProcessor::~SpectralProcessor()
    {
        destroy();
    }

    bool SpectralProcessor::init(size_t max_rank)
    {
        nRank           = max_rank;
        nMaxRank        = max_rank;
        fPreGain        = 1.0f;
        fPhase          = 0.0f;
        bUpdate         = true;

        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;

        // TODO: allocate buffers

        return true;
    }

    void SpectralProcessor::destroy()
    {
        // TODO: free buffers

        nRank           = 0;
        nMaxRank        = 0;
        fPreGain        = 0.0f;
        fPhase          = 0.0f;
        bUpdate         = false;

        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;
    }

    void SpectralProcessor::bind(spectral_processor_func_t func, void *object, void *subject)
    {
        pFunc           = func;
        pObject         = object;
        pSubject        = subject;
    }

    void SpectralProcessor::unbind()
    {
        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;
    }

    void SpectralProcessor::update_settings()
    {
        // TODO

        // Mark settings applied
        bUpdate         = false;
    }

    void SpectralProcessor::set_phase(float phase)
    {
        fPhase          = (phase < 0.0f) ? 0.0f : (phase > 1.0f) ? 1.0f : phase;
        bUpdate         = true;
    }

    void SpectralProcessor::set_rank(size_t rank)
    {
        if ((rank == nRank) || (rank > nMaxRank))
            return;

        nRank           = rank;
        bUpdate         = true;
    }

    void SpectralProcessor::process(float *dst, const float *src, size_t count)
    {
        // Check if we need to commit new settings
        if (bUpdate)
            update_settings();

        // TODO implement this
        dsp::mul_k3(dst, src, fPreGain, count);
    }

} /* namespace lsp */
