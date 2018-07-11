/*
 * impulse_responses.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#ifndef LSP_NO_EXPERIMENTAL

#include <core/dsp.h>

#include <plugins/impulse_responses.h>
#include <core/debug.h>
#include <core/status.h>

#include <string.h>

#define BUFFER_SIZE         (0x10000 / sizeof(float))

namespace lsp
{
    impulse_responses_base::IRLoader::IRLoader(impulse_responses_base *base, af_descriptor_t *descr)
    {
        pCore       = base;
        pDescr      = descr;
    }

    impulse_responses_base::IRLoader::~IRLoader()
    {
        pCore       = NULL;
        pDescr      = NULL;
    }

    int impulse_responses_base::IRLoader::run()
    {
        return pCore->load(pDescr);
    }

    //-------------------------------------------------------------------------
    impulse_responses_base::impulse_responses_base(const plugin_metadata_t &metadata): plugin_t(metadata)
    {
        pExecutor       = NULL;
        nChannels       = 0;
        nFiles          = 0;
        vFiles          = NULL;
        vChannels       = NULL;
        nMaxSamples     = 0;
        vBuffer         = NULL;
    }

    impulse_responses_base::~impulse_responses_base()
    {
        pExecutor       = NULL;
        nChannels       = 0;
        nFiles          = 0;
        vFiles          = NULL;
        vChannels       = NULL;
        nMaxSamples     = 0;
        vBuffer         = NULL;
    }

    impulse_responses_base::af_descriptor_t *impulse_responses_base::create_files(size_t count)
    {
        if (count <= 0)
            return NULL;

        // Try to allocate
        af_descriptor_t    *files = new af_descriptor_t[count];
        if (files == NULL)
            return NULL;

        // Initialize
        for (size_t i=0; i<count; ++i)
        {
            af_descriptor_t *af = &files[i];
            af->pCurr       = NULL;
            af->pNew        = NULL;
            af->pOld        = NULL;
            af->pLoader     = NULL;
            af->pPort       = NULL;
        }

        // Create loaders
        for (size_t i=0; i<count; ++i)
        {
            IRLoader *loader = new IRLoader(this, &files[i]);
            if (loader == NULL)
            {
                destroy_files(files, count);
                return NULL;
            }
            files[i].pLoader    = loader;
        }

        return files;
    }

    void impulse_responses_base::destroy_files(af_descriptor_t *files, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            af_descriptor_t *af = &files[i];

            // Destroy loader
            if (af->pLoader != NULL)
            {
                delete af->pLoader;
                af->pLoader     = NULL;
            }

            // Destroy file pointers
            if (af->pCurr != NULL)
            {
                af->pCurr->destroy();
                delete af->pCurr;
                af->pCurr       = NULL;
            }

            if (af->pNew != NULL)
            {
                af->pNew->destroy();
                delete af->pNew;
                af->pNew        = NULL;
            }

            if (af->pOld != NULL)
            {
                af->pOld->destroy();
                delete af->pOld;
                af->pOld    = NULL;
            }

            // Forget port
            af->pPort       = NULL;
        }
    }

    impulse_responses_base::ac_descriptor_t *impulse_responses_base::create_channels(size_t count)
    {
        if (count <= 0)
            return NULL;

        // Allocate channels
        ac_descriptor_t *channels = new ac_descriptor_t[count];
        if (channels == NULL)
            return NULL;

        // Initialize
        for (size_t i=0; i<count; ++i)
        {
            ac_descriptor_t *ac = &channels[i];
            ac->pInput          = NULL;
            ac->pOutput         = NULL;
            ac->pConv           = NULL;
            ac->nConvLen        = 0;
            ac->nFile           = 0;
            ac->nChannel        = 0;
            ac->fLength         = 0;
        }

        return channels;
    }

    void impulse_responses_base::destroy_channels(ac_descriptor_t *channels, size_t count)
    {
        if (channels == NULL)
            return;

        for (size_t i=0; i<count; ++i)
        {
            ac_descriptor_t *ac = &channels[i];

            // Destroy buffer
            ac->vBuffer.destroy();

            // Clear other fields
            ac->pInput          = NULL;
            ac->pOutput         = NULL;
            ac->pConv           = NULL;
            ac->nConvLen        = 0;
            ac->nFile           = 0;
            ac->nChannel        = 0;
            ac->fLength         = 0;
        }
    }

    void impulse_responses_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        pExecutor       = wrapper->get_executor();
        lsp_trace("Executor = %p", pExecutor);

        // Determine number of objects
        nChannels       = 0;
        nFiles          = 0;
        for (const port_t *p = pMetadata->ports; p->id != NULL; ++p)
        {
            if ((p->role == R_AUDIO) && (p->flags & F_OUT))
                nChannels   ++;
            if (p->role == R_PATH)
                nFiles      ++;
        }

        lsp_trace("channels=%d, files=%d", int(nChannels), int(nFiles));

        // Allocate temporary buffer
        vBuffer         = new float[BUFFER_SIZE];
        if (vBuffer != NULL)
        {
            // Allocate objects
            vFiles          = create_files(nFiles);
            if (vFiles != NULL)
            {
                vChannels       = create_channels(nChannels);
                if (vChannels != NULL)
                    return;

                destroy_files(vFiles, nFiles);
                vFiles      = NULL;
            }

            delete []   vBuffer;
            vBuffer     = NULL;
        }
    }

    void impulse_responses_base::destroy()
    {
        // Drop buffers
        if (vChannels != NULL)
        {
            destroy_channels(vChannels, nChannels);
            vChannels       = NULL;
        }

        // Drop files
        if (vFiles != NULL)
        {
            destroy_files(vFiles, nFiles);
            vFiles          = NULL;
        }

        // Drop buffer
        if (vBuffer != NULL)
        {
            delete[] vBuffer;
            vBuffer         = NULL;
        }

        // Clear fields
        pExecutor       = NULL;
        nChannels       = 0;
        nFiles          = 0;
    }

    void impulse_responses_base::update_settings()
    {
//        lsp_trace("updating settings");

        // Process file load requests
        for (size_t i=0; i<nFiles; ++i)
        {
            // Get descriptor
            af_descriptor_t *af     = &vFiles[i];
            if (af->pPort == NULL)
                continue;

            // Get path
            path_t *path = reinterpret_cast<path_t *>(af->pPort->getBuffer());
            if ((path == NULL) || (!path->pending()))
                continue;

//            lsp_trace("request port=%s path=%s", af->pPort->metadata()->id, path->get_path());

            // Check task state
            if (af->pLoader->idle())
            {
                // Try to submit task
                if (pExecutor->submit(af->pLoader))
                {
                    lsp_trace("successfully submitted task");
                    path->accept();
                }
            }
        }

        // Call re-configuration
        reconfigure();
    }

    void impulse_responses_base::update_sample_rate(long sr)
    {
        nMaxSamples     = sr * impulse_responses_base_metadata::CONVOLUTION_TIME_MAX;

        // Resize buffers
        for (size_t i=0; i<nChannels; ++i)
        {
            // Init buffer
            vChannels[i].vBuffer.resize
            (
                nMaxSamples * impulse_responses_base_metadata::CONVOLUTION_BUFFER_SIZE,
                nMaxSamples
            );

            // Init bypass
            vChannels[i].vBypass.init(sr);
        }
    }

    void impulse_responses_base::process(size_t samples)
    {
        //---------------------------------------------------------------------
        // Stage 1: process file events
        bool reroute        = false;

        // Process file load requests
        for (size_t i=0; i<nFiles; ++i)
        {
            // Get descriptor
            af_descriptor_t *af     = &vFiles[i];
            if (af->pPort == NULL)
                continue;

            // Get path
            path_t *path = reinterpret_cast<path_t *>(af->pPort->getBuffer());
            if ((path == NULL) || (!path->accepted()))
                continue;

            // Check task state
            if (af->pLoader->completed())
            {
                // Task has been completed
                lsp_trace("task has been completed");

                // Need to swap data
                af->pOld        = af->pCurr;
                af->pCurr       = af->pNew;
                af->pNew        = NULL;
                reroute         = true;

                // Now we surely can commit changes and reset task state
                path->commit();
                af->pLoader->reset();
            }
        }

        // Re-route channels if files were updated
        if (reroute)
            reconfigure();

        //---------------------------------------------------------------------
        // Stage 2: perform convolution
        for (size_t i=0; i<nChannels; ++i)
        {
            // Get channel
            ac_descriptor_t *ac     = &vChannels[i];

            // Get output data
            if (ac->pOutput == NULL)
                continue;
            float *out          = reinterpret_cast<float *>(ac->pOutput -> getBuffer());
            if (out == NULL)
                continue;

            // Get input data
            float *in           = NULL;
            if (ac->pInput != NULL)
                in          = reinterpret_cast<float *>(ac->pInput -> getBuffer());

            // Get convolution
            const float *conv   = ac->pConv;
            if ((ac->nConvLen == 0) || (in == NULL))
                conv        = NULL;

            // Now we are ready to convolve
            size_t count    = samples;
            while (count > 0)
            {
                // Put data to buffer
                size_t process      = (count >= BUFFER_SIZE) ? BUFFER_SIZE : count;
                size_t added        = ac->vBuffer.append(in, process);
                if (added <= 0)
                    continue;

                // Perform convolution
                if (conv != NULL)
                {
                    dsp::convolve(vBuffer, ac->vBuffer.tail() - added - ac->nConvLen + 1, conv, ac->nConvLen, added);
                    dsp::mix(vBuffer, in, vBuffer, ac->fDry, ac->fWet, added);
                }
                else
                    dsp::fill_zero(vBuffer, added);

                ac->vBypass.process(out, in, vBuffer, added);

                // Remove old data from buffer
                ac->vBuffer.shift(NULL, added);

                // Move pointers and counters
                out     += added;
                count   -= added;
            }
        }
    }

    int impulse_responses_base::load(af_descriptor_t *descr)
    {
        lsp_trace("descr = %p", descr);
        // Check state
        if (descr == NULL)
            return -1;

        if (descr->pNew != NULL)
            return -2;

        // Destroy OLD data if exists
        if (descr->pOld != NULL)
        {
            descr->pOld     ->destroy();
            delete descr->pOld;
            descr->pOld     = NULL;
        }

        // Check port binding
        if (descr->pPort == NULL)
            return -3;

        // Get path
        path_t *path        = reinterpret_cast<path_t *>(descr->pPort->getBuffer());
        if (path == NULL)
            return -4;

        // Get file name
        const char *fname   = path->get_path();
        if (strlen(fname) <= 0)
            return -5;

        // Load audio file
        AudioFile *af = new AudioFile();
        if (af == NULL)
            return -6;

        status_t status = af->load(fname);
        if (status != STATUS_OK)
        {
            lsp_trace("load failed");
            af->destroy();
            delete af;
            return status;
        }

        status = af->resample(fSampleRate);
        if (status != STATUS_OK)
        {
            lsp_trace("resample failed");
            af->destroy();
            delete af;
            return status;
        }

        // Pre-process file
        if (!af->reverse())
        {
            lsp_trace("reverse failed");
            af->destroy();
            delete af;
            return -9;
        }

        // Store pointer to file
        descr->pNew     = af;

        lsp_trace("success execution");

        return 0;
    }

    void impulse_responses_base::reconfigure()
    {
        for (size_t i=0; i<nChannels; ++i)
        {
            size_t file     = vChannels[i].nFile;
            if ((file < nFiles) && (vFiles[i].pCurr != NULL))
            {
                const float *conv   = vFiles[i].pCurr->channel(vChannels[i].nChannel);
                size_t conv_len     = vFiles[i].pCurr->samples();
                if ((conv != NULL) && (conv_len > 0))
                {
                    size_t real_len     = size_t(conv_len * vChannels[i].fLength * 0.01) & (~size_t(0x03));
                    if (real_len > 0)
                    {
                        vChannels[i].pConv      = &conv[conv_len - real_len];
                        vChannels[i].nConvLen   = real_len;

                        lsp_trace("vChannels[%d].pConv      = %p", int(i), vChannels[i].pConv);
                        lsp_trace("vChannels[%d].nConvLen   = %d", int(i), int(vChannels[i].nConvLen));
                        continue; // Configuration OK
                    }
                }
            }

            // Reset channel
            vChannels[i].pConv      = NULL;
            vChannels[i].nConvLen   = 0;
            lsp_trace("vChannels[%d].pConv      = %p", int(i), vChannels[i].pConv);
            lsp_trace("vChannels[%d].nConvLen   = %d", int(i), int(vChannels[i].nConvLen));
        }
    }

    //-------------------------------------------------------------------------

    impulse_responses_mono::impulse_responses_mono(): impulse_responses_base(metadata)
    {
    }

    impulse_responses_mono::~impulse_responses_mono()
    {
    }

    void impulse_responses_mono::init(IWrapper *wrapper)
    {
        impulse_responses_base::init(wrapper);

        vFiles[0].pPort         = vPorts[FNAME];
        vChannels[0].pInput     = vPorts[IN];
        vChannels[0].pOutput    = vPorts[OUT];
    }

    void impulse_responses_mono::reconfigure()
    {
        bool bypass             = vPorts[BYPASS]    -> getValue() >= 0.5;
        float out_gain          = vPorts[OUT_GAIN]  -> getValue();

        vChannels[0].nFile      = 0;
        vChannels[0].nChannel   = vPorts[CHANNEL]   -> getValue();
        vChannels[0].fDry       = vPorts[DRY]       -> getValue() * out_gain;
        vChannels[0].fWet       = vPorts[WET]       -> getValue() * out_gain;
        vChannels[0].fLength    = vPorts[LENGTH]    -> getValue();
        vChannels[0].vBypass.set_bypass(bypass);

        lsp_trace("vChannels[0].nFile = %d", int(vChannels[0].nFile));
        lsp_trace("vChannels[0].nChannel = %d", int(vChannels[0].nChannel));
        lsp_trace("vChannels[0].fDry = %.5f", vChannels[0].fDry);
        lsp_trace("vChannels[0].fWet = %.5f", vChannels[0].fWet);
        lsp_trace("vChannels[0].fLength = %.5f", vChannels[0].fLength);
        lsp_trace("vChannels[0].bypass = %s", (bypass) ? "true" : "false");

        // Call for reconfiguration
        impulse_responses_base::reconfigure();
    }

} /* namespace lsp */

#endif /* LSP_NO_EXPERIMENTAL */
