/*
 * wrapper.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_WRAPPER_H_
#define CONTAINER_LV2_WRAPPER_H_

#include <data/cvector.h>
#include <data/chashmap.h>

#include <dsp/endian.h>
#include <core/IWrapper.h>
#include <core/ipc/NativeExecutor.h>
#include <core/KVTDispatcher.h>
#include <container/lv2/lv2_sink.h>

#ifndef LSP_NO_LV2_UI
    #include <container/CairoCanvas.h>
#endif

namespace lsp
{
    /** LV2 common plugin wrapper class
     *
     */
    class LV2Wrapper: public IWrapper
    {
        private:
            cvector<LV2Port>        vExtPorts;
            cvector<LV2Port>        vAllPorts;      // List of all created ports, for garbage collection
            cvector<LV2Port>        vPluginPorts;   // All plugin ports sorted in urid order
            cvector<LV2Port>        vMeshPorts;
            cvector<LV2Port>        vFrameBufferPorts;
            cvector<LV2Port>        vMidiInPorts;
            cvector<LV2Port>        vMidiOutPorts;
            cvector<LV2Port>        vOscInPorts;
            cvector<LV2Port>        vOscOutPorts;
            cvector<LV2AudioPort>   vAudioPorts;
            cvector<port_t>         vGenMetadata;   // Generated metadata

            plugin_t               *pPlugin;
            LV2Extensions          *pExt;
            ipc::IExecutor         *pExecutor;      // Executor service
            void                   *pAtomIn;        // Atom input port
            void                   *pAtomOut;       // Atom output port
            float                  *pLatency;       // Latency output port
            size_t                  nPatchReqs;     // Number of patch requests
            size_t                  nStateReqs;     // Number of state requests
            ssize_t                 nSyncTime;      // Synchronization time
            ssize_t                 nSyncSamples;   // Synchronization counter
            ssize_t                 nClients;       // Number of clients
            ssize_t                 nDirectClients; // Number of direct clients
            bool                    bQueueDraw;     // Queue draw request
            bool                    bUpdateSettings;// Settings update
            float                   fSampleRate;
            uint8_t                *pOscPacket;     // OSC packet data

            position_t              sPosition;
            KVTStorage              sKVT;
            ipc::Mutex              sKVTMutex;
            KVTDispatcher          *pKVTDispatcher;

#ifndef LSP_NO_LV2_UI
            CairoCanvas            *pCanvas;        // Canvas for drawing inline display
            LV2_Inline_Display_Image_Surface sSurface; // Canvas surface
#endif

        protected:
            LV2Port *create_port(const port_t *meta, const char *postfix);
            void create_ports(const port_t *meta);

            void receive_atoms(size_t samples);
            void transmit_atoms(size_t samples);
            static LV2Port *find_by_urid(cvector<LV2Port> &v, LV2_URID urid);
            static void sort_by_urid(cvector<LV2Port> &v);

            void parse_midi_event(const LV2_Atom_Event *ev);
            void parse_raw_osc_event(osc::parse_frame_t *frame);
            void parse_atom_object(const LV2_Atom_Event *ev);
            void serialize_midi_events(LV2Port *p);
            void transmit_osc_events(LV2Port *p);
            void transmit_kvt_events();
            void receive_kvt_events();
            void clear_midi_ports();

        public:
            inline explicit LV2Wrapper(plugin_t *plugin, LV2Extensions *ext)
            {
                pPlugin         = plugin;
                pExt            = ext;
                pExecutor       = NULL;
                pAtomIn         = NULL;
                pAtomOut        = NULL;
                pLatency        = NULL;
                nPatchReqs      = 0;
                nStateReqs      = 0;
                nSyncTime       = 0;
                nSyncSamples    = 0;
                nClients        = 0;
                nDirectClients  = 0;
#ifndef LSP_NO_LV2_UI
                pCanvas         = NULL;
                sSurface.data   = NULL;
                sSurface.width  = 0;
                sSurface.height = 0;
                sSurface.stride = 0;
#endif
                bQueueDraw      = false;
                bUpdateSettings = true;
                fSampleRate     = DEFAULT_SAMPLE_RATE;
                pOscPacket      = reinterpret_cast<uint8_t *>(::malloc(OSC_PACKET_MAX));
                pKVTDispatcher  = NULL;

                position_t::init(&sPosition);
            }

            virtual ~LV2Wrapper()
            {
                pPlugin         = NULL;
                pExt            = NULL;
                pExecutor       = NULL;
                pAtomIn         = NULL;
                pAtomOut        = NULL;
                pLatency        = NULL;
                nPatchReqs      = 0;
                nStateReqs      = 0;
                nSyncTime       = 0;
                nSyncSamples    = 0;
                nClients        = 0;
                nDirectClients  = 0;
#ifndef LSP_NO_LV2_UI
                pCanvas         = NULL;
                sSurface.data   = NULL;
                sSurface.width  = 0;
                sSurface.height = 0;
                sSurface.stride = 0;
#endif
            }

        public:
            // Basic LV2 part
            void init(float srate);
            void destroy();

            inline void activate()      {   pPlugin->activate();    }
            inline void deactivate()    {   pPlugin->deactivate();  }

            inline void connect(size_t id, void *data);
            inline void run(size_t samples);

            // State part
            inline void save_state(
                LV2_State_Store_Function   store,
                LV2_State_Handle           handle,
                uint32_t                   flags,
                const LV2_Feature *const * features
            );

            inline void restore_state(
                LV2_State_Retrieve_Function retrieve,
                LV2_State_Handle            handle,
                uint32_t                    flags,
                const LV2_Feature *const *  features
            );

            // Job part
            virtual ipc::IExecutor *get_executor();

            inline void job_run(
                LV2_Worker_Respond_Handle   handle,
                LV2_Worker_Respond_Function respond,
                uint32_t                    size,
                const void*                 data
            )
            {
                LV2Executor *executor = static_cast<LV2Executor *>(pExecutor);
                executor->run_job(handle, respond, size, data);
            }

            inline void job_response(size_t size, const void *body) {}
            inline void job_end() {}

            // Inline display part
            LV2_Inline_Display_Image_Surface *render_inline_display(size_t width, size_t height);
            inline void query_display_draw()
            {
                bQueueDraw      = true;
            }

            virtual const position_t *position()
            {
                return &sPosition;
            }

#ifndef LSP_NO_LV2_UI
            virtual ICanvas *create_canvas(ICanvas *&cv, size_t width, size_t height);
#endif

            LV2Port *get_port(const char *id);

            void connect_direct_ui()
            {
                // Increment number of clients
                nDirectClients++;
                if (pKVTDispatcher != NULL)
                    pKVTDispatcher->connect_client();
            }

            void disconnect_direct_ui()
            {
                if (nDirectClients <= 0)
                    return;
                --nDirectClients;
                if (pKVTDispatcher != NULL)
                    pKVTDispatcher->disconnect_client();
            }

            inline float get_sample_rate() const { return fSampleRate; }

            virtual KVTStorage *kvt_lock();

            virtual KVTStorage *kvt_trylock();

            virtual bool kvt_release();

            inline KVTDispatcher *kvt_dispatcher() { return pKVTDispatcher; }
    };

    LV2Port *LV2Wrapper::create_port(const port_t *p, const char *postfix)
    {
        LV2Port *result = NULL;

        switch (p->role)
        {
            case R_MESH:
                if (pExt->atom_supported())
                {
                    result = new LV2MeshPort(p, pExt);
                    vMeshPorts.add(result);
                }
                else
                    result = new LV2Port(p, pExt);
                break;
            case R_FBUFFER:
                if (pExt->atom_supported())
                {
                    result = new LV2FrameBufferPort(p, pExt);
                    vFrameBufferPorts.add(result);
                }
                else
                    result = new LV2Port(p, pExt);
                break;
            case R_MIDI:
                if (pExt->atom_supported())
                    result = new LV2MidiPort(p, pExt);
                else
                    result = new LV2Port(p, pExt);
                break;
            case R_OSC:
                if (pExt->atom_supported())
                    result = new LV2OscPort(p, pExt);
                else
                    result = new LV2Port(p, pExt);
                break;
            case R_PATH:
                if (pExt->atom_supported())
                    result      = new LV2PathPort(p, pExt);
                else
                    result      = new LV2Port(p, pExt);
                break;

            case R_AUDIO:
            {
                LV2AudioPort *ap = new LV2AudioPort(p, pExt);
                vAudioPorts.add(ap);
                result          = ap;
                break;
            }

            case R_PORT_SET:
            {
                char postfix_buf[LSP_MAX_PARAM_ID_BYTES];
                LV2PortGroup   *pg      = new LV2PortGroup(p, pExt);
                pPlugin->add_port(pg);
                vPluginPorts.add(pg);

                for (size_t row=0; row<pg->rows(); ++row)
                {
                    // Generate postfix
                    snprintf(postfix_buf, sizeof(postfix_buf)-1, "%s_%d", (postfix != NULL) ? postfix : "", int(row));

                    // Clone port metadata
                    port_t *cm          = clone_port_metadata(p->members, postfix_buf);
                    if (cm == NULL)
                        continue;

                    vGenMetadata.add(cm);

                    size_t col          = 0;
                    for (; cm->id != NULL; ++cm, ++col)
                    {
                        if (IS_GROWING_PORT(cm))
                            cm->start    = cm->min + ((cm->max - cm->min) * row) / float(pg->rows());
                        else if (IS_LOWERING_PORT(cm))
                            cm->start    = cm->max - ((cm->max - cm->min) * row) / float(pg->rows());

                        LV2Port *p          = create_port(cm, postfix_buf);
                        if ((p != NULL) && (p->metadata()->role != R_PORT_SET))
                        {
                            vPluginPorts.add(p);
                            pPlugin->add_port(p);
                        }
                    }
                }

                result  = pg;
                break;
            }

            case R_CONTROL:
            case R_METER:
                if (IS_OUT_PORT(p))
                    result      = new LV2OutputPort(p, pExt);
                else
                    result      = new LV2InputPort(p, pExt);
                break;

            case R_BYPASS:
                if (IS_OUT_PORT(p))
                    result      = new LV2Port(p, pExt);
                else
                    result      = new LV2BypassPort(p, pExt);
                break;

            default:
                break;
        }

        // Register created port for garbage collection
        if (result != NULL)
            vAllPorts.add(result);

        return result;
    }

    void LV2Wrapper::create_ports(const port_t *meta)
    {
        for (const port_t *port = meta; port->id != NULL; ++port)
        {
            // Create port
            LV2Port *p = create_port(port, NULL);
            if (p == NULL)
                continue;

            // Register port
            switch (port->role)
            {
                case R_PORT_SET:
                    break;

                case R_MIDI:
                    pPlugin->add_port(p);
                    if (IS_OUT_PORT(port))
                        vMidiOutPorts.add(p);
                    else
                        vMidiInPorts.add(p);
                    break;
                case R_OSC:
                    pPlugin->add_port(p);
                    if (IS_OUT_PORT(port))
                        vOscOutPorts.add(p);
                    else
                        vOscInPorts.add(p);
                    break;
                case R_MESH:
                case R_FBUFFER:
                case R_PATH:
                    pPlugin->add_port(p);
                    vPluginPorts.add(p);
                    break;

                case R_AUDIO:
                case R_CONTROL:
                case R_METER:
                    p->set_id(pPlugin->ports_count());
                    pPlugin->add_port(p);
                    vPluginPorts.add(p);
                    vExtPorts.add(p);
                    lsp_trace("Added external port id=%s, external_id=%d", p->metadata()->id, int(vExtPorts.size() - 1));
                    break;

                case R_BYPASS:
                    p->set_id(pPlugin->ports_count());
                    pPlugin->add_port(p);
                    vPluginPorts.add(p);
                    vExtPorts.add(p);
                    lsp_trace("Added bypass port id=%s, external_id=%d", p->metadata()->id, int(vExtPorts.size() - 1));
                    break;

                default:
                    break;
            }
        }
    }

    void LV2Wrapper::sort_by_urid(cvector<LV2Port> &v)
    {
        ssize_t items = v.size();
        if (items < 2)
            return;

        for (ssize_t i=0; i<(items-1); ++i)
            for (ssize_t j=i+1; j<items; ++j)
                if (v.at(i)->get_urid() > v.at(j)->get_urid())
                    v.swap(i, j);
    }

    void LV2Wrapper::init(float srate)
    {
        // Update sample rate
        fSampleRate = srate;

        // Get plugin metadata
        const plugin_metadata_t *m  = pPlugin->get_metadata();

        // Bind ports
        lsp_trace("Binding ports");
        create_ports(m->ports);

        // Sort port lists
        sort_by_urid(vPluginPorts);
        sort_by_urid(vMeshPorts);
        sort_by_urid(vFrameBufferPorts);

        // Need to create and start KVT dispatcher?
        lsp_trace("Plugin extensions=0x%x", int(m->extensions));
        if (m->extensions & E_KVT_SYNC)
        {
            lsp_trace("Creating KVT dispatcher thread...");
            pKVTDispatcher         = new KVTDispatcher(&sKVT, &sKVTMutex);
            lsp_trace("Starting KVT dispatcher thread...");
            pKVTDispatcher->start();
        }

        // Initialize plugin
        lsp_trace("Initializing plugin");
        pPlugin->init(this);
        pPlugin->set_sample_rate(srate);
        bUpdateSettings     = true;

        // Update refresh rate
        nSyncSamples        = srate / pExt->ui_refresh_rate();
        nClients            = 0;
    }

    LV2Port *LV2Wrapper::find_by_urid(cvector<LV2Port> &v, LV2_URID urid)
    {
        // Try to find the corresponding port
        ssize_t first = 0, last = v.size() - 1;
        while (first <= last)
        {
            size_t center   = (first + last) >> 1;
            LV2Port *p      = v[center];
            if (urid == p->get_urid())
                return p;
            else if (urid < p->get_urid())
                last    = center - 1;
            else
                first   = center + 1;
        }
        return NULL;
    }

    LV2Port *LV2Wrapper::get_port(const char *id)
    {
        for (size_t i=0, n = vPluginPorts.size(); i<n; ++i)
        {
            LV2Port *p = vPluginPorts.at(i);
            if (p == NULL)
                continue;
            const port_t *meta = p->metadata();
            if (meta == NULL)
                continue;
            if (!strcmp(meta->id, id))
                return p;
        }
        return NULL;
    }

    void LV2Wrapper::receive_atoms(size_t samples)
    {
        // Update synchronization
        if (nSyncTime <= 0)
        {
            size_t n_ports  = vMeshPorts.size();
            for (size_t i=0; i<n_ports; ++i)
            {
                mesh_t *mesh    = vMeshPorts[i]->getBuffer<mesh_t>();
//                lsp_trace("Mesh id=%s is waiting=%s", vMeshPorts[i]->metadata()->id, (mesh->isWaiting()) ? "true" : "false");
                if ((mesh != NULL) && (mesh->isWaiting()))
                    mesh->cleanup();
            }
        }

        // Get sequence
        if (pAtomIn == NULL)
            return;

        const LV2_Atom_Sequence *seq = reinterpret_cast<const LV2_Atom_Sequence *>(pAtomIn);

//        lsp_trace("pSequence->atom.type (%d) = %s", int(pSequence->atom.type), pExt->unmap_urid(pSequence->atom.type));
//        lsp_trace("pSequence->atom.size = %d", int(pSequence->atom.size));

        for (
            const LV2_Atom_Event* ev = lv2_atom_sequence_begin(&seq->body);
            !lv2_atom_sequence_is_end(&seq->body, seq->atom.size, ev);
            ev = lv2_atom_sequence_next(ev)
        )
        {
//            lsp_trace("ev->body.type (%d) = %s", int(ev->body.type), pExt->unmap_urid(ev->body.type));
            if (ev->body.type == pExt->uridMidiEventType)
                parse_midi_event(ev);
            else if (ev->body.type == pExt->uridOscRawPacket)
            {
                osc::parser_t parser;
                osc::parser_frame_t root;
                status_t res = osc::parse_begin(&root, &parser, &ev[1], ev->body.size);
                if (res == STATUS_OK)
                {
                    parse_raw_osc_event(&root);
                    osc::parse_end(&root);
                    osc::parse_destroy(&parser);
                }
            }
            else if ((ev->body.type == pExt->uridObject) || (ev->body.type == pExt->uridBlank))
                parse_atom_object(ev);
        }
    }

    void LV2Wrapper::parse_midi_event(const LV2_Atom_Event *ev)
    {
        // Are there any MIDI input ports in plugin?
        if (vMidiInPorts.size() <= 0)
            return;

        // Decode MIDI event
        midi::event_t me;
        const uint8_t *bytes        = reinterpret_cast<const uint8_t*>(ev + 1);

        // Debug
        #ifdef LSP_TRACE
            #define TRACE_KEY(x)    case midi::MIDI_MSG_ ## x: evt_type = #x; break;
            lsp_trace("midi dump: %02x %02x %02x", int(bytes[0]), int(bytes[1]), int(bytes[2]));

            char tmp_evt_type[32];
            const char *evt_type = NULL;
            switch (me.type)
            {
                TRACE_KEY(NOTE_OFF)
                TRACE_KEY(NOTE_ON)
                TRACE_KEY(NOTE_PRESSURE)
                TRACE_KEY(NOTE_CONTROLLER)
                TRACE_KEY(PROGRAM_CHANGE)
                TRACE_KEY(CHANNEL_PRESSURE)
                TRACE_KEY(PITCH_BEND)
                TRACE_KEY(SYSTEM_EXCLUSIVE)
                TRACE_KEY(MTC_QUARTER)
                TRACE_KEY(SONG_POS)
                TRACE_KEY(SONG_SELECT)
                TRACE_KEY(TUNE_REQUEST)
                TRACE_KEY(END_EXCLUSIVE)
                TRACE_KEY(CLOCK)
                TRACE_KEY(START)
                TRACE_KEY(CONTINUE)
                TRACE_KEY(STOP)
                TRACE_KEY(ACTIVE_SENSING)
                TRACE_KEY(RESET)
                default:
                    snprintf(tmp_evt_type, sizeof(tmp_evt_type), "UNKNOWN(0x%02x)", int(me.type));
                    evt_type = tmp_evt_type;
                    break;
            }

            lsp_trace("MIDI Event: type=%s, timestamp=%ld", evt_type, (long)(me.timestamp));

            #undef TRACE_KEY

        #endif /* LSP_TRACE */

        if (midi::decode(&me, bytes) <= 0)
        {
            lsp_warn("Could not decode MIDI message");
            return;
        }

        // Put the event to the queue
        me.timestamp      = uint32_t(ev->time.frames);

        // For each MIDI port: add event to the queue
        for (size_t i=0, n=vMidiInPorts.size(); i<n; ++i)
        {
            LV2Port *midi   = vMidiInPorts.at(i);
            if (midi == NULL)
                continue;
            midi_t *buf     = midi->getBuffer<midi_t>();
            if (buf == NULL)
                continue;
            if (!buf->push(me))
                lsp_warn("MIDI event queue overflow");
        }
    }

    void LV2Wrapper::parse_raw_osc_event(osc::parse_frame_t *frame)
    {
        osc::parse_token_t token;
        status_t res = osc::parse_token(frame, &token);
        if (res != STATUS_OK)
            return;

        if (token == osc::PT_BUNDLE)
        {
            osc::parse_frame_t child;
            uint64_t time_tag;
            status_t res = osc::parse_begin_bundle(&child, frame, &time_tag);
            if (res != STATUS_OK)
                return;
            parse_raw_osc_event(&child); // Perform recursive call
            osc::parse_end(&child);
        }
        else if (token == osc::PT_MESSAGE)
        {
            const void *msg_start;
            size_t msg_size;
            const char *msg_addr;

            // Perform address lookup and routing
            status_t res = osc::parse_raw_message(frame, &msg_start, &msg_size, &msg_addr);
            if (res != STATUS_OK)
                return;

            lsp_trace("Received OSC message of %d bytes, address=%s", int(msg_size), msg_addr);
            osc::dump_packet(msg_start, msg_size);

            if (::strstr(msg_addr, "/KVT/") == msg_addr)
                pKVTDispatcher->submit(msg_start, msg_size);
            else
            {
                for (size_t i=0, n=vOscInPorts.size(); i<n; ++i)
                {
                    IPort *p = vOscInPorts.at(i);
                    if (p == NULL)
                        continue;

                    // Submit message to the buffer
                    osc_buffer_t *buf = p->getBuffer<osc_buffer_t>();
                    if (buf != NULL)
                        buf->submit(msg_start, msg_size);
                }
            }
        }
    }

    void LV2Wrapper::parse_atom_object(const LV2_Atom_Event *ev)
    {
            // Analyze object type
        const LV2_Atom_Object *obj = reinterpret_cast<const LV2_Atom_Object*>(&ev->body);
//        lsp_trace("obj->body.otype (%d) = %s", int(obj->body.otype), pExt->unmap_urid(obj->body.otype));
//        lsp_trace("obj->body.id (%d) = %s", int(obj->body.id), pExt->unmap_urid(obj->body.id));

        if ((obj->body.id == pExt->uridState) && (obj->body.otype == pExt->uridStateChange)) // State change
        {
            lsp_trace("triggered state change");
            size_t flags = 0;

            for (
                LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                body = lv2_atom_object_next(body)
            )
            {
                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));
                if ((body->key == pExt->uridStateFlags) && (body->value.type == pExt->forge.Int))
                    flags = (reinterpret_cast<LV2_Atom_Int *>(&body->value))->body;
                else
                {
                    // Try to find the corresponding port
                    LV2Port *p = find_by_urid(vPluginPorts, body->key);
                    if ((p != NULL) && (p->get_type_urid() == body->value.type))
                        p->deserialize(&body->value, flags);
                }
            }
        }
        else if ((obj->body.id == pExt->uridState) && (obj->body.otype == pExt->uridStateRequest)) // State request
        {
            lsp_trace("triggered state request");
            nStateReqs  ++;
        }
        else if (obj->body.otype == pExt->uridPatchGet) // PatchGet request
        {
            lsp_trace("triggered patch request");
            #ifdef LSP_TRACE
            for (
                LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                body = lv2_atom_object_next(body)
            )
            {
                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));
            }
            #endif /* LSP_TRACE */

            // Increment the number of patch requests
            nPatchReqs  ++;
        }
        else if (obj->body.otype == pExt->uridPatchSet) // PatchSet request
        {
            // Parse atom body
            const LV2_Atom_URID    *key     = NULL;
            const LV2_Atom         *value   = NULL;

            for (
                LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                body = lv2_atom_object_next(body)
            )
            {
                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key  == pExt->uridPatchProperty) && (body->value.type == pExt->uridAtomUrid))
                {
                    key     = reinterpret_cast<const LV2_Atom_URID *>(&body->value);
                    lsp_trace("body->value.body (%d) = %s", int(key->body), pExt->unmap_urid(key->body));
                }
                else if (body->key   == pExt->uridPatchValue)
                    value   = &body->value;

                if ((key != NULL) && (value != NULL))
                {
                    LV2Port *p = find_by_urid(vPluginPorts, body->value.type);
                    if ((p != NULL) && (p->get_type_urid() == value->type))
                        p->deserialize(value, 0); // No flags for simple PATCH message

                    key     = NULL;
                    value   = NULL;
                }
            }
        }
        else if (obj->body.otype == pExt->uridTimePosition) // Time position notification
        {
            position_t pos      = sPosition;

            pos.sampleRate      = fSampleRate;
            pos.ticksPerBeat    = DEFAULT_TICKS_PER_BEAT;

            for (
                LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                body = lv2_atom_object_next(body)
            )
            {
//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key == pExt->uridTimeFrame) && (body->value.type == pExt->forge.Long))
                    pos.frame           = (reinterpret_cast<LV2_Atom_Long *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeSpeed) && (body->value.type == pExt->forge.Float))
                    pos.speed           = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBeatsPerMinute) && (body->value.type == pExt->forge.Float))
                    pos.beatsPerMinute  = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBeatUnit) && (body->value.type == pExt->forge.Int))
                    pos.denominator     = (reinterpret_cast<LV2_Atom_Int *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBeatsPerBar) && (body->value.type == pExt->forge.Float))
                    pos.numerator       = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBarBeat) && (body->value.type == pExt->forge.Float))
                    pos.tick            = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body * pos.ticksPerBeat;
            }
