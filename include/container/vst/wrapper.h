/*
 * wrapper.h
 *
 *  Created on: 08 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_VST_WRAPPER_H_
#define CONTAINER_VST_WRAPPER_H_

#include <container/vst/defs.h>
#include <container/vst/chunk.h>
#include <core/ipc/NativeExecutor.h>

#ifndef LSP_NO_VST_UI
    #define IF_VST_UI_ON(...)       __VA_ARGS__
#else
    #define IF_VST_UI_ON(...)
#endif

namespace lsp
{
    class VSTAudioPort;
    class VSTParameterPort;
    class VSTPort;
    class VSTUIPort;

    class VSTWrapper: public IWrapper IF_VST_UI_ON(, public IUIWrapper)
    {
        private:
            plugin_t                   *pPlugin;
            AEffect                    *pEffect;
            audioMasterCallback         pMaster;
            ipc::IExecutor             *pExecutor;
            vst_chunk_t                 sChunk;
            bool                        bUpdateSettings;
            float                       fLatency;
            VSTPort                    *pBypass;

            cvector<VSTAudioPort>       vInputs;        // List of input audio ports
            cvector<VSTAudioPort>       vOutputs;       // List of output audio ports
            cvector<VSTParameterPort>   vParams;        // List of controllable parameters
            cvector<VSTPort>            vPorts;         // List of all created VST ports
            cvector<VSTPort>            vProxyPorts;    // List of all created VST proxy ports
            cvector<port_t>             vGenMetadata;   // Generated metadata

            position_t                  sPosition;

            KVTStorage                  sKVT;
            ipc::Mutex                  sKVTMutex;

            IF_VST_UI_ON(
                plugin_ui                  *pUI;
                ERect                       sRect;
                cvector<VSTUIPort>          vUIPorts;       // List of all created UI ports
            )

        private:
            IF_VST_UI_ON(
                void transfer_dsp_to_ui();
            )

            VSTPort *create_port(const port_t *port, const char *postfix);
            VSTPort *find_by_id(const char *id);
            void create_ports(const port_t *meta);

        protected:
            IF_VST_UI_ON(
                static status_t slot_ui_resize(LSPWidget *sender, void *ptr, void *data);
            )

            status_t check_vst_header(const fxBank *bank, size_t size);
            void deserialize_v1(const fxBank *bank);
            void deserialize_v2_v3(const uint8_t *data, size_t bytes);
            void deserialize_new_chunk_format(const uint8_t *data, size_t bytes);

            void sync_position();

        public:
            VSTWrapper(
                    AEffect *effect,
                    plugin_t *plugin,
                    const char *name,
                    audioMasterCallback callback
            )
            {
                pPlugin         = plugin;
                pEffect         = effect;

                pMaster         = callback;
                pExecutor       = NULL;

                fLatency        = 0.0f;
                pBypass         = NULL;
                bUpdateSettings = true;

                IF_VST_UI_ON(
                    pUI             = NULL;
                    sRect.top       = 0;
                    sRect.left      = 0;
                    sRect.bottom    = 0;
                    sRect.right     = 0;
                )

                position_t::init(&sPosition);
            }

            virtual ~VSTWrapper()
            {
                pPlugin         = NULL;
                pEffect         = NULL;
                IF_VST_UI_ON(
                    pUI             = NULL;
                )

                pMaster         = NULL;
            }

        public:
            inline const plugin_metadata_t *get_metadata() const    {   return pPlugin->get_metadata();     };
            inline VSTParameterPort *get_parameter(size_t index)    {   return vParams[index];              };

            void init();
            void destroy();
            inline void open() { };
            void run(float** inputs, float** outputs, size_t samples);
            void run_legacy(float** inputs, float** outputs, size_t samples);
            void process_events(const VstEvents *e);

            inline void set_sample_rate(float sr)
            {
                if (sr > MAX_SAMPLE_RATE)
                {
                    lsp_warn("Unsupported sample rate: %f, maximum supported sample rate is %ld", sr, long(MAX_SAMPLE_RATE));
                    sr = MAX_SAMPLE_RATE;
                }
                pPlugin->set_sample_rate(sr);
                bUpdateSettings = true;
            }

            inline void set_block_size(size_t size);

            inline void mains_changed(VstIntPtr value)
            {
                if (value)
                    pPlugin->activate();
                else
                    pPlugin->deactivate();
            }

#ifndef LSP_NO_VST_UI
            bool show_ui(void *root_widget);
            void hide_ui();
            void iterate_ui();
            void destroy_ui();
            void resize_ui(const realize_t *r);
            ERect *get_ui_rect();
#endif

            inline bool has_bypass() const
            {
                return pBypass != NULL;
            }

            inline void set_bypass(bool bypass);

            virtual ipc::IExecutor *get_executor()
            {
                lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
                if (pExecutor != NULL)
                    return pExecutor;

                lsp_trace("Creating native executor service");
                ipc::NativeExecutor *exec = new ipc::NativeExecutor();
                if (exec == NULL)
                    return NULL;
                if (exec->start() != STATUS_OK)
                {
                    delete exec;
                    return NULL;
                }
                return pExecutor = exec;
            }

            virtual const position_t *position()
            {
                return &sPosition;
            }

            virtual ICanvas *create_canvas(ICanvas *&cv, size_t width, size_t height);

            size_t serialize_state(const void **dst);
            void deserialize_state(const void *data, size_t size);

            /**
             * Lock KVT storage
             * @return pointer to locked storage or NULL
             */
            virtual KVTStorage *kvt_lock();

            /**
             * Try to lock KVT storage and return pointer to the storage on success
             * @return pointer to KVT storage or NULL
             */
            virtual KVTStorage *kvt_trylock();

            /**
             * Release the KVT storage
             * @return true on success
             */
            virtual bool kvt_release();
    };
}

// Here Port description should be included
#include <container/vst/ports.h>
#ifndef LSP_NO_VST_UI
    #include <container/vst/ui_ports.h>
#endif

