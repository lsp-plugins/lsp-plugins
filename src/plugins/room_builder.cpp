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
    room_builder_base::SceneLoader::~SceneLoader()
    {
    }


    void room_builder_base::SceneLoader::init(room_builder_base *base)
    {
        pCore   = base;
        sScene.clear();
    }

    void room_builder_base::SceneLoader::destroy()
    {
        sScene.destroy();
    }

    void room_builder_base::SceneLoader::drop_props()
    {
        // Destroy all related memory
        for (size_t i=0, n=vProps.size(); i<n; ++i)
        {
            obj_props_t *props = vProps.get(i);
            if ((props != NULL) && (props->sName != NULL))
                free(props->sName);
        }

        vProps.flush();
    }

    status_t room_builder_base::SceneLoader::run()
    {
        // Clear scene
        sScene.clear();

        // Check state
        if (pCore->p3DFile == NULL)
            return STATUS_UNKNOWN_ERR;

        // Load the scene file
        status_t res = Model3DFile::load(&sScene, sPath, true);
        if (res != STATUS_OK)
            return res;

        // Initialize object properties
        size_t nobjs = sScene.num_objects();

        // Synchronize size of object properties array with scene
        while (vProps.size() > nobjs)
        {
            obj_props_t props;
            if (vProps.pop(&props))
            {
                drop_props();
                return STATUS_CORRUPTED;
            }
            if (props.sName != NULL)
                free(props.sName);
        }
        while (vProps.size() < nobjs)
        {
            obj_props_t *props = vProps.add();
            if (!props)
            {
                drop_props();
                return STATUS_NO_MEM;
            }
            props->sName    = NULL;
        }

        // Now initialize object properties
        for (size_t i=0; i<nobjs; ++i)
        {
            Object3D *obj       = sScene.object(i);
            if (obj == NULL)
                return STATUS_UNKNOWN_ERR;
            obj_props_t *props  = vProps.get(i);
            if (props == NULL)
                return STATUS_UNKNOWN_ERR;

            const char *name    = obj->get_name();
            const point3d_t *c  = obj->center();
            if (props->sName != NULL)
                free(props->sName);

            // Set-up object parameters
            props->sName    = (name != NULL) ? strdup(name) : NULL;
            props->bEnabled = true;
            props->sPos     = *c;
            props->fYaw     = 0.0f;
            props->fPitch   = 0.0f;
            props->fRoll    = 0.0f;
            props->fSizeX   = 1.0f;
            props->fSizeY   = 1.0f;
            props->fSizeZ   = 1.0f;
            props->fHue     = float(i) / float(nobjs);

            // Initialize material as concrete
            props->fAbsorption[0]    = 0.02f;
            props->fDispersion[0]    = 1.0f;
            props->fDissipation[0]   = 1.0f;
            props->fTransparency[0]  = 0.48f;

            props->fAbsorption[1]    = 0.0f;
            props->fDispersion[1]    = 1.0f;
            props->fDissipation[1]   = 1.0f;
            props->fTransparency[1]  = 0.52f;

            props->fSndSpeed         = 12.88f * SOUND_SPEED_M_S;

            // Set-up synchronization flags
            props->nSync    = PS_SYNC_ALL;
        }

        return STATUS_OK;
    }

    //-------------------------------------------------------------------------
    room_builder_base::room_builder_base(const plugin_metadata_t &metadata, size_t inputs):
        plugin_t(metadata)
    {
        nInputs         = inputs;
        nReconfigReq    = 0;
        nReconfigResp   = 0;

        nSceneStatus    = STATUS_UNSPECIFIED;
        fSceneProgress  = 0.0f;

        pOscIn          = NULL;
        pOscOut         = NULL;
        pBypass         = NULL;
        pRank           = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pOutGain        = NULL;
        pPredelay       = NULL;
        p3DFile         = NULL;
        p3DStatus       = NULL;
        p3DProgress     = NULL;
        p3DOrientation  = NULL;

        pData           = NULL;
        pExecutor       = NULL;
        nOscSync        = OSC_SYNC_ALL;
    }

    room_builder_base::~room_builder_base()
    {
    }

    void room_builder_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Remember executor service
        pExecutor       = wrapper->get_executor();
        lsp_trace("Executor = %p", pExecutor);

        // Allocate memory
        size_t tmp_buf_size = TMP_BUF_SIZE * sizeof(float);
        size_t alloc        = tmp_buf_size * (impulse_reverb_base_metadata::CONVOLVERS + 2);
        uint8_t *ptr        = alloc_aligned<uint8_t>(pData, alloc);
        if (pData == NULL)
            return;

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

        // Initialize sources
        for (size_t i=0; i<room_builder_base_metadata::SOURCES; ++i)
        {
            source_t *src   = &vSources[i];

            src->bEnabled       = false;
            src->enType         = RT_AS_TRIANGLE;
            src->bPhase         = false;
            src->nChannel       = 0;
            dsp::init_point_xyz(&src->sPos, 0.0f, -1.0f, 0.0f);
            src->fYaw           = 0.0f;
            src->fPitch         = 0.0f;
            src->fRoll          = 0.0f;
            src->fSize          = 0.0f;
            src->fHeight        = 0.0f;
            src->fAngle         = 0.0f;
            src->fCurvature     = 1.0f;

            src->pEnabled       = NULL;
            src->pType          = NULL;
            src->pChannel       = NULL;
            src->pPhase         = NULL;
            src->pPosX          = NULL;
            src->pPosY          = NULL;
            src->pPosZ          = NULL;
            src->pYaw           = NULL;
            src->pPitch         = NULL;
            src->pRoll          = NULL;
            src->pSize          = NULL;
            src->pHeight        = NULL;
            src->pAngle         = NULL;
            src->pCurvature     = NULL;
        }

        // Initialize captures
        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *cap  = &vCaptures[i];

            cap->bEnabled       = (i == 0);
            cap->nRMin          = 1;
            cap->nRMax          = -1;
            dsp::init_point_xyz(&cap->sPos, 0.0f, 1.0f, 0.0f);
            cap->fYaw           = 0.0f;
            cap->fPitch         = 0.0f;
            cap->fRoll          = 0.0f;
            cap->fCapsule       = room_builder_base_metadata::CAPSULE_DFL;
            cap->sConfig        = RT_CC_XY;
            cap->fAngle         = room_builder_base_metadata::ANGLE_DFL;
            cap->fDistance      = room_builder_base_metadata::DISTANCE_DFL;
            cap->enDirection    = RT_AC_OMNI;
            cap->enSide         = RT_AC_BIDIR;

            cap->pEnabled       = NULL;
            cap->pRMin          = NULL;
            cap->pRMax          = NULL;
            cap->pPosX          = NULL;
            cap->pPosY          = NULL;
            cap->pPosZ          = NULL;
            cap->pYaw           = NULL;
            cap->pPitch         = NULL;
            cap->pRoll          = NULL;
            cap->pCapsule       = NULL;
            cap->pConfig        = NULL;
            cap->pAngle         = NULL;
            cap->pDistance      = NULL;
            cap->pDirection     = NULL;
            cap->pSide          = NULL;
        }

        // Bind ports
        size_t port_id = 0;

        lsp_trace("Binding audio ports");
        for (size_t i=0; i<nInputs; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vInputs[i].pIn      = vPorts[port_id++];
        }
        for (size_t i=0; i<2; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut   = vPorts[port_id++];
        }

        TRACE_PORT(vPorts[port_id]);
        pOscIn          = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOscOut         = vPorts[port_id++];

        // Bind controlling ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);            // Skip view selector
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // Skip editor selector
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // FFT rank
        pRank           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);            // Pre-delay
        pPredelay       = vPorts[port_id++];

        for (size_t i=0; i<nInputs; ++i)        // Panning ports
        {
            TRACE_PORT(vPorts[port_id]);
            vInputs[i].pPan     = vPorts[port_id++];
        }

        TRACE_PORT(vPorts[port_id]);
        pDry            = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWet            = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DFile         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DStatus       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DProgress     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DOrientation  = vPorts[port_id++];

        // Skip camera settings
        TRACE_PORT(vPorts[port_id]);            // Skip camera x
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // Skip camera y
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // Skip camera z
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // Skip camera yaw
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // Skip camera pitch
        port_id++;

        // Bind sources
        TRACE_PORT(vPorts[port_id]);            // Skip source selector
        port_id++;

        for (size_t i=0; i<room_builder_base_metadata::SOURCES; ++i)
        {
            source_t *src   = &vSources[i];

            if (nInputs > 1)
            {
                TRACE_PORT(vPorts[port_id]);
                src->pChannel          = vPorts[port_id++];
            }
            TRACE_PORT(vPorts[port_id]);
            src->pEnabled       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pType          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pPhase         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pPosX          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pPosY          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pPosZ          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pYaw           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pPitch         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pRoll          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pSize          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pHeight        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pAngle         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            src->pCurvature     = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            port_id++;          // Skip hue value
        }

        // Bind captures
        TRACE_PORT(vPorts[port_id]);            // Skip capture selector
        port_id++;

        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *cap  = &vCaptures[i];

            TRACE_PORT(vPorts[port_id]);
            cap->pEnabled       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pRMin          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pRMax          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pPosX          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pPosY          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pPosZ          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pYaw           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pPitch         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pRoll          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pCapsule       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pConfig        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pAngle         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pDistance      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pDirection     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pSide          = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            port_id++;          // Skip hue value
        }
    }

    void room_builder_base::ui_activated()
    {
        nOscSync    = OSC_SYNC_ALL;
        for (size_t i=0, n=vObjectProps.size(); i<n; ++i)
        {
            obj_props_t *prop = vObjectProps.get(i);
            if (prop == NULL)
                continue;
            prop->nSync     = PS_SYNC_ALL;
        }
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

    size_t room_builder_base::get_fft_rank(size_t rank)
    {
        return room_builder_base_metadata::FFT_RANK_MIN + rank;
    }

    void room_builder_base::perform_osc_receive()
    {
        // TODO: process different incoming OSC messages
    }

    void room_builder_base::perform_osc_transmit()
    {
        char path[0x100], *tail;

        // Perform an optimistic sync of state
        if (pOscOut == NULL)
            return;
        osc_buffer_t *b = pOscOut->getBuffer<osc_buffer_t>();
        if (b == NULL)
            return;

        #define TX_SYNC(var, flag, ...) \
                if (var & flag) { \
                    status_t __; \
                    __VA_ARGS__; \
                    if (__ != STATUS_OK) return; \
                    var &= ~flag; \
                }

        // Output number of objects
        TX_SYNC(nOscSync, OSC_OBJECT_COUNT,
            __ = b->submit_int32("/scene/num_objects", vObjectProps.size())
        );

        // Output object states
        if (nOscSync & OSC_OBJECTS)
        {
            // For each object
            for (size_t i=0, n=vObjectProps.size(); i<n; ++i)
            {
                obj_props_t *p = vObjectProps.get(i);
                if (p == NULL)
                    continue;

                // Sync regular parameters?
                if (!p->nSync)
                    continue;

                // Generate object's parameter base address
                int count   = sprintf(path, "/scene/objects/%d/", int(i));
                if (count < 0)
                    return;
                tail        = &path[count];

                // Output object parameters
                TX_SYNC(p->nSync, PS_NAME,
                        strcpy(tail, "name");
                        __ = b->submit_string(path, p->sName);
                    );
                TX_SYNC(p->nSync, PS_ENABLED,
                        strcpy(tail, "enabled");
                        __ = b->submit_bool(path, p->bEnabled);
                    );

                TX_SYNC(p->nSync, PS_POS_X,
                        strcpy(tail, "position/x");
                        __ = b->submit_float32(path, p->sPos.x);
                    );
                TX_SYNC(p->nSync, PS_POS_Y,
                        strcpy(tail, "position/y");
                        __ = b->submit_float32(path, p->sPos.y);
                    );
                TX_SYNC(p->nSync, PS_POS_Z,
                        strcpy(tail, "position/z");
                        __ = b->submit_float32(path, p->sPos.z);
                    );

                TX_SYNC(p->nSync, PS_YAW,
                        strcpy(tail, "rotation/yaw");
                        __ = b->submit_float32(path, p->fYaw);
                    );
                TX_SYNC(p->nSync, PS_PITCH,
                        strcpy(tail, "rotation/pitch");
                        __ = b->submit_float32(path, p->fPitch);
                    );
                TX_SYNC(p->nSync, PS_ROLL,
                        strcpy(tail, "rotation/roll");
                        __ = b->submit_float32(path, p->fRoll);
                    );

                TX_SYNC(p->nSync, PS_SIZE_X,
                        strcpy(tail, "scale/x");
                        __ = b->submit_float32(path, p->fSizeX);
                    );
                TX_SYNC(p->nSync, PS_SIZE_Y,
                        strcpy(tail, "scale/y");
                        __ = b->submit_float32(path, p->fSizeY);
                    );
                TX_SYNC(p->nSync, PS_SIZE_Z,
                        strcpy(tail, "scale/z");
                        __ = b->submit_float32(path, p->fSizeZ);
                    );

                TX_SYNC(p->nSync, PS_HUE,
                        strcpy(tail, "color/hue");
                        __ = b->submit_float32(path, p->fHue);
                    );

                // Generate base address
                count   = sprintf(path, "/scene/objects/%d/material/", int(i));
                if (count < 0)
                    return;
                tail        = &path[count];

                // Updated material
                // Output parameters
                TX_SYNC(p->nSync, PS_ABSORPTION_0,
                        strcpy(tail, "absorption/outer");
                        __ = b->submit_float32(path, p->fAbsorption[0]);
                    );
                TX_SYNC(p->nSync, PS_ABSORPTION_1,
                        strcpy(tail, "absorption/inner");
                        __ = b->submit_float32(path, p->fAbsorption[1]);
                    );
                TX_SYNC(p->nSync, PS_DISPERSION_0,
                        strcpy(tail, "dispersion/outer");
                        __ = b->submit_float32(path, p->fDispersion[0]);
                    );
                TX_SYNC(p->nSync, PS_DISPERSION_1,
                        strcpy(tail, "dispersion/inner");
                        __ = b->submit_float32(path, p->fDispersion[1]);
                    );
                TX_SYNC(p->nSync, PS_DISSIPATION_0,
                        strcpy(tail, "dissipation/outer");
                        __ = b->submit_float32(path, p->fDissipation[0]);
                    );
                TX_SYNC(p->nSync, PS_DISSIPATION_1,
                        strcpy(tail, "dissipation/inner");
                        __ = b->submit_float32(path, p->fDissipation[1]);
                    );
                TX_SYNC(p->nSync, PS_TRANSPARENCY_0,
                        strcpy(tail, "transparency/outer");
                        __ = b->submit_float32(path, p->fTransparency[0]);
                    );
                TX_SYNC(p->nSync, PS_TRANSPARENCY_1,
                        strcpy(tail, "transparency/inner");
                        __ = b->submit_float32(path, p->fTransparency[1]);
                    );
                TX_SYNC(p->nSync, PS_SOUND_SPEED,
                        strcpy(tail, "sound_speed");
                        __ = b->submit_float32(path, p->fSndSpeed);
                    );
            } // for

            // Reset sync flag
            nOscSync &= ~OSC_OBJECTS;
        }

        #undef TX_ASSERT
    }

    void room_builder_base::update_settings()
    {
        float out_gain      = pOutGain->getValue();
        float dry_gain      = pDry->getValue() * out_gain;
        float wet_gain      = pWet->getValue() * out_gain;
        bool bypass         = pBypass->getValue() >= 0.5f;
        float predelay      = pPredelay->getValue();
        size_t rank         = get_fft_rank(pRank->getValue());

        // Adjust volume of dry channel
        if (nInputs == 1)
        {
            float pan               = vInputs[0].pPan->getValue();
            vChannels[0].fDryPan[0] = (100.0f - pan) * 0.005f * dry_gain;
            vChannels[0].fDryPan[1] = 0.0f;
            vChannels[1].fDryPan[0] = (100.0f + pan) * 0.005f * dry_gain;
            vChannels[1].fDryPan[1] = 0.0f;
        }
        else
        {
            float pan_l             = vInputs[0].pPan->getValue();
            float pan_r             = vInputs[1].pPan->getValue();

            vChannels[0].fDryPan[0] = (100.0f - pan_l) * 0.005f * dry_gain;
            vChannels[0].fDryPan[1] = (100.0f - pan_r) * 0.005f * dry_gain;
            vChannels[1].fDryPan[0] = (100.0f + pan_l) * 0.005f * dry_gain;
            vChannels[1].fDryPan[1] = (100.0f + pan_r) * 0.005f * dry_gain;
        }

        // Update source settings
        for (size_t i=0; i<room_builder_base_metadata::SOURCES; ++i)
        {
            source_t *src       = &vSources[i];
            src->bEnabled       = src->pEnabled->getValue() >= 0.5f;
            src->enType         = decode_source_type(src->pType->getValue());
            src->bPhase         = src->pPhase->getValue() >= 0.5f;
            src->nChannel       = (src->pChannel != NULL) ? src->pChannel->getValue() : 0;
            src->sPos.x         = src->pPosX->getValue();
            src->sPos.y         = src->pPosY->getValue();
            src->sPos.z         = src->pPosZ->getValue();
            src->sPos.w         = 1.0f;
            src->fYaw           = src->pYaw->getValue();
            src->fPitch         = src->pPitch->getValue();
            src->fRoll          = src->pRoll->getValue();
            src->fSize          = src->pSize->getValue() * 0.01f;   // cm -> m
            src->fHeight        = src->pHeight->getValue() * 0.01f; // cm -> m
            src->fAngle         = src->pAngle->getValue();
            src->fCurvature     = src->pCurvature->getValue();
        }

        // Update capture settings
        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *cap  = &vCaptures[i];

            cap->bEnabled       = cap->pEnabled->getValue() >= 0.5f;
            cap->nRMin          = ssize_t(cap->pRMin->getValue()) - 1;
            cap->nRMax          = ssize_t(cap->pRMax->getValue()) - 1;
            cap->sPos.x         = cap->pPosX->getValue();
            cap->sPos.y         = cap->pPosY->getValue();
            cap->sPos.z         = cap->pPosZ->getValue();
            cap->sPos.w         = 1.0f;
            cap->fYaw           = cap->pYaw->getValue();
            cap->fPitch         = cap->pPitch->getValue();
            cap->fRoll          = cap->pRoll->getValue();
            cap->fCapsule       = cap->pCapsule->getValue() * 0.5f;
            cap->sConfig        = decode_config(cap->pConfig->getValue());
            cap->fAngle         = cap->pAngle->getValue();
            cap->fDistance      = cap->pDistance->getValue();
            cap->enDirection    = decode_direction(cap->pDirection->getValue());
            cap->enSide         = decode_side_direction(cap->pSide->getValue());
        }
    }

    rt_audio_source_t room_builder_base::decode_source_type(float value)
    {
        switch (ssize_t(value))
        {
            case 1:     return RT_AS_TETRA;
            case 2:     return RT_AS_OCTA;
            case 3:     return RT_AS_BOX;
            case 4:     return RT_AS_ICO;
            case 5:     return RT_AS_CYLINDER;
            case 6:     return RT_AS_CONE;
            case 7:     return RT_AS_OCTASPHERE;
            case 8:     return RT_AS_ICOSPHERE;
            case 9:     return RT_AS_FSPOT;
            case 10:    return RT_AS_CSPOT;
            case 11:    return RT_AS_SSPOT;
            default:    break;
        }
        return RT_AS_TRIANGLE;
    }

    rt_capture_config_t room_builder_base::decode_config(float value)
    {
        switch (ssize_t(value))
        {
            case 1:     return RT_CC_XY;
            case 2:     return RT_CC_AB;
            case 3:     return RT_CC_ORTF;
            case 4:     return RT_CC_MS;
            default:    break;
        }
        return RT_CC_MONO;
    }

    rt_audio_capture_t room_builder_base::decode_direction(float value)
    {
        switch (ssize_t(value))
        {
            case 1:     return RT_AC_SCARDIO; break;
            case 2:     return RT_AC_HCARDIO; break;
            case 3:     return RT_AC_BIDIR; break;
            case 4:     return RT_AC_EIGHT; break;
            case 5:     return RT_AC_OMNI; break;
            default:    break;
        }
        return RT_AC_CARDIO;
    }

    rt_audio_capture_t room_builder_base::decode_side_direction(float value)
    {
        switch (ssize_t(value))
        {
            case 1: return RT_AC_EIGHT;
            default: break;
        }
        return RT_AC_BIDIR;
    }

    void room_builder_base::update_sample_rate(long sr)
    {
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
            vConvolvers[i].sDelay.init(millis_to_samples(sr, impulse_reverb_base_metadata::PREDELAY_MAX * 4.0f));

        for (size_t i=0; i<2; ++i)
        {
            vChannels[i].sBypass.init(sr);
            vChannels[i].sEqualizer.set_sample_rate(sr);
        }
    }

    void room_builder_base::process(size_t samples)
    {
        // Stage 1: process incoming OSC events
        perform_osc_receive();

        // Stage 2: process reconfiguration requests and file events
        sync_offline_tasks();

        // Stage 3: generate outgoing OSC events
        perform_osc_transmit();

        // Stage 4: output additional metering parameters
        if (p3DStatus != NULL)
            p3DStatus->setValue(nSceneStatus);
        if (p3DProgress != NULL)
            p3DProgress->setValue(fSceneProgress);
    }

    void room_builder_base::sync_offline_tasks()
    {
        // Check the state of input file
        path_t *path        = p3DFile->getBuffer<path_t>();
        if (path != NULL)
        {
            if ((path->pending()) && (s3DLoader.idle())) // There is pending request for 3D file reload
            {
                // Copy path
                strncpy(s3DLoader.sPath, path->get_path(), PATH_MAX);

                // Try to submit task
                if (pExecutor->submit(&s3DLoader))
                {
                    lsp_trace("successfully submitted load task");
                    nSceneStatus    = STATUS_LOADING;
                    fSceneProgress  = 0.0f;
                    path->accept();
                }
            }
            else if ((path->accepted()) && (s3DLoader.completed())) // The reload request has been processed
            {
                // Update file status and set re-rendering flag
                nSceneStatus    = s3DLoader.code();
                fSceneProgress  = 100.0f;

                sScene.swap(&s3DLoader.sScene);
                vObjectProps.swap(&s3DLoader.vProps);
                nOscSync        = OSC_SYNC_ALL;
                nReconfigReq    ++;

                // Now we surely can commit changes and reset task state
                lsp_trace("File loading task has completed with status %d", int(nSceneStatus));
                path->commit();
                s3DLoader.reset();
            }
        }
    }

    status_t room_builder_base::configure_capture(room_capture_settings_t *out, const room_capture_config_t *in)
    {
        matrix3d_t m, delta;
        float a[2];
        vector3d_t dp[2];

//        // Compute parameters of capture in point(0, 0, 0)
//        for (size_t i=0; i<2; ++i)
//        {
//            dsp::init_point_xyz(&out->pos[i].z, 0.0f, 0.0f, 0.0f);
//            dsp::init_vector_dxyz(&out->pos[i].v, 1.0f, 0.0f, 0.0f);
//        }
        dsp::init_vector_dxyz(&dp[0], 0.0f, 0.0f, 0.0f);
        dsp::init_vector_dxyz(&dp[1], 0.0f, 0.0f, 0.0f);

        out->r[0]       = in->fCapsule * 0.01f; // cm -> m
        out->r[1]       = in->fCapsule * 0.01f; // cm -> m
        out->type[0]    = in->enDirection;
        out->type[1]    = in->enDirection;

        switch (in->sConfig)
        {
            case RT_CC_MONO:
                out->n              = 1;
                a[0]                = 0.0f;
                a[1]                = 0.0f;
                break;
            case RT_CC_XY:
                out->n              = 2;
                dp[0].dy           += out->r[0];
                dp[1].dy           -= out->r[1];
                a[0]                = -45.0f - (in->fAngle - 90.0f) * 0.5f;
                a[1]                = 45.0f + (in->fAngle - 90.0f) * 0.5f;
                break;
            case RT_CC_AB:
                out->n              = 2;
                dp[0].dy           += in->fDistance * 0.5f;
                dp[1].dy           -= in->fDistance * 0.5f;
                a[0]                = 0.0f;
                a[1]                = 0.0f;
                break;
            case RT_CC_ORTF:
                out->n              = 2;
                dp[0].dy           += 0.075f;  // Half of human's head width
                dp[1].dy           -= 0.075f;  // Half of human's head width
                a[0]                =  45.0f + (in->fAngle - 90.0f) * 0.5f;  // -45 + (a - 90) * 45 / 90
                a[1]                = -45.0f - (in->fAngle - 90.0f) * 0.5f; // -45 - (a - 90) * 45 / 90
                break;
            case RT_CC_MS:
                out->n              = 2;
                dp[0].dz           += out->r[0];
                dp[1].dz           -= out->r[1];
                out->type[1]        = in->enSide;
                a[0]                = 0.0f;
                a[1]                = 90.0f;
                break;

            default:
                return STATUS_BAD_ARGUMENTS;
        }

        // Compute rotation matrix
        dsp::init_matrix3d_translate(&delta, in->sPos.x, in->sPos.y, in->sPos.z);

        dsp::init_matrix3d_rotate_z(&m, in->fYaw * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        dsp::init_matrix3d_rotate_y(&m, in->fPitch * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        dsp::init_matrix3d_rotate_x(&m, in->fRoll * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        // Compute initial matrices
        for (size_t i=0; i<2; ++i)
        {
            out->pos[i] = delta;

            dsp::init_matrix3d_translate(&m, dp[i].dx, dp[i].dy, dp[i].dz);
            dsp::apply_matrix3d_mm1(&out->pos[i], &m);

            dsp::init_matrix3d_rotate_z(&m, a[i] * M_PI / 180.0f);
            dsp::apply_matrix3d_mm1(&out->pos[i], &m);
        }

        return STATUS_OK;
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