//            lsp_trace("triggered timePosition event\n"
//                      "  frame      = %lld\n"
//                      "  speed      = %f\n"
//                      "  bpm        = %f\n"
//                      "  numerator  = %f\n"
//                      "  denominator= %f\n"
//                      "  tick       = %f\n",
//                      (long long)(pos.frame), pos.speed, pos.beatsPerMinute, pos.denominator, pos.numerator, pos.tick
//                    );

            // Call plugin callback and update position
            bUpdateSettings = pPlugin->set_position(&pos);
            sPosition = pos;
        }
        else if ((obj->body.otype == pExt->uridUINotification) && (obj->body.id == pExt->uridConnectUI))
        {
            nClients    ++;
            lsp_trace("UI has connected, current number of clients=%d", int(nClients));
            if (pKVTDispatcher != NULL)
                pKVTDispatcher->connect_client();

            // Notify all ports that UI has connected to backend
            for (size_t i=0, n = vAllPorts.size(); i<n; ++i)
            {
                LV2Port *p = vAllPorts.get(i);
                if (p != NULL)
                    p->ui_connected();
            }
        }
        else if ((obj->body.otype == pExt->uridUINotification) && (obj->body.id == pExt->uridDisconnectUI))
        {
            nClients    --;
            if (pKVTDispatcher != NULL)
                pKVTDispatcher->disconnect_client();
            lsp_trace("UI has disconnected, current number of clients=%d", int(nClients));
        }
        else
        {
            lsp_trace("Unknown object: \n"
                      "  ev->body.type (%d) = %s\n"
                      "  obj->body.otype (%d) = %s\n"
                      "  obj->body.id (%d) = %s",
                     int(ev->body.type), pExt->unmap_urid(ev->body.type),
                     int(obj->body.otype), pExt->unmap_urid(obj->body.otype),
                     int(obj->body.id), pExt->unmap_urid(obj->body.id)
             );
        }
    }

    void LV2Wrapper::serialize_midi_events(LV2Port *p)
    {
        midi_t  *midi   = p->getBuffer<midi_t>();
        if ((midi == NULL) || (midi->nEvents <= 0))  // There are no events ?
            return;

        midi->sort();   // Sort buffer chronologically

        // Serialize MIDI events
        LV2_Atom_Midi buf;
        buf.atom.type       = pExt->uridMidiEventType;

        for (size_t i=0; i<midi->nEvents; ++i)
        {
            const midi::event_t *me = &midi->vEvents[i];

            // Debug
            #ifdef LSP_TRACE
                #define TRACE_KEY(x)    case midi::MIDI_MSG_ ## x: evt_type = #x; break;

                char tmp_evt_type[32];
                const char *evt_type = NULL;
                switch (me->type)
                {
                    TRACE_KEY(NOTE_OFF)
                    TRACE_KEY(NOTE_ON)
                    TRACE_KEY(NOTE_PRESSURE)
                    TRACE_KEY(NOTE_CONTROLLER)
                    TRACE_KEY(PROGRAM_CHANGE)
                    TRACE_KEY(CHANNEL_PRESSURE)
                    TRACE_KEY(PITCH_BEND)
                    TRACE_KEY(SYSTEM_EXCLUSIVE)
                    TRACE_KEY(MTC_QUARTER)
                    TRACE_KEY(SONG_POS)
                    TRACE_KEY(SONG_SELECT)
                    TRACE_KEY(TUNE_REQUEST)
                    TRACE_KEY(END_EXCLUSIVE)
                    TRACE_KEY(CLOCK)
                    TRACE_KEY(START)
                    TRACE_KEY(CONTINUE)
                    TRACE_KEY(STOP)
                    TRACE_KEY(ACTIVE_SENSING)
                    TRACE_KEY(RESET)
                    default:
                        snprintf(tmp_evt_type, sizeof(tmp_evt_type), "UNKNOWN(0x%02x)", int(me->type));
                        evt_type = tmp_evt_type;
                        break;
                }

                lsp_trace("MIDI Event: type=%s, timestamp=%ld", evt_type, (long)(me->timestamp));

                #undef TRACE_KEY

            #endif /* LSP_TRACE */

            ssize_t size = midi::encode(buf.body, me);
            if (size <= 0)
            {
                lsp_error("Tried to serialize invalid MIDI event, error=%d", int(-size));
                continue;
            }
            buf.atom.size = size;

            lsp_trace("midi dump: %02x %02x %02x (%d: %d)",
                int(buf.body[0]), int(buf.body[1]), int(buf.body[2]), int(buf.atom.size), int(buf.atom.size + sizeof(LV2_Atom)));

            // Serialize object
            pExt->forge_frame_time(0);
            pExt->forge_raw(&buf.atom, sizeof(LV2_Atom) + buf.atom.size);
            pExt->forge_pad(sizeof(LV2_Atom) + buf.atom.size);
        }
    }

    void LV2Wrapper::transmit_osc_events(LV2Port *p)
    {
        osc_buffer_t *osc   = p->getBuffer<osc_buffer_t>();
        if (osc == NULL)  // There are no events ?
            return;

        size_t size;
        LV2_Atom atom;

        while (true)
        {
            // Try to fetch record from buffer
            status_t res = osc->fetch(pOscPacket, &size, OSC_PACKET_MAX);

            switch (res)
            {
                case STATUS_OK:
                {
                    lsp_trace("Transmitting OSC packet of %d bytes", int(size));
                    osc::dump_packet(pOscPacket, size);

                    atom.size       = size;
                    atom.type       = pExt->uridOscRawPacket;

                    pExt->forge_frame_time(0);
                    pExt->forge_raw(&atom, sizeof(LV2_Atom));
                    pExt->forge_raw(pOscPacket, size);
                    pExt->forge_pad(sizeof(LV2_Atom) + size);
                    break;
                }

                case STATUS_NO_DATA: // No more data to transmit
                    return;

                case STATUS_OVERFLOW:
                {
                    lsp_warn("Too large OSC packet in the buffer, skipping");
                    osc->skip();
                    break;
                }

                default:
                {
                    lsp_warn("OSC packet parsing error %d, skipping", int(res));
                    osc->skip();
                    break;
                }
            }
        }
    }

    void LV2Wrapper::transmit_kvt_events()
    {
        // We need to transmit data only if there are clients connected to the Atom port
        if ((pKVTDispatcher == NULL) || (nClients == 0))
            return;

        LV2_Atom atom;

        size_t size;
        while (true)
        {
            status_t res = pKVTDispatcher->fetch(pOscPacket, &size, OSC_PACKET_MAX);

            switch (res)
            {
                case STATUS_OK:
                {
                    lsp_trace("Transmitting OSC packet of %d bytes", int(size));
                    osc::dump_packet(pOscPacket, size);

                    atom.size       = size;
                    atom.type       = pExt->uridOscRawPacket;

                    pExt->forge_frame_time(0);
                    pExt->forge_raw(&atom, sizeof(LV2_Atom));
                    pExt->forge_raw(pOscPacket, size);
                    pExt->forge_pad(sizeof(LV2_Atom) + size);
                    break;
                }

                case STATUS_OVERFLOW:
                    lsp_warn("Received too big OSC packet, skipping");
                    pKVTDispatcher->skip();
                    break;

                case STATUS_NO_DATA:
                    return;

                default:
                    lsp_warn("Received error while deserializing KVT changes: %d", int(res));
                    return;
            }
        }
    }

    void LV2Wrapper::clear_midi_ports()
    {
        // Clear all MIDI_IN ports
        for (size_t i=0, n=vMidiInPorts.size(); i<n; ++i)
        {
            LV2Port *p      = vMidiInPorts.at(i);
            if ((p == NULL) || (p->metadata()->role != R_MIDI))
                continue;
            midi_t *midi    = p->getBuffer<midi_t>();
            if (midi != NULL)
                midi->clear();
        }

        // Clear all MIDI_OUT ports
        for (size_t i=0, n=vMidiOutPorts.size(); i<n; ++i)
        {
            LV2Port *p      = vMidiOutPorts.at(i);
            if ((p == NULL) || (p->metadata()->role != R_MIDI))
                continue;
            midi_t *midi    = p->getBuffer<midi_t>();
            if (midi != NULL)
                midi->clear();
        }
    }

    void LV2Wrapper::transmit_atoms(size_t samples)
    {
        // Get sequence
        if (pAtomOut == NULL)
            return;

        // Update synchronization time
        nSyncTime      -= samples;
        bool sync_req       = nSyncTime <= 0;
        if (sync_req)
        {
            nSyncTime      += nSyncSamples;

            // Check that queue_draw() request for inline display is pending
            if ((bQueueDraw) && (pExt->iDisplay != NULL))
            {
                pExt->iDisplay->queue_draw(pExt->iDisplay->handle);
                bQueueDraw      = false;
            }
        }

        // Check that patch request is pending
        bool patch_req  = nPatchReqs > 0;
        if (patch_req)
            nPatchReqs      --;

        // Check that state request is pending
        bool state_req  = nStateReqs > 0;
        if (state_req)
            nStateReqs      --;

        size_t bytes_out    = 0;

        // Initialize forge
        LV2_Atom_Sequence *sequence = reinterpret_cast<LV2_Atom_Sequence *>(pAtomOut);
        pExt->forge_set_buffer(sequence, sequence->atom.size);

        // Forge sequence header
        LV2_Atom_Forge_Frame    seq;
        LV2_Atom_Forge_Frame    frame;
        pExt->forge_sequence_head(&seq, 0);

        // For each MIDI port, serialize it's data
        for (size_t i=0, n_midi=vMidiOutPorts.size(); i<n_midi; ++i)
        {
            LV2Port *p      = vMidiOutPorts.at(i);
            if ((p == NULL) || (p->metadata()->role != R_MIDI))
                continue;
            serialize_midi_events(p);
        }

        // For each OSC port, serialize it's data
        for (size_t i=0, n_osc=vOscOutPorts.size(); i<n_osc; ++i)
        {
            LV2Port *p      = vOscOutPorts.at(i);
            if ((p == NULL) || (p->metadata()->role != R_OSC))
                continue;
            transmit_osc_events(p);
        }

        // Transmit KVT state
        transmit_kvt_events();

        // Serialize paths that are visible in global space
        size_t n_ports      = vExtPorts.size();
        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            LV2Port *p = vExtPorts.at(i);
            if ((p == NULL) || (p->metadata()->role != R_PATH))
                continue;
            else if (p->get_id() < 0) // Non-global paths are serialized via STATE CHANGE primitive
                continue;

            // Check that we need to transmit the value
            if ((!patch_req) && (!p->tx_pending()))
                continue;

            // Serialize path as patch
            lsp_trace("Serialize path id=%s, bytes_out=%d", p->metadata()->id, int(bytes_out));
            pExt->forge_frame_time(0); // Event header
            LV2_Atom *msg = pExt->forge_object(&frame, pExt->uridPatchMessage, pExt->uridPatchSet);
            pExt->forge_key(pExt->uridPatchProperty);
            pExt->forge_urid(p->get_urid());
            pExt->forge_key(pExt->uridPatchValue);
            p->serialize();
            pExt->forge_pop(&frame);
            bytes_out   += lv2_atom_total_size(msg);
        }

        // Allow transport only when there is at least one UI connected
        if (nClients > 0)
        {
            // Serialize time/position of plugin
            pExt->forge_frame_time(0); // Event header
            LV2_Atom *msg       = pExt->forge_object(&frame, 0, pExt->uridTimePosition);

            pExt->forge_key(pExt->uridTimeFrame);
            pExt->forge_long(int64_t(sPosition.frame));

            pExt->forge_key(pExt->uridTimeFrameRate);
            pExt->forge_float(fSampleRate);

            pExt->forge_key(pExt->uridTimeSpeed);
            pExt->forge_float(sPosition.speed);

            pExt->forge_key(pExt->uridTimeBarBeat);
            pExt->forge_float(sPosition.tick / sPosition.ticksPerBeat);

            pExt->forge_key(pExt->uridTimeBar);
            pExt->forge_long(0);

            pExt->forge_key(pExt->uridTimeBeatUnit);
            pExt->forge_int(int(sPosition.denominator));

            pExt->forge_key(pExt->uridTimeBeatUnit);
            pExt->forge_float(sPosition.numerator);

            pExt->forge_key(pExt->uridTimeBeatsPerMinute);
            pExt->forge_float(sPosition.beatsPerMinute);

            pExt->forge_pop(&frame);

            // Initialize byte counter
            n_ports             = vPluginPorts.size();
            bytes_out           = 0;
            msg                 = NULL;

            // Serialize pending for transmission ports
            for (size_t i=0; i<n_ports; ++i)
            {
                // Get port
                LV2Port *p = vPluginPorts[i];
                if (p == NULL)
                    continue;

                // Skip MESH, FBUFFER, PATH ports visible in global space
                switch (p->metadata()->role)
                {
                    case R_AUDIO:
                    case R_MIDI:
                    case R_UI_SYNC:
                    case R_MESH:
                    case R_FBUFFER:
                        continue;
                    case R_PATH:
                        if (p->get_id() >= 0) // Skip global PATH ports
                            continue;
                        break;
                    default:
                        break;
                }

                // Check that we need to transmit the value
                if ((!state_req) && (!p->tx_pending()))
                    continue;

                // Serialize value of the port
                lsp_trace("Serialize port id=%s, bytes_out=%d", p->metadata()->id, int(bytes_out));

                // Emit object header (if needed)
                if (msg == NULL)
                {
                    pExt->forge_frame_time(0);
                    msg         = pExt->forge_object(&frame, pExt->uridState, pExt->uridStateChange);
                }

                pExt->forge_key(p->get_urid());
                p->serialize();
                bytes_out       = lv2_atom_total_size(msg);

                // Emit object tail (if needed)
                if (bytes_out >= 0x1000)
                {
                    pExt->forge_pop(&frame);
                    msg         = NULL;
                    bytes_out   = 0;
                }
            }

            // Emit object tail (if needed)
            if (msg != NULL)
                pExt->forge_pop(&frame);

            // Serialize meshes (it's own primitive MESH)
            n_ports         = vMeshPorts.size();
            for (size_t i=0; i<n_ports; ++i)
            {
                LV2Port *p = vMeshPorts[i];
                if (p == NULL)
                    continue;
                if ((!sync_req) && (!p->tx_pending()))
                    continue;
                mesh_t *mesh = p->getBuffer<mesh_t>();
                if ((mesh == NULL) || (!mesh->containsData()))
                    continue;

//                lsp_trace("transmit mesh id=%s", p->metadata()->id);
                pExt->forge_frame_time(0);  // Event header
                msg         = pExt->forge_object(&frame, p->get_urid(), pExt->uridMeshType);
                p->serialize();
                pExt->forge_pop(&frame);
                bytes_out   += lv2_atom_total_size(msg);

                // Cleanup data of the mesh for refill
                mesh->markEmpty();
            }

            // Serialize frame buffers (it's own primitive FRAMEBUFFER)
            n_ports         = vFrameBufferPorts.size();
            for (size_t i=0; i<n_ports; ++i)
            {
                LV2Port *p = vFrameBufferPorts[i];
                if ((p == NULL) || (!p->tx_pending()))
                    continue;
                frame_buffer_t *fb= p->getBuffer<frame_buffer_t>();
                if (fb == NULL)
                    continue;

                pExt->forge_frame_time(0);  // Event header
                msg         = pExt->forge_object(&frame, p->get_urid(), pExt->uridFrameBufferType);
                p->serialize();
                pExt->forge_pop(&frame);
                bytes_out   += lv2_atom_total_size(msg);
            }
        }

        // Complete sequence
        pExt->forge_pop(&seq);
    }

    void LV2Wrapper::destroy()
    {
        // Stop KVT dispatcher
        if (pKVTDispatcher != NULL)
        {
            lsp_trace("Stopping KVT dispatcher thread...");
            pKVTDispatcher->cancel();
            pKVTDispatcher->join();
            delete pKVTDispatcher;
        }

#ifndef LSP_NO_LV2_UI
        // Drop surface
        sSurface.data           = NULL;
        sSurface.width          = 0;
        sSurface.height         = 0;
        sSurface.stride         = 0;

        // Drop canvas
        lsp_trace("canvas = %p", pCanvas);
        if (pCanvas != NULL)
        {
            pCanvas->destroy();
            delete pCanvas;
            pCanvas     = NULL;
        }
#endif

        // Shutdown and delete executor if exists
        if (pExecutor != NULL)
        {
            pExecutor->shutdown();
            delete pExecutor;
            pExecutor   = NULL;
        }

        // Drop plugin
        if (pPlugin != NULL)
        {
            pPlugin->destroy();
            delete pPlugin;

            pPlugin     = NULL;
        }

        // Cleanup ports
        for (size_t i=0; i < vAllPorts.size(); ++i)
        {
            lsp_trace("destroy port id=%s", vAllPorts[i]->metadata()->id);
            delete vAllPorts[i];
        }

        // Cleanup generated metadata
        for (size_t i=0; i<vGenMetadata.size(); ++i)
        {
            lsp_trace("destroy generated port metadata %p", vGenMetadata[i]);
            drop_port_metadata(vGenMetadata[i]);
        }

        vAllPorts.clear();
        vExtPorts.clear();
        vMeshPorts.clear();
        vMidiInPorts.clear();
        vMidiOutPorts.clear();
        vOscInPorts.clear();
        vOscOutPorts.clear();
        vFrameBufferPorts.clear();
        vPluginPorts.clear();
        vGenMetadata.clear();

        // Delete temporary buffer for OSC serialization
        if (pOscPacket != NULL)
        {
            ::free(pOscPacket);
            pOscPacket = NULL;
        }

        // Drop extensions
        if (pExt != NULL)
        {
            delete pExt;
            pExt        = NULL;
        }
    }

    inline void LV2Wrapper::connect(size_t id, void *data)
    {
        size_t ports_count  = vExtPorts.size();
        if (id < ports_count)
        {
            LV2Port *p      = vExtPorts.get(id);
            if (p != NULL)
                p->bind(data);
        }
        else
        {
            switch (id - ports_count)
            {
                case 0: pAtomIn     = data; break;
                case 1: pAtomOut    = data; break;
                case 2: pLatency    = reinterpret_cast<float *>(data); break;
                default:
                    lsp_warn("Unknown port number: %d", int(id));
                    break;
            }
        }
    }

    inline void LV2Wrapper::run(size_t samples)
    {
        // Activate/deactivate the UI
        ssize_t clients = nClients + nDirectClients;
        if (clients > 0)
        {
            if (!pPlugin->ui_active())
                pPlugin->activate_ui();
        }
        else if (pPlugin->ui_active())
            pPlugin->deactivate_ui();

        // First pre-process transport ports
        clear_midi_ports();
        receive_atoms(samples);

        // Pre-rocess regular ports
        size_t n_all_ports      = vAllPorts.size();
        LV2Port **v_all_ports   = vAllPorts.get_array();
        for (size_t i=0; i<n_all_ports; ++i)
        {
            // Get port
            LV2Port *port = v_all_ports[i];
            if (port == NULL)
                continue;

            // Pre-process data in port
            if (port->pre_process(samples))
            {
                lsp_trace("port changed: %s, value=%f", port->metadata()->id, port->getValue());
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

        // Call the main processing unit (split data buffers into chunks not greater than MaxBlockLength)
        size_t n_in_ports = vAudioPorts.size();
        for (size_t off=0; off < samples; )
        {
            size_t to_process = samples - off;
            if (to_process > pExt->nMaxBlockLength)
                to_process = pExt->nMaxBlockLength;

            for (size_t i=0; i<n_in_ports; ++i)
                vAudioPorts.at(i)->sanitize(off, to_process);
            pPlugin->process(to_process);

            off += to_process;
        }

        // Transmit atoms (if possible)
        transmit_atoms(samples);
        clear_midi_ports();

        // Post-process regular ports for changes
        for (size_t i=0; i<n_all_ports; ++i)
        {
            LV2Port *port = v_all_ports[i];
            if (port != NULL)
                port->post_process(samples);
        }

        // Transmit latency (if possible)
        if (pLatency != NULL)
        {
//            lsp_trace("Reporting latency: %d", int(pPlugin->get_latency()));
            *pLatency   = pPlugin->get_latency();
        }
        else
        {
            lsp_trace("Could not report latency, pLatency is NULL");
        }
    }

    ipc::IExecutor *LV2Wrapper::get_executor()
    {
        lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
        if (pExecutor != NULL)
            return pExecutor;

        // Create executor service
        if (pExt->sched != NULL)
        {
            lsp_trace("Creating LV2 executor service");
            pExecutor       = new LV2Executor(pExt->sched);
        }
        else
        {
            lsp_trace("Creating native executor service");
            ipc::NativeExecutor *exec = new ipc::NativeExecutor();
            if (exec == NULL)
                return NULL;
            status_t res = exec->start();
            if (res != STATUS_OK)
            {
                delete exec;
                return NULL;
            }
            pExecutor   = exec;
        }
        return pExecutor;
    }

    inline void LV2Wrapper::save_state(
        LV2_State_Store_Function   store,
        LV2_State_Handle           handle,
        uint32_t                   flags,
        const LV2_Feature *const * features)
    {
        pExt->init_state_context(store, NULL, handle, flags, features);

        // Save state of all ports
        size_t ports_count = vAllPorts.size();

        for (size_t i=0; i<ports_count; ++i)
        {
            // Get port
            LV2Port *lvp    = vAllPorts[i];
            if (lvp == NULL)
                continue;

            // Save state of port
            lvp->save();
        }

// This works, LV2:State does not allow to save objects
/*
        {
            uint8_t buf[0x100];
            LV2_Atom_Forge &forge   = pExt->forge;
            LV2_URID_Map *map       = pExt->map;
            LV2_Atom_Forge_Frame frame;

            lv2_atom_forge_set_buffer(&forge, buf, sizeof(buf));
            lv2_atom_forge_object(&forge, &frame, 0, map->map(map->handle, "http://lsp-plug.in/types/lv2/types#KVT"));
            lv2_atom_forge_key(&forge, map->map(map->handle, "http://lsp-plug.in/kvt/scene/selected"));
            lv2_atom_forge_int(&forge, int32_t(0));
            lv2_atom_forge_key(&forge, map->map(map->handle, "http://lsp-plug.in/kvt/scene/objects"));
            lv2_atom_forge_int(&forge, int32_t(0));
            lv2_atom_forge_key(&forge, map->map(map->handle, "http://lsp-plug.in/kvt/scene/object/0/enabled"));
            lv2_atom_forge_float(&forge, 1.0f);

            LV2_Atom *msg           = reinterpret_cast<LV2_Atom *>(buf);

            lv2_atom_forge_pop(&forge, &frame);
            store(handle, map->map(map->handle, "http://lsp-plug.in/plugins/lv2/room_builder_mono#KVT"),
                    &msg[1], msg->size,
                    msg->type, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
        }
*/

        // Save state of all KVT parameters
        const kvt_param_t *p;

        if (sKVTMutex.lock())
        {
            status_t res    = STATUS_OK;

            // We should use our own forge to prevent from race condition
            LV2_Atom_Forge forge;
            LV2_Atom_Forge_Frame frame;
            lv2_sink        sink(0x100);

            // Initialize sink
            lv2_atom_forge_init(&forge, pExt->map);
            lv2_atom_forge_set_sink(&forge, lv2_sink::sink, lv2_sink::deref, &sink);
            lv2_atom_forge_object(&forge, &frame, 0, pExt->uridKvtType);

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

                int flags       = 0;
                if (it->is_private())
                    flags          |= LSP_LV2_PRIVATE;

                const char *name = it->name();
                if (name == NULL)
                {
                    lsp_trace("it->name() returned NULL");
                    break;
                }

                kvt_dump_parameter("Saving state of KVT parameter: %s = ", p, name);

                LV2_URID key        =  pExt->map_kvt(name);

                LV2_Atom_Forge_Frame prop;
                lv2_atom_forge_key(&forge, key);
                lv2_atom_forge_object(&forge, &prop, 0, pExt->uridKvtPropertyType);
                lv2_atom_forge_key(&forge, pExt->uridKvtPropertyFlags);
                lv2_atom_forge_int(&forge, flags);

                switch (p->type)
                {
                    case KVT_INT32:
                    case KVT_UINT32:
                    {
                        LV2_Atom_Int v;
                        v.atom.type     = (p->type == KVT_INT32) ? pExt->forge.Int : pExt->uridTypeUInt;
                        v.atom.size     = sizeof(v) - sizeof(LV2_Atom);
                        v.body          = p->i32;

                        lv2_atom_forge_key(&forge, pExt->uridKvtPropertyValue);
                        lv2_atom_forge_primitive(&forge, &v.atom);
                        break;
                    }

                    case KVT_INT64:
                    case KVT_UINT64:
                    {
                        LV2_Atom_Long v;
                        v.atom.type     = (p->type == KVT_INT64) ? pExt->forge.Long : pExt->uridTypeULong;
                        v.atom.size     = sizeof(v) - sizeof(LV2_Atom);
                        v.body          = p->i64;
                        lv2_atom_forge_key(&forge, pExt->uridKvtPropertyValue);
                        lv2_atom_forge_primitive(&forge, &v.atom);
                        break;
                    }
                    case KVT_FLOAT32:
                    {
                        LV2_Atom_Float v;
                        v.atom.type     = pExt->forge.Float;
                        v.atom.size     = sizeof(v) - sizeof(LV2_Atom);
                        v.body          = p->f32;
                        lv2_atom_forge_key(&forge, pExt->uridKvtPropertyValue);
                        lv2_atom_forge_primitive(&forge, &v.atom);
                        break;
                    }
                    case KVT_FLOAT64:
                    {
                        LV2_Atom_Double v;
                        v.atom.type     = pExt->forge.Double;
                        v.atom.size     = sizeof(v) - sizeof(LV2_Atom);
                        v.body          = p->f64;
                        lv2_atom_forge_key(&forge, pExt->uridKvtPropertyValue);
                        lv2_atom_forge_primitive(&forge, &v.atom);
                        break;
                    }
                    case KVT_STRING:
                    {
                        lv2_atom_forge_key(&forge, pExt->uridKvtPropertyValue);
                        const char *str = (p->str != NULL) ? p->str : "";
                        lv2_atom_forge_typed_string(&forge, forge.String, str, ::strlen(str));
                        break;
                    }
                    case KVT_BLOB:
                    {
                        LV2_Atom_Forge_Frame obj;

                        lv2_atom_forge_key(&forge, pExt->uridKvtPropertyValue);
                        lv2_atom_forge_object(&forge, &obj, 0, pExt->uridBlobType);
                        {
                            if (p->blob.ctype != NULL)
                            {
                                lv2_atom_forge_key(&forge, pExt->uridContentType);
                                lv2_atom_forge_typed_string(&forge, forge.String, p->blob.ctype, ::strlen(p->blob.ctype));
                            }

                            uint32_t size = ((p->blob.size > 0) && (p->blob.data != NULL)) ? p->blob.size : 0;
                            lv2_atom_forge_key(&forge, pExt->uridContent);
                            lv2_atom_forge_atom(&forge, size, forge.Chunk);
                            if (size > 0)
                                lv2_atom_forge_write(&forge, p->blob.data, size);
                        }
                        lv2_atom_forge_pop(&forge, &obj);
                        break;
                    }

                    default:
                        res     = STATUS_BAD_TYPE;
                        break;
                }

                lv2_atom_forge_pop(&forge, &prop);

                // Successful status?
                if (res != STATUS_OK)
                    break;
            } // while

            if ((res == STATUS_OK) && (sink.res == STATUS_OK))
            {
                // TEST
                /*{
                    kvt_param_t xp;
                    xp.blob.ctype   = "text/plain";
                    xp.blob.data    = "Test text";
                    xp.blob.size    = strlen("Test text") + 1;
                    p = &xp;

                    LV2_Atom_Forge_Frame obj;
                    LV2_URID key        =  pExt->map_kvt("/TEST_BLOB");

                    lv2_atom_forge_key(&forge, key);
                    lv2_atom_forge_object(&forge, &obj, 0, pExt->uridBlobType);
                    {
                        if (p->blob.ctype != NULL)
                        {
                            lv2_atom_forge_key(&forge, pExt->uridContentType);
                            lv2_atom_forge_typed_string(&forge, forge.String, p->blob.ctype, ::strlen(p->blob.ctype));
                        }

                        uint32_t size = ((p->blob.size > 0) && (p->blob.data != NULL)) ? p->blob.size : 0;
                        lv2_atom_forge_key(&forge, pExt->uridContent);
                        lv2_atom_forge_atom(&forge, size, forge.Chunk);
                        if (size > 0)
                            lv2_atom_forge_write(&forge, p->blob.data, size);
                    }
                    lv2_atom_forge_pop(&forge, &obj);
                }*/
                
                lv2_atom_forge_pop(&forge, &frame);
                LV2_Atom *msg = reinterpret_cast<LV2_Atom *>(sink.buf);
                pExt->store_value(pExt->uridKvtObject, msg->type, &msg[1], msg->size);
            }
            else
                lsp_trace("Failed execution, result=%d, sink state=%d", int(res), int(sink.res));

            sKVT.gc();
            sKVTMutex.unlock();
        }

        pExt->reset_state_context();
        pPlugin->state_saved();
    }

    inline void LV2Wrapper::restore_state(
        LV2_State_Retrieve_Function retrieve,
        LV2_State_Handle            handle,
        uint32_t                    flags,
        const LV2_Feature *const *  features
    )
    {
        pExt->init_state_context(NULL, retrieve, handle, flags, features);

        // Restore posts state
        for (size_t i=0, n=vAllPorts.size(); i<n; ++i)
        {
            // Get port
            LV2Port *lvp    = vAllPorts[i];
            if (lvp == NULL)
                continue;

            // Restore state of port
            lvp->restore();
        }

        // Restore KVT state
        if (sKVTMutex.lock())
        {
            // Clear KVT
            sKVT.clear();

            uint32_t p_type;
            size_t p_size;
            const void *ptr = pExt->retrieve_value(pExt->uridKvtObject, &p_type, &p_size);
            size_t prefix_len = ::strlen(LSP_KVT_URI);

            if (ptr != NULL)
            {
//                lsp_dumpb("Contents of the atom object:", ptr, p_size);
                lsp_trace("p_type = %d (%s), p_size = %d", int(p_type), pExt->unmap_urid(p_type), int(p_size));

                if ((p_type == pExt->forge.Object) || (p_type == pExt->uridBlank))
                {
                    const LV2_Atom_Object_Body *obody = reinterpret_cast<const LV2_Atom_Object_Body *>(ptr);

                    for (
                        LV2_Atom_Property_Body *body = lv2_atom_object_begin(obody) ;
                        !lv2_atom_object_is_end(obody, p_size, body) ;
                        body = lv2_atom_object_next(body)
                    )
                    {
//                        lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                        lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                        const LV2_Atom_Object *pobject = reinterpret_cast<const LV2_Atom_Object *>(&body->value);
                        if ((pobject->atom.type != pExt->uridObject) && (pobject->atom.type != pExt->uridBlank))
                        {
                            lsp_warn("Unsupported value type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));
                            continue;
                        }
//                        lsp_trace("pobject->body.otype (%d) = %s", int(pobject->body.otype), pExt->unmap_urid(pobject->body.otype));
                        if (pobject->body.otype != pExt->uridKvtPropertyType)
                        {
                            lsp_warn("Unsupported object type (%d) = %s", int(pobject->body.otype), pExt->unmap_urid(pobject->body.otype));
                            continue;
                        }

                        const char *uri = pExt->unmap_urid(body->key);
                        if (::strstr(uri, LSP_KVT_URI "/") != uri)
                        {
                            lsp_warn("Invalid property: urid=%d, uri=%s", body->key, uri);
                            continue;
                        }

                        const char *name = &uri[prefix_len];
                        kvt_param_t p;
                        size_t flags    = KVT_TX;
                        p.type          = KVT_ANY;

                        for (
                            LV2_Atom_Property_Body *xbody = lv2_atom_object_begin(&pobject->body) ;
                            !lv2_atom_object_is_end(&pobject->body, body->value.size, xbody) ;
                            xbody = lv2_atom_object_next(xbody)
                        )
                        {
//                            lsp_trace("xbody->key (%d) = %s", int(xbody->key), pExt->unmap_urid(xbody->key));
//                            lsp_trace("xbody->value.type (%d) = %s", int(xbody->value.type), pExt->unmap_urid(xbody->value.type));

                            // Analyze type of value
                            if (xbody->key == pExt->uridKvtPropertyFlags)
                            {
                                if (xbody->value.type == pExt->forge.Int)
                                {
                                    size_t pflags   = (reinterpret_cast<const LV2_Atom_Int *>(&xbody->value))->body;
                                    lsp_trace("pflags = %d", int(pflags));
                                    if (pflags & LSP_LV2_PRIVATE)
                                        flags          |= KVT_PRIVATE;
                                }
                            }
                            else if (xbody->key == pExt->uridKvtPropertyValue)
                            {
                                p_type      = xbody->value.type;

                                if (p_type == pExt->forge.Int)
                                {
                                    p.type  = KVT_INT32;
                                    p.i32   = (reinterpret_cast<const LV2_Atom_Int *>(&xbody->value))->body;
                                }
                                else if (p_type == pExt->uridTypeUInt)
                                {
                                    p.type  = KVT_UINT32;
                                    p.u32   = (reinterpret_cast<const LV2_Atom_Int *>(&xbody->value))->body;
                                }
                                else if (p_type == pExt->forge.Long)
                                {
                                    p.type  = KVT_INT64;
                                    p.i64   = (reinterpret_cast<const LV2_Atom_Long *>(&xbody->value))->body;
                                }
                                else if (p_type == pExt->uridTypeULong)
                                {
                                    p.type  = KVT_UINT64;
                                    p.u64   = (reinterpret_cast<const LV2_Atom_Long *>(&xbody->value))->body;
                                }
                                else if (p_type == pExt->forge.Float)
                                {
                                    p.type  = KVT_FLOAT32;
                                    p.f32   = (reinterpret_cast<const LV2_Atom_Float *>(&xbody->value))->body;
                                }
                                else if (p_type == pExt->forge.Double)
                                {
                                    p.type  = KVT_FLOAT64;
                                    p.f64   = (reinterpret_cast<const LV2_Atom_Double *>(&xbody->value))->body;
                                }
                                else if (p_type == pExt->forge.String)
                                {
                                    p.type  = KVT_STRING;
                                    p.str   = reinterpret_cast<const char *>(&body[1]);
                                }
                                else if ((p_type == pExt->uridObject) || (p_type == pExt->uridBlank))
                                {
                                    const LV2_Atom_Object *obj = reinterpret_cast<const LV2_Atom_Object *>(&xbody->value);
                                    lsp_trace("obj->atom.type = %d (%s), obj->body.id = %d (%s), obj->body.otype = %d (%s)",
                                            obj->atom.type, pExt->unmap_urid(obj->atom.type),
                                            obj->body.id, pExt->unmap_urid(obj->body.id),
                                            obj->body.otype, pExt->unmap_urid(obj->body.otype)
                                            );

                                    if (obj->body.otype != pExt->uridBlobType)
                                        break;

                                    // Read the value
                                    p.blob.ctype    = NULL;
                                    p.blob.data     = NULL;
                                    p.blob.size     = ~size_t(0);

                                    for (
                                        LV2_Atom_Property_Body *blob = lv2_atom_object_begin(&obj->body) ;
                                        !lv2_atom_object_is_end(&obj->body, obj->atom.size, blob) ;
                                        blob = lv2_atom_object_next(blob)
                                    )
                                    {
                                        lsp_trace("blob->key (%d) = %s", int(blob->key), pExt->unmap_urid(blob->key));
                                        lsp_trace("blob->value.type (%d) = %s", int(blob->value.type), pExt->unmap_urid(blob->value.type));

                                        if ((blob->key == pExt->uridContentType) && (blob->value.type == pExt->forge.String))
                                            p.blob.ctype    = reinterpret_cast<const char *>(&blob[1]);
                                        else if ((blob->key == pExt->uridContent) && (blob->value.type == pExt->forge.Chunk))
                                        {
                                            p.blob.size     = blob->value.size;
                                            if (p.blob.size > 0)
                                                p.blob.data     = &blob[1];
                                        }
                                    }

                                    // Change type
                                    if (p.blob.size != (~size_t(0)))
                                        p.type          = KVT_BLOB;
                                }
                            }
                        }

                        // Store property
                        if (p.type != KVT_ANY)
                        {
                            kvt_dump_parameter("Fetched parameter %s = ", &p, name);
                            status_t res = sKVT.put(name, &p, flags);
                            if (res != STATUS_OK)
                                lsp_warn("Could not store parameter to KVT");
                        }
                        else
                            lsp_warn("KVT property %s has unsupported type or is invalid: 0x%x (%s)",
                                    name, p_type, pExt->unmap_urid(p_type));
                    }
                } // for
            }

            sKVT.gc();
            sKVTMutex.unlock();
        }

        pExt->reset_state_context();
        pPlugin->state_loaded();
    }

    inline LV2_Inline_Display_Image_Surface *LV2Wrapper::render_inline_display(size_t width, size_t height)
    {
#ifndef LSP_NO_LV2_UI
        // Check for Inline display support
        const plugin_metadata_t *meta = pPlugin->get_metadata();
        if ((meta == NULL) || (!(meta->extensions & E_INLINE_DISPLAY)))
            return NULL;

        // Lazy initialization
//        lsp_trace("pCanvas = %p", pCanvas);
        if (pCanvas == NULL)
        {
            pCanvas     =   new CairoCanvas();
            if (pCanvas == NULL)
                return NULL;
//            lsp_trace("pCanvas = %p", pCanvas);
        }

        // Call plugin for rendering
        if (!pPlugin->inline_display(pCanvas, width, height))
        {
            lsp_trace("failed pPlugin->inline_display");
            return NULL;
        }

        // Get data of canvas
        canvas_data_t *data     = pCanvas->get_data();
//        lsp_trace("canvas data = %p", data);
        if ((data == NULL) || (data->pData == NULL))
            return NULL;

        // Fill-in surface and return
        sSurface.data           = reinterpret_cast<unsigned char *>(data->pData);
        sSurface.width          = data->nWidth;
        sSurface.height         = data->nHeight;
        sSurface.stride         = data->nStride;

//        lsp_trace("surface data=%p, width=%d, height=%d, stride=%d",
//            sSurface.data, int(sSurface.width), int(sSurface.height), int(sSurface.stride));

        return &sSurface;
#else
        return NULL;
#endif
    }

#ifndef LSP_NO_LV2_UI
    ICanvas *LV2Wrapper::create_canvas(ICanvas *&cv, size_t width, size_t height)
    {
        if ((cv != NULL) && (cv->width() == width) && (cv->height() == height))
            return cv;

        ICanvas *ncv = new CairoCanvas();
        if (ncv == NULL)
            return NULL;
        if (!ncv->init(width, height))
        {
            delete ncv;
            return NULL;
        }

        if (cv != NULL)
        {
            cv->destroy();
            delete cv;
        }

        return cv = ncv;
    }
#endif

    KVTStorage *LV2Wrapper::kvt_lock()
    {
        return (sKVTMutex.lock()) ? &sKVT : NULL;
    }

    KVTStorage *LV2Wrapper::kvt_trylock()
    {
        return (sKVTMutex.try_lock()) ? &sKVT : NULL;
    }

    bool LV2Wrapper::kvt_release()
    {
        return sKVTMutex.unlock();
    }
}

#endif /* CONTAINER_LV2_WRAPPER_H_ */
