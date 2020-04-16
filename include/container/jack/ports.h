/*
 * ports.h
 *
 *  Created on: 13 мая 2016 г.
 *      Author: sadko
 */

#ifndef COINTAINER_JACK_PORTS_H_
#define COINTAINER_JACK_PORTS_H_

namespace lsp
{
    class JACKPort: public IPort
    {
        protected:
            JACKWrapper        *pWrapper;

        public:
            explicit JACKPort(const port_t *meta, JACKWrapper *w): IPort(meta)
            {
                pWrapper        = w;
            }

            virtual ~JACKPort()
            {
                pWrapper        = NULL;
            }

        public:
            virtual int init()
            {
                return STATUS_OK;
            }

            virtual void destroy()
            {
            }
    };

    class JACKPortGroup: public JACKPort
    {
        private:
            float                   nCurrRow;
            size_t                  nCols;
            size_t                  nRows;

        public:
            explicit JACKPortGroup(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
                nCurrRow            = meta->start;
                nCols               = port_list_size(meta->members);
                nRows               = list_size(meta->items);
            }

            virtual ~JACKPortGroup()
            {
                nCurrRow            = 0;
                nCols               = 0;
                nRows               = 0;
            }

        public:
            virtual void setValue(float value)
            {
                int32_t v = value;
                if ((v >= 0) && (v < ssize_t(nRows)))
                    nCurrRow        = v;
            }

            virtual float getValue()
            {
                return nCurrRow;
            }

        public:
            inline size_t rows() const      { return nRows; }
            inline size_t cols() const      { return nCols; }
            inline size_t curr_row() const  { return nCurrRow; }
    };

    class JACKDataPort: public JACKPort
    {
        private:
            jack_port_t    *pPort;              // JACK port descriptor
            void           *pDataBuffer;        // Real data buffer passed from JACK
            void           *pBuffer;            // Data buffer
            midi_t         *pMidi;              // Midi buffer for operating MIDI messages
            float          *pSanitized;         // Input float data for sanitized buffers
            size_t          nBufSize;           // Size of sanitized buffer in samples

        public:
            explicit JACKDataPort(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
                pPort       = NULL;
                pDataBuffer = NULL;
                pBuffer     = NULL;
                pMidi       = NULL;
                pSanitized  = NULL;
                nBufSize    = 0;
            }

            virtual ~JACKDataPort()
            {
                pPort       = NULL;
                pDataBuffer = NULL;
                pBuffer     = NULL;
                pMidi       = NULL;
                pSanitized  = NULL;
                nBufSize    = 0;
            };

        public:
            virtual void *getBuffer()
            {
                return pBuffer;
            };

            virtual int init()
            {
                return connect();
            }

            int disconnect()
            {
                if (pPort == NULL)
                    return STATUS_OK;

                jack_client_t *cl   = pWrapper->client();
                if (cl != NULL)
                    jack_port_unregister(cl, pPort);

                if (pSanitized != NULL)
                {
                    ::free(pSanitized);
                    pSanitized = NULL;
                }

                if (pMidi != NULL)
                {
                    delete pMidi;
                    pMidi       = NULL;
                }

                pPort       = NULL;
                nBufSize    = 0;

                return STATUS_OK;
            }

            int connect()
            {
                // Determine port type
                const char *port_type = NULL;
                if (pMetadata->role == R_AUDIO)
                    port_type = JACK_DEFAULT_AUDIO_TYPE;
                else if (pMetadata->role == R_MIDI)
                {
                    port_type   = JACK_DEFAULT_MIDI_TYPE;
                    pMidi       = new midi_t;
                    if (pMidi == NULL)
                        return STATUS_NO_MEM;
                    pMidi->clear();
                }
                else
                    return STATUS_BAD_FORMAT;

                // Determine flags
                size_t flags = (IS_OUT_PORT(pMetadata)) ? JackPortIsOutput : JackPortIsInput;

                // Get client
                jack_client_t *cl   = pWrapper->client();
                if (cl == NULL)
                {
                    if (pMidi != NULL)
                    {
                        delete pMidi;
                        pMidi   = NULL;
                    }
                    return STATUS_DISCONNECTED;
                }

                // Register port
                pPort       = jack_port_register(cl, pMetadata->id, port_type, flags, 0);

                return (pPort != NULL) ? STATUS_OK : STATUS_UNKNOWN_ERR;
            }