namespace lsp
{
    VSTPort *VSTWrapper::create_port(const port_t *port, const char *postfix)
    {
        VSTPort *vp = NULL;
        IF_VST_UI_ON(VSTUIPort *vup = NULL;)

        switch (port->role)
        {
            case R_MESH:
                vp  = new VSTMeshPort(port, pEffect, pMaster);
                IF_VST_UI_ON(vup = new VSTUIMeshPort(port, vp);)
                break;

            case R_FBUFFER:
                vp  = new VSTFrameBufferPort(port, pEffect, pMaster);
                IF_VST_UI_ON(vup = new VSTUIFrameBufferPort(port, vp);)
                break;

            case R_MIDI:
                if (IS_OUT_PORT(port))
                    vp = new VSTMidiOutputPort(port, pEffect, pMaster);
                else
                {
                    pEffect->flags         |= effFlagsIsSynth;
                    vp = new VSTMidiInputPort(port, pEffect, pMaster);
                }
                break;

            case R_OSC:
                vp      = new VSTOscPort(port, pEffect, pMaster);
                IF_VST_UI_ON(
                    if (IS_OUT_PORT(port))
                        vup     = new VSTUIOscPortIn(port, vp);
                    else
                        vup     = new VSTUIOscPortOut(port, vp);
                )
                break;

            case R_PATH:
                vp  = new VSTPathPort(port, pEffect, pMaster);
                IF_VST_UI_ON(vup = new VSTUIPathPort(port, vp);)
                break;

            case R_AUDIO:
                vp = new VSTAudioPort(port, pEffect, pMaster);
                break;

            case R_CONTROL:
            case R_METER:
            case R_BYPASS:
                // VST specifies only INPUT parameters, output should be read in different way
                if (IS_OUT_PORT(port))
                {
                    vp      = new VSTMeterPort(port, pEffect, pMaster);
                    IF_VST_UI_ON(vup     = new VSTUIMeterPort(port, vp);)
                }
                else
                {
                    vp      = new VSTParameterPort(port, pEffect, pMaster);
                    IF_VST_UI_ON(vup     = new VSTUIParameterPort(port, static_cast<VSTParameterPort *>(vp));)
                }
                if (port->role == R_BYPASS)
                    pBypass     = vp;
                break;

            case R_PORT_SET:
            {
                char postfix_buf[LSP_MAX_PARAM_ID_BYTES];
                VSTPortGroup       *pg      = new VSTPortGroup(port, pEffect, pMaster);
                pPlugin->add_port(pg);

                IF_VST_UI_ON(
                    VSTUIPortGroup     *upg     = new VSTUIPortGroup(pg);
                    vUIPorts.add(upg);
                )

                for (size_t row=0; row<pg->rows(); ++row)
                {
                    // Generate postfix
                    snprintf(postfix_buf, sizeof(postfix_buf)-1, "%s_%d", (postfix != NULL) ? postfix : "", int(row));

                    // Clone port metadata
                    port_t *cm          = clone_port_metadata(port->members, postfix_buf);
                    if (cm != NULL)
                    {
                        vGenMetadata.add(cm);

                        for (; cm->id != NULL; ++cm)
                        {
                            if (IS_GROWING_PORT(cm))
                                cm->start    = cm->min + ((cm->max - cm->min) * row) / float(pg->rows());
                            else if (IS_LOWERING_PORT(cm))
                                cm->start    = cm->max - ((cm->max - cm->min) * row) / float(pg->rows());

                            VSTPort *p = create_port(cm, postfix_buf);
                            if ((p != NULL) && (p->metadata()->role != R_PORT_SET))
                                pPlugin->add_port(p);
                        }
                    }
                }

                vp      = pg;
                break;
            }

            default:
                break;
        }

        if (vp != NULL)
            vPorts.add(vp);
        IF_VST_UI_ON(
            if (vup != NULL)
                vUIPorts.add(vup);
        )

        return vp;
    }

    void VSTWrapper::create_ports(const port_t *meta)
    {
        for ( ; meta->id != NULL; ++meta)
        {
            VSTPort *vp = create_port(meta, NULL);
            if (vp == NULL)
                continue;

            switch (meta->role)
            {
                case R_PORT_SET:
                    break;

                case R_MESH:
                case R_FBUFFER:
                case R_MIDI:
                case R_PATH:
                    pPlugin->add_port(vp);
                    break;

                case R_AUDIO:
                    pPlugin->add_port(vp);
                    if (IS_OUT_PORT(meta))
                        vOutputs.add(static_cast<VSTAudioPort *>(vp));
                    else
                        vInputs.add(static_cast<VSTAudioPort *>(vp));
                    break;

                case R_CONTROL:
                case R_BYPASS:
                case R_METER:
                    pPlugin->add_port(vp);
                    if (IS_IN_PORT(meta)) // VST specifies only INPUT parameters, output should be read in different way
                        vParams.add(static_cast<VSTParameterPort *>(vp));
                    break;

                default:
                    break;
            }
        }
    }

    void VSTWrapper::init()
    {
        AEffect *e                      = pEffect;
        const plugin_metadata_t *m      = pPlugin->get_metadata();

        // Bind ports
        lsp_trace("Binding ports");
        create_ports(m->ports);

        // Get buffer size
        ssize_t blk_size = pMaster(pEffect, audioMasterGetBlockSize, 0, 0, 0, 0);
        if (blk_size > 0)
            set_block_size(blk_size);

        // Update instance parameters
        e->numInputs                    = vInputs.size();
        e->numOutputs                   = vOutputs.size();
        e->numParams                    = vParams.size();

        // Generate IDs for parameter ports
        for (ssize_t id=0; id < e->numParams; ++id)
            vParams[id]->setID(id);

        // Initialize state chunk
        pEffect->flags                 |= effFlagsProgramChunks;

        // Initialize plugin
        pPlugin->init(this);
    }

