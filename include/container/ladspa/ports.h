/*
 * ports.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LADSPA_PORTS_H_
#define CONTAINER_LADSPA_PORTS_H_

#define LADSPA_MAX_BLOCK_LENGTH             8192

namespace lsp
{
    // Specify port classes
    class LADSPAPort: public IPort
    {
        protected:
            float      *pData;

        public:
            explicit LADSPAPort(const port_t *meta) : IPort(meta), pData(NULL) {};
            virtual ~LADSPAPort()
            {
                pData   = NULL;
            }

        public:
            virtual void bind(void *data)
            {
                pData   = reinterpret_cast<float *>(data);
            }
    };

    class LADSPAAudioPort: public LADSPAPort
    {
        protected:
            float      *pSanitized;
            float      *pBuffer;

        public:
            explicit LADSPAAudioPort(const port_t *meta) : LADSPAPort(meta)
            {
                pBuffer     = NULL;
                pSanitized  = NULL;
                if (IS_IN_PORT(meta))
                {
                    pSanitized = reinterpret_cast<float *>(::malloc(sizeof(float) * LADSPA_MAX_BLOCK_LENGTH));
                    if (pSanitized != NULL)
                        dsp::fill_zero(pSanitized, LADSPA_MAX_BLOCK_LENGTH);
                    else
                        lsp_warn("Failed to allocate sanitize buffer for port %s", pMetadata->id);
                }
            }

            virtual ~LADSPAAudioPort()
            {
                if (pSanitized != NULL)
                {
                    ::free(pSanitized);
                    pSanitized = NULL;
                }
            };

        public:
            virtual void *getBuffer()   { return pBuffer; };

            // Should be always called at least once after bind() and before processing
            void sanitize(size_t off, size_t samples)
            {
                pBuffer     = &pData[off];
                if (pSanitized == NULL)
                    return;

                if (samples <= LADSPA_MAX_BLOCK_LENGTH)
                {
                    dsp::sanitize2(pSanitized, reinterpret_cast<float *>(pBuffer), samples);
                    pBuffer     = pSanitized;
                }
            }
    };

    class LADSPAInputPort: public LADSPAPort
    {
        private:
            float   fPrev;
            float   fValue;

        public:
            explicit LADSPAInputPort(const port_t *meta) : LADSPAPort(meta)
            {
                fPrev       = meta->start;
                fValue      = meta->start;
            }

            virtual ~LADSPAInputPort()
            {
                fPrev       = NAN;
                fValue      = NAN;
            }

        public:
            virtual float getValue() { return fValue; }

            virtual bool pre_process(size_t samples)
            {
                if (pData == NULL)
                    return false;

                fValue      = limit_value(pMetadata, *pData);
                return fPrev != fValue;
            }

            virtual void post_process(size_t samples) { fPrev = fValue; };
    };

    class LADSPAOutputPort: public LADSPAPort
    {
        protected:
            float fValue;

        public:
            explicit LADSPAOutputPort(const port_t *meta) : LADSPAPort(meta)
            {
                fValue      = meta->start;
            }

            virtual ~LADSPAOutputPort()
            {
                fValue      = NAN;
            };

        public:
            virtual float getValue()
            {
                return      fValue;
            }

            virtual void setValue(float value)
            {
                value       = limit_value(pMetadata, value);
                if (pMetadata->flags & F_PEAK)
                {
                    if (fabs(fValue) < fabs(value))
                        fValue = value;
                }
                else
                    fValue = value;
            };

            virtual void bind(void *data) { pData = reinterpret_cast<float *>(data); };

            virtual bool pre_process(size_t samples)
            {
                if (pMetadata->flags & F_PEAK)
                    fValue      = 0.0f;
                return false;
            }

            virtual void post_process(size_t samples)
            {
                if (pData != NULL)
                    *pData      = fValue;
                if (pMetadata->flags & F_PEAK)
                    fValue      = 0.0f;
            }
    };

}


#endif /* CONTAINER_LADSPA_PORTS_H_ */