            void set_buffer_size(size_t size)
            {
                // set_buffer_size should affect only input audio ports currently
                if ((!IS_IN_PORT(pMetadata)) || (pMidi != NULL))
                    return;

                // Buffer size has changed?
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

            void report_latency(ssize_t latency)
            {
                // Only output ports should report latency
                if ((pMetadata == NULL) || (!IS_OUT_PORT(pMetadata)))
                    return;

                // Report latency
                jack_latency_range_t range;
                jack_port_get_latency_range(pPort, JackCaptureLatency, &range);
                range.min += latency;
                range.max += latency;
                jack_port_set_latency_range (pPort, JackCaptureLatency, &range);
            }

            virtual void destroy()
            {
                disconnect();
            }

            virtual bool pre_process(size_t samples)
            {
                if (pPort == NULL)
                {
                    pBuffer     = NULL;
                    return false;
                }

                pDataBuffer = jack_port_get_buffer(pPort, samples);
                pBuffer     = pDataBuffer;

                if (pMidi != NULL)
                {
                    if ((pBuffer != NULL) && IS_IN_PORT(pMetadata))
                    {
                        // Clear our buffer
                        pMidi->clear();

                        // Read MIDI events
                        jack_midi_event_t   midi_event;
                        midi::event_t       ev;

                        jack_nframes_t event_count = jack_midi_get_event_count(pBuffer);
                        for (jack_nframes_t i=0; i<event_count; i++)
                        {
                            // Read MIDI event
                            if (jack_midi_event_get(&midi_event, pBuffer, i) != 0)
                            {
                                lsp_warn("Could not fetch MIDI event #%d from JACK port", int(i));
                                continue;
                            }

                            // Convert MIDI event
                            if (midi::decode(&ev, midi_event.buffer) <= 0)
                            {
                                lsp_warn("Could not decode MIDI event #%d at timestamp %d from JACK port", int(i), int(midi_event.time));
                                continue;
                            }

                            // Update timestamp and store event
                            ev.timestamp    = midi_event.time;
                            if (!pMidi->push(ev))
                                lsp_warn("Could not append MIDI event #%d at timestamp %d due to buffer overflow", int(i), int(midi_event.time));
                        }

                        // All MIDI events ARE ordered chronologically, we do not need to perform sort
                    }

                    // Replace pBuffer with pMidi
                    pBuffer     = pMidi;
                }
                else if (pSanitized != NULL) // Need to sanitize?
                {
                    // Perform sanitize() if possible
                    if (samples <= nBufSize)
                    {
                        dsp::sanitize2(pSanitized, reinterpret_cast<float *>(pDataBuffer), samples);
                        pBuffer = pSanitized;
                    }
                    else
                    {
                        lsp_warn("Could not sanitize buffer data for port %s, not enough buffer size (required: %d, actual: %d)",
                                pMetadata->id, int(samples), int(nBufSize));
                    }
                }

                return false;
            }

            virtual void post_process(size_t samples)
            {
                if ((pMidi != NULL) && (pDataBuffer != NULL) && IS_OUT_PORT(pMetadata))
                {
                    // Reset buffer
                    jack_midi_clear_buffer(pDataBuffer);

                    // Transfer MIDI events
                    pMidi->sort();  // All events SHOULD be ordered chonologically

                    // Transmit all events
                    for (size_t i=0, events=pMidi->nEvents; i<events; ++i)
                    {
                        // Determine size of the message
                        midi::event_t *ev   = &pMidi->vEvents[i];
                        ssize_t size        = midi::size_of(ev);
                        if (size <= 0)
                        {
                            lsp_warn("Could not encode output MIDI message of type 0x%02x, timestamp=%d", int(ev->type), int(ev->timestamp));
                            continue;
                        }

                        // Allocate MIDI event
                        jack_midi_data_t *midi_data     = jack_midi_event_reserve(pDataBuffer, ev->timestamp, size);
                        if (midi_data == NULL)
                        {
                            lsp_warn("Could not write MIDI message of type 0x%02x, size=%d, timestamp=%d to JACK output port buffer=%p",
                                    int(ev->type), int(size), int(ev->timestamp), pBuffer);
                            continue;
                        }

                        // Encode MIDI event
                        midi::encode(midi_data, ev);
                    }

                    // Cleanup the output buffer
                    pMidi->clear();
                }

                pBuffer     = NULL;
            }
    };

