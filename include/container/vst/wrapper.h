/*
 * wrapper.h
 *
 *  Created on: 08 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_VST_WRAPPER_H_
#define CONTAINER_VST_WRAPPER_H_

#include <container/vst/defs.h>
#include <core/NativeExecutor.h>

namespace lsp
{
    class VSTAudioPort;
    class VSTParameterPort;
    class VSTPort;
    class VSTUIPort;

    class VSTWrapper: public IWrapper, public IUIWrapper
    {
        private:
            plugin_t                   *pPlugin;
            AEffect                    *pEffect;
            plugin_ui                  *pUI;
            ERect                       sRect;
            audioMasterCallback         pMaster;
            IExecutor                  *pExecutor;
            vst_state_buffer           *pState;
            bool                        bUpdateSettings;
            float                       fLatency;

            cvector<VSTAudioPort>       vInputs;        // List of input audio ports
            cvector<VSTAudioPort>       vOutputs;       // List of output audio ports
            cvector<VSTParameterPort>   vParams;        // List of controllable parameters
            cvector<VSTPort>            vPorts;         // List of all created VST ports
            cvector<VSTPort>            vProxyPorts;    // List of all created VST proxy ports
            cvector<VSTUIPort>          vUIPorts;       // List of all created UI ports
            cvector<port_t>             vGenMetadata;   // Generated metadata

            position_t                  sPosition;

        private:
            void transfer_dsp_to_ui();

            VSTPort *create_port(const port_t *port, const char *postfix);
            void create_ports(const port_t *meta);

        protected:
            static status_t slot_ui_resize(LSPWidget *sender, void *ptr, void *data);

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
                pUI             = NULL;
                pMaster         = callback;
                pExecutor       = NULL;
                pState          = NULL;
                sRect.top       = 0;
                sRect.left      = 0;
                sRect.bottom    = 0;
                sRect.right     = 0;
                fLatency        = 0.0f;
                bUpdateSettings = true;

                position_t::init(&sPosition);
            }

            virtual ~VSTWrapper()
            {
                pPlugin         = NULL;
                pEffect         = NULL;
                pUI             = NULL;
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
                pPlugin->set_sample_rate(sr);
                bUpdateSettings = true;
            }

            inline void mains_changed(VstIntPtr value)
            {
                if (value)
                    pPlugin->activate();
                else
                    pPlugin->deactivate();
            }

            bool show_ui(void *root_widget);
            void hide_ui();
            void iterate_ui();
            void destroy_ui();
            void resize_ui(const realize_t *r);
            ERect *get_ui_rect();

            virtual IExecutor *get_executor()
            {
                lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
                if (pExecutor != NULL)
                    return pExecutor;

                lsp_trace("Creating native executor service");
                pExecutor       = new NativeExecutor();
                return pExecutor;
            }

            virtual const position_t *position()
            {
                return &sPosition;
            }

            virtual ICanvas *create_canvas(ICanvas *&cv, size_t width, size_t height);

            void init_state_chunk();
            size_t serialize_state(const void **dst);
            void deserialize_state(const void *data);
    };
}

// Here Port description should be included
#include <container/vst/ports.h>
#include <container/vst/ui_ports.h>

namespace lsp
{
    VSTPort *VSTWrapper::create_port(const port_t *port, const char *postfix)
    {
        VSTPort *vp = NULL;
        VSTUIPort *vup = NULL;

        switch (port->role)
        {
            case R_MESH:
                vp  = new VSTMeshPort(port, pEffect, pMaster);
                vup = new VSTUIMeshPort(port, vp);
                break;

            case R_FBUFFER:
                vp  = new VSTFrameBufferPort(port, pEffect, pMaster);
                vup = new VSTUIFrameBufferPort(port, vp);
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

            case R_PATH:
                vp  = new VSTPathPort(port, pEffect, pMaster);
                vup = new VSTUIPathPort(port, vp);
                break;

            case R_AUDIO:
                vp = new VSTAudioPort(port, pEffect, pMaster);
                break;

            case R_CONTROL:
            case R_METER:
                // VST specifies only INPUT parameters, output should be read in different way
                if (IS_OUT_PORT(port))
                {
                    vp      = new VSTMeterPort(port, pEffect, pMaster);
                    vup     = new VSTUIMeterPort(port, vp);
                }
                else
                {
                    vp      = new VSTParameterPort(port, pEffect, pMaster);
                    vup     = new VSTUIParameterPort(port, static_cast<VSTParameterPort *>(vp));
                }
                break;

            case R_PORT_SET:
            {
                char postfix_buf[LSP_MAX_PARAM_ID_BYTES];
                VSTPortGroup       *pg      = new VSTPortGroup(port, pEffect, pMaster);
                pPlugin->add_port(pg);

                VSTUIPortGroup     *upg     = new VSTUIPortGroup(pg);
                vUIPorts.add(upg);

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
        if (vup != NULL)
            vUIPorts.add(vup);

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

        // Update instance parameters
        e->numInputs                    = vInputs.size();
        e->numOutputs                   = vOutputs.size();
        e->numParams                    = vParams.size();

        // Generate IDs for parameter ports
        for (ssize_t id=0; id < e->numParams; ++id)
            vParams[id]->setID(id);

        // Initialize state chunk
        init_state_chunk();

        // Initialize plugin
        pPlugin->init(this);
    }

    void VSTWrapper::destroy()
    {
        // First destroy the UI
        destroy_ui();

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
        for (size_t i=0; i<vUIPorts.size(); ++i)
        {
            lsp_trace("destroy ui port id=%s", vUIPorts[i]->metadata()->id);
            delete vUIPorts[i];
        }

        // Destroy ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            lsp_trace("destroy port id=%s", vPorts[i]->metadata()->id);
            delete vPorts[i];
        }

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
        vPorts.clear();
        vUIPorts.clear();

        if (pState != NULL)
        {
            lsp_trace("Destroy state %p", pState);
            delete [] reinterpret_cast<uint8_t *>(pState);
            pState          = NULL;
        }

        pMaster     = NULL;
        pEffect     = NULL;

        lsp_trace("destroy complete");
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
//                float(npos.sampleRate), long(npos.frame), float(npos.numerator), float(npos.denominator), float(npos.tick), float(npos.beatsPerMinute));

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

        // Sync UI state
        if (pUI != NULL)
        {
            if (!pPlugin->ui_active())
                pPlugin->activate_ui();
        }
        else if (pPlugin->ui_active())
            pPlugin->deactivate_ui();

        // Synchronize position
        sync_position();

        // Bind audio ports
        size_t n_inputs = vInputs.size();
        for (size_t i=0; i < n_inputs; ++i)
        {
            VSTAudioPort *p = vInputs.at(i);
            if (p != NULL)
                p->bind(inputs[i]);
        }
        size_t n_outputs = vOutputs.size();
        for (size_t i=0; i < n_outputs; ++i)
        {
            VSTAudioPort *p = vOutputs.at(i);
            if (p != NULL)
                p->bind(outputs[i]);
        }

        // Process ALL ports for changes
        size_t n_ports = vPorts.size();
        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            VSTPort *port = vPorts.at(i);
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
            VSTPort *port = vPorts.at(i);
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

    bool VSTWrapper::show_ui(void *root_widget)
    {
        lsp_trace("show ui");
        const plugin_metadata_t *m  = pPlugin->get_metadata();

        if (pUI == NULL)
        {
            // Create UI pointer
            lsp_trace("create ui");
            pUI                         = new plugin_ui(m, root_widget);
            if (pUI == NULL)
                return false;

            // Add pre-generated ports
            for (size_t i=0; i<vUIPorts.size(); ++i)
            {
                VSTUIPort  *vp      = vUIPorts[i];
                lsp_trace("Adding UI port id=%s", vp->metadata()->id);
                vp->resync();
                pUI->add_port(vp);
            }

            // Initialize UI
            lsp_trace("init ui");
            pUI->init(this, 0, NULL);

            LSPWindow *wnd  = pUI->root_window();
            if (wnd != NULL)
                wnd->slots()->bind(LSPSLOT_RESIZE, slot_ui_resize, this);
        }

        pUI->show();

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
//
//        wnd->set_width(sr.nMinWidth);
//        wnd->set_height(sr.nMinHeight);

//        // Show window
//        lsp_trace("create widget hierarchy pWidget=%p", pWidget);
//        gtk_widget_show_all(pWidget);
//        gdk_display_sync(gdk_display_get_default());


        // Transfer state
        transfer_dsp_to_ui();

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

        size_request_t sr;
        wnd->size_request(&sr);
        lsp_trace("Size request width=%d, height=%d", int(sr.nMinWidth), int(sr.nMinHeight));

        ssize_t r_width     = sr.nMinWidth;
        ssize_t r_height    = sr.nMinHeight;

        lsp_trace("audioMasterSizeWindow width=%d, height=%d", int(r_width), int(r_height));
        if (((sRect.right - sRect.left) != r_width) ||
              ((sRect.bottom - sRect.top) != r_height))
            pMaster(pEffect, audioMasterSizeWindow, r_width, r_height, 0, 0);
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

//        LSPWindow *wnd  = pUI->root_window();
//        if ((wnd != NULL) && (wnd->size_request_pending()))
//        {
//            size_request_t sr;
//            wnd->size_request(&sr);
//            sRect.top       = 0;
//            sRect.left      = 0;
//            sRect.right     = sr.nMinWidth;
//            sRect.bottom    = sr.nMinHeight;
//            lsp_trace("Window request width=%d, height=%d", int(sr.nMinWidth), int(sr.nMinHeight));
//            pMaster(pEffect, audioMasterSizeWindow, sr.nMinWidth, sr.nMinHeight, 0, 0);
//
//            wnd->query_draw();
//            realize_t r;
//            r.nLeft         = 0;
//            r.nTop          = 0;
//            r.nWidth        = sr.nMinWidth;
//            r.nHeight       = sr.nMinHeight;
//            wnd->set_geometry(&r);
//
////            sr.nMaxWidth    = sr.nMinWidth;
////            sr.nMaxHeight   = sr.nMinHeight;
////
////            wnd->set_size_constraints(&sr);
//            wnd->realize(&r);
////            wnd->query_draw();
////            wnd->set_geometry(&r);
//        }

        // Try to sync position
        pUI->position_updated(&sPosition);

        // DSP -> UI communication
        for (size_t i=0, nports=vUIPorts.size(); i < nports; ++i)
        {
            // Get UI port
            VSTUIPort *vup          = vUIPorts[i];
            if ((vup != NULL) && (vup->sync()))
                vup->notify_all();
        } // for port_id
    }

    void VSTWrapper::init_state_chunk()
    {
        // Calculate the overall maximum size of the chunk
        size_t chunk_size       = 0;
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            size_t p_size           = vPorts[i]->serial_size();
            if (p_size > 0)
            {
                chunk_size             += p_size;
                chunk_size             += LSP_MAX_PARAM_ID_BYTES;
            }
        }

        // Allocate chunk
        if (chunk_size <= 0)
            return;
        chunk_size                      = ALIGN_SIZE(chunk_size, DEFAULT_ALIGN);
        size_t alloc_size               = chunk_size + sizeof(vst_state_buffer);
        uint8_t *data                   = new uint8_t[alloc_size];
        if (data == NULL)
            return;

        // Update state
        pEffect->flags                 |= effFlagsProgramChunks;
        pState                          = reinterpret_cast<vst_state_buffer *>(data);

        // Initialize state with constant values
        pState->nDataSize               = chunk_size;

        memset(&pState->sHeader, 0x00, sizeof(fxBank));
        pState->sHeader.chunkMagic      = CPU_TO_BE(VstInt32(cMagic));
        pState->sHeader.byteSize        = 0;
        pState->sHeader.fxMagic         = CPU_TO_BE(VstInt32(chunkBankMagic));
        pState->sHeader.version         = CPU_TO_BE(1);
        pState->sHeader.fxID            = CPU_TO_BE(VstInt32(pEffect->uniqueID));
        pState->sHeader.fxVersion       = CPU_TO_BE(VstInt32(pEffect->version));
        pState->sHeader.numPrograms     = 0;

        pState->sState.nItems           = 0;
    }


    #ifdef LSP_TRACE
        static void dump_vst_bank(const fxBank *bank)
        {
            size_t ck_size              = BE_TO_CPU(bank->byteSize) + 2 * sizeof(VstInt32);

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
        if (pState == NULL)
            return 0;

        uint8_t *ptr                    = pState->sState.vData;
        uint8_t *tail                   = reinterpret_cast<uint8_t *>(&pState->sState) + pState->nDataSize;
        size_t params                   = 0;

        for (size_t i=0; i<vPorts.size(); ++i)
        {
            // Get VST port
            VSTPort *vp             = vPorts[i];
            if (vp == NULL)
                continue;

            // Get metadata
            const port_t *p         = vp->metadata();
            if ((p == NULL) || (p->id == NULL) || (IS_OUT_PORT(p)))
                continue;

            // Check that port is serializable
            size_t p_size           = vp->serial_size();
            if (p_size <= 0)
                continue;

            lsp_trace("Serializing port id=%s", p->id);

            // Write ID of the port
            ssize_t delta           = vst_serialize_string(p->id, ptr, tail - ptr);
            if (delta < 0)
            {
                lsp_error("Error serializing port id=%s", p->id);
                return 0;
            }
            ptr                    += delta;

            // Serialize port
            delta                   = vp->serialize(ptr, tail - ptr);
            if (delta < 0)
            {
                lsp_error("Error serializing port id=%s", p->id);
                return 0;
            }
            ptr                    += delta;

            // Increment number of params
            params                  ++;
        }

        // Write the size of chunk
        pState->sState.nItems           = CPU_TO_BE(uint32_t(params));
        pState->sHeader.byteSize        = CPU_TO_BE(VstInt32(ptr - reinterpret_cast<uint8_t *>(&pState->sState) + VST_BANK_HDR_SIZE));
        size_t ck_size                  = ptr - reinterpret_cast<uint8_t *>(&pState->sHeader);

        dump_vst_bank(&pState->sHeader);

        // Return result
        *dst = &pState->sHeader;
        return ck_size;
    }

    void VSTWrapper::deserialize_state(const void *data)
    {
        const fxBank *bank          = reinterpret_cast<const fxBank *>(data);
        dump_vst_bank(bank);

        // Validate chunkMagic
        if (bank->chunkMagic != BE_TO_CPU(cMagic))
        {
            lsp_trace("bank->chunkMagic (%08x) != BE_DATA(VST_CHUNK_MAGIC) (%08x)", int(bank->chunkMagic), int(BE_TO_CPU(cMagic)));
            return;
        }

        // Get size of chunk
        size_t byte_size                = BE_TO_CPU(VstInt32(bank->byteSize));
        if (byte_size < VST_STATE_BUFFER_SIZE)
        {
            lsp_trace("byte_size (%d) < VST_STATE_BUFFER_SIZE (%d)", int(byte_size), int(VST_STATE_BUFFER_SIZE));
            return;
        }

        // Validate fxMagic
        if (bank->fxMagic != BE_TO_CPU(chunkBankMagic))
        {
            lsp_trace("bank->fxMagic (%08x) != BE_DATA(VST_OPAQUE_BANK_MAGIC) (%08x)", int(bank->fxMagic), int(BE_TO_CPU(chunkBankMagic)));
            return;
        }

        // Validate fxID
        if (bank->fxID != BE_TO_CPU(VstInt32(pEffect->uniqueID)))
        {
            lsp_trace("bank->fxID (%08x) != BE_DATA(VstInt32(pEffect->uniqueID)) (%08x)", int(bank->fxID), int(BE_TO_CPU(VstInt32(pEffect->uniqueID))));
            return;
        }

        // Validate the version
        VstInt32 version  = BE_TO_CPU(bank->version);
        if (version > pEffect->version)
        {
            lsp_error("Unsupported effect version (%d)", version);
            return;
        }

        // Validate the numParams
        if (bank->numPrograms != 0)
        {
            lsp_trace("bank->numPrograms (%d) != 0", int(bank->numPrograms));
            return;
        }

        // Ready to de-serialize
        const vst_state *state  = reinterpret_cast<const vst_state *>(bank + 1);
        size_t params           = BE_TO_CPU(state->nItems);
        const uint8_t *ptr      = state->vData;
        const uint8_t *tail     = reinterpret_cast<const uint8_t *>(state) + byte_size - sizeof(vst_state);
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
            VSTPort *vp             = NULL;
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
                if (!strcmp(p->id, param_id))
                {
                    vp                      = sp;
                    break;
                }
            }

            if (vp == NULL)
            {
                lsp_error("Bank data corrupted: port id=%s not found", param_id);
                return;
            }

            // Deserialize port data
            delta                   = vp->deserialize(ptr, tail - ptr);
            if (delta <= 0)
            {
                lsp_error("bank data corrupted, could not deserialize port id=%s", param_id);
                return;
            }
            ptr                    += delta;
        }
    }

    ICanvas *VSTWrapper::create_canvas(ICanvas *&cv, size_t width, size_t height)
    {
        return NULL;
    }
}

#endif /* CONTAINER_VST_WRAPPER_H_ */
