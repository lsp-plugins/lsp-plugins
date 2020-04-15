/*
 * corellator.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/debug.h>
#include <core/fade.h>
#include <core/protocol/midi.h>

#include <plugins/sampler.h>

#define TRACE_PORT(p) lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    //-------------------------------------------------------------------------
    sampler_kernel::AFLoader::AFLoader(sampler_kernel *base, afile_t *descr)
    {
        pCore       = base;
        pFile       = descr;
    }

    sampler_kernel::AFLoader::~AFLoader()
    {
        pCore       = NULL;
        pFile       = NULL;
    }

    status_t sampler_kernel::AFLoader::run()
    {
        return pCore->load_file(pFile);
    };

    //-------------------------------------------------------------------------
    sampler_kernel::sampler_kernel()
    {
        pExecutor       = NULL;
        vFiles          = NULL;
        vActive         = NULL;
        nFiles          = 0;
        nActive         = 0;
        nChannels       = 0;
        vBuffer         = NULL;
        bBypass         = false;
        bReorder        = false;
        fFadeout        = 10.0f;
        fDynamics       = sampler_base_metadata::DYNA_DFL;
        fDrift          = sampler_base_metadata::DRIFT_DFL;
        nSampleRate     = 0;

        pDynamics       = NULL;
        pDrift          = NULL;
        pActivity       = NULL;
        pListen         = NULL;
        pData           = NULL;
    }

    sampler_kernel::~sampler_kernel()
    {
        lsp_trace("this = %p", this);
        destroy_state();
    }

    void sampler_kernel::set_fadeout(float length)
    {
        fFadeout        = length;
    }

    bool sampler_kernel::init(ipc::IExecutor *executor, size_t files, size_t channnels)
    {
        // Validate parameters
        if (channnels > TRACKS_MAX)
            channnels           = TRACKS_MAX;

        // Now we may store values
        nFiles          = files;
        nChannels       = channnels;
        bReorder        = true;
        nActive         = 0;
        pExecutor       = executor;

        // Now determine object sizes
        size_t afsample_size        = ALIGN_SIZE(sizeof(afsample_t), DEFAULT_ALIGN);
        size_t afile_size           = AFI_TOTAL * afsample_size;
        size_t array_size           = ALIGN_SIZE(sizeof(afile_t *) * files, DEFAULT_ALIGN);

        lsp_trace("afsample_size        = %d", int(afsample_size));
        lsp_trace("afile_size           = %d", int(afile_size));
        lsp_trace("array_size           = %d", int(array_size));

        // Allocate raw chunk and link data
        size_t allocate             = array_size * 2 + afile_size * files;
        uint8_t *ptr                = alloc_aligned<uint8_t>(pData, allocate);
        if (ptr == NULL)
            return false;

        #ifdef LSP_TRACE
            uint8_t *tail               = &ptr[allocate];
            lsp_trace("allocate = %d, ptr range=%p-%p", int(allocate), ptr, tail);
        #endif /* LSP_TRACE */

        // Allocate files
        vFiles                      = new afile_t[files];
        if (vFiles == NULL)
            return false;

        vActive                     = reinterpret_cast<afile_t **>(ptr);
        ptr                        += array_size;
        lsp_trace("vActive              = %p", vActive);

        for (size_t i=0; i<files; ++i)
        {
            afile_t *af                 = &vFiles[i];

            af->nID                     = i;
            af->pLoader                 = NULL;

            af->bDirty                  = false;
            af->fVelocity               = 1.0f;
            af->fHeadCut                = 0.0f;
            af->fTailCut                = 0.0f;
            af->fFadeIn                 = 0.0f;
            af->fFadeOut                = 0.0f;
            af->fPreDelay               = PREDELAY_DFL;
            af->sListen.init();
            af->bOn                     = true;
            af->fMakeup                 = 1.0f;
            af->fLength                 = 0.0f;
            af->nStatus                 = STATUS_UNSPECIFIED;

            af->pFile                   = NULL;
            af->pHeadCut                = NULL;
            af->pTailCut                = NULL;
            af->pFadeIn                 = NULL;
            af->pFadeOut                = NULL;
            af->pVelocity               = NULL;
            af->pMakeup                 = NULL;
            af->pPreDelay               = NULL;
            af->pOn                     = NULL;
            af->pListen                 = NULL;
            af->pLength                 = NULL;
            af->pStatus                 = NULL;
            af->pMesh                   = NULL;
            af->pActive                 = NULL;
            af->pNoteOn                 = NULL;

            for (size_t j=0; j < TRACKS_MAX; ++j)
            {
                af->fGains[j]               = 1.0f;
                af->pGains[j]               = NULL;
            }

            for (size_t j=0; j<AFI_TOTAL; ++j)
            {
                afsample_t *afs             = reinterpret_cast<afsample_t *>(ptr);
                ptr                        += afsample_size;

                af->vData[j]                = afs;
                lsp_trace("vFiles[%d]->vData[%d]    = %p", int(i), int(j), afs);

                afs->pFile                  = NULL;
                afs->fNorm                  = 1.0f;
                afs->pSample                = NULL;

                for (size_t k=0; k<TRACKS_MAX; ++k)
                    afs->vThumbs[k]     = NULL;
            }

            vActive[i]                  = NULL;
        }

        // Create additional objects: tasks for file loading
        lsp_trace("Create loaders");
        for (size_t i=0; i<files; ++i)
        {
            afile_t  *af        = &vFiles[i];

            // Create loader
            AFLoader *ldr       = new AFLoader(this, af);
            if (ldr == NULL)
            {
                destroy_state();
                return false;
            }

            // Store loader
            af->pLoader         = ldr;
        }

        // Initialize channels
        lsp_trace("Initialize channels");
        for (size_t i=0; i<nChannels; ++i)
        {
            if (!vChannels[i].init(nFiles, sampler_base_metadata::PLAYBACKS_MAX))
            {
                destroy_state();
                return false;
            }
        }

        // Allocate buffer
        lsp_trace("Allocate buffer size=%d", int(sampler_base_metadata::BUFFER_SIZE));
        float *buf              = new float[sampler_base_metadata::BUFFER_SIZE];
        if (buf == NULL)
        {
            destroy_state();
            return false;
        }
        lsp_trace("buffer = %p", buf);
        vBuffer                 = buf;

        // Initialize toggle
        sListen.init();

        return true;
    }

    size_t sampler_kernel::bind(cvector<IPort> &ports, size_t port_id, bool dynamics)
    {
        lsp_trace("Binding listen toggle...");
        TRACE_PORT(ports[port_id]);
        pListen             = ports[port_id++];

        if (dynamics)
        {
            lsp_trace("Binding dynamics and drifting...");
            TRACE_PORT(ports[port_id]);
            pDynamics           = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            pDrift              = ports[port_id++];
        }

        lsp_trace("Skipping sample selector port...");
        TRACE_PORT(ports[port_id]);
        port_id++;

        // Iterate each file
        for (size_t i=0; i<nFiles; ++i)
        {
            lsp_trace("Binding sample %d", int(i));

            afile_t *af             = &vFiles[i];
            // Allocate files
            TRACE_PORT(ports[port_id]);
            af->pFile               = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pHeadCut            = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pTailCut            = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pFadeIn             = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pFadeOut            = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pMakeup             = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pVelocity           = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pPreDelay           = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pOn                 = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pListen             = ports[port_id++];

            for (size_t j=0; j<nChannels; ++j)
            {
                TRACE_PORT(ports[port_id]);
                af->pGains[j]           = ports[port_id++];
            }

            TRACE_PORT(ports[port_id]);
            af->pActive             = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pNoteOn             = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pLength             = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pStatus             = ports[port_id++];
            TRACE_PORT(ports[port_id]);
            af->pMesh               = ports[port_id++];
        }

        // Initialize randomizer
        sRandom.init();

        lsp_trace("Init OK");

        return port_id;
    }

    void sampler_kernel::bind_activity(IPort *activity)
    {
        lsp_trace("Binding activity...");
        TRACE_PORT(activity);
        pActivity       = activity;
    }

    void sampler_kernel::destroy_state()
    {
        DROP_ARRAY(vBuffer);

        for (size_t i=0; i<nChannels; ++i)
            vChannels[i].destroy(false);

        if (vFiles != NULL)
        {
            for (size_t i=0; i<nFiles;++i)
            {
                // Delete audio file loaders
                AFLoader *ldr   = vFiles[i].pLoader;
                if (ldr != NULL)
                {
                    delete ldr;
                    vFiles[i].pLoader = NULL;
                }

                // Destroy samples
                for (size_t j=0; j<AFI_TOTAL; ++j)
                    destroy_afsample(vFiles[i].vData[j]);
            }

            // Drop list of files
            delete [] vFiles;
            vFiles = NULL;
        }

        free_aligned(pData);

        // Foget variables
        pExecutor       = NULL;
        nFiles          = 0;
        nChannels       = 0;
        bReorder        = false;
        bBypass         = false;

        pDynamics       = NULL;
        pDrift          = NULL;
    }

    void sampler_kernel::destroy()
    {
        destroy_state();
    }

    void sampler_kernel::update_settings()
    {
        // Process listen toggle
        if (pListen != NULL)
            sListen.submit(pListen->getValue());

        // Process file load requests
        for (size_t i=0; i<nFiles; ++i)
        {
            // Get descriptor
            afile_t *af             = &vFiles[i];
            if (af->pFile == NULL)
                continue;

            // Get path
            path_t *path = af->pFile->getBuffer<path_t>();
            if ((path == NULL) || (!path->pending()))
                continue;

            // Check task state
            if (af->pLoader->idle())
            {
                // Try to submit task
                if (pExecutor->submit(af->pLoader))
                {
                    af->nStatus     = STATUS_LOADING;
                    lsp_trace("successfully submitted task");
                    path->accept();
                }
            }
        }

        // Update note and octave
        lsp_trace("Initializing samples...");

        // Iterate all samples
        for (size_t i=0; i<nFiles; ++i)
        {
            afile_t *af         = &vFiles[i];

            // On/off switch
            bool on             = (af->pOn->getValue() >= 0.5f);
            if (af->bOn != on)
            {
                af->bOn             = on;
                bReorder            = true;
            }

            // Pre-delay gain
            af->fPreDelay       = af->pPreDelay->getValue();

            // Listen trigger
//            lsp_trace("submit listen%d = %f", int(i), af->pListen->getValue());
            af->sListen.submit(af->pListen->getValue());
//            lsp_trace("listen[%d].pending = %s", int(i), (af->sListen.pending()) ? "true" : "false");

            // Makeup gain + mix gain
            af->fMakeup         = (af->pMakeup != NULL) ? af->pMakeup->getValue() : 1.0f;
            if (nChannels == 1)
                af->fGains[0]       = af->pGains[0]->getValue();
            else if (nChannels == 2)
            {
                af->fGains[0]       = (100.0f - af->pGains[0]->getValue()) * 0.005f;
                af->fGains[1]       = (af->pGains[1]->getValue() + 100.0f) * 0.005f;
            }
            else
            {
                for (size_t j=0; j<nChannels; ++j)
                    af->fGains[j]       = af->pGains[j]->getValue();
            }
//            #ifdef LSP_TRACE
//                for (size_t j=0; j<nChannels; ++j)
//                    lsp_trace("gains[%d,%d] = %f", int(i), int(j), af->fGains[j]);
//            #endif

            // Update velocity
            float value     = af->pVelocity->getValue();
            if (value != af->fVelocity)
            {
                af->fVelocity   = value;
                bReorder        = true;
            }

            // Update sample timings
            value           = af->pHeadCut->getValue();
            if (value != af->fHeadCut)
            {
                af->fHeadCut    = value;
                af->bDirty      = true;
            }

            value           = af->pTailCut->getValue();
            if (value != af->fTailCut)
            {
                af->fTailCut    = value;
                af->bDirty      = true;
            }

            value           = af->pFadeIn->getValue();
            if (value != af->fFadeIn)
            {
                af->fFadeIn     = value;
                af->bDirty      = true;
            }

            value           = af->pFadeOut->getValue();
            if (value != af->fFadeOut)
            {
                af->fFadeOut    = value;
                af->bDirty      = true;
            }
        }

        // Get humanisation parameters
        fDynamics       = (pDynamics != NULL) ? pDynamics->getValue() * 0.01 : 0.0f; // fDynamics = 0..1.0
        fDrift          = (pDrift != NULL) ? pDrift->getValue() : 0.0f;
    }

    void sampler_kernel::update_sample_rate(long sr)
    {
        // Store new sample rate
        nSampleRate     = sr;

        // Update activity counter
        sActivity.init(sr);

        for (size_t i=0; i<nFiles; ++i)
            vFiles[i].sNoteOn.init(sr);
    }

    void sampler_kernel::destroy_afsample(afsample_t *af)
    {
        if (af->pFile != NULL)
        {
            af->pFile->destroy();
            delete af->pFile;
            af->pFile           = NULL;
        }

        if (af->vThumbs[0] != NULL)
        {
            delete [] af->vThumbs[0];

            for (size_t i=0; i<TRACKS_MAX; ++i)
                af->vThumbs[i]      = NULL;
        }

        if (af->pSample != NULL)
        {
            af->pSample->destroy();
            delete af->pSample;
            af->pSample     = NULL;
        }
    }

    int sampler_kernel::load_file(afile_t *file)
    {
        // Load sample
        lsp_trace("file = %p", file);

        // Validate arguments
        if (file == NULL)
            return STATUS_UNKNOWN_ERR;

        // Destroy OLD data if exists
        destroy_afsample(file->vData[AFI_OLD]);

        // Check state
        afsample_t *snew        = file->vData[AFI_NEW];
        if ((snew->pFile != NULL) || (snew->pSample != NULL))
            return STATUS_UNKNOWN_ERR;

        // Check port binding
        if (file->pFile == NULL)
            return STATUS_UNKNOWN_ERR;

        // Get path
        path_t *path        = file->pFile->getBuffer<path_t>();
        if (path == NULL)
            return STATUS_UNKNOWN_ERR;

        // Get file name
        const char *fname   = path->get_path();
        if (strlen(fname) <= 0)
            return STATUS_UNSPECIFIED;

        // Load audio file
        snew->pFile         = new AudioFile();
        if (snew->pFile == NULL)
            return STATUS_NO_MEM;

        status_t status = snew->pFile->load(fname, SAMPLE_LENGTH_MAX * 0.001f);
        if (status != STATUS_OK)
        {
            lsp_trace("load failed: status=%d (%s)", status, get_status(status));
            destroy_afsample(snew);
            return status;
        }

        status = snew->pFile->resample(nSampleRate);
        if (status != STATUS_OK)
        {
            lsp_trace("resample failed: status=%d (%s)", status, get_status(status));
            destroy_afsample(snew);
            return status;
        }

        // Create samples
        size_t channels     = snew->pFile->channels();
        size_t samples      = snew->pFile->samples();
        if (channels > nChannels)
            channels           = nChannels;

        float *thumbs   = new float[channels * MESH_SIZE];
        if (thumbs == NULL)
        {
            destroy_afsample(snew);
            return STATUS_NO_MEM;
        }

        snew->vThumbs[0]        = thumbs;
        float max = 0.0f;

        // Create and initialize sample
        snew->pSample           = new Sample();
        if ((snew->pSample == NULL) || (!snew->pSample->init(channels, samples)))
        {
            lsp_trace("sample initialization failed");
            destroy_afsample(snew);
            return STATUS_NO_MEM;
        }

        // Determine the normalizing factor
        for (size_t i=0; i<channels; ++i)
        {
            snew->vThumbs[i]        = thumbs;
            thumbs                 += MESH_SIZE;

            // Determine the maximum amplitude
            float a_max = dsp::abs_max(snew->pFile->channel(i), samples);
            lsp_trace("dsp::abs_max(%p, %d): a_max=%f", snew->pFile->channel(i), int(samples), a_max);

            if (max < a_max)
                max     = a_max;
        }

        lsp_trace("max=%f", max);
        snew->fNorm     = (max != 0.0f) ? 1.0f / max : 1.0f;

        lsp_trace("file successful loaded: %s", fname);

        return STATUS_OK;
    }

    void sampler_kernel::copy_asample(afsample_t *dst, const afsample_t *src)
    {
        dst->pFile          = src->pFile;
        dst->fNorm          = src->fNorm;
        dst->pSample        = src->pSample;

        for (size_t j=0; j<TRACKS_MAX; ++j)
            dst->vThumbs[j]     = src->vThumbs[j];
    }

    void sampler_kernel::clear_asample(afsample_t *dst)
    {
        dst->pFile          = NULL;
        dst->pSample        = NULL;
        dst->fNorm          = 1.0f;

        for (size_t j=0; j<TRACKS_MAX; ++j)
            dst->vThumbs[j]     = NULL;
    }

    void sampler_kernel::render_sample(afile_t *af)
    {
        // Get maximum sample count
        afsample_t *afs     = af->vData[AFI_CURR];
        if (afs->pFile != NULL)
        {
            ssize_t head        = millis_to_samples(nSampleRate, af->fHeadCut);
            ssize_t tail        = millis_to_samples(nSampleRate, af->fTailCut);
            ssize_t tot_samples = millis_to_samples(nSampleRate, af->fLength);
            ssize_t max_samples = tot_samples - head - tail;
            Sample *s           = afs->pSample;

            if (max_samples > 0)
            {
                lsp_trace("re-render sample max_samples=%d", int(max_samples));

                // Re-render sample
                for (size_t j=0; j<s->channels(); ++j)
                {
                    float *dst          = s->getBuffer(j);
                    const float *src    = afs->pFile->channel(j);
                    dsp::copy(dst, &src[head], max_samples);

                    // Apply fade-in and fade-out to the buffer
                    fade_in(dst, dst, millis_to_samples(nSampleRate, af->fFadeIn), max_samples);
                    fade_out(dst, dst, millis_to_samples(nSampleRate, af->fFadeOut), max_samples);

                    // Now render thumbnail
                    src                 = dst;
                    dst                 = afs->vThumbs[j];
                    for (size_t k=0; k<MESH_SIZE; ++k)
                    {
                        size_t first    = (k * max_samples) / MESH_SIZE;
                        size_t last     = ((k + 1) * max_samples) / MESH_SIZE;
                        if (first < last)
                            dst[k]          = dsp::abs_max(&src[first], last - first);
                        else
                            dst[k]          = fabs(src[first]);
                    }

                    // Normalize graph if possible
                    if (afs->fNorm != 1.0f)
                        dsp::mul_k2(dst, afs->fNorm, MESH_SIZE);
                }

                // Update length of the sample
                s->setLength(max_samples);

                // (Re)bind sample
                for (size_t j=0; j<nChannels; ++j)
                    vChannels[j].bind(af->nID, s, false);
            }
            else
            {
                // Mark  sample empty
                s->setLength(0);

                // Unbind empty sample
                for (size_t j=0; j<nChannels; ++j)
                    vChannels[j].unbind(af->nID);
            }
        }
        else
        {
            // Unbind empty sample
            for (size_t j=0; j<nChannels; ++j)
                vChannels[j].unbind(af->nID);
        }

        // Reset dirty flag
        af->bDirty      = false;
    }

    void sampler_kernel::reorder_samples()
    {
        lsp_trace("Reordering active files");

        // Compute the list of active files
        nActive     = 0;
        for (size_t i=0; i<nFiles; ++i)
        {
            if (!vFiles[i].bOn)
                continue;
            if (vFiles[i].vData[AFI_CURR]->pSample == NULL)
                continue;

            lsp_trace("file %d is active", int(nActive));
            vActive[nActive++]  = &vFiles[i];
        }

        // Sort the list of active files
        if (nActive > 1)
        {
            for (size_t i=0; i<(nActive-1); ++i)
                for (size_t j=i+1; j<nActive; ++j)
                    if (vActive[i]->fVelocity > vActive[j]->fVelocity)
                    {
                        // Swap file pointers
                        afile_t    *af  = vActive[i];
                        vActive[i]      = vActive[j];
                        vActive[j]      = af;
                    }
        }

        #ifdef LSP_TRACE
            for (size_t i=0; i<nActive; ++i)
                lsp_trace("active file #%d: velocity=%.3f", int(vActive[i]->nID), vActive[i]->fVelocity);
        #endif /* LSP_TRACE */
    }

    void sampler_kernel::play_sample(const afile_t *af, float gain, size_t delay)
    {
        lsp_trace("id=%d, gain=%f, delay=%d", int(af->nID), gain, int(delay));

        // Scale the final output gain
        gain    *= af->fMakeup;

        if (nChannels == 1)
        {
            lsp_trace("channels[%d].play(%d, %d, %f, %d)", int(0), int(af->nID), int(0), gain * af->fGains[0], int(delay));
            vChannels[0].play(af->nID, 0, gain * af->fGains[0], delay);
        }
        else if (nChannels == 2)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                size_t j=i^1; // j = (i + 1) % 2
                lsp_trace("channels[%d].play(%d, %d, %f, %d)", int(i), int(af->nID), int(i), gain * af->fGains[i], int(delay));
                vChannels[i].play(af->nID, i, gain * af->fGains[i], delay);
                lsp_trace("channels[%d].play(%d, %d, %f, %d)", int(j), int(i), int(af->nID), gain * (1.0f - af->fGains[i]), int(delay));
                vChannels[j].play(af->nID, i, gain * (1.0f - af->fGains[i]), delay);
            }
        }
        else
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                lsp_trace("channels[%d].play(%d, %d, %f, %d)", int(i), int(af->nID), int(i), gain * af->fGains[i], int(delay));
                vChannels[i].play(af->nID, i, gain * af->fGains[i], delay);
            }
        }
    }

    void sampler_kernel::cancel_sample(const afile_t *af, size_t fadeout, size_t delay)
    {
        lsp_trace("id=%d, delay=%d", int(af->nID), int(delay));

        // Cancel all playbacks
        for (size_t i=0; i<nChannels; ++i)
        {
            lsp_trace("channels[%d].cancel(%d, %d, %d)", int(af->nID), int(i), int(fadeout), int(delay));
            vChannels[i].cancel_all(af->nID, i, fadeout, delay);
        }
    }

    void sampler_kernel::trigger_on(size_t timestamp, float level)
    {
        if (nActive <= 0)
            return;

        // Binary search of sample
        lsp_trace("normalized velocity = %f", level);
        level      *=   100.0f; // Make velocity in percentage
        ssize_t f_first = 0, f_last = nActive-1;
        while (f_last > f_first)
        {
            ssize_t f_mid = (f_last + f_first) >> 1;
            if (level <= vActive[f_mid]->fVelocity)
                f_last  = f_mid;
            else
                f_first = f_mid + 1;
        }
        if (f_last < 0)
            f_last      = 0;
        else if (f_last >= ssize_t(nActive))
            f_last      = nActive - 1;

        // Get the file and ajdust gain
        afile_t *af     = vActive[f_last];
        size_t delay    = millis_to_samples(nSampleRate, af->fPreDelay) + timestamp;

        lsp_trace("f_last=%d, af->id=%d, af->velocity=%.3f", int(f_last), int(af->nID), af->fVelocity);

        // Apply changes to all ports
        if (af->fVelocity > 0.0f)
        {
            // Apply 'Humanisation' parameters
            level       = level * ((1.0f - fDynamics*0.5) + fDynamics * sRandom.random(RND_EXP)) / af->fVelocity;
            delay      += millis_to_samples(nSampleRate, fDrift) * sRandom.random(RND_EXP);

            // Play sample
            play_sample(af, level, delay);

            // Trigger the note On indicator
            af->sNoteOn.blink();
            sActivity.blink();
        }
    }

    void sampler_kernel::trigger_off(size_t timestamp, float level)
    {
        if (nActive <= 0)
            return;

        size_t delay    = timestamp;
        size_t fadeout  = millis_to_samples(nSampleRate, fFadeout);

        for (size_t i=0; i<nActive; ++i)
            cancel_sample(vActive[i], fadeout, delay);
    }

    void sampler_kernel::trigger_stop(size_t timestamp)
    {
        // Apply changes to all ports
        for (size_t j=0; j<nChannels; ++j)
            vChannels[j].stop();
    }

    void sampler_kernel::process_listen_events()
    {
        if (sListen.pending())
        {
            trigger_on(0, 0.5f);
            sListen.commit();
        }

        for (size_t i=0; i<nFiles; ++i)
        {
            // Get descriptor
            afile_t *af         = &vFiles[i];
            if (af->pFile == NULL)
                continue;

            // Trigger the event
            if (af->sListen.pending())
            {
                // Play sample
                play_sample(af, 0.5f, 0); // Listen at mid-velocity

                // Update states
                af->sListen.commit();
                af->sNoteOn.blink();
            }
        }
    }

    void sampler_kernel::process_file_load_requests()
    {
        for (size_t i=0; i<nFiles; ++i)
        {
            // Get descriptor
            afile_t *af         = &vFiles[i];
            if (af->pFile == NULL)
                continue;

            // Get path and check task state
            path_t *path = af->pFile->getBuffer<path_t>();
            if ((path != NULL) && (path->accepted()) && (af->pLoader->completed()))
            {
                // Task has been completed
                lsp_trace("task has been completed");

                // Update state of audio file
                copy_asample(af->vData[AFI_OLD], af->vData[AFI_CURR]);
                copy_asample(af->vData[AFI_CURR], af->vData[AFI_NEW]);
                clear_asample(af->vData[AFI_NEW]);

                afsample_t *afs = af->vData[AFI_CURR];
                af->nStatus     = af->pLoader->code();
                af->bDirty      = true; // Mark sample for re-rendering
                af->fLength     = (af->nStatus == STATUS_OK) ? samples_to_millis(nSampleRate, afs->pFile->samples()) : 0.0f;

                lsp_trace("Current file: status=%d (%s), length=%f msec\n",
                    int(af->nStatus), get_status(af->nStatus), af->fLength);

                // Now we surely can commit changes and reset task state
                path->commit();
                af->pLoader->reset();

                // Trigger the state for reorder
                bReorder        = true;
            }

            // Check that we need to re-render sample
            if (af->bDirty)
                render_sample(af);
        }
    }

    void sampler_kernel::process(float **outs, const float **ins, size_t samples)
    {
        // Step 1
        // Process file load requests
        process_file_load_requests();

        // Reorder the files in ascending velocity order if needed
        if (bReorder)
        {
            // Reorder samples and reset the reorder flag
            reorder_samples();
            bReorder = false;
        }

        // Step 2
        // Process events
        process_listen_events();

        // Step 3
        // Process the channels individually
        if (ins != NULL)
        {
            for (size_t i=0; i<nChannels; ++i)
                vChannels[i].process(outs[i], ins[i], samples);
        }
        else
        {
            for (size_t i=0; i<nChannels; ++i)
                vChannels[i].process(outs[i], NULL, samples);
        }

        // Step 4
        // Output parameters
        output_parameters(samples);
    }

    void sampler_kernel::output_parameters(size_t samples)
    {
        // Update activity led output
        if (pActivity != NULL)
            pActivity->setValue(sActivity.process(samples));

        for (size_t i=0; i<nFiles; ++i)
        {
            afile_t *af         = &vFiles[i];

            // Output information about the file
            af->pLength->setValue(af->fLength);
            af->pStatus->setValue(af->nStatus);

            // Output information about the activity
            af->pNoteOn->setValue(af->sNoteOn.process(samples));

            // Get file sample
            afsample_t *afs     = af->vData[AFI_CURR];
            size_t channels     = (afs->pSample != NULL) ? afs->pSample->channels() : 0;
            if (channels > nChannels)
                channels             =  nChannels;

            // Output activity flag
            af->pActive->setValue(((af->bOn) && (channels > 0)) ? 1.0f : 0.0f);

            // Store file dump to mesh
            mesh_t *mesh        = reinterpret_cast<mesh_t *>(af->pMesh->getBuffer());
            if ((mesh == NULL) || (!mesh->isEmpty()))
                continue;

            if (channels > 0)
            {
                // Copy thumbnails
                for (size_t j=0; j<channels; ++j)
                    dsp::copy(mesh->pvData[j], afs->vThumbs[j], MESH_SIZE);

                mesh->data(channels, MESH_SIZE);
            }
            else
                mesh->data(0, 0);
        }
    }

    //-------------------------------------------------------------------------
    sampler_base::sampler_base(const plugin_metadata_t &metadata, size_t samplers, size_t channels, size_t files, bool dry_ports): plugin_t(metadata)
    {
        nChannels       = channels;
        nSamplers       = lsp_min(sampler_base_metadata::INSTRUMENTS_MAX, samplers);
        nFiles          = files;
        nDOMode         = 0;
        bDryPorts       = dry_ports;
        vSamplers       = NULL;

        for (size_t i=0; i<sampler_kernel_metadata::TRACKS_MAX; ++i)
        {
            channel_t *tc   = &vChannels[i];

            tc->vIn         = NULL;
            tc->vOut        = NULL;
            tc->vTmpIn      = NULL;
            tc->vTmpOut     = NULL;
            tc->pIn         = NULL;
            tc->pOut        = NULL;
        }

        pBuffer         = NULL;
        fDry            = 1.0f;
        fWet            = 1.0f;

        pMidiIn         = NULL;
        pMidiOut        = NULL;

        pBypass         = NULL;
        pMute           = NULL;
        pMuting         = NULL;
        pNoteOff        = NULL;
        pFadeout        = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pGain           = NULL;
        pDOGain         = NULL;
        pDOPan          = NULL;
    }

    sampler_base::~sampler_base()
    {
        destroy();
    }

    void sampler_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Allocate samplers
        vSamplers       = new sampler_t[nSamplers];
        lsp_trace("samplers = %d, channels=%d, vSamplers=%p", int(nSamplers), int(nChannels), vSamplers);
        if (vSamplers == NULL)
            return;

        // Initialize toggle
        sMute.init();

        // Initialize samplers
        ipc::IExecutor *executor    = wrapper->get_executor();

        for (size_t i=0; i<nSamplers; ++i)
        {
            // Get sampler pointer
            sampler_t *s = &vSamplers[i];

            // Initialize sampler
            lsp_trace("Initializing sampler #%d...", int(i));
            if (!s->sSampler.init(executor, nFiles, nChannels))
                return;

            s->nNote        = sampler_kernel_metadata::NOTE_DFL + sampler_kernel_metadata::OCTAVE_DFL * 12;
            s->nChannel     = sampler_kernel_metadata::CHANNEL_DFL;
            s->nMuteGroup   = i;
            s->bMuting      = false;
            s->bNoteOff     = false;

            // Initialize channels
            lsp_trace("Initializing channel group #%d...", int(i));
            for (size_t j=0; j<sampler_kernel_metadata::TRACKS_MAX; ++j)
            {
                sampler_channel_t *c    = &s->vChannels[j];
                c->vDry     = NULL;
                c->fPan     = 1.0f;
                c->pDry     = NULL;
                c->pPan     = NULL;
            }

            // Cleanup gain pointer
            s->pGain        = NULL;
            s->pBypass      = NULL;
            s->pDryBypass   = NULL;
            s->pChannel     = NULL;
            s->pNote        = NULL;
            s->pOctave      = NULL;
            s->pMuteGroup   = NULL;
            s->pMuting      = NULL;
            s->pMidiNote    = NULL;
            s->pNoteOff     = NULL;
        }

        // Initialize temporary buffers
        size_t allocate         = sampler_base_metadata::BUFFER_SIZE * nChannels * 2; // vTmpIn + vTmpOut
        lsp_trace("Allocating temporary buffer of %d samples", int(allocate));
        pBuffer                 = new float[allocate];
        if (pBuffer == NULL)
            return;

        lsp_trace("Initializing temporary buffers");
        float *fptr             = pBuffer;
        for (size_t i=0; i<nChannels; ++i)
        {
            vChannels[i].vTmpIn     = fptr;
            fptr                   += sampler_base_metadata::BUFFER_SIZE;
            vChannels[i].vTmpOut    = fptr;
            fptr                   += sampler_base_metadata::BUFFER_SIZE;
        }

        // Initialize metadata
        size_t port_id          = 0;

        // Bind audio inputs
        lsp_trace("Binding audio inputs...");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn        = vPorts[port_id++];
            vChannels[i].vIn        = NULL;
        }

        // Bind audio outputs
        lsp_trace("Binding audio outputs...");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut       = vPorts[port_id++];
            vChannels[i].vOut       = NULL;
        }

        // Bind MIDI ports
        lsp_trace("Binding MIDI ports...");
        TRACE_PORT(vPorts[port_id]);
        pMidiIn     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMidiOut    = vPorts[port_id++];

        // Bind ports
        lsp_trace("Binding Global ports...");
        TRACE_PORT(vPorts[port_id]);
        pBypass     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMute       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMuting     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pNoteOff    = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFadeout    = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDry        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWet        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGain       = vPorts[port_id++];
        if (bDryPorts)
        {
            TRACE_PORT(vPorts[port_id]);
            pDOGain     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            pDOPan      = vPorts[port_id++];
        }

        // If number of samplers <= 2 - skip area selector
        if (nSamplers > 2)
        {
            lsp_trace("Skipping mixer selector port...");
            TRACE_PORT(vPorts[port_id]);
            port_id++;
        }

        // If number of samplers > 0 - skip instrument selector
        if (nSamplers > 1)
        {
            lsp_trace("Skipping instrument selector...");
            TRACE_PORT(vPorts[port_id]);
            port_id     ++;
        }

        // Now process each instrument
        for (size_t i=0; i<nSamplers; ++i)
        {
            sampler_t *s    = &vSamplers[i];

            // Bind trigger
            lsp_trace("Binding trigger #%d ports...", int(i));
            TRACE_PORT(vPorts[port_id]);
            s->pChannel     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            s->pNote        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            s->pOctave      = vPorts[port_id++];
            if (nSamplers > 1)
            {
                TRACE_PORT(vPorts[port_id]);
                s->pMuteGroup   = vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                s->pMuting      = vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                s->pNoteOff     = vPorts[port_id++];
            }
            TRACE_PORT(vPorts[port_id]);
            s->pMidiNote    = vPorts[port_id++];

            // Bind sampler
            lsp_trace("Binding sampler #%d ports...", int(i));
            port_id         = s->sSampler.bind(vPorts, port_id, true);
        }

        if (nSamplers > 1)
        {
            for (size_t i=0; i<nSamplers; ++i)
            {
                sampler_t *s = &vSamplers[i];

                // Bind Bypass port
                lsp_trace("Binding bypass port...");
                TRACE_PORT(vPorts[port_id]);
                s->pBypass      = vPorts[port_id++];

                // Bind mixing gain port
                lsp_trace("Binding gain port...");
                TRACE_PORT(vPorts[port_id]);
                s->pGain    = vPorts[port_id++];

                // Bind panorama port
                if (nChannels > 1)
                {
                    lsp_trace("Binding panorama ports...");
                    for (size_t j=0; j<nChannels; ++j)
                    {
                        TRACE_PORT(vPorts[port_id]);
                        s->vChannels[j].pPan    = vPorts[port_id++];
                    }
                }

                // Bind activity port
                s->sSampler.bind_activity(vPorts[port_id++]);

                // Bind dry port if present
                if (bDryPorts)
                {
                    lsp_trace("Binding dry ports...");
                    TRACE_PORT(vPorts[port_id]);
                    s->pDryBypass       = vPorts[port_id++];

                    for (size_t j=0; j<nChannels; ++j)
                    {
                        TRACE_PORT(vPorts[port_id]);
                        s->vChannels[j].pDry    = vPorts[port_id++];
                    }
                }
            }
        }

        // Call for initial settings update
        lsp_trace("Calling settings update");
        update_settings();
    }

    void sampler_base::destroy()
    {
        if (vSamplers != NULL)
        {
            for (size_t i=0; i<nSamplers; ++i)
            {
                sampler_t *s    = &vSamplers[i];
                s->sSampler.destroy();

                for (size_t j=0; j<nChannels; ++j)
                {
                    sampler_channel_t *c    = &s->vChannels[j];
                    c->vDry         = NULL;
                    c->pDry         = NULL;
                    c->pPan         = NULL;
                }

                s->pGain        = NULL;
                s->pBypass      = NULL;
                s->pDryBypass   = NULL;
                s->pChannel     = NULL;
                s->pNote        = NULL;
                s->pOctave      = NULL;
                s->pMidiNote    = NULL;
            }

            delete [] vSamplers;
            vSamplers       = NULL;
        }

        if (pBuffer != NULL)
        {
            delete      [] pBuffer;
            pBuffer     = NULL;

            for (size_t i=0; i<sampler_kernel_metadata::TRACKS_MAX; ++i)
            {
                channel_t *tc   = &vChannels[i];
                tc->vIn         = NULL;
                tc->vOut        = NULL;
                tc->vTmpIn      = NULL;
                tc->vTmpOut     = NULL;
                tc->pIn         = NULL;
                tc->pOut        = NULL;
            }
        }
    }

    void sampler_base::update_settings()
    {
        // Update dry & wet parameters
        float dry   = (pDry != NULL)    ? pDry->getValue()  : 1.0f;
        float wet   = (pWet != NULL)    ? pWet->getValue()  : 1.0f;
        float gain  = (pGain != NULL)   ? pGain->getValue() : 1.0f;
        fDry        = dry * gain;
        fWet        = wet * gain;

        lsp_trace("dry = %f, wet=%f, gain=%f", dry, wet, gain);

        // Update muting state
        if (pMute != NULL)
            sMute.submit(pMute->getValue());

        // Update bypass (if present)
        if (pBypass != NULL)
        {
            bool bypass     = pBypass->getValue() >= 0.5f;
            for (size_t i=0; i<nChannels; ++i)
                vChannels[i].sBypass.set_bypass(bypass);
        }

        // Update settings on all samplers and triggers
        bool muting     = pMuting->getValue() >= 0.5f;
        bool note_off   = pNoteOff->getValue() >= 0.5f;
        nDOMode         = 0;
        if ((pDOGain != NULL) && (pDOGain->getValue() >= 0.5f))
            nDOMode        |= DM_APPLY_GAIN;
        if ((pDOPan != NULL) && (pDOPan->getValue() >= 0.5f))
            nDOMode        |= DM_APPLY_PAN;

        lsp_trace("muting=%s", (muting) ? "true" : "false");
        lsp_trace("note_off=%s", (note_off) ? "true" : "false");
        lsp_trace("do_mode=0x%x", int(nDOMode));

        for (size_t i=0; i<nSamplers; ++i)
        {
            sampler_t *s    = &vSamplers[i];

            // MIDI note and channel
            s->nNote        = (s->pOctave->getValue() * 12) + s->pNote->getValue();
            s->nChannel     = s->pChannel->getValue();
            s->nMuteGroup   = (s->pMuteGroup != NULL) ? s->pMuteGroup->getValue() : i;
            s->bMuting      = (s->pMuting != NULL) ? s->pMuting->getValue() >= 0.5f : false;
            s->bMuting      = s->bMuting || muting;
            s->bNoteOff     = (s->pNoteOff != NULL) ? s->pNoteOff->getValue() >= 0.5f : false;
            s->bNoteOff     = s->bNoteOff || note_off;

            lsp_trace("Sampler %d channel=%d, note=%d", int(i), int(s->nChannel), int(s->nNote));
            if (s->pMidiNote != NULL)
                s->pMidiNote->setValue(s->nNote);

            // Get gain values
            s->fGain        = (s->pGain != NULL) ? s->pGain->getValue() : 1.0f;
            if (nChannels <= 2)
            {
                sampler_channel_t *c    = &s->vChannels[0];
                c->fPan                 = (c->pPan != NULL) ? ((100.0f - c->pPan->getValue()) * 0.005f) : 1.0f;
                if (nChannels  == 2)
                {
                    c                       = &s->vChannels[1];
                    c->fPan                 = (c->pPan != NULL) ? ((100.0f + c->pPan->getValue()) * 0.005f) : 1.0f;
                }
            }
            else
            {
                for (size_t j=0; j<nChannels; ++j)
                {
                    sampler_channel_t *c    = &s->vChannels[j];
                    c->fPan                 = (c->pPan != NULL) ? ((100.0f - c->pPan->getValue()) * 0.005f) : 1.0f;
                }
            }

            // Get bypass
            bool bypass     = (s->pBypass != NULL) ? s->pBypass->getValue() < 0.5f : 0.0f;
            bool dry_bypass = (s->pDryBypass != NULL) ? s->pDryBypass->getValue() < 0.5f : 0.0f;
            for (size_t j=0; j<nChannels; ++j)
            {
                sampler_channel_t *c    = &s->vChannels[j];
                c->sBypass.set_bypass(bypass);
                c->sDryBypass.set_bypass(dry_bypass);
            }

            // Additional parameters
            s->sSampler.set_fadeout(pFadeout->getValue());
            s->sSampler.update_settings();
        }
    }

    void sampler_base::update_sample_rate(long sr)
    {
        // Update sample rate for bypass
        for (size_t i=0; i<nChannels; ++i)
            vChannels[i].sBypass.init(sr);

        // Update settings on all samplers
        for (size_t i=0; i<nSamplers; ++i)
        {
            sampler_t *s = &vSamplers[i];
            s->sSampler.update_sample_rate(sr);

            for (size_t j=0; j<nChannels; ++j)
            {
                sampler_channel_t *sc   = &s->vChannels[j];
                sc->sBypass.init(sr);
                sc->sDryBypass.init(sr);
            }
        }
    }

    void sampler_base::process_trigger_events()
    {
        // Process muting button
        if ((pMute != NULL) && (sMute.pending()))
        {
            // Cancel playback for all samplers
            for (size_t i=0; i<nSamplers; ++i)
                vSamplers[i].sSampler.trigger_stop(0);
            sMute.commit(true);
        }

        // Get MIDI input, return if none
        midi_t *in          = (pMidiIn != NULL) ? reinterpret_cast<midi_t *>(pMidiIn->getBuffer()) : NULL;
        if (in == NULL)
            return;

        // Bypass MIDI events
        midi_t *out         = (pMidiOut != NULL) ? reinterpret_cast<midi_t *>(pMidiOut->getBuffer()) : NULL;
        if (out != NULL)
            out->copy_from(in);

        #ifdef LSP_TRACE
            if (in->nEvents > 0)
                lsp_trace("trigger this=%p, number of events = %d", this, int(in->nEvents));
        #endif

        // Process MIDI events for all samplers
        for (size_t i=0; i<in->nEvents; ++i)
        {
            // Analyze MIDI event
            const midi::event_t *me     = &in->vEvents[i];
            switch (me->type)
            {
                case midi::MIDI_MSG_NOTE_ON:
                {
                    lsp_trace("NOTE_ON: channel=%d, pitch=%d, velocity=%d",
                            int(me->channel), int(me->note.pitch), int(me->note.velocity));

                    uint32_t mg[BITMASK_MAX]; // Triggered mute groups
                    uint32_t ts[BITMASK_MAX]; // Triggered samplers

                    // Initialize parameters
                    float gain  = me->note.velocity / 127.0f;
                    for (size_t j=0; j<BITMASK_MAX; ++j)
                    {
                        mg[j]       = 0;
                        ts[j]       = 0;
                    }

                    // Scan state of samplers
                    for (size_t j=0; j<nSamplers; ++j)
                    {
                        sampler_t *s = &vSamplers[j];
                        if ((s->nNote != me->note.pitch) || (s->nChannel != me->channel))
                            continue;

                        size_t g    = s->nMuteGroup;
                        mg[g >> 5] |= (1 << (g & 0x1f));        // Mark mute group as triggered
                        ts[j >> 5] |= (1 << (j & 0x1f));        // Mark sampler as triggered
                    }

                    // Apply changes
                    for (size_t j=0; j<nSamplers; ++j)
                    {
                        sampler_t *s    = &vSamplers[j];
                        size_t g        = s->nMuteGroup;
                        bool muted      = (g > 0) && (mg[g >> 5] & (1 << (g & 0x1f)));
                        bool triggered  = ts[j >> 5] & (1 << (j & 0x1f));

                        if (triggered)
                            s->sSampler.trigger_on(me->timestamp, gain);
                        else if (muted)
                            s->sSampler.trigger_off(me->timestamp, gain);
                    }
                    break;
                }

                case midi::MIDI_MSG_NOTE_OFF:
                {
                    lsp_trace("NOTE_OFF: channel=%d, pitch=%d, velocity=%d",
                            int(me->channel), int(me->note.pitch), int(me->note.velocity));
                    float gain = me->note.velocity / 127.0f;

                    for (size_t j=0; j<nSamplers; ++j)
                    {
                        sampler_t *s = &vSamplers[j];
                        if ((!s->bNoteOff) || (s->nNote != me->note.pitch) || (s->nChannel != me->channel))
                            continue;

                        s->sSampler.trigger_off(me->timestamp, gain);
                    }
                    break;
                }

                case midi::MIDI_MSG_NOTE_CONTROLLER:
                    lsp_trace("NOTE_CONTROLLER: channel=%d, control=%02x, value=%d",
                            int(me->channel), int(me->ctl.control), int(me->ctl.value));
                    if (me->ctl.control != midi::MIDI_CTL_ALL_NOTES_OFF)
                        break;

                    for (size_t j=0; j<nSamplers; ++j)
                    {
                        sampler_t *s = &vSamplers[j];
                        if ((!s->bMuting) || (me->channel != s->nChannel))
                            continue;

                        s->sSampler.trigger_stop(me->timestamp);
                    }
                    break;

                default:
                    break;
            }
        } // for i
    }

    void sampler_base::process(size_t samples)
    {
        // Process all MIDI events
        process_trigger_events();

        // Prepare audio channels
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];
            c->vIn          = c->pIn->getBuffer<float>();
            c->vOut         = c->pOut->getBuffer<float>();
        }

        // Prepare sampler's buffers
        float *tmp_outs[sampler_kernel_metadata::TRACKS_MAX];
        const float *tmp_ins[sampler_kernel_metadata::TRACKS_MAX];

        for (size_t i=0; i<nChannels; ++i)
        {
            tmp_ins[i]      = NULL;
            tmp_outs[i]     = vChannels[i].vTmpOut;

            // Bind direct channels (if present)
            for (size_t j=0; j<nSamplers; ++j)
            {
                sampler_t *s            = &vSamplers[j];
                sampler_channel_t *c    = &s->vChannels[i];
                c->vDry         = (c->pDry != NULL) ? c->pDry->getBuffer<float>() : NULL;
            }
        }

        // Process samples
        size_t left         = samples;

        while (left > 0)
        {
            // Determine number of elements to process
            size_t count        = (left > sampler_base_metadata::BUFFER_SIZE) ? sampler_base_metadata::BUFFER_SIZE : left;

            // Save input data into temporary input buffer
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c            = &vChannels[i];
                dsp::copy(c->vTmpIn, c->vIn, count);
                dsp::fill_zero(c->vOut, count);
            }

            // Execute all samplers
            for (size_t i=0; i<nSamplers; ++i)
            {
                sampler_t *s = &vSamplers[i];

                // Call sampler for processing
                s->sSampler.process(tmp_outs, tmp_ins, left);

                // Preprocess dry channels: fill with zeros
                for (size_t j=0; j<nChannels; ++j)
                {
                    sampler_channel_t *c    = &s->vChannels[j];
                    if (c->vDry != NULL)
                        dsp::fill_zero(c->vDry, count);
                }

                // Now post-process all channels for sampler
                for (size_t j=0; j<nChannels; ++j)
                {
                    sampler_channel_t *c    = &s->vChannels[j];

                    // Copy data to direct output buffer if present
                    float gain  = (nDOMode & DM_APPLY_GAIN) ? s->fGain : 1.0f;
                    float pan   = (nDOMode & DM_APPLY_PAN) ? c->fPan : 1.0f;
                    if (s->vChannels[j].vDry != NULL)
                        dsp::fmadd_k3(s->vChannels[j].vDry, tmp_outs[j], pan * gain, count);
                    if (s->vChannels[j^1].vDry != NULL)
                        dsp::fmadd_k3(s->vChannels[j^1].vDry, tmp_outs[j], (1.0f - pan) * gain, count);

                    // Process output
                    c->sBypass.process(tmp_outs[j], NULL, tmp_outs[j], count);

                    // Mix output to common sampler's bus
                    if (vChannels[j].vOut != NULL)
                        dsp::fmadd_k3(vChannels[j].vOut, tmp_outs[j], c->fPan * s->fGain, count);

                    // Apply pan to the other stereo channel (if present)
                    if (vChannels[j^1].vOut != NULL)
                        dsp::fmadd_k3(vChannels[j^1].vOut, tmp_outs[j], (1.0f - c->fPan) * s->fGain, count);
                }

                // Post-process dry channels
                for (size_t j=0; j<nChannels; ++j)
                {
                    sampler_channel_t *c    = &s->vChannels[j];
                    if (c->vDry != NULL)
                    {
                        c->sDryBypass.process(c->vDry, NULL, c->vDry, count);
                        c->vDry    += count;
                    }
                }
            }

            // Post-process the summarized signal from samplers
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c            = &vChannels[i];

                dsp::mix2(c->vOut, c->vTmpIn, fWet, fDry, count); // Adjust volume between dry and wet channels
                if (pBypass != NULL)
                    c->sBypass.process(c->vOut, c->vTmpIn, c->vOut, count);

                // Increment pointers
                c->vOut                += count;
                c->vIn                 += count;
            }

            // Decrement counter
            left                   -= count;
        }
    }

    //-------------------------------------------------------------------------
    sampler_mono::sampler_mono(): sampler_base(metadata, 1, 1, SAMPLE_FILES, false)
    {
    }

    sampler_stereo::sampler_stereo(): sampler_base(metadata, 1, 2, SAMPLE_FILES, false)
    {
    }

    multisampler_x12::multisampler_x12(): sampler_base(metadata, 12, 2, SAMPLE_FILES, false)
    {
    }

    multisampler_x24::multisampler_x24(): sampler_base(metadata, 24, 2, SAMPLE_FILES, false)
    {
    }

    multisampler_x48::multisampler_x48(): sampler_base(metadata, 48, 2, SAMPLE_FILES, false)
    {
    }

    multisampler_x12_do::multisampler_x12_do(): sampler_base(metadata, 12, 2, SAMPLE_FILES, true)
    {
    }

    multisampler_x24_do::multisampler_x24_do(): sampler_base(metadata, 24, 2, SAMPLE_FILES, true)
    {
    }

    multisampler_x48_do::multisampler_x48_do(): sampler_base(metadata, 48, 2, SAMPLE_FILES, true)
    {
    }
} /* namespace lsp */