    void VSTWrapper::destroy()
    {
#ifndef LSP_NO_VST_UI
        // First destroy the UI
        destroy_ui();
#endif

        // Shutdown and delete executor if exists
        if (pExecutor != NULL)
        {
            pExecutor->shutdown();
            delete pExecutor;
            pExecutor   = NULL;
        }

        // Destrop plugin
        lsp_trace("destroying plugin");
        if (pPlugin != NULL)
        {
            pPlugin->destroy();
            delete pPlugin;

            pPlugin = NULL;
        }

        // Destroy UI ports
#ifndef LSP_NO_VST_UI
            for (size_t i=0; i<vUIPorts.size(); ++i)
            {
                lsp_trace("destroy ui port id=%s", vUIPorts[i]->metadata()->id);
                delete vUIPorts[i];
            }
            vUIPorts.clear();
#endif

        // Destroy ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            lsp_trace("destroy port id=%s", vPorts[i]->metadata()->id);
            delete vPorts[i];
        }
        vPorts.clear();

        // Cleanup generated metadata
        for (size_t i=0; i<vGenMetadata.size(); ++i)
        {
            lsp_trace("destroy generated port metadata %p", vGenMetadata[i]);
            drop_port_metadata(vGenMetadata[i]);
        }

        // Clear all port lists
        vInputs.clear();
        vOutputs.clear();
        vParams.clear();

        pMaster     = NULL;
        pEffect     = NULL;

        lsp_trace("destroy complete");
    }

    void VSTWrapper::set_block_size(size_t size)
    {
        lsp_trace("Block size for audio processing: %d", int(size));

        // Sync buffer size to all input ports
        for (size_t i=0, n=vInputs.size(); i<n; ++i)
        {
            VSTAudioPort *p = vInputs.at(i);
            if (p != NULL)
                p->set_blk_size(size);
        }
    }

    void VSTWrapper::sync_position()
    {
        VstTimeInfo *info   = FromVstPtr<VstTimeInfo>(pMaster(pEffect, audioMasterGetTime, 0, kVstPpqPosValid | kVstTempoValid | kVstBarsValid | kVstCyclePosValid | kVstTimeSigValid, NULL, 0.0f));
        if (info == NULL)
            return;

        position_t npos     = sPosition;

        npos.sampleRate     = info->sampleRate;
        npos.speed          = 1.0f;
        npos.ticksPerBeat   = DEFAULT_TICKS_PER_BEAT;
        npos.frame          = info->samplePos;

//        lsp_trace("info->flags          = 0x%08x", int(info->flags));
//        lsp_trace("info->sampleRate     = %f", info->sampleRate);
//        lsp_trace("info->samplePos      = %f", info->samplePos);
//        lsp_trace("info->numerator      = %d", int(info->timeSigNumerator));
//        lsp_trace("info->denominator    = %d", int(info->timeSigDenominator));
//        lsp_trace("info->bpm            = %f", info->tempo);

        if (info->flags & kVstTimeSigValid)
        {
            npos.numerator      = info->timeSigNumerator;
            npos.denominator    = info->timeSigDenominator;

//            lsp_trace("ppq_pos = %f, bar_start_pos = %f", float(info->ppqPos), float(info->barStartPos));
            if ((info->flags & (kVstPpqPosValid | kVstBarsValid)) == (kVstPpqPosValid | kVstBarsValid))
            {
                double uppqPos      = (info->ppqPos - info->barStartPos) * info->timeSigDenominator * 0.25;
                npos.tick           = npos.ticksPerBeat * (uppqPos - int64_t(uppqPos));
            }
        }

        if (info->flags & kVstTempoValid)
            npos.beatsPerMinute = info->tempo;

//        lsp_trace("position: sr=%f, frame=%ld, key=%f/%f tick=%f bpm=%f",
//                float(npos.sampleRate), long(npos.frame), float(npos.numerator),
//                float(npos.denominator), float(npos.tick), float(npos.beatsPerMinute));

        // Report new position to plugin and update position
        if (pPlugin->set_position(&npos))
            bUpdateSettings = true;
        sPosition       = npos;
    }

    void VSTWrapper::run(float** inputs, float** outputs, size_t samples)
    {
        // DO NOTHING if sample_rate is not set (fill output buffers with zeros)
        if (pPlugin->get_sample_rate() <= 0)
        {
            size_t n_outputs = vOutputs.size();
            for (size_t i=0; i < n_outputs; ++i)
                dsp::fill_zero(outputs[i], samples);
            return;
        }

#ifndef LSP_NO_VST_UI
        // Sync UI state
        if (pUI != NULL)
        {
            if (!pPlugin->ui_active())
                pPlugin->activate_ui();
        }
        else if (pPlugin->ui_active())
            pPlugin->deactivate_ui();
#endif

        // Synchronize position
        sync_position();

        // Bind audio ports
        size_t n_inputs = vInputs.size();
        for (size_t i=0; i < n_inputs; ++i)
        {
            VSTAudioPort *p = vInputs.at(i);
            if (p != NULL)
                p->bind(inputs[i], samples);
        }
        size_t n_outputs = vOutputs.size();
        for (size_t i=0; i < n_outputs; ++i)
        {
            VSTAudioPort *p = vOutputs.at(i);
            if (p != NULL)
                p->bind(outputs[i], samples);
        }

        // Process ALL ports for changes
        size_t n_ports      = vPorts.size();
        VSTPort **v_ports   = vPorts.get_array();
        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            VSTPort *port = v_ports[i];
            if (port == NULL)
                continue;

            // Pre-process data in port
            if (port->pre_process(samples))
            {
                lsp_trace("port changed: %s", port->metadata()->id);
                bUpdateSettings = true;
            }
        }

        // Check that input parameters have changed
        if (bUpdateSettings)
        {
            lsp_trace("updating settings");
            pPlugin->update_settings();
            bUpdateSettings     = false;
        }

        // Call the main processing unit
        pPlugin->process(samples);

        // Report latency
        float latency           = pPlugin->get_latency();
        if (fLatency != latency)
        {
            pEffect->initialDelay   = latency;
            fLatency                = latency;
            if (pMaster)
            {
                lsp_trace("Reporting latency = %d samples to the host", int(latency));
                pMaster(pEffect, audioMasterIOChanged, 0, 0, 0, 0);
            }
        }

        // Post-process ALL ports
        for (size_t i=0; i<n_ports; ++i)
        {
            VSTPort *port = v_ports[i];
            if (port != NULL)
                port->post_process(samples);
        }
    }

    void VSTWrapper::process_events(const VstEvents *e)
    {
        // We need to deliver MIDI events to MIDI ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            VSTPort *p          = vPorts[i];
            const port_t *meta  = p->metadata();

            // Find MIDI port(s)
            if (!IS_IN_PORT(meta))
                continue;
            if (meta->role != R_MIDI)
                continue;

            // Call for event processing
            VSTMidiInputPort *mp    = static_cast<VSTMidiInputPort *>(p);
            mp->deserialize(e);
        }
    }

    void VSTWrapper::run_legacy(float** inputs, float** outputs, size_t samples)
    {
        run(inputs, outputs, samples);
    }

