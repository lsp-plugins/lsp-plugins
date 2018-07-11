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
                else if (pMetadata->unit == U_ENUM) { /* nothing */ }
                else if (pMetadata->unit == U_SAMPLES)
                    value = truncf(value);
                else
                {
                    if (pMetadata->flags & F_INT)
                        value = truncf(value);
                    else
                    {
                        float min = 0.0f, max = 1.0f;
                        get_port_parameters(pMetadata, &min, &max, NULL);
                        value = min + value * (max - min);
                    }
                }

//                lsp_trace("result = %.3f", value);
                return value;
            }

            float to_vst(float value)
            {
//                lsp_trace("input = %.3f", value);
                // Set value as integer or normalized
                if (pMetadata->unit == U_BOOL) { /* nothing */ }
                else if (pMetadata->unit == U_ENUM) { /* nothing */ }
                else if (pMetadata->unit == U_SAMPLES) { /* nothing */ }
                else
                {
                    if (pMetadata->flags & F_INT) { /* nothing */ }
                    else
                    {
                        // Normalize value
                        float min = 0.0f, max = 1.0f;
                        get_port_parameters(pMetadata, &min, &max, NULL);
                        value = (max != min) ? (value - min) / (max - min) : 0.0f;
                    }
                }

//                lsp_trace("result = %.3f", value);
                return value;
            }

        public:
            VSTPort(const port_t *meta, AEffect *effect, audioMasterCallback callback): IPort(meta)
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

            virtual void writeValue(float value)    {};
    };

    class VSTAudioPort: public VSTPort
    {
        private:
            float *pBuffer;

        public:
            VSTAudioPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                pBuffer     = NULL;
            }

            virtual ~VSTAudioPort()
            {
                pBuffer     = NULL;
            };

        public:
            virtual void *getBuffer() { return pBuffer; };

            void bind(float *data)
            {
                pBuffer = data;
            };
    };

    class VSTParameterPort: public VSTPort
    {
        private:
            float       fValue;         // The internal value
            float       fVstPrev;       // Previous value in VST standard notation
            float       fVstValue;      // Current value in VST standard notation
            volatile vst_serial_t nSID; // Serial ID of the parameter

        public:
            VSTParameterPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
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
                    lsp_trace("hCallback=%p, pEffect=%p, operation=%d, id=%d, value=%.5f",
                            hCallback, pEffect, int(audioMasterAutomate), int(nID), value);
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
    };

    class VSTMeterPort: public VSTPort
    {
        public:
            float   fValue;

        public:
            VSTMeterPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
            {
                fValue      = meta->start;
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
                if (pMetadata->flags & F_UPPER)
                {
                    if (value > pMetadata->max)
                        value = pMetadata->max;
                }
                if (pMetadata->flags & F_LOWER)
                {
                    if (value < pMetadata->min)
                        value = pMetadata->min;
                }
                fValue      = value;
            }
    };

    class VSTMeshPort: public VSTPort
    {
        private:
            mesh_t     *pMesh;

        public:
            VSTMeshPort(const port_t *meta, AEffect *effect, audioMasterCallback callback) : VSTPort(meta, effect, callback)
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


}


#endif /* CONTAINER_VST_PORTS_H_ */
