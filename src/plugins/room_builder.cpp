/*
 * room_builder.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <core/files/Model3DFile.h>

#include <plugins/room_builder.h>

#define TMP_BUF_SIZE            4096
#define CONV_RANK               10
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    //-------------------------------------------------------------------------
    // 3D Scene loader
    void room_builder_base::SceneLoader::init(room_builder_base *base)
    {
        pCore   = base;
        sScene.clear();
    }

    void room_builder_base::destroy()
    {
        sScene.destroy();
    }

    void room_builder_base::SceneLoader::apply_changes()
    {
        pCore->sScene.swap(&sScene);
        if (pCore->p3DStatus != NULL)
            pCore->p3DStatus->setValue(this->code());
    }

    status_t room_builder_base::SceneLoader::run()
    {
        // Clear scene
        sScene.clear();

        // Check state
        if (pCore->p3DFile == NULL)
            return STATUS_UNKNOWN_ERR;

        // Get path
        path_t *path = pCore->p3DFile->getBuffer<path_t>();
        if (path == NULL)
            return STATUS_UNKNOWN_ERR;

        // Get file name
        const char *fname = path->get_path();
        if (strlen(fname) <= 0)
            return STATUS_UNSPECIFIED;

        // Load the scene file
        return Model3DFile::load(&sScene, fname, true);
    }

    //-------------------------------------------------------------------------
    room_builder_base::room_builder_base(const plugin_metadata_t &metadata, size_t inputs):
        plugin_t(metadata)
    {
        nInputs         = inputs;
        nReconfigReq    = 0;
        nReconfigResp   = 0;

        pBypass         = NULL;
        p3DFile         = NULL;
        p3DStatus       = NULL;

        pData           = NULL;
    }

    room_builder_base::~room_builder_base()
    {
    }

    void room_builder_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Allocate memory
        size_t tmp_buf_size = TMP_BUF_SIZE * sizeof(float);
        size_t alloc        = tmp_buf_size * (impulse_reverb_base_metadata::CONVOLVERS + 2);
        uint8_t *ptr        = alloc_aligned<uint8_t>(pData, alloc);
        if (pData == NULL)
            return;
        uint8_t *ptr    = ALIGN_PTR(pData, DEFAULT_ALIGN);

        // Initialize 3D loader
        s3DLoader.init(this);

        // Initialize inputs
        for (size_t i=0; i<2; ++i)
        {
            input_t *in     = &vInputs[i];
            in->vIn         = NULL;
            in->pIn         = NULL;
            in->pPan        = NULL;
        }

        // Initialize output channels
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c    = &vChannels[i];

            if (!c->sPlayer.init(impulse_reverb_base_metadata::FILES, 32))
                return;
            if (!c->sEqualizer.init(impulse_reverb_base_metadata::EQ_BANDS + 2, CONV_RANK))
                return;
            c->sEqualizer.set_mode(EQM_BYPASS);

            c->fDryPan[0]   = 0.0f;
            c->fDryPan[1]   = 0.0f;

            c->vOut         = NULL;
            c->vBuffer      = reinterpret_cast<float *>(ptr);
            ptr            += tmp_buf_size;

            c->pOut         = NULL;

            c->pWetEq       = NULL;
            c->pLowCut      = NULL;
            c->pLowFreq     = NULL;
            c->pHighCut     = NULL;
            c->pHighFreq    = NULL;

            for (size_t j=0; j<impulse_reverb_base_metadata::EQ_BANDS; ++j)
                c->pFreqGain[j]     = NULL;
        }

        // Bind ports
    }

    void room_builder_base::destroy()
    {
        sScene.destroy();
        s3DLoader.destroy();

        if (pData != NULL)
        {
            free_aligned(pData);
            pData       = NULL;
        }
    }

    void room_builder_base::update_settings()
    {

    }

    void room_builder_base::update_sample_rate(long sr)
    {

    }

    void room_builder_base::process(size_t samples)
    {

    }

    //-------------------------------------------------------------------------
    room_builder_mono::room_builder_mono(): room_builder_base(metadata, 1)
    {
    }

    room_builder_mono::~room_builder_mono()
    {
    }

    room_builder_stereo::room_builder_stereo(): room_builder_base(metadata, 1)
    {
    }

    room_builder_stereo::~room_builder_stereo()
    {
    }
}