#ifndef LSP_NO_VST_UI
    bool VSTWrapper::show_ui(void *root_widget)
    {
        lsp_trace("show ui");
        const plugin_metadata_t *m  = pPlugin->get_metadata();

        if (pUI == NULL)
        {
            // Create custom UI object
            lsp_trace("create ui");
            #define MOD_PLUGIN(plugin, ui) \
                if ((!pUI) && (!strcmp(plugin::metadata.vst_uid, m->vst_uid))) \
                    pUI = new ui(m, root_widget);
            #include <metadata/modules.h>

            if (pUI == NULL)
                return false;

            // Add pre-generated ports
            for (size_t i=0; i<vUIPorts.size(); ++i)
            {
                VSTUIPort  *vp      = vUIPorts.at(i);
                lsp_trace("Adding UI port id=%s", vp->metadata()->id);
                vp->resync();
                pUI->add_port(vp);
            }

            // Initialize UI
            lsp_trace("init ui");
            status_t res = pUI->init(this, 0, NULL);
            if (res == STATUS_OK)
                res = pUI->build();

            LSPWindow *wnd  = pUI->root_window();
            if (wnd != NULL)
                wnd->slots()->bind(LSPSLOT_RESIZE, slot_ui_resize, this);
        }

        // Force all parameters to be re-shipped to the UI
        for (size_t i=0; i<vUIPorts.size(); ++i)
        {
            VSTUIPort  *vp      = vUIPorts.at(i);
            if (vp != NULL)
                vp->notify_all();
        }

        if (sKVTMutex.lock())
        {
            sKVT.touch_all(KVT_TO_UI);
            sKVTMutex.unlock();
        }
        transfer_dsp_to_ui();

        // Show the UI window
        LSPWindow *wnd  = pUI->root_window();
        size_request_t sr;
        wnd->size_request(&sr);

        sRect.top       = 0;
        sRect.left      = 0;
        sRect.right     = sr.nMinWidth;
        sRect.bottom    = sr.nMinHeight;

        realize_t r;
        r.nLeft         = 0;
        r.nTop          = 0;
        r.nWidth        = sr.nMinWidth;
        r.nHeight       = sr.nMinHeight;
        resize_ui(&r);

        pUI->show();

        return true;
    }

    void VSTWrapper::destroy_ui()
    {
        lsp_trace("destroy ui");

        // Destroy UI
        if (pUI != NULL)
        {
            pUI->destroy();
            delete pUI;
            pUI         = NULL;
        }

        // Unbind all UI ports
        for (size_t i=0; i<vUIPorts.size(); ++i)
            vUIPorts[i]->unbind_all();
    }

    void VSTWrapper::iterate_ui()
    {
        if (pUI != NULL)
        {
            transfer_dsp_to_ui();
            pUI->main_iteration();
        }
    }

    status_t VSTWrapper::slot_ui_resize(LSPWidget *sender, void *ptr, void *data)
    {
        VSTWrapper *_this = static_cast<VSTWrapper *>(ptr);
        _this->resize_ui(static_cast<realize_t *>(data));
        return STATUS_OK;
    }

    ERect *VSTWrapper::get_ui_rect()
    {
        lsp_trace("left=%d, top=%d, right=%d, bottom=%d",
                int(sRect.left), int(sRect.top), int(sRect.right), int(sRect.bottom)
            );
        return &sRect;
    };

    void VSTWrapper::resize_ui(const realize_t *r)
    {
        lsp_trace("UI has been resized");
        if (pUI == NULL)
            return;

        LSPWindow *wnd      = pUI->root_window();

        sRect.top           = 0;
        sRect.left          = 0;
        sRect.right         = r->nWidth;
        sRect.bottom        = r->nHeight;

        realize_t rr;
        wnd->get_geometry(&rr);
        lsp_trace("Get geometry: width=%d, height=%d", int(rr.nWidth), int(rr.nHeight));

        if ((rr.nWidth <= 0) || (rr.nHeight <= 0))
        {
            size_request_t sr;
            wnd->size_request(&sr);
            lsp_trace("Size request: width=%d, height=%d", int(sr.nMinWidth), int(sr.nMinHeight));
            rr.nWidth   = sr.nMinWidth;
            rr.nHeight  = sr.nMinHeight;
        }

        lsp_trace("audioMasterSizeWindow width=%d, height=%d", int(rr.nWidth), int(rr.nHeight));
        if (((sRect.right - sRect.left) != rr.nWidth) ||
              ((sRect.bottom - sRect.top) != rr.nHeight))
            pMaster(pEffect, audioMasterSizeWindow, rr.nWidth, rr.nHeight, 0, 0);
    }

    void VSTWrapper::hide_ui()
    {
        destroy_ui();
    }

    void VSTWrapper::transfer_dsp_to_ui()
    {
//        lsp_trace("pUI = %p", pUI);
        // Get number of ports
        if (pUI == NULL)
            return;

        // Try to sync position
        pUI->position_updated(&sPosition);
        pUI->sync_meta_ports();

        // DSP -> UI communication
        for (size_t i=0, nports=vUIPorts.size(); i < nports; ++i)
        {
            // Get UI port
            VSTUIPort *vup          = vUIPorts[i];
            do {
                if (vup->sync())
                    vup->notify_all();
            } while (vup->sync_again());
        } // for port_id

        // Perform KVT synchronization
        if (sKVTMutex.try_lock())
        {
            // Synchronize DSP -> UI transfer
            size_t sync;
            const char *kvt_name;
            const kvt_param_t *kvt_value;

            do
            {
                sync = 0;

                KVTIterator *it = sKVT.enum_tx_pending();
                while (it->next() == STATUS_OK)
                {
                    kvt_name = it->name();
                    if (kvt_name == NULL)
                        break;
                    status_t res = it->get(&kvt_value);
                    if (res != STATUS_OK)
                        break;
                    if ((res = it->commit(KVT_TX)) != STATUS_OK)
                        break;

                    kvt_dump_parameter("TX kvt param (DSP->UI): %s = ", kvt_value, kvt_name);
                    pUI->kvt_write(&sKVT, kvt_name, kvt_value);
                    ++sync;
                }
            } while (sync > 0);

            // Synchronize UI -> DSP transfer
            #ifdef LSP_DEBUG
            {
                KVTIterator *it = sKVT.enum_rx_pending();
                while (it->next() == STATUS_OK)
                {
                    kvt_name = it->name();
                    if (kvt_name == NULL)
                        break;
                    status_t res = it->get(&kvt_value);
                    if (res != STATUS_OK)
                        break;
                    if ((res = it->commit(KVT_RX)) != STATUS_OK)
                        break;

                    kvt_dump_parameter("RX kvt param (UI->DSP): %s = ", kvt_value, kvt_name);
                }
            }
            #else
                sKVT.commit_all(KVT_RX);    // Just clear all RX queue for non-debug version
            #endif

            // Call garbage collection and release KVT storage
            sKVT.gc();
            sKVTMutex.unlock();
        }
    }

