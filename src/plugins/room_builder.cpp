/*
 * room_builder.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <core/files/Model3DFile.h>

#include <plugins/room_builder.h>
#include <dsp/endian.h>
#include <core/fade.h>
#include <core/stdlib/math.h>
#include <core/files/lspc/LSPCAudioWriter.h>
#include <core/files/AudioFile.h>

#define TMP_BUF_SIZE            4096
#define CONV_RANK               10
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    static const float band_freqs[] =
    {
        73.0f,
        156.0f,
        332.0f,
        707.0f,
        1507.0f,
        3213.0f,
        6849.0f
    };

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

    status_t room_builder_base::RenderLauncher::run()
    {
        return pBuilder->start_rendering();
    }

    template <class T>
        static bool kvt_deploy(KVTStorage *s, const char *base, const char *branch, T value, size_t flags)
        {
            char name[0x100]; // Should be enough
            size_t len = ::strlen(base) + ::strlen(branch) + 2;
            if (len >= 0x100)
                return false;

            char *tail = ::stpcpy(name, base);
            *(tail++)  = '/';
            stpcpy(tail, branch);

            return s->put(name, value, flags) == STATUS_OK;
        }

    status_t room_builder_base::SceneLoader::run()
    {
        // Clear scene
        sScene.clear();

        // Check state
        size_t nobjs = 0;
        status_t res = STATUS_UNSPECIFIED;

        // Load the scene file
        if (pCore->p3DFile == NULL)
            res = STATUS_UNKNOWN_ERR;
        else if (::strlen(sPath) > 0)
        {
            res = Model3DFile::load(&sScene, sPath, true);
            if (res == STATUS_OK)
            {
                // Initialize object properties
                nobjs = sScene.num_objects();
            }
        }

        // Get KVT storage and deploy new values
        KVTStorage *kvt = pCore->kvt_lock();
        if (kvt == NULL)
            return STATUS_UNKNOWN_ERR;

        // Now initialize object properties
        lsp_trace("Extra loading flags=0x%x", int(nFlags));
        size_t f_extra  = (nFlags & (PF_STATE_IMPORT | PF_PRESET_IMPORT | PF_STATE_RESTORE)) ? KVT_KEEP | KVT_TX : KVT_TX;
        size_t f_hue    = (nFlags & (PF_STATE_IMPORT | PF_STATE_RESTORE)) ? KVT_KEEP | KVT_TX : KVT_TX;

        char base[128];
        kvt_deploy(kvt, "/scene", "objects", int32_t(nobjs), KVT_TX);
        kvt_deploy(kvt, "/scene", "selected", 0.0f, f_extra);

        for (size_t i=0; i<nobjs; ++i)
        {
            Object3D *obj       = sScene.object(i);
            if (obj == NULL)
                return STATUS_UNKNOWN_ERR;
            const point3d_t *c  = obj->center();

            sprintf(base, "/scene/object/%d", int(i));
            lsp_trace("Deploying KVT parameters for %s", base);

            kvt_deploy(kvt, base, "name", obj->get_name(), KVT_TX); // Always overwrite name

            kvt_deploy(kvt, base, "enabled", 1.0f, f_extra);
            kvt_deploy(kvt, base, "center/x", c->x, KVT_TX | KVT_TRANSIENT); // Always overwrite, do not save in state
            kvt_deploy(kvt, base, "center/y", c->y, KVT_TX | KVT_TRANSIENT); // Always overwrite, do not save in state
            kvt_deploy(kvt, base, "center/z", c->z, KVT_TX | KVT_TRANSIENT); // Always overwrite, do not save in state
            kvt_deploy(kvt, base, "position/x", 0.0f, f_extra);
            kvt_deploy(kvt, base, "position/y", 0.0f, f_extra);
            kvt_deploy(kvt, base, "position/z", 0.0f, f_extra);
            kvt_deploy(kvt, base, "rotation/yaw", 0.0f, f_extra);
            kvt_deploy(kvt, base, "rotation/pitch", 0.0f, f_extra);
            kvt_deploy(kvt, base, "rotation/roll", 0.0f, f_extra);
            kvt_deploy(kvt, base, "scale/x", 100.0f, f_extra);
            kvt_deploy(kvt, base, "scale/y", 100.0f, f_extra);
            kvt_deploy(kvt, base, "scale/z", 100.0f, f_extra);
            kvt_deploy(kvt, base, "color/hue", float(i) / float(nobjs), f_hue); // Always overwrite hue

            kvt_deploy(kvt, base, "material/absorption/outer", 1.5f, f_extra); // Absorption of concrete material
            kvt_deploy(kvt, base, "material/dispersion/outer", 1.0f, f_extra);
            kvt_deploy(kvt, base, "material/diffusion/outer", 1.0f, f_extra);
            kvt_deploy(kvt, base, "material/transparency/outer", 48.0f, f_extra);

            kvt_deploy(kvt, base, "material/absorption/inner", 1.5f, f_extra);
            kvt_deploy(kvt, base, "material/dispersion/inner", 1.0f, f_extra);
            kvt_deploy(kvt, base, "material/diffusion/inner", 1.0f, f_extra);
            kvt_deploy(kvt, base, "material/transparency/inner", 52.0f, f_extra);

            kvt_deploy(kvt, base, "material/absorption/link", 1.0f, f_extra);
            kvt_deploy(kvt, base, "material/dispersion/link", 1.0f, f_extra);
            kvt_deploy(kvt, base, "material/diffusion/link", 1.0f, f_extra);
            kvt_deploy(kvt, base, "material/transparency/link", 1.0f, f_extra);

            kvt_deploy(kvt, base, "material/sound_speed", 4250.0f, f_extra);  // Sound speed in concrete material
        }

        // Drop rare (unused) objects
        kvt_cleanup_objects(kvt, nobjs);

        pCore->kvt_release();

        return res;
    }

    void room_builder_base::kvt_cleanup_objects(KVTStorage *kvt, size_t objects)
    {
        KVTIterator *it = kvt->enum_branch("/scene/object");
        while (it->next() == STATUS_OK)
        {
            const char *id = it->id();
            if (id == NULL)
                continue;

            // Must be a pure object identifier
            errno = 0;
            char *endptr;
            long value = ::strtol(id, &endptr, 10);
            if ((errno != 0) || (size_t(endptr - id) != size_t(::strlen(id))))
                continue;

            // Remove the object
            if ((value < 0) || (value >= ssize_t(objects)))
            {
                lsp_trace("Removing KVT parameters from %s", it->name());
                it->remove_branch();
            }
        }
    }

    template <class T>
        static bool kvt_fetch(KVTStorage *s, const char *base, const char *branch, T *value, T dfl)
        {
            char name[0x100]; // Should be enough;
            size_t len = ::strlen(base) + ::strlen(branch) + 2;
            if (len >= 0x100)
                return false;

            char *tail = ::stpcpy(name, base);
            *(tail++)  = '/';
            stpcpy(tail, branch);

            return s->get_dfl(name, value, dfl);
        }

    void room_builder_base::read_object_properties(obj_props_t *props, const char *base, KVTStorage *kvt)
    {
        float enabled;

        kvt_fetch(kvt, base, "name", &props->sName, "unnamed");
        kvt_fetch(kvt, base, "enabled", &enabled, 1.0f);
        kvt_fetch(kvt, base, "center/x", &props->sCenter.x, 0.0f);
        kvt_fetch(kvt, base, "center/y", &props->sCenter.y, 0.0f);
        kvt_fetch(kvt, base, "center/z", &props->sCenter.z, 0.0f);
        kvt_fetch(kvt, base, "position/x", &props->sMove.dx, 0.0f);
        kvt_fetch(kvt, base, "position/y", &props->sMove.dy, 0.0f);
        kvt_fetch(kvt, base, "position/z", &props->sMove.dz, 0.0f);
        kvt_fetch(kvt, base, "rotation/yaw", &props->fYaw, 0.0f);
        kvt_fetch(kvt, base, "rotation/pitch", &props->fPitch, 0.0f);
        kvt_fetch(kvt, base, "rotation/roll", &props->fRoll, 0.0f);
        kvt_fetch(kvt, base, "scale/x", &props->sScale.dx, 1.0f);
        kvt_fetch(kvt, base, "scale/y", &props->sScale.dy, 1.0f);
        kvt_fetch(kvt, base, "scale/z", &props->sScale.dz, 1.0f);
        kvt_fetch(kvt, base, "color/hue", &props->fHue, 0.0f);

        kvt_fetch(kvt, base, "material/absorption/outer", &props->fAbsorption[0], 1.5f);
        kvt_fetch(kvt, base, "material/dispersion/outer", &props->fDispersion[0], 1.0f);
        kvt_fetch(kvt, base, "material/dissipation/outer", &props->fDiffusion[0], 1.0f);
        kvt_fetch(kvt, base, "material/transparency/outer", &props->fTransparency[0], 48.0f);

        kvt_fetch(kvt, base, "material/absorption/inner", &props->fAbsorption[1], 1.5f);
        kvt_fetch(kvt, base, "material/dispersion/inner", &props->fDispersion[1], 1.0f);
        kvt_fetch(kvt, base, "material/diffusion/inner", &props->fDiffusion[1], 1.0f);
        kvt_fetch(kvt, base, "material/transparency/inner", &props->fTransparency[1], 52.0f);

        kvt_fetch(kvt, base, "material/absorption/link", &props->lnkAbsorption, 1.0f);
        kvt_fetch(kvt, base, "material/dispersion/link", &props->lnkDispersion, 1.0f);
        kvt_fetch(kvt, base, "material/diffusion/link", &props->lnkDiffusion, 1.0f);
        kvt_fetch(kvt, base, "material/transparency/link", &props->lnkTransparency, 1.0f);

        kvt_fetch(kvt, base, "material/sound_speed", &props->fSndSpeed, 4250.0f);

        props->bEnabled = (enabled >= 0.5f);
    }

    void room_builder_base::build_object_matrix(matrix3d_t *m, const obj_props_t *props, const matrix3d_t *world)
    {
        matrix3d_t tmp;

        // Copy world matrix
        *m  = *world;

        // Apply translation
        dsp::init_matrix3d_translate(&tmp,
                props->sCenter.x + props->sMove.dx,
                props->sCenter.y + props->sMove.dy,
                props->sCenter.z + props->sMove.dz
        );
        dsp::apply_matrix3d_mm1(m, &tmp);

        // Apply rotation
        dsp::init_matrix3d_rotate_z(&tmp, props->fYaw * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(m, &tmp);

        dsp::init_matrix3d_rotate_y(&tmp, props->fPitch * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(m, &tmp);

        dsp::init_matrix3d_rotate_x(&tmp, props->fRoll * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(m, &tmp);

        // Apply scale
        dsp::init_matrix3d_scale(&tmp, props->sScale.dx * 0.01f, props->sScale.dy * 0.01f, props->sScale.dz * 0.01f);
        dsp::apply_matrix3d_mm1(m, &tmp);

        // Move center to (0, 0, 0) point
        dsp::init_matrix3d_translate(&tmp, -props->sCenter.x, -props->sCenter.y, -props->sCenter.z);
        dsp::apply_matrix3d_mm1(m, &tmp);
    }

    status_t room_builder_base::Renderer::run()
    {
        // Perform processing
        lsp_trace("Launching process() method");
        pBuilder->enRenderStatus    = STATUS_IN_PROCESS;
        status_t res    = pRT->process(nThreads, 1.0f);

        // Deploy success result
        if (res == STATUS_OK)
            res = pBuilder->commit_samples(vSamples);

        // Free all resources
        if (lkTerminate.lock())
        {
            pRT->destroy(true);
            delete pRT;
            pRT = NULL;
            lkTerminate.unlock();
        }

        room_builder_base::destroy_samples(vSamples);

        return pBuilder->enRenderStatus = res;
    }

    void room_builder_base::Renderer::terminate()
    {
        if (lkTerminate.lock())
        {
            if (pRT != NULL)
                pRT->cancel();
            lkTerminate.unlock();
        }
    }

    status_t room_builder_base::Configurator::run()
    {
        return pBuilder->reconfigure(&sConfig);
    }

    void room_builder_base::SampleSaver::bind(size_t sample_id, capture_t *capture)
    {
        nSampleID   = sample_id;
        IPort *p    = capture->pOutFile;
        if (p == NULL)
            return;
        path_t *path = p->getBuffer<path_t>();
        if (path == NULL)
            return;
        const char *spath = path->get_path();
        if (spath != NULL)
        {
            ::strncpy(sPath, spath, PATH_MAX);
            sPath[PATH_MAX] = '\0';
        }
        else
            sPath[0] = '\0';
    }

    status_t room_builder_base::SampleSaver::run()
    {
        return pBuilder->save_sample(sPath, nSampleID);
    }

    //-------------------------------------------------------------------------
    room_builder_base::room_builder_base(const plugin_metadata_t &metadata, size_t inputs):
        plugin_t(metadata),
        s3DLauncher(this),
        sConfigurator(this),
        sSaver(this)
    {
        nInputs         = inputs;
        nReconfigReq    = 0;
        nReconfigResp   = 0;

        nRenderThreads  = 0;
        fRenderQuality  = 0.5f;
        bRenderNormalize= true;
        enRenderStatus  = STATUS_OK;
        fRenderProgress = 0.0f;
        fRenderCmd      = 0.0f;
        nFftRank        = 0;

        nSceneStatus    = STATUS_UNSPECIFIED;
        fSceneProgress  = 0.0f;
        nSync           = 0;

        pBypass         = NULL;
        pRank           = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pRenderThreads  = NULL;
        pRenderQuality  = NULL;
        pRenderStatus   = NULL;
        pRenderProgress = NULL;
        pRenderNormalize= NULL;
        pRenderCmd      = NULL;
        pOutGain        = NULL;
        pPredelay       = NULL;
        p3DFile         = NULL;
        p3DProgress     = NULL;
        p3DStatus       = NULL;
        p3DOrientation  = NULL;
        pScaleX         = NULL;
        pScaleY         = NULL;
        pScaleZ         = NULL;
        pRenderer       = NULL;

        pData           = NULL;
        pExecutor       = NULL;

        dsp::init_vector_dxyz(&sScale, 1.0f, 1.0f, 1.0f);
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
        size_t thumb_size   = room_builder_base_metadata::MESH_SIZE *
                              room_builder_base_metadata::TRACKS_MAX * sizeof(float);
        size_t alloc        = tmp_buf_size * (room_builder_base_metadata::CONVOLVERS + 2) +
                              thumb_size * room_builder_base_metadata::CAPTURES;
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

            if (!c->sPlayer.init(room_builder_base_metadata::CAPTURES, 32))
                return;
            if (!c->sEqualizer.init(room_builder_base_metadata::EQ_BANDS + 2, CONV_RANK))
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

            for (size_t j=0; j<room_builder_base_metadata::EQ_BANDS; ++j)
                c->pFreqGain[j]     = NULL;
        }

        // Initialize sources
        for (size_t i=0; i<room_builder_base_metadata::SOURCES; ++i)
        {
            source_t *src   = &vSources[i];

            src->bEnabled       = false;
            src->enType         = RT_AS_TRIANGLE;
            dsp::init_point_xyz(&src->sPos, 0.0f, -1.0f, 0.0f);
            src->fYaw           = 0.0f;
            src->fPitch         = 0.0f;
            src->fRoll          = 0.0f;
            src->fSize          = 0.0f;
            src->fHeight        = 0.0f;
            src->fAngle         = 0.0f;
            src->fCurvature     = 1.0f;
            src->fAmplitude     = 1.0f;

            src->pEnabled       = NULL;
            src->pType          = NULL;
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

            cap->bEnabled       = (i == 0);
            cap->nRMin          = 1;
            cap->nRMax          = -1;

            cap->fHeadCut       = 0.0f;
            cap->fTailCut       = 0.0f;
            cap->fFadeIn        = 0.0f;
            cap->fFadeOut       = 0.0f;
            cap->bReverse       = false;
            cap->fMakeup        = 1.0f;
            cap->nLength        = 0;
            cap->nStatus        = STATUS_NO_DATA;
            cap->fCurrLen       = 0.0f;
            cap->fMaxLen        = 0.0f;

            cap->nChangeReq     = 0;
            cap->nChangeResp    = 0;
            cap->bCommit        = false;
            cap->bSync          = false;
            cap->bExport        = false;

            cap->pCurr          = NULL;
            cap->pSwap          = NULL;

            for (size_t j=0; j<room_builder_base_metadata::TRACKS_MAX; ++j)
            {
                cap->vThumbs[j]     = reinterpret_cast<float *>(ptr);
                ptr                += room_builder_base_metadata::MESH_SIZE * sizeof(float);
            }

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

            cap->pHeadCut       = NULL;
            cap->pTailCut       = NULL;
            cap->pFadeIn        = NULL;
            cap->pFadeOut       = NULL;
            cap->pListen        = NULL;
            cap->pReverse       = NULL;
            cap->pMakeup        = NULL;
            cap->pStatus        = NULL;
            cap->pLength        = NULL;
            cap->pCurrLen       = NULL;
            cap->pMaxLen        = NULL;
            cap->pThumbs        = NULL;

            cap->pOutFile       = NULL;
            cap->pSaveCmd       = NULL;
            cap->pSaveStatus    = NULL;
            cap->pSaveProgress  = NULL;
        }

        // Initialize convolvers
        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
        {
            lsp_trace("Binding convolution #%d ports", int(i));
            convolver_t *c  = &vConvolvers[i];

            c->pCurr            = NULL;
            c->pSwap            = NULL;

            c->nSampleID        = 0;
            c->nTrackID         = 0;

            c->vBuffer          = reinterpret_cast<float *>(ptr);
            ptr                += tmp_buf_size;

            c->fPanIn[0]        = 0.0f;
            c->fPanIn[1]        = 0.0f;
            c->fPanOut[0]       = 0.0f;
            c->fPanOut[1]       = 0.0f;

            c->pMakeup          = NULL;
            c->pPanIn           = NULL;
            c->pPanOut          = NULL;
            c->pSample          = NULL;
            c->pTrack           = NULL;
            c->pPredelay        = NULL;
            c->pMute            = NULL;
            c->pActivity        = NULL;
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

        // Bind controlling ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);            // Skip view selector
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // Skip editor selector
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // Skip processor selector
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
        pRenderThreads  = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRenderQuality  = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRenderStatus   = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRenderProgress = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRenderNormalize= vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRenderCmd      = vPorts[port_id++];

        TRACE_PORT(vPorts[port_id]);
        p3DFile         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DStatus       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DProgress     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DOrientation  = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pScaleX         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pScaleY         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pScaleZ         = vPorts[port_id++];

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
            cap->pHeadCut       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pTailCut       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pFadeIn        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pFadeOut       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pListen        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pReverse       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pMakeup        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pStatus        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pLength        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pCurrLen       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pMaxLen        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pThumbs        = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            cap->pOutFile       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pSaveCmd       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pSaveStatus    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            cap->pSaveProgress  = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            port_id++;          // Skip hue value
        }

        // Bind convolver ports
        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
        {
            lsp_trace("Binding convolution #%d ports", int(i));
            convolver_t *c  = &vConvolvers[i];

            if (nInputs > 1)    // Input panning
            {
                TRACE_PORT(vPorts[port_id]);
                c->pPanIn       = vPorts[port_id++];
            }

            TRACE_PORT(vPorts[port_id]);
            c->pSample      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pTrack       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMakeup      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMute        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pActivity    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pPredelay    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pPanOut      = vPorts[port_id++];
        }

        // Bind wet processing ports
        lsp_trace("Binding wet processing ports");
        size_t port         = port_id;
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c        = &vChannels[i];

            TRACE_PORT(vPorts[port_id]);
            c->pWetEq           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pLowCut          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pLowFreq         = vPorts[port_id++];

            for (size_t j=0; j<room_builder_base_metadata::EQ_BANDS; ++j)
            {
                TRACE_PORT(vPorts[port_id]);
                c->pFreqGain[j]     = vPorts[port_id++];
            }

            TRACE_PORT(vPorts[port_id]);
            c->pHighCut         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pHighFreq        = vPorts[port_id++];

            port_id         = port;
        }
    }

    void room_builder_base::destroy()
    {
        // Stop active rendering task
        if (pRenderer != NULL)
        {
            pRenderer->terminate();
            pRenderer->join();
            delete pRenderer;
            pRenderer = NULL;
        }

        sScene.destroy();
        s3DLoader.destroy();

        if (pData != NULL)
        {
            free_aligned(pData);
            pData       = NULL;
        }

        // Destroy captures
        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *c    = &vCaptures[i];
            if (c->pCurr != NULL)
            {
                c->pCurr->destroy();
                delete c->pCurr;
                c->pCurr        = NULL;
            }
            if (c->pSwap != NULL)
            {
                c->pSwap->destroy();
                delete c->pSwap;
                c->pSwap        = NULL;
            }
        }

        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *c  = &vConvolvers[i];
            if (c->pCurr != NULL)
            {
                c->pCurr->destroy();
                delete c->pCurr;
                c->pCurr    = NULL;
            }
            if (c->pSwap != NULL)
            {
                c->pSwap->destroy();
                delete c->pSwap;
                c->pSwap    = NULL;
            }

            c->sDelay.destroy();
        }

        // Destroy channels
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c = &vChannels[i];
            c->sEqualizer.destroy();
            c->sPlayer.destroy(false);
            c->vOut     = NULL;
            c->vBuffer  = NULL;
        }
    }

    size_t room_builder_base::get_fft_rank(size_t rank)
    {
        return room_builder_base_metadata::FFT_RANK_MIN + rank;
    }

    void room_builder_base::update_settings()
    {
        float out_gain      = pOutGain->getValue();
        float dry_gain      = pDry->getValue() * out_gain;
        float wet_gain      = pWet->getValue() * out_gain;
        bool bypass         = pBypass->getValue() >= 0.5f;
        float predelay      = pPredelay->getValue();
        size_t rank         = get_fft_rank(pRank->getValue());

        // Adjust FFT rank
        if (rank != nFftRank)
        {
            nFftRank            = rank;
            sConfigurator.queue_launch();
        }

        // Adjust size of scene and number of threads to render
        sScale.dx           = pScaleX->getValue() * 0.01f;
        sScale.dy           = pScaleY->getValue() * 0.01f;
        sScale.dz           = pScaleZ->getValue() * 0.01f;
        nRenderThreads      = pRenderThreads->getValue();
        bRenderNormalize    = pRenderNormalize->getValue() >= 0.5f;
        fRenderQuality      = pRenderQuality->getValue() * 0.01f;

        // Check that render request has been triggered
        float old_cmd       = fRenderCmd;
        fRenderCmd          = pRenderCmd->getValue();
        if ((old_cmd >= 0.5f) && (fRenderCmd < 0.5f))
        {
            lsp_trace("Triggered render request");
            nSync              |= SYNC_TOGGLE_RENDER;
        }

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
            src->fAmplitude     = (src->pPhase->getValue() >= 0.5f) ? -1.0f : 1.0f;
        }

        // Update capture settings
        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *cap      = &vCaptures[i];

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
            cap->fMakeup        = cap->pMakeup->getValue();

            // Accept changes
            path_t *path        = cap->pOutFile->getBuffer<path_t>();
            if ((path != NULL) && (path->pending()))
            {
                path->accept();
                path->commit();
            }
            if (cap->pSaveCmd->getValue() >= 0.5f) // Toggle the export flag
                cap->bExport        = true;

            // Check that we need to synchronize capture settings with convolver
            float hcut      = cap->pHeadCut->getValue();
            float tcut      = cap->pTailCut->getValue();
            float fadein    = cap->pFadeIn->getValue();
            float fadeout   = cap->pFadeOut->getValue();
            bool  reverse   = cap->pReverse->getValue() >= 0.5f;

            if ((cap->fHeadCut != hcut) ||
                (cap->fTailCut != tcut) ||
                (cap->fFadeIn != fadein) ||
                (cap->fFadeOut != fadeout) ||
                (cap->bReverse != reverse))
            {
                cap->fHeadCut       = hcut;
                cap->fTailCut       = tcut;
                cap->fFadeIn        = fadein;
                cap->fFadeOut       = fadeout;
                cap->bReverse       = reverse;

                atomic_add(&cap->nChangeReq, 1);
                sConfigurator.queue_launch();
            }

            // Listen button pressed?
            if (cap->pListen->getValue() >= 0.5f)
            {
                size_t n_c = (cap->pCurr != NULL) ? cap->pCurr->channels() : 0;
                if (n_c > 0)
                {
                    for (size_t j=0; j<2; ++j)
                        vChannels[j].sPlayer.play(i, j % n_c, cap->fMakeup, 0);
                }
            }
        }

        // Adjust channel setup
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c        = &vChannels[i];
            c->sBypass.set_bypass(bypass);
            c->sPlayer.set_gain(out_gain);

            // Update equalization parameters
            Equalizer *eq               = &c->sEqualizer;
            equalizer_mode_t eq_mode    = (c->pWetEq->getValue() >= 0.5f) ? EQM_IIR : EQM_BYPASS;
            eq->set_mode(eq_mode);

            if (eq_mode != EQM_BYPASS)
            {
                filter_params_t fp;
                size_t band     = 0;

                // Set-up parametric equalizer
                while (band < room_builder_base_metadata::EQ_BANDS)
                {
                    if (band == 0)
                    {
                        fp.fFreq        = band_freqs[band];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = FLT_MT_LRX_LOSHELF;
                    }
                    else if (band == (room_builder_base_metadata::EQ_BANDS - 1))
                    {
                        fp.fFreq        = band_freqs[band-1];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = FLT_MT_LRX_HISHELF;
                    }
                    else
                    {
                        fp.fFreq        = band_freqs[band-1];
                        fp.fFreq2       = band_freqs[band];
                        fp.nType        = FLT_MT_LRX_LADDERPASS;
                    }

                    fp.fGain        = c->pFreqGain[band]->getValue();
                    fp.nSlope       = 2;
                    fp.fQuality     = 0.0f;

                    // Update filter parameters
                    eq->set_params(band++, &fp);
                }

                // Setup hi-pass filter
                size_t hp_slope = c->pLowCut->getValue() * 2;
                fp.nType        = (hp_slope > 0) ? FLT_BT_BWC_HIPASS : FLT_NONE;
                fp.fFreq        = c->pLowFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = hp_slope;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);

                // Setup low-pass filter
                size_t lp_slope = c->pHighCut->getValue() * 2;
                fp.nType        = (lp_slope > 0) ? FLT_BT_BWC_LOPASS : FLT_NONE;
                fp.fFreq        = c->pHighFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = lp_slope;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);
            }
        }

        // Update settings of convolvers
        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *cv         = &vConvolvers[i];

            // Allow to reconfigure convolver only when configuration task is in idle state
            size_t sampleid         = cv->pSample->getValue();
            size_t trackid          = cv->pTrack->getValue();

            if ((cv->nSampleID != sampleid) ||
                (cv->nTrackID != trackid))
            {
                cv->nSampleID           = sampleid;
                cv->nTrackID            = trackid;
                sConfigurator.queue_launch();
            }

            // Apply panning to each convolver
            float smakeup           = (sampleid > 0) ? vCaptures[sampleid-1].fMakeup : 1.0f; // Sample makeup
            float makeup            = (cv->pMute->getValue() < 0.5f) ? cv->pMakeup->getValue() * wet_gain * smakeup : 0.0f;
            if (nInputs == 1)
            {
                cv->fPanIn[0]       = 1.0f;
                cv->fPanIn[1]       = 0.0f;
            }
            else
            {
                float pan           = cv->pPanIn->getValue();
                cv->fPanIn[0]       = (100.0f - pan) * 0.005f;
                cv->fPanIn[1]       = (100.0f + pan) * 0.005f;
            }

            float pan           = cv->pPanOut->getValue();
            cv->fPanOut[0]      = (100.0f - pan) * 0.005f * makeup;
            cv->fPanOut[1]      = (100.0f + pan) * 0.005f * makeup;

            // Set pre-delay
            cv->sDelay.set_delay(millis_to_samples(fSampleRate, predelay + cv->pPredelay->getValue()));
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
        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
            vConvolvers[i].sDelay.init(millis_to_samples(sr, room_builder_base_metadata::PREDELAY_MAX * 4.0f));

        for (size_t i=0; i<2; ++i)
        {
            vChannels[i].sBypass.init(sr);
            vChannels[i].sEqualizer.set_sample_rate(sr);
        }
    }

    void room_builder_base::process(size_t samples)
    {
        // Stage 1: Process reconfiguration requests and file events
        sync_offline_tasks();

        // Stage 2: Main processing
        for (size_t i=0; i<nInputs; ++i)
            vInputs[i].vIn      = vInputs[i].pIn->getBuffer<float>();

        for (size_t i=0; i<2; ++i)
            vChannels[i].vOut   = vChannels[i].pOut->getBuffer<float>();

        // Process samples
        while (samples > 0)
        {
            // Determine number of samples to process
            size_t to_do        = TMP_BUF_SIZE;
            if (to_do > samples)
                to_do               = samples;

            // Clear temporary channel buffer
            dsp::fill_zero(vChannels[0].vBuffer, to_do);
            dsp::fill_zero(vChannels[1].vBuffer, to_do);

            // Call convolvers
            for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
            {
                convolver_t *c      = &vConvolvers[i];

                // Prepare input buffer: apply panning if present
                if (nInputs == 1)
                    dsp::copy(c->vBuffer, vInputs[0].vIn, to_do);
                else
                    dsp::mix_copy2(c->vBuffer, vInputs[0].vIn, vInputs[1].vIn, c->fPanIn[0], c->fPanIn[1], to_do);

                // Do processing
                if (c->pCurr != NULL)
                    c->pCurr->process(c->vBuffer, c->vBuffer, to_do);
                else
                    dsp::fill_zero(c->vBuffer, to_do);
                c->sDelay.process(c->vBuffer, c->vBuffer, to_do);

                // Apply processed signal to output channels
                dsp::fmadd_k3(vChannels[0].vBuffer, c->vBuffer, c->fPanOut[0], to_do);
                dsp::fmadd_k3(vChannels[1].vBuffer, c->vBuffer, c->fPanOut[1], to_do);
            }

            // Now apply equalization, bypass control and players
            for (size_t i=0; i<2; ++i)
            {
                channel_t *c        = &vChannels[i];

                // Apply equalization
                c->sEqualizer.process(c->vBuffer, c->vBuffer, to_do);

                // Pass dry sound to output channels
                if (nInputs == 1)
                    dsp::fmadd_k3(c->vBuffer, vInputs[0].vIn, c->fDryPan[0], to_do);
                else
                    dsp::mix_add2(c->vBuffer, vInputs[0].vIn, vInputs[1].vIn, c->fDryPan[0], c->fDryPan[1], to_do);

                // Apply player and bypass
                c->sPlayer.process(c->vBuffer, c->vBuffer, to_do);
                c->sBypass.process(c->vOut, vInputs[i%nInputs].vIn, c->vBuffer, to_do);

                // Update pointers
                c->vOut            += to_do;
            }

            for (size_t i=0; i<nInputs; ++i)
                vInputs[i].vIn     += to_do;

            samples            -= to_do;
        }

        // Stage 3: output additional metering parameters
        if (p3DStatus != NULL)
            p3DStatus->setValue(nSceneStatus);
        if (p3DProgress != NULL)
            p3DProgress->setValue(fSceneProgress);
        if (pRenderStatus != NULL)
            pRenderStatus->setValue(enRenderStatus);
        if (pRenderProgress != NULL)
            pRenderProgress->setValue(fRenderProgress);

        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
        {
            // Output information about the convolver
            convolver_t *c          = &vConvolvers[i];
            c->pActivity->setValue(c->pCurr != NULL);
        }

        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *c            = &vCaptures[i];

            // Output information about the file
            size_t length           = c->nLength;
            c->pLength->setValue(samples_to_millis(fSampleRate, length));
            c->pCurrLen->setValue(c->fCurrLen);
            c->pMaxLen->setValue(c->fMaxLen);
            c->pStatus->setValue(c->nStatus);

            // Store file dump to mesh
            mesh_t *mesh        = c->pThumbs->getBuffer<mesh_t>();
            if ((mesh == NULL) || (!mesh->isEmpty()) || (!c->bSync))
                continue;

            size_t channels     = (c->pCurr != NULL) ? c->pCurr->channels() : 0;
            if (channels > 0)
            {
                // Copy thumbnails
                for (size_t j=0; j<channels; ++j)
                    dsp::copy(mesh->pvData[j], c->vThumbs[j], room_builder_base_metadata::MESH_SIZE);
                mesh->data(channels, room_builder_base_metadata::MESH_SIZE);
            }
            else
                mesh->data(0, 0);
            c->bSync            = false;
        }
    }

    void room_builder_base::sync_offline_tasks()
    {
        // The render signal is pending?
        if ((nSync & SYNC_TOGGLE_RENDER) && (s3DLauncher.idle()) && (s3DLoader.idle()))
        {
            if (pExecutor->submit(&s3DLauncher))
            {
                lsp_trace("Successfully submitted Render launcher task");
                nSync &= ~SYNC_TOGGLE_RENDER;       // Reset render request flag
            }
        }
        else if (s3DLauncher.completed())
        {
            status_t res = s3DLauncher.code();
            if (res != STATUS_OK)
            {
                fRenderProgress = 0.0f;
                enRenderStatus  = s3DLauncher.code();
            }
            s3DLauncher.reset();
        }

        // Check the state of input file
        path_t *path        = p3DFile->getBuffer<path_t>();
        if (path != NULL)
        {
            if ((path->pending()) && (s3DLoader.idle()) && (s3DLauncher.idle())) // There is pending request for 3D file reload
            {
                // Copy path
                ::strncpy(s3DLoader.sPath, path->get_path(), PATH_MAX);
                s3DLoader.nFlags            = path->get_flags();
                s3DLoader.sPath[PATH_MAX]   = '\0';
                lsp_trace("Submitted scene file %s", s3DLoader.sPath);

                // Try to submit task
                if (pExecutor->submit(&s3DLoader))
                {
                    lsp_trace("Successfully submitted load task");
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
                nReconfigReq    ++;

                // Now we surely can commit changes and reset task state
                lsp_trace("File loading task has completed with status %d", int(nSceneStatus));
                path->commit();
                s3DLoader.reset();
            }
        }

        if (sSaver.idle())
        {
            // Submit save requests
            for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
            {
                capture_t *cap      = &vCaptures[i];
                if (!cap->bExport)
                    continue;

                sSaver.bind(i, cap);
                if (pExecutor->submit(&sSaver))
                {
                    cap->bExport        = false;
                    cap->pSaveStatus->setValue(STATUS_LOADING);
                    cap->pSaveProgress->setValue(0.0f);
                    break;
                }
            }
        }
        else if (sSaver.completed())
        {
            capture_t *cap = &vCaptures[sSaver.nSampleID];
            cap->pSaveStatus->setValue(sSaver.code());
            cap->pSaveProgress->setValue(100.0f);

            sSaver.reset();
        }

        // Do we need to launch configurator task?
        if ((sConfigurator.idle()) && (sConfigurator.need_launch()))
        {
            reconfig_t *cfg = &sConfigurator.sConfig;

            // Deploy actual configuration to the configurator
            for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
            {
                capture_t *cap      = &vCaptures[i];
                size_t req          = cap->nChangeReq;

                cfg->bReconfigure[i]    = (cap->nChangeResp != req);
                cfg->nChangeResp[i]     = req;
            }

            for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
            {
                convolver_t *cv     = &vConvolvers[i];

                cfg->nSampleID[i]   = cv->nSampleID;
                cfg->nTrack[i]      = cv->nTrackID;
                cfg->nRank[i]       = nFftRank;
            }

            // Try to launch configurator
            if (pExecutor->submit(&sConfigurator))
            {
                sConfigurator.launched();
                lsp_trace("Successfully submitted reconfigurator task");
            }
        }
        else if ((sConfigurator.completed()) && (sSaver.idle()))
        {
            lsp_trace("Reconfiguration task has completed with status %d", int(sConfigurator.code()));

            // Commit state of convolvers
            for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
            {
                convolver_t *c  = &vConvolvers[i];
                Convolver *cv   = c->pCurr;
                c->pCurr        = c->pSwap;
                c->pSwap        = cv;
            }

            for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
            {
                capture_t  *c   = &vCaptures[i];
                if (!c->bCommit)
                    continue;

                c->bCommit      = false;
                c->bSync        = true;

                Sample *s       = c->pCurr;
                c->pCurr        = c->pSwap;
                c->pSwap        = s;

                // Bind sample player
                for (size_t j=0; j<2; ++j)
                {
                    channel_t *sc = &vChannels[j];
                    sc->sPlayer.bind(i, c->pCurr, false);
                }
            }

            // Accept the configurator task
            sConfigurator.reset();
        }
    }

    status_t room_builder_base::bind_sources(RayTrace3D *rt)
    {
        size_t sources = 0;

        for (size_t i=0; i<room_builder_base_metadata::SOURCES; ++i)
        {
            source_t *src = &vSources[i];
            if (!src->bEnabled)
                continue;

            // Configure source
            rt_source_settings_t ss;
            status_t res = rt_configure_source(&ss, src);
            if (res != STATUS_OK)
                return res;

            // Add source to capture
            res = rt->add_source(&ss);
            if (res != STATUS_OK)
                return res;

            ++sources;
        }

        return (sources <= 0) ? STATUS_NO_SOURCES : STATUS_OK;
    }

    status_t room_builder_base::bind_captures(cvector<sample_t> &samples, RayTrace3D *rt)
    {
        size_t captures = 0;

        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *cap = &vCaptures[i];
            if (!cap->bEnabled)
                continue;
            else if ((cap->nRMax >= 0) && (cap->nRMax < cap->nRMin)) // Validate nRMin and nRMax
                continue;

            // Configure capture
            size_t n = 0;
            rt_capture_settings_t cs[2];
            status_t res = rt_configure_capture(&n, cs, cap);
            if (res != STATUS_OK)
                return res;

            // Create sample, add to list and initialize
            sample_t *s = new sample_t();
            if (s == NULL)
                return STATUS_NO_MEM;
            else if (!samples.add(s))
            {
                delete s;
                return STATUS_NO_MEM;
            }
            s->nID          = i;
            s->enConfig     = cap->sConfig;
            if (!s->sSample.init(n, 512))
                return STATUS_NO_MEM;

            // Bind captures to samples
            for (size_t i=0; i<n; ++i)
            {
                ssize_t cap_id = rt->add_capture(&cs[i]);
                if (cap_id < 0)
                    return status_t(-cap_id);

                res = rt->bind_capture(cap_id, &s->sSample, i, cap->nRMin, cap->nRMax);
                if (res != STATUS_OK)
                    return res;

                ++captures;
            }
        }

        return (captures <= 0) ? STATUS_NO_CAPTURES : STATUS_OK;
    }

    void room_builder_base::destroy_samples(cvector<sample_t> &samples)
    {
        for (size_t i=0, n=samples.size(); i<n; ++i)
        {
            sample_t *s = samples.at(i);
            if (s != NULL)
            {
                s->sSample.destroy();
                delete s;
            }
        }
        samples.flush();
    }

    status_t room_builder_base::bind_scene(KVTStorage *kvt, RayTrace3D *rt)
    {
        // Clone the scene
        Scene3D *dst = new Scene3D();
        if (dst == NULL)
            return STATUS_NO_MEM;

        status_t res = dst->clone_from(&sScene);
        if (res != STATUS_OK)
        {
            delete dst;
            return res;
        }

        // Set-up scene
        res = rt->set_scene(dst, true);
        if (res != STATUS_OK)
        {
            dst->destroy();
            delete dst;
            return res;
        }

        // Update object properties
        obj_props_t props;
        char base[0x40];
        rt_material_t mat;
        matrix3d_t world;
        dsp::init_matrix3d_scale(&world, sScale.dx, sScale.dy, sScale.dz);

        for (size_t i=0, n=dst->num_objects(); i<n; ++i)
        {
            Object3D *obj = dst->object(i);
            if (obj == NULL)
                continue;

            // Read object properties
            sprintf(base, "/scene/object/%d", int(i));
            read_object_properties(&props, base, kvt);

            // Update object matrix and visibility
            build_object_matrix(obj->matrix(), &props, &world);
            obj->set_visible(props.bEnabled);

            // Initialize material
            mat.absorption[0]   = props.fAbsorption[0] * 0.01f; // % -> units
            mat.absorption[1]   = props.fAbsorption[1] * 0.01f; // % -> units
            mat.diffusion[0]    = props.fDiffusion[0];
            mat.diffusion[1]    = props.fDiffusion[1];
            mat.dispersion[0]   = props.fDispersion[0];
            mat.dispersion[1]   = props.fDispersion[1];
            mat.transparency[0] = props.fTransparency[0] * 0.01f; // % -> units
            mat.transparency[1] = props.fTransparency[1] * 0.01f; // % -> units
            mat.permeability    = props.fSndSpeed / SOUND_SPEED_M_S;

            // Commit material properties
            res = rt->set_material(i, &mat);
            if (res != STATUS_OK)
                return res;
        }

        return STATUS_OK;
    }

    status_t room_builder_base::progress_callback(float progress, void *ptr)
    {
        room_builder_base *_this    = reinterpret_cast<room_builder_base *>(ptr);
        _this->enRenderStatus       = STATUS_IN_PROCESS;
        _this->fRenderProgress      = progress * 100.0f;    // Update the progress value
        return STATUS_OK;
    }

    status_t room_builder_base::start_rendering()
    {
        // Terminate previous thread (if active)
        if (pRenderer != NULL)
        {
            bool finished = pRenderer->finished();

            pRenderer->terminate();
            pRenderer->join();
            delete pRenderer;
            pRenderer = NULL;

            // Current task has been cancelled?
            if (!finished)
            {
                fRenderProgress = 0;
                enRenderStatus  = STATUS_CANCELLED;
                return STATUS_OK;
            }
        }

        // Create raytracing object and initialize with basic values
        RayTrace3D *rt = new RayTrace3D();
        if (rt == NULL)
            return STATUS_NO_MEM;

        status_t res = rt->init();
        if (res != STATUS_OK)
        {
            rt->destroy(false);
            delete rt;
            return res;
        }

        rt->set_sample_rate(fSampleRate);

        float energy    = 1e-3f * expf(-4.0f * M_LN10 * fRenderQuality);    // 1e-3 .. 1e-7
        float tolerance = 1e-4f * expf(-2.0f * M_LN10 * fRenderQuality);    // 1e-4 .. 1e-6
        float details   = 1e-8f * expf(-2.0f * M_LN10 * fRenderQuality);    // 1e-8 .. 1e-10

        rt->set_energy_threshold(energy);
        rt->set_tolerance(tolerance);
        rt->set_detalization(details);
        rt->set_normalize(bRenderNormalize);
        rt->set_progress_callback(progress_callback, this);

        // Bind scene to the raytracing
        KVTStorage *kvt = kvt_lock();
        if (kvt != NULL)
        {
            res = bind_scene(kvt, rt);
            kvt_release();
        }
        else
            res = STATUS_NO_DATA;

        // Bind sources
        res = bind_sources(rt);
        if (res != STATUS_OK)
        {
            rt->destroy(true);
            delete rt;
            return res;
        }

        // Bind captures
        cvector<sample_t> samples;
        res = bind_captures(samples, rt);
        if (res != STATUS_OK)
        {
            destroy_samples(samples);
            rt->destroy(true);
            delete rt;
            return res;
        }

        // Create renderer and start execution
        if (res == STATUS_OK)
        {
            pRenderer = new Renderer(this, rt, nRenderThreads, samples);
            if (pRenderer == NULL)
                res = STATUS_NO_MEM;
            else if ((res = pRenderer->start()) != STATUS_OK)
            {
                delete pRenderer;
                pRenderer = NULL;
            }
        }

        if (res != STATUS_OK)
        {
            destroy_samples(samples);
            rt->destroy(true);
            delete rt;
            return res;
        }

        // All seems to be OK
        return STATUS_OK;
    }

    status_t room_builder_base::commit_samples(cvector<sample_t> &samples)
    {
        // Put each sample to KVT and toggle the reload flag
        kvt_param_t p;
        char path[0x40];

        for (size_t i=0, n=samples.size(); i<n; ++i)
        {
            sample_t *s     = samples.at(i);
            if (s == NULL)
                continue;

            // Create sample data
            size_t slen         = s->sSample.length();
            size_t payload      = sizeof(sample_header_t) + slen * s->sSample.channels() * sizeof(float);
            sample_header_t *hdr = reinterpret_cast<sample_header_t *>(::malloc(payload));
            if (hdr == NULL)
                return STATUS_NO_MEM;
            hdr->version        = __IF_LEBE(0, 1);
            hdr->channels       = s->sSample.channels();
            hdr->sample_rate    = fSampleRate;
            hdr->samples        = s->sSample.length();

            hdr->version        = CPU_TO_BE(hdr->version);
            hdr->channels       = CPU_TO_BE(hdr->channels);
            hdr->sample_rate    = CPU_TO_BE(hdr->sample_rate);
            hdr->samples        = CPU_TO_BE(hdr->samples);

            float *fdst         = reinterpret_cast<float *>(&hdr[1]);
            for (size_t i=0; i<s->sSample.channels(); ++i, fdst += slen)
                ::memcpy(fdst, s->sSample.getBuffer(i), slen * sizeof(float));

            // Post-process Mid/Side audio data
            if (s->enConfig == RT_CC_MS)
            {
                float *l            = reinterpret_cast<float *>(&hdr[1]);
                float *r            = &l[slen];
                dsp::ms_to_lr(l, r, l, r, slen);
            }

            // Create KVT parameter
            p.type          = KVT_BLOB;
            p.blob.ctype    = ::strdup(AUDIO_SAMPLE_CONTENT_TYPE);
            if (p.blob.ctype == NULL)
            {
                ::free(hdr);
                return STATUS_NO_MEM;
            }
            p.blob.size     = payload;
            p.blob.data     = hdr;

            // Deploy KVT parameter
            sprintf(path, "/samples/%d", int(s->nID));
            KVTStorage *kvt = kvt_lock();
            if (kvt != NULL)
            {
                kvt->put(path, &p, KVT_PRIVATE | KVT_DELEGATE); // Delegate memory management to KVT Storage
                kvt->gc();
                kvt_release();
            }
            else
                return STATUS_BAD_STATE;

            // Update the number of changes for the sample and toggle configurator launch
            atomic_add(&vCaptures[s->nID].nChangeReq, 1);
            sConfigurator.queue_launch();
        }

        return STATUS_OK;
    }

    status_t room_builder_base::reconfigure(const reconfig_t *cfg)
    {
        status_t res;

        // Collect the garbage
        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *c  = &vConvolvers[i];
            if (c->pSwap != NULL)
            {
                c->pSwap->destroy();
                delete c->pSwap;
                c->pSwap    = NULL;
            }
        }

        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *c  = &vCaptures[i];
            if (c->pSwap != NULL)
            {
                c->pSwap->destroy();
                delete c->pSwap;
                c->pSwap    = NULL;
            }
        }

        // Re-render samples
        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *c    = &vCaptures[i];

            // Do we need to change the sample?
            if (!cfg->bReconfigure[i])
                continue;

            // Update status and commit request
            c->nStatus      = STATUS_OK;
            c->nChangeResp  = cfg->nChangeResp[i];
            c->bCommit      = true;

            // Lock KVT and fetch sample data
            KVTStorage *kvt = kvt_lock();
            if (kvt == NULL)
            {
                c->nStatus      = STATUS_BAD_STATE;
                continue;
            }

            // Fetch KVT sample
            sample_header_t hdr;
            const float *samples;
            res = fetch_kvt_sample(kvt, i, &hdr, &samples);
            if (res != STATUS_OK)
            {
                c->nStatus      = res;
                kvt_release();
                continue;
            }

            // Allocate new sample
            Sample *s           = new Sample();
            if (s == NULL)
            {
                kvt_release();
                c->nStatus      = STATUS_NO_MEM;
                continue;
            }
            c->nLength          = hdr.samples;
            c->fMaxLen          = samples_to_millis(hdr.sample_rate, hdr.samples);
            c->pSwap            = s;
            lsp_trace("Allocated sample=%p, original length=%d samples", s, int(c->nLength));

            // Initialize sample
            if (!s->init(hdr.channels, hdr.samples, hdr.samples))
            {
                kvt_release();
                c->nStatus      = STATUS_NO_MEM;
                continue;
            }

            // Sample is present, check boundaries
            size_t head_cut     = millis_to_samples(fSampleRate, c->fHeadCut);
            size_t tail_cut     = millis_to_samples(fSampleRate, c->fTailCut);
            ssize_t fsamples    = hdr.samples - head_cut - tail_cut;
            if (fsamples <= 0)
            {
                s->setLength(0);
                c->fCurrLen         = 0.0f;
                kvt_release();

                for (size_t j=0; j<hdr.channels; ++j)
                    dsp::fill_zero(c->vThumbs[j], room_builder_base_metadata::MESH_SIZE);
                continue;
            }
            c->fCurrLen         = samples_to_millis(hdr.sample_rate, fsamples);

            // Render the sample
            float norm          = 0.0f;
            for (size_t j=0; j<hdr.channels; ++j)
            {
                const float *src    = &samples[j * hdr.samples];
                float *dst          = s->getBuffer(j);

                // Get normalizing factor
                float xnorm         = dsp::abs_max(src, hdr.samples);
                if (xnorm > norm)
                    norm            = xnorm;

                // Copy sample data and apply fading
                if (c->bReverse)
                    dsp::reverse2(dst, &src[tail_cut], fsamples);
                else
                    dsp::copy(dst, &src[head_cut], fsamples);
                if ((hdr.version & 1) != __IF_LEBE(0, 1)) // Endianess does not match?
                    byte_swap(dst, fsamples);

                fade_in(dst, dst, millis_to_samples(fSampleRate, c->fFadeIn), fsamples);
                fade_out(dst, dst, millis_to_samples(fSampleRate, c->fFadeOut), fsamples);

                // Now render thumbnail
                src                 = dst;
                dst                 = c->vThumbs[j];
                for (size_t k=0; k<room_builder_base_metadata::MESH_SIZE; ++k)
                {
                    size_t first    = (k * fsamples) / room_builder_base_metadata::MESH_SIZE;
                    size_t last     = ((k + 1) * fsamples) / room_builder_base_metadata::MESH_SIZE;
                    if (first < last)
                        dst[k]          = dsp::abs_max(&src[first], last - first);
                    else
                        dst[k]          = fabs(src[first]);
                }
            }

            // Normalize graph if possible
            if (norm != 0.0f)
            {
                norm    = 1.0f / norm;
                for (size_t j=0; j<hdr.channels; ++j)
                    dsp::mul_k2(c->vThumbs[j], norm, room_builder_base_metadata::MESH_SIZE);
            }

            // Release KVT storage
            kvt_release();
        }

        // Randomize phase of the convolver
        uint32_t phase  = seed_addr(this);
        phase           = ((phase << 16) | (phase >> 16)) & 0x7fffffff;
        uint32_t step   = 0x80000000 / (room_builder_base_metadata::CONVOLVERS + 1);

        // Reconfigure convolvers
        for (size_t i=0; i<room_builder_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *c  = &vConvolvers[i];

            // Check that routing has changed
            size_t capture  = cfg->nSampleID[i];
            size_t track    = cfg->nTrack[i];
            if ((capture <= 0) || (capture > room_builder_base_metadata::CAPTURES))
                continue;
            else
                --capture;

            // Analyze sample
            Sample *s       = (vCaptures[capture].bCommit) ? vCaptures[capture].pSwap: vCaptures[capture].pCurr;
            if ((s == NULL) || (!s->valid()) || (s->channels() <= track))
                continue;

            // Now we can create convolver
            Convolver *cv   = new Convolver();
            if (!cv->init(s->getBuffer(track), s->length(), cfg->nRank[i], float((phase + i*step)& 0x7fffffff)/float(0x80000000)))
            {
                cv->destroy();
                delete cv;
                return STATUS_NO_MEM;
            }

            lsp_trace("Allocated convolver pSwap=%p for channel %d (pCurr=%p)", cv, int(i), c->pCurr);
            c->pSwap        = cv;
        }

        return STATUS_OK;
    }

    status_t room_builder_base::fetch_kvt_sample(KVTStorage *kvt, size_t sample_id, sample_header_t *hdr, const float **samples)
    {
        status_t res;
        const kvt_param_t *p;
        const sample_header_t *phdr;
        char path[0x40];

        sprintf(path, "/samples/%d", int(sample_id));

        // Fetch parameter
        res = kvt->get(path, &p, KVT_BLOB);
        if ((res != STATUS_OK) ||
            (p == NULL))
            return STATUS_NO_DATA;

        // Validate blob settings
        if ((p->blob.ctype == NULL) ||
            (p->blob.data == NULL) ||
            (p->blob.size < sizeof(sample_header_t)) ||
            (::strcmp(p->blob.ctype, AUDIO_SAMPLE_CONTENT_TYPE) != 0))
            return STATUS_CORRUPTED;

        // Decode sample data
        phdr                = reinterpret_cast<const sample_header_t *>(p->blob.data);
        hdr->version        = BE_TO_CPU(phdr->version);
        hdr->channels       = BE_TO_CPU(phdr->channels);
        hdr->sample_rate    = BE_TO_CPU(phdr->sample_rate);
        hdr->samples        = BE_TO_CPU(phdr->samples);

        if (((hdr->version >> 1) != 0) ||
            ((hdr->samples * hdr->channels * sizeof(float) + sizeof(sample_header_t)) != p->blob.size))
            return STATUS_CORRUPTED;

        *samples            = reinterpret_cast<const float *>(&phdr[1]);
        return STATUS_OK;
    }

    status_t room_builder_base::save_sample(const char *path, size_t sample_id)
    {
        if (::strlen(path) <= 0)
            return STATUS_BAD_PATH;

        LSPString sp, lspc;
        if ((!sp.set_utf8(path)) || (!lspc.set_ascii(".lspc")))
            return STATUS_NO_MEM;

        // Lock KVT storage
        KVTStorage *kvt = kvt_lock();
        if (kvt == NULL)
            return STATUS_BAD_STATE;

        sample_header_t hdr;
        const float *samples;
        status_t res    = fetch_kvt_sample(kvt, sample_id, &hdr, &samples);

        // Check the extension of file
        if (sp.ends_with_nocase(&lspc))
        {
            lspc_audio_parameters_t params;
            params.channels         = hdr.channels;
            params.sample_format    = (hdr.version & 1) ? LSPC_SAMPLE_FMT_F32BE : LSPC_SAMPLE_FMT_F32LE;
            params.sample_rate      = hdr.sample_rate;
            params.codec            = LSPC_CODEC_PCM;
            params.frames           = hdr.samples;

            // Initialize sample array
            const float **vs        = reinterpret_cast<const float **>(::malloc(params.channels * sizeof(float *)));
            if (vs == NULL)
            {
                kvt_release();
                return STATUS_NO_MEM;
            }
            for (size_t i=0; i<params.channels; ++i)
                vs[i]               = &samples[i * params.frames];

            // Create LSPC writer
            LSPCAudioWriter wr;
            res = wr.create(&sp, &params);
            if (res != STATUS_OK)
            {
                ::free(vs);
                kvt_release();
                return res;
            }

            // Write all samples to the file
            res = wr.write_samples(vs, params.frames);
            status_t res2 = wr.close();
            if (res == STATUS_OK)
                res     = res2;
            ::free(vs);
        }
        else
        {
            AudioFile af;
            res     = af.create_samples(hdr.channels, hdr.sample_rate, hdr.samples);
            if (res != STATUS_OK)
            {
                kvt_release();
                return res;
            }

            // Prepare file contents
            for (size_t i=0; i<hdr.channels; ++i)
            {
                float *dst = af.channel(i);
                dsp::copy(dst, &samples[i * hdr.samples], hdr.samples);
                if ((hdr.version & 1) != __IF_LEBE(0, 1))
                    byte_swap(dst, hdr.samples);
            }

            // Store file contents
            res     = af.store(&sp);
            af.destroy();
        }

        // Release KVT storage and return result
        kvt_release();
        return res;
    }

    void room_builder_base::state_loaded()
    {
        // We need to sync all loaded samples in KVT with internal state
        for (size_t i=0; i<room_builder_base_metadata::CAPTURES; ++i)
        {
            capture_t *cap      = &vCaptures[i];
            atomic_add(&cap->nChangeReq, 1);
            sConfigurator.queue_launch();
        }
    }

    void room_builder_base::ui_activated()
    {
        // Synchronize thumbnails with UI
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
            vCaptures[i].bSync  = true;
    }

    //-------------------------------------------------------------------------
    room_builder_mono::room_builder_mono(): room_builder_base(metadata, 1)
    {
    }

    room_builder_mono::~room_builder_mono()
    {
    }

    room_builder_stereo::room_builder_stereo(): room_builder_base(metadata, 2)
    {
    }

    room_builder_stereo::~room_builder_stereo()
    {
    }
}