    class JACKControlPort: public JACKPort
    {
        private:
            float       fNewValue;
            float       fCurrValue;

        public:
            explicit JACKControlPort(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
                fNewValue   = meta->start;
                fCurrValue  = meta->start;
            }

            virtual ~JACKControlPort()
            {
                fNewValue   = pMetadata->start;
                fCurrValue  = pMetadata->start;
            };

        public:
            virtual bool pre_process(size_t samples)
            {
                if (fNewValue == fCurrValue)
                    return false;

                fCurrValue   = fNewValue;
                return true;
            }

            virtual float getValue()
            {
                return fCurrValue;
            }

            void updateValue(float value)
            {
                fNewValue   = limit_value(pMetadata, value);
            }
    };

    class JACKMeterPort: public JACKPort
    {
        private:
            float       fValue;
            bool        bForce;

        public:
            explicit JACKMeterPort(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
                fValue      = meta->start;
                bForce      = true;
            }

            virtual ~JACKMeterPort()
            {
                fValue      = pMetadata->start;
            };

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual void setValue(float value)
            {
                value   = limit_value(pMetadata, value);

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
                bForce  = true;
                return value;
            }
    };

    class JACKMeshPort: public JACKPort
    {
        private:
            mesh_t     *pMesh;

        public:
            explicit JACKMeshPort(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
                pMesh   = NULL;
            }

            virtual ~JACKMeshPort()
            {
                pMesh   = NULL;
            }

        public:
            virtual void *getBuffer()
            {
                return pMesh;
            }

            virtual int init()
            {
                pMesh   = jack_create_mesh(pMetadata);
                return (pMesh == NULL) ? STATUS_NO_MEM : STATUS_OK;
            }

            virtual void destroy()
            {
                if (pMesh != NULL)
                {
                    jack_destroy_mesh(pMesh);
                    pMesh = NULL;
                }
            }
    };

    class JACKFrameBufferPort: public JACKPort
    {
        private:
            frame_buffer_t      sFB;

        public:
            explicit JACKFrameBufferPort(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
            }

            virtual ~JACKFrameBufferPort()
            {
            }

        public:
            virtual void *getBuffer()
            {
                return &sFB;
            }

            virtual int init()
            {
                return sFB.init(pMetadata->start, pMetadata->step);
            }

            virtual void destroy()
            {
                sFB.destroy();
            }
    };

    class JACKOscPort: public JACKPort
    {
        private:
            osc_buffer_t     *pFB;

        public:
            explicit JACKOscPort(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
                pFB     = NULL;
            }

            virtual ~JACKOscPort()
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

    class JACKPathPort: public JACKPort
    {
        private:
            jack_path_t     sPath;

        public:
            explicit JACKPathPort(const port_t *meta, JACKWrapper *w) : JACKPort(meta, w)
            {
                sPath.init();
            }

            virtual ~JACKPathPort()
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
    };

}


#endif /* COINTAINER_JACK_PORTS_H_ */