#endif

    #ifdef LSP_TRACE
        static void dump_vst_bank(const void *bank, size_t ck_size)
        {
            const uint8_t *ddump        = reinterpret_cast<const uint8_t *>(bank);
            lsp_trace("Chunk dump:");

            for (size_t offset=0; offset < ck_size; offset += 16)
            {
                // Print HEX dump
                lsp_nprintf("%08x: ", int(offset));
                for (size_t i=0; i<0x10; ++i)
                {
                    if ((offset + i) < ck_size)
                        lsp_nprintf("%02x ", int(ddump[i]));
                    else
                        lsp_nprintf("   ");
                }
                lsp_nprintf("   ");

                // Print character dump
                for (size_t i=0; i<0x10; ++i)
                {
                    if ((offset + i) < ck_size)
                    {
                        uint8_t c   = ddump[i];
                        if ((c < 0x20) || (c >= 0x80))
                            c           = '.';
                        lsp_nprintf("%c", c);
                    }
                    else
                        lsp_nprintf(" ");
                }
                lsp_printf("");

                // Move pointer
                ddump       += 0x10;
            }
        }
    #else
        #define dump_vst_bank(...)
    #endif /* LSP_TRACE */

    size_t VSTWrapper::serialize_state(const void **dst)
    {
        // Clear chunk
        sChunk.clear();

        // Write the bank header
        fxBank bank;
        ::bzero(&bank, sizeof(bank));

        bank.chunkMagic     = CPU_TO_BE(VstInt32(cMagic));
        bank.byteSize       = 0;
        bank.fxMagic        = CPU_TO_BE(VstInt32(chunkBankMagic));
        bank.version        = CPU_TO_BE(VstInt32(1));
        bank.fxID           = CPU_TO_BE(VstInt32(pEffect->uniqueID));
        bank.fxVersion      = CPU_TO_BE(VstInt32(2000)); // Version 2.0.0 of the bank
        bank.numPrograms    = 0;
        bank.currentProgram = 0;

        size_t bank_off     = sChunk.write(&bank, offsetof(fxBank, content.data.chunk));

        vst_state_header hdr;
        ::bzero(&hdr, sizeof(hdr));
        hdr.nMagic1         = CPU_TO_BE(VstInt32(LSP_VST_USER_MAGIC));
        hdr.nSize           = 0;
        hdr.nVersion        = CPU_TO_BE(VstInt32(VST_FX_VERSION_JUCE_FIX));
        hdr.nMagic2         = CPU_TO_BE(VstInt32(LSP_VST_USER_MAGIC));
        size_t hdr_off      = sChunk.write(&hdr, sizeof(hdr));

        size_t data_off     = sChunk.offset;
        size_t param_off    = 0;

        // Serialize all regular ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            // Get VST port
            VSTPort *vp             = vPorts[i];
            if (vp == NULL)
                continue;

            // Get metadata
            const port_t *p         = vp->metadata();
            if ((p == NULL) || (p->id == NULL) || (IS_OUT_PORT(p)) || (!vp->serializable()))
                continue;

            // Check that port is serializable
            lsp_trace("Serializing port id=%s", p->id);

            // Write port data to the chunk
            param_off   = sChunk.write(uint32_t(0)); // Reserve space for size
            sChunk.write_string(p->id);     // ID of the port
            vp->serialize(&sChunk);         // Value of the port
            sChunk.write_at(param_off, uint32_t(sChunk.offset - param_off - sizeof(uint32_t))); // Write the actual size

            if (sChunk.res != STATUS_OK)
            {
                lsp_warn("Error serializing parameter is=%s, code=%d", p->id, int(sChunk.res));
                *dst = NULL;
                return 0;
            }
        }

        status_t res = STATUS_OK;

        // Serialize KVT storage
        if (sKVTMutex.lock())
        {
            const kvt_param_t *p;

            // Read the whole KVT storage
            KVTIterator *it = sKVT.enum_all();
            while (it->next() == STATUS_OK)
            {
                res             = it->get(&p);
                if (res == STATUS_NOT_FOUND) // Not a parameter
                    continue;
                else if (res != STATUS_OK)
                {
                    lsp_warn("it->get() returned %d", int(res));
                    break;
                }
                else if (it->is_transient()) // Skip transient parameters
                    continue;

                const char *name = it->name();
                if (name == NULL)
                {
                    lsp_trace("it->name() returned NULL");
                    break;
                }

                uint8_t flags = 0;
                if (it->is_private())
                    flags      |= LSP_VST_PRIVATE;

                kvt_dump_parameter("Saving state of KVT parameter: %s = ", p, name);

                param_off   = sChunk.write(uint32_t(0)); // Reserve space for size
                sChunk.write_string(name); // Name of the KVT parameter
                sChunk.write_byte(flags);

                // Serialize parameter according to it's type
                switch (p->type)
                {
                    case KVT_INT32:
                    {
                        sChunk.write_byte(LSP_VST_INT32);
                        sChunk.write(p->i32);
                        break;
                    };
                    case KVT_UINT32:
                    {
                        sChunk.write_byte(LSP_VST_UINT32);
                        sChunk.write(p->u32);
                        break;
                    }
                    case KVT_INT64:
                    {
                        sChunk.write_byte(LSP_VST_INT64);
                        sChunk.write(p->i64);
                        break;
                    };
                    case KVT_UINT64:
                    {
                        sChunk.write_byte(LSP_VST_UINT64);
                        sChunk.write(p->u64);
                        break;
                    }
                    case KVT_FLOAT32:
                    {
                        sChunk.write_byte(LSP_VST_FLOAT32);
                        sChunk.write(p->f32);
                        break;
                    }
                    case KVT_FLOAT64:
                    {
                        sChunk.write_byte(LSP_VST_FLOAT64);
                        sChunk.write(p->f64);
                        break;
                    }
                    case KVT_STRING:
                    {
                        sChunk.write_byte(LSP_VST_STRING);
                        sChunk.write_string((p->str != NULL) ? p->str : "");
                        break;
                    }
                    case KVT_BLOB:
                    {
                        if ((p->blob.size > 0) && (p->blob.data == NULL))
                        {
                            res = STATUS_INVALID_VALUE;
                            break;
                        }

                        sChunk.write_byte(LSP_VST_BLOB);
                        sChunk.write_string((p->blob.ctype != NULL) ? p->blob.ctype : "");
                        if (p->blob.size > 0)
                            sChunk.write(p->blob.data, p->blob.size);
                        break;
                    }

                    default:
                        res     = STATUS_BAD_TYPE;
                        break;
                }

                // Successful status?
                if (res != STATUS_OK)
                {
                    lsp_trace("it->name() returned NULL");
                    break;
                }

                // Complete the parameter size
                sChunk.write_at(param_off, uint32_t(sChunk.offset - param_off - sizeof(uint32_t))); // Write the actual size
            }

            sKVT.gc();
            sKVTMutex.unlock();
        }

        if (res != STATUS_OK)
        {
            *dst    = NULL;
            return 0;
        }

        // Issue callback
        pPlugin->state_saved();

        // Write the size of chunk
        fxBank *pbank               = sChunk.fetch<fxBank>(bank_off);
        VstInt32 size               = sChunk.offset - VST_BANK_HDR_SKIP;
        pbank->content.data.size    = CPU_TO_BE(VstInt32(sChunk.offset - hdr_off));
        pbank->byteSize             = CPU_TO_BE(size);

        vst_state_header *phdr      = sChunk.fetch<vst_state_header>(hdr_off);
        phdr->nSize                 = CPU_TO_BE(VstInt32(sChunk.offset - data_off));

        dump_vst_bank(pbank, sChunk.offset);
        lsp_trace("Plugin state has been saved");

        // Return result
        *dst = pbank;
        return sChunk.offset;
    }

    status_t VSTWrapper::check_vst_header(const fxBank *bank, size_t size)
    {
        // Validate size
        if (size < size_t(offsetof(fxBank, content.data.chunk)))
        {
            lsp_warn("block size too small (0x%08x bytes)", int(size));
            return STATUS_NOT_FOUND;
        }

        // Validate chunkMagic
        if (bank->chunkMagic != BE_TO_CPU(cMagic))
        {
            lsp_warn("bank->chunkMagic (%08x) != BE_DATA(VST_CHUNK_MAGIC) (%08x)", int(bank->chunkMagic), int(BE_TO_CPU(cMagic)));
            return STATUS_NOT_FOUND;
        }

        // Validate fxMagic
        if (bank->fxMagic != BE_TO_CPU(chunkBankMagic))
        {
            lsp_warn("bank->fxMagic (%08x) != BE_DATA(VST_OPAQUE_BANK_MAGIC) (%08x)", int(bank->fxMagic), int(BE_TO_CPU(chunkBankMagic)));
            return STATUS_UNSUPPORTED_FORMAT;
        }

        // Validate fxID
        if (bank->fxID != BE_TO_CPU(VstInt32(pEffect->uniqueID)))
        {
            lsp_warn("bank->fxID (%08x) != BE_DATA(VstInt32(pEffect->uniqueID)) (%08x)", int(bank->fxID), int(BE_TO_CPU(VstInt32(pEffect->uniqueID))));
            return STATUS_UNSUPPORTED_FORMAT;
        }

        // Validate the version
        VstInt32 version  = BE_TO_CPU(bank->version);
        if (version > pEffect->version)
        {
            lsp_warn("Unsupported effect version (%d)", version);
            return STATUS_UNSUPPORTED_FORMAT;
        }

        // Validate the numParams
        if (bank->numPrograms != 0)
        {
            lsp_warn("bank->numPrograms (%d) != 0", int(bank->numPrograms));
            return STATUS_UNSUPPORTED_FORMAT;
        }

        return STATUS_OK;
    }

    void VSTWrapper::deserialize_state(const void *data, size_t size)
    {
        const fxBank *bank          = reinterpret_cast<const fxBank *>(data);
        const uint8_t *head         = reinterpret_cast<const uint8_t *>(data);

        status_t res                = check_vst_header(bank, size);

        if (res == STATUS_OK)
        {
            lsp_warn("Found standard VST 2.x chunk header");
            dump_vst_bank(bank, (BE_TO_CPU(bank->byteSize) + 2 * sizeof(VstInt32)));

            // Check the version
            VstInt32 fxVersion = BE_TO_CPU(bank->fxVersion);
            if (fxVersion != VST_FX_VERSION_KVT_SUPPORT)
                deserialize_v1(bank);       // Load V1 bank for legacy support
            else
            {
                // Get size of chunk
                size_t bytes                    = BE_TO_CPU(VstInt32(bank->byteSize));
                if (bytes < offsetof(fxBank, content.data.chunk))
                {
                    lsp_trace("byte_size (%d) < VST_STATE_BUFFER_SIZE (%d)", int(bytes), int(VST_STATE_BUFFER_SIZE));
                    return;
                }

                const uint8_t *tail = &head[bytes + VST_BANK_HDR_SKIP];
                head               += offsetof(fxBank, content.data.chunk);
                bytes               = BE_TO_CPU(bank->content.data.size);
                if (size_t(tail - head) != bytes)
                {
                    lsp_trace("Content size=0x%x does not match specified=0x%x", int(tail-head), int(bytes));
                    return;
                }

                deserialize_new_chunk_format(head, bytes);
            }
        }
        else if (res == STATUS_NOT_FOUND)
        {
            // Do stuff considering that there is NO chunk headers, just raw data
            lsp_warn("No VST 2.x chunk header found, assuming the body is in valid state");
            dump_vst_bank(head, size);
            deserialize_new_chunk_format(head, size);
        }
        else
            return;

        // Call callback
        pPlugin->state_loaded();
        lsp_trace("Plugin state has been loaded");
    }

    void VSTWrapper::deserialize_new_chunk_format(const uint8_t *data, size_t bytes)
    {
        // Lookup extension header
        vst_state_header hdr;
        ::bzero(&hdr, sizeof(hdr));
        if (bytes >= sizeof(vst_state_header))
        {
            const vst_state_header *src = reinterpret_cast<const vst_state_header *>(data);
            hdr.nMagic1     = BE_TO_CPU(src->nMagic1);
            hdr.nSize       = BE_TO_CPU(src->nSize);
            hdr.nVersion    = BE_TO_CPU(src->nVersion);
            hdr.nMagic2     = BE_TO_CPU(src->nMagic2);
        }

        // Analyze version
        if ((hdr.nMagic1 != LSP_VST_USER_MAGIC) || (hdr.nMagic2 != LSP_VST_USER_MAGIC))
        {
            lsp_debug("Performing V2 parameter deserialization (0x%x bytes)", int(bytes));
            deserialize_v2_v3(data, bytes);
        }
        else if (hdr.nVersion >= VST_FX_VERSION_JUCE_FIX)
        {
            lsp_debug("Performing V3 parameter deserialization");
            deserialize_v2_v3(&data[sizeof(hdr)], hdr.nSize);
        }
        else
            lsp_warn("Unsupported format, don't know how to deserialize chunk");
    }

    VSTPort *VSTWrapper::find_by_id(const char *id)
    {
        for (size_t i=0; i< vPorts.size(); ++i)
        {
            // Get VST port
            VSTPort *sp             = vPorts[i];
            if (sp == NULL)
                continue;

            // Get port metadata
            const port_t *p         = sp->metadata();
            if ((p == NULL) || (p->id == NULL))
                continue;

            // Check that ID of the port matches
            if (!::strcmp(p->id, id))
                return sp;
        }

        return NULL;
    }

    void VSTWrapper::deserialize_v1(const fxBank *bank)
    {
        lsp_debug("Performing V1 parameter deserialization");

        // Get size of chunk
        size_t bytes                    = BE_TO_CPU(VstInt32(bank->byteSize));
        if (bytes < VST_STATE_BUFFER_SIZE)
        {
            lsp_trace("byte_size (%d) < VST_STATE_BUFFER_SIZE (%d)", int(bytes), int(VST_STATE_BUFFER_SIZE));
            return;
        }

        // Ready to de-serialize
        const vst_state *state  = reinterpret_cast<const vst_state *>(bank + 1);
        size_t params           = BE_TO_CPU(state->nItems);
        const uint8_t *ptr      = state->vData;
        const uint8_t *tail     = reinterpret_cast<const uint8_t *>(state) + bytes - sizeof(vst_state);
        char param_id[LSP_MAX_PARAM_ID_BYTES];

        while ((params--) > 0)
        {
            // Deserialize port ID
            ssize_t delta           = vst_deserialize_string(param_id, LSP_MAX_PARAM_ID_BYTES, ptr, tail - ptr);
            if (delta <= 0)
            {
                lsp_error("Bank data corrupted");
                return;
            }
            ptr                    += delta;

            // Find port
            lsp_trace("Deserializing port id=%s", param_id);
            VSTPort *vp             = find_by_id(param_id);
            if (vp == NULL)
            {
                lsp_error("Bank data corrupted: port id=%s not found", param_id);
                return;
            }

            // Deserialize port data
            delta                   = vp->deserialize_v1(ptr, tail - ptr);
            if (delta <= 0)
            {
                lsp_error("bank data corrupted, could not deserialize port id=%s", param_id);
                return;
            }
            ptr                    += delta;
        }
    }

    void VSTWrapper::deserialize_v2_v3(const uint8_t *data, size_t bytes)
    {
        const uint8_t *head = data;
        const uint8_t *tail = &head[bytes];

        lsp_debug("Reading regular ports...");
        while (size_t(tail - head) >= sizeof(uint32_t))
        {
            // Read parameter length
            uint32_t len        = BE_TO_CPU(*(reinterpret_cast<const uint32_t *>(head))) + sizeof(uint32_t);
            if (len > size_t(tail - head))
            {
                lsp_warn("Unexpected end of chunk while fetching parameter size");
                return;
            }
            const uint8_t *next = &head[len];

            // Read name of port
            head               += sizeof(uint32_t);
            const char *name    = reinterpret_cast<const char *>(head);
            len                 = ::strnlen(name, next - head) + 1;
            if (len > size_t(next - head))
            {
                lsp_warn("Unexpected end of chunk while fetching parameter name");
                return;
            }
            if (name[0] == '/') // This is KVT port?
            {
                head               -= sizeof(uint32_t); // Rollback head pointer
                break;
            }
            head               += len;

            // Find port
            lsp_trace("Deserializing port id=%s", name);
            VSTPort *vp             = find_by_id(name);
            if (vp == NULL)
            {
                lsp_warn("Port id=%s not found, skipping", name);
                head        = next;
                continue;
            }

            // Deserialize port
            if (!vp->deserialize_v2(head, next - head))
            {
                lsp_warn("Error deserializing port %s, skipping", name);
                head        = next;
                continue;
            }

            // Move to next parameter
            head        = next;
        }

        // Nothing to de-serialize more?
        if (head >= tail)
            return;

        // Deserialize KVT state
        lsp_debug("Reading KVT ports...");
        if (sKVTMutex.lock())
        {
            sKVT.clear();

            while (size_t(tail - head) >= sizeof(uint32_t))
            {
                // Read parameter length
                uint32_t len        = BE_TO_CPU(*(reinterpret_cast<const uint32_t *>(head))) + sizeof(uint32_t);
                lsp_trace("Reading block: off=0x%x, size=%d", int(head - data), int(len));
                if (len > size_t(tail - head))
                {
                    lsp_warn("Unexpected end of chunk while fetching KVT parameter size");
                    break;
                }
                const uint8_t *next = &head[len];

                // Read name of parameter
                head               += sizeof(uint32_t);
                const char *name    = reinterpret_cast<const char *>(head);
                len                 = ::strnlen(name, next - head) + 1;
                if (len > size_t(next - head))
                {
                    lsp_warn("Unexpected end of chunk while fetching KVT parameter name");
                    break;
                }
                head               += len;

                // Deserialize KVT parameter
                kvt_param_t p;
                p.type              = KVT_ANY;
                uint8_t flags       = *(head++);
                uint8_t type        = *(head++);

                lsp_trace("Deserializing KVT parameter id=%s, type=0x%x", name, int(type));

                switch (type)
                {
                    case LSP_VST_INT32:
                        if ((next - head) != sizeof(int32_t))
                            break;
                        p.type      = KVT_INT32;
                        p.i32       = BE_TO_CPU(*(reinterpret_cast<const int32_t *>(head)));
                        head       += sizeof(int32_t);
                        break;

                    case LSP_VST_UINT32:
                        if ((next - head) != sizeof(uint32_t))
                            break;
                        p.type      = KVT_UINT32;
                        p.u32       = BE_TO_CPU(*(reinterpret_cast<const uint32_t *>(head)));
                        head       += sizeof(uint32_t);
                        break;

                    case LSP_VST_INT64:
                        if ((next - head) != sizeof(int64_t))
                            break;
                        p.type      = KVT_INT64;
                        p.i64       = BE_TO_CPU(*(reinterpret_cast<const int64_t *>(head)));
                        head       += sizeof(int64_t);
                        break;

                    case LSP_VST_UINT64:
                        if ((next - head) != sizeof(uint64_t))
                            break;
                        p.type      = KVT_UINT64;
                        p.u64       = BE_TO_CPU(*(reinterpret_cast<const uint64_t *>(head)));
                        head       += sizeof(uint64_t);
                        break;

                    case LSP_VST_FLOAT32:
                        if ((next - head) != sizeof(float))
                            break;
                        p.type      = KVT_FLOAT32;
                        p.f32       = BE_TO_CPU(*(reinterpret_cast<const float *>(head)));
                        head       += sizeof(float);
                        break;

                    case LSP_VST_FLOAT64:
                        if ((next - head) != sizeof(double))
                            break;
                        p.type      = KVT_FLOAT64;
                        p.f64       = BE_TO_CPU(*(reinterpret_cast<const double *>(head)));
                        head       += sizeof(double);
                        break;

                    case LSP_VST_STRING:
                        p.str       = reinterpret_cast<const char *>(head);
                        if (::strnlen(p.str, next-head) < size_t(next - head))
                            p.type      = KVT_STRING;
                        break;

                    case LSP_VST_BLOB:
                        p.blob.ctype    = reinterpret_cast<const char *>(head);
                        len             = ::strnlen(p.blob.ctype, next-head) + 1;
                        if (len > size_t(next - head))
                        {
                            lsp_trace("BLOB: clen=%d out of range %d", int(len), int(next-head));
                            break;
                        }

                        head           += len;
                        p.blob.size     = next - head;
                        p.blob.data     = (p.blob.size > 0) ? head : NULL;
                        p.type          = KVT_BLOB;
                        break;
                    default:
                        lsp_warn("Unknown KVT parameter type: %d ('%c') for id=%s", type, type, name);
                        break;
                }

                if (p.type != KVT_ANY)
                {
                    size_t kflags = KVT_TX;
                    if (flags & LSP_VST_PRIVATE)
                        kflags     |= KVT_PRIVATE;

                    kvt_dump_parameter("Fetched parameter %s = ", &p, name);
                    sKVT.put(name, &p, kflags);
                }

                // Move to next parameter
                head        = next;
            }

            sKVT.gc();
            sKVTMutex.unlock();
        }
    }

    void VSTWrapper::set_bypass(bool bypass)
    {
        pBypass->writeValue((bypass) ? 1.0f : 0.0f);
    }

    ICanvas *VSTWrapper::create_canvas(ICanvas *&cv, size_t width, size_t height)
    {
        return NULL;
    }

    KVTStorage *VSTWrapper::kvt_lock()
    {
        return (sKVTMutex.lock()) ? &sKVT : NULL;
    }

    KVTStorage *VSTWrapper::kvt_trylock()
    {
        return (sKVTMutex.try_lock()) ? &sKVT : NULL;
    }

    bool VSTWrapper::kvt_release()
    {
        return sKVTMutex.unlock();
    }
}

#endif /* CONTAINER_VST_WRAPPER_H_ */
