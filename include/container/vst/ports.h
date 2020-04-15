/*
 * ports.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_VST_PORTS_H_
#define CONTAINER_VST_PORTS_H_

namespace lsp
{
    // Specify port classes
    class VSTPort: public IPort
    {
        protected:
            AEffect                *pEffect;
            audioMasterCallback     hCallback;
            ssize_t                 nID;

        protected:
            float from_vst(float value)
            {
//                lsp_trace("input = %.3f", value);
                // Set value as integer or normalized
                if (pMetadata->unit == U_BOOL)
                    value = (value >= 0.5f) ? 1.0f : 0.0f;
//                else if (pMetadata->unit == U_ENUM) { /* nothing */ }
//                else if (pMetadata->unit == U_SAMPLES)
//                    value = truncf(value);
                else
                {
//                    if (pMetadata->flags & F_INT)
//                        value = truncf(value);
//                    else
//                    {
                        float min = 0.0f, max = 1.0f;
                        get_port_parameters(pMetadata, &min, &max, NULL);
                        value = min + value * (max - min);
//                    }
                     if ((pMetadata->flags & F_INT) || (pMetadata->unit == U_ENUM) || (pMetadata->unit == U_SAMPLES))
                         value  = truncf(value);
                }

//                lsp_trace("result = %.3f", value);
                return value;
            }

            float to_vst(float value)
            {
//                lsp_trace("input = %.3f", value);
                // Set value as integer or normalized
                if (pMetadata->unit == U_BOOL) // { /* nothing */ }
                    value = (value >= 0.5f) ? 1.0f : 0.0f;
//                else if (pMetadata->unit == U_ENUM) { /* nothing */ }
//                else if (pMetadata->unit == U_SAMPLES) { /* nothing */ }
                else
                {
                    if ((pMetadata->flags & F_INT) || (pMetadata->unit == U_ENUM) || (pMetadata->unit == U_SAMPLES))
                        value  = truncf(value);

//                    if (pMetadata->flags & F_INT) { /* nothing */ }
//                    else
//                    {
                        // Normalize value
                        float min = 0.0f, max = 1.0f;
                        get_port_parameters(pMetadata, &min, &max, NULL);
                        value = (max != min) ? (value - min) / (max - min) : 0.0f;
//                    }
                }

//                lsp_trace("result = %.3f", value);
                return value;
            }

        public:
            explicit VSTPort(const port_t *meta, AEffect *effect, audioMasterCallback callback): IPort(meta)
            {
                pEffect         = effect;
                hCallback       = callback;
                nID             = -1;
            }
            virtual ~VSTPort()
            {
                pEffect         = NULL;
                hCallback       = NULL;
                nID             = -1;
            }

        public:
            inline AEffect                 *getEffect()         { return pEffect;               };
            inline audioMasterCallback      getCallback()       { return hCallback;             };
            inline ssize_t                  getID() const       { return nID;                   };
            inline void                     setID(ssize_t id)   { nID = id;                     };

            inline VstIntPtr                masterCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
            {
                return hCallback(pEffect, opcode, index, value, ptr, opt);
            }

            virtual void writeValue(float value)    {};

            virtual bool serializable() const { return false; }

            /** Serialize the state of the port to the chunk
             *
             * @param chunk chunk to perform serialization
             */
            virtual void serialize(vst_chunk_t *chunk) {}

            /** Deserialize the state of the port from the chunk (legacy version)
             *
             * @param data data buffer
             * @param length length of the buffer in bytes
             * @return number of bytes deserialized or error
             */
            virtual ssize_t deserialize_v1(const void *data, size_t length)
            {
                return -1;
            }

            /**
             * Deserialize the state of the port from the chunk data, data pointer should
             * be updated
             * @param data chunk data
             * @param limit the data size
             * @return true on success
             */
            virtual bool deserialize_v2(const uint8_t *data, size_t size)
            {
                return true;
            }
    };

    class VSTPortGroup: public VSTPort
    {
        private:
            float                   nCurrRow;
            size_t                  nCols;
            size_t                  nRows;

        public:
            explicit VSTPortGroup(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                nCurrRow            = meta->start;
                nCols               = port_list_size(meta->members);
                nRows               = list_size(meta->items);
            }

            virtual ~VSTPortGroup()
            {
                nCurrRow            = 0;
                nCols               = 0;
                nRows               = 0;
            }

        public:
            virtual void setValue(float value)
            {
                int32_t v   = value;
                if ((v >= 0) && (v < ssize_t(nRows)))
                    nCurrRow        = v;
            }

            virtual float getValue()
            {
                return nCurrRow;
            }

            virtual bool serializable() const { return true; }

            virtual void serialize(vst_chunk_t *chunk)
            {
                int32_t v = CPU_TO_BE(int32_t(nCurrRow));
                chunk->write(&v, sizeof(v));
            }

            virtual ssize_t deserialize_v1(const void *data, size_t length)
            {
                if (length < sizeof(int32_t))
                    return -1;
                int32_t value   = BE_TO_CPU(*(reinterpret_cast<const int32_t *>(data)));
                if ((value >= 0) && (value < ssize_t(nRows)))
                    nCurrRow        = value;
                return sizeof(int32_t);
            }

            virtual bool deserialize_v2(const uint8_t *data, size_t size)
            {
                if (size < sizeof(int32_t))
                    return false;

                int32_t v       = BE_TO_CPU(*(reinterpret_cast<const int32_t *>(data)));
                if ((v >= 0) && (v < ssize_t(nRows)))
                    nCurrRow        = v;

                return true;
            }

        public:
            inline size_t rows() const      { return nRows; }
            inline size_t cols() const      { return nCols; }
            inline size_t curr_row() const  { return nCurrRow; }
    };

    class VSTAudioPort: public VSTPort
    {
        private:
            float          *pBuffer;
            float          *pSanitized;
            size_t          nBufSize;

        public:
            explicit VSTAudioPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                pBuffer     = NULL;
                pSanitized  = NULL;
                nBufSize    = 0;
            }

            virtual ~VSTAudioPort()
            {
                pBuffer     = NULL;

                if (pSanitized != NULL)
                {
                    ::free(pSanitized);
                    pSanitized  = NULL;
                    nBufSize    = 0;
                }
            };

        public:
            virtual void *getBuffer() { return pBuffer; };

            void bind(float *data, size_t samples)
            {
                pBuffer     = data;
                if (pSanitized == NULL)
                    return;

                // Perform sanitize() if possible
                if (samples <= nBufSize)
                {
                    dsp::sanitize2(pSanitized, reinterpret_cast<float *>(pBuffer), samples);
                    pBuffer = pSanitized;
                }
                else
                {
                    lsp_warn("Could not sanitize buffer data for port %s, not enough buffer size (required: %d, actual: %d)",
                            pMetadata->id, int(samples), int(nBufSize));
                }
            };

            void set_blk_size(size_t size)
            {
                if (!IS_IN_PORT(pMetadata))
                    return;
                if (nBufSize == size)
                    return;

                float *buf  = reinterpret_cast<float *>(::realloc(pSanitized, sizeof(float) * size));
                if (buf == NULL)
                {
                    ::free(pSanitized);
                    pSanitized = NULL;
                    return;
                }

                nBufSize    = size;
                pSanitized  = buf;
                dsp::fill_zero(pSanitized, nBufSize);
            }
    };

    class VSTParameterPort: public VSTPort
    {
        private:
            float       fValue;         // The internal value
            float       fVstPrev;       // Previous value in VST standard notation
            float       fVstValue;      // Current value in VST standard notation
            volatile vst_serial_t nSID; // Serial ID of the parameter

        public:
            explicit VSTParameterPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                fValue      = meta->start;
                fVstPrev    = to_vst(meta->start);
                fVstValue   = fVstPrev;
                nSID        = 0;
            }

            virtual ~VSTParameterPort()
            {
                fValue      = pMetadata->start;
                fVstPrev    = 0.0f;
                fVstValue   = 0.0f;
                nSID        = 0;
            }

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual void setValue(float value)
            {
                fValue      = limit_value(pMetadata, value);
                fVstValue   = to_vst(fValue);
            }

            virtual bool pre_process(size_t samples)
            {
                return fVstValue != fVstPrev;
            }

            virtual void post_process(size_t samples)
            {
                fVstPrev        = fVstValue;
            }

            virtual void writeValue(float value)
            {
                setValue(value);
                if ((nID >= 0) && (pEffect != NULL) && (hCallback != NULL))
                {
                    lsp_trace("hCallback=%p, pEffect=%p, operation=%d, id=%d, value=%.5f, vst_value=%.5f",
                            hCallback, pEffect, int(audioMasterAutomate), int(nID), value, fVstValue);
                    hCallback(pEffect, audioMasterAutomate, nID, 0, NULL, fVstValue);
                }
            }

            void setVstValue(float value)
            {
                if (fVstValue == value)
                    return;
                fValue          = limit_value(pMetadata, from_vst(value));
                fVstValue       = value;
                nSID            ++;
            }

            inline float getVstValue()
            {
                return fVstValue;
            }

            inline vst_serial_t getSID()
            {
                return nSID;
            }

            virtual bool serializable() const { return true; }

            virtual void serialize(vst_chunk_t *chunk)
            {
                float v = CPU_TO_BE(fValue);
                chunk->write(&v, sizeof(v));
            }

            virtual ssize_t deserialize_v1(const void *data, size_t length)
            {
                if (length < sizeof(float))
                    return -1;
                float value     = BE_TO_CPU(*(reinterpret_cast<const float *>(data)));
                writeValue(value);
                return sizeof(float);
            }

            virtual bool deserialize_v2(const uint8_t *data, size_t size)
            {
                if (size < sizeof(float))
                    return false;

                float v         = BE_TO_CPU(*(reinterpret_cast<const float *>(data)));
                writeValue(v);
                return true;
            }
    };

    class VSTMeterPort: public VSTPort
    {
        public:
            float   fValue;
            bool    bForce;

        public:
            explicit VSTMeterPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                fValue      = meta->start;
                bForce      = true;
            }

            virtual ~VSTMeterPort()
            {
                fValue      = pMetadata->start;
            }

        public:
            // Native Interface
            virtual float getValue()
            {
                return fValue;
            }

            virtual void setValue(float value)
            {
                value       = limit_value(pMetadata, value);

                if (pMetadata->flags & F_PEAK)
                {
                    if ((bForce) || (fabs(fValue) < fabs(value)))
                    {
                        fValue  = value;
                        bForce  = false;
                    }
                }
                else
                    fValue = value;
            }

            float syncValue()
            {
                float value = fValue;
                bForce      = true;
                return value;
            }
    };

    class VSTMeshPort: public VSTPort
    {
        private:
            mesh_t     *pMesh;

        public:
            explicit VSTMeshPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                pMesh   = vst_create_mesh(meta);
            }

            virtual ~VSTMeshPort()
            {
                vst_destroy_mesh(pMesh);
                pMesh = NULL;
            }

        public:
            virtual void *getBuffer()
            {
                return pMesh;
            }
    };

    class VSTFrameBufferPort: public VSTPort
    {
        private:
            frame_buffer_t     sFB;

        public:
            explicit VSTFrameBufferPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                sFB.init(pMetadata->start, pMetadata->step);
            }

            virtual ~VSTFrameBufferPort()
            {
                sFB.destroy();
            }

        public:
            virtual void *getBuffer()
            {
                return &sFB;
            }

            virtual void destroy()
            {
                sFB.destroy();
            }
    };

    class VSTMidiInputPort: public VSTPort
    {
        private:
            midi_t      sQueue;         // MIDI event buffer

        public:
            explicit VSTMidiInputPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                sQueue.clear();
            }

            virtual ~VSTMidiInputPort()
            {
            }

        public:
            virtual void *getBuffer()
            {
                return &sQueue;
            }

            virtual void post_process(size_t samples)
            {
                sQueue.clear();
            }

        public:
            void deserialize(const VstEvents *e)
            {
                size_t count    = e->numEvents;
                for (size_t i=0; i<count; ++i)
                {
                    // Get event and check type
                    const VstEvent *ve      = e->events[i];
                    if (ve->type != kVstMidiType)
                        continue;

                    // Cast to VST MIDI event
                    const VstMidiEvent *vme     = reinterpret_cast<const VstMidiEvent *>(ve);
                    const uint8_t *bytes        = reinterpret_cast<const uint8_t *>(vme->midiData);

                    // Decode MIDI event
                    midi::event_t me;
                    if (midi::decode(&me, bytes) <= 0)
                        return;

                    // Put the event to the queue
                    me.timestamp      = vme->deltaFrames;

                    // Debug
                    #ifdef LSP_TRACE
                        #define TRACE_KEY(x)    case midi::MIDI_MSG_ ## x: evt_type = #x; break;
                        lsp_trace("midi dump: %02x %02x %02x", int(bytes[0]) & 0xff, int(bytes[1]) & 0xff, int(bytes[2]) & 0xff);

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

                    // Add event to the queue
                    if (!sQueue.push(me))
                        lsp_error("MIDI event queue overflow");
                }

                // We don't know anything about ordering of events, reorder them chronologically
                sQueue.sort();
            }
    };

    class VSTMidiOutputPort: public VSTPort
    {
        private:
            midi_t          sQueue;                     // MIDI event buffer
            VstEvents      *pEvents;                    // Root pointer to VST MIDI events
            VstMidiEvent    vEvents[MIDI_EVENTS_MAX];   // Buffer for VST MIDI events

        public:
            explicit VSTMidiOutputPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                sQueue.clear();

                // Allocate buffer for VST MIDI events
                size_t evt_size = ALIGN_SIZE(sizeof(VstEvents) + MIDI_EVENTS_MAX * sizeof(VstMidiEvent *), DEFAULT_ALIGN);
                pEvents         = reinterpret_cast<VstEvents *>(new uint8_t[evt_size]);
            }

            virtual ~VSTMidiOutputPort()
            {
            }

        public:
            virtual void *getBuffer()
            {
                return &sQueue;
            }

            virtual void post_process(size_t samples)
            {
                // Check that there are pending MIDI events
                if (sQueue.nEvents <= 0)
                    return;

                // We don't know anything about ordering of events, reorder them chronologically
                sQueue.sort();

                // Translate events
                pEvents->numEvents  = 0;

                for (size_t i=0; i<sQueue.nEvents; ++i)
                {
                    const midi::event_t    *me  = &sQueue.vEvents[i];
                    VstMidiEvent           *dst = &vEvents[pEvents->numEvents];

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

                    ssize_t bytes = midi::encode(reinterpret_cast<uint8_t *>(dst->midiData), me);
                    if (bytes <= 0)
                    {
                        lsp_error("Tried to serialize invalid MIDI event");
                        continue;
                    }

                    dst->type           = kVstMidiType;
                    dst->byteSize       = sizeof(VstMidiEvent);
                    dst->deltaFrames    = me->timestamp;
                    dst->flags          = (me->type >= midi::MIDI_MSG_CLOCK) ? kVstMidiEventIsRealtime : 0;
                    dst->noteLength     = 0;
                    dst->noteOffset     = 0;
                    dst->detune         = 0;
                    dst->noteOffVelocity= (me->type == midi::MIDI_MSG_NOTE_OFF) ? me->note.velocity : 0;

                    lsp_trace("midi dump: %02x %02x %02x",
                        int(dst->midiData[0]) & 0xff, int(dst->midiData[1]) & 0xff, int(dst->midiData[2]) & 0xff);

                    // Add pointer to the VST event to the VST eventn list
                    pEvents->events[pEvents->numEvents++]       = reinterpret_cast<VstEvent *>(dst);
                }

                // Call host to process MIDI events if they are
                if (pEvents->numEvents > 0)
                {
                    masterCallback(audioMasterProcessEvents, 0, 0, pEvents, 0.0f);
                    pEvents->numEvents      = 0;
                }

                sQueue.clear();
            }
    };

    class VSTPathPort: public VSTPort
    {
        private:
            vst_path_t     sPath;

        public:
            explicit VSTPathPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                sPath.init();
            }

            virtual ~VSTPathPort()
            {
            }

        public:
            virtual void *getBuffer()
            {
                return static_cast<path_t *>(&sPath);
            }

            virtual bool pre_process(size_t samples)
            {
                return sPath.pending();
            }

            size_t sizeof_state()
            {
                size_t bytes    = strlen(sPath.sPath);
                if (bytes >= 0x7fff)
                    bytes           = 0x7fff;
                if (bytes > 0x7f)
                    bytes          += 2;
                else
                    bytes          += 1;

                return bytes;
            }

            virtual void serialize(vst_chunk_t *chunk)
            {
                chunk->write_string(sPath.sPath);
            }

            virtual ssize_t deserialize_v1(const void *data, size_t length)
            {
                const uint8_t  *ptr     = reinterpret_cast<const uint8_t *>(data);
                const uint8_t  *tail    = ptr + length;
                if (ptr >= tail)
                    return -1;

                // Read length of string
                size_t bytes        = *(ptr++);
                if (bytes & 0x80)
                {
                    if (ptr >= tail)
                        return -1;

                    bytes       = ((bytes << 8) | (*(ptr++))) & 0x7fff;
                }

                // Read string
                tail           -= bytes;
                if (ptr > tail)
                    return -1;

                // Submit data
                sPath.submit(reinterpret_cast<const char *>(ptr), bytes, false, PF_STATE_RESTORE);
                ptr            += bytes;
                return ptr - reinterpret_cast<const uint8_t *>(data);
            }

            virtual bool deserialize_v2(const uint8_t *data, size_t size)
            {
                const char *str = reinterpret_cast<const char *>(data);
                size_t len  = ::strnlen(str, size) + 1;
                if (len > size)
                    return false;

                sPath.submit(str, len, false, PF_STATE_RESTORE);
                return true;
            }

            virtual bool serializable() const { return true; }
    };

    class VSTOscPort: public VSTPort
    {
        private:
            osc_buffer_t     *pFB;

        public:
            explicit VSTOscPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                pFB     = NULL;
            }

            virtual ~VSTOscPort()
            {
            }

        public:
            virtual void *getBuffer()
            {
                return pFB;
            }

            virtual int init()
            {
                pFB = osc_buffer_t::create(OSC_BUFFER_MAX);
                return (pFB == NULL) ? STATUS_NO_MEM : STATUS_OK;
            }

            virtual void destroy()
            {
                if (pFB != NULL)
                {
                    osc_buffer_t::destroy(pFB);
                    pFB     = NULL;
                }
            }
    };
}


#endif /* CONTAINER_VST_PORTS_H_ */
