/*
 * ports.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LADSPA_PORTS_H_
#define CONTAINER_LADSPA_PORTS_H_

namespace lsp
{
    // Specify port classes
    class LADSPAPort: public IPort
    {
        protected:
            float      *pData;

        public:
            LADSPAPort(const port_t *meta) : IPort(meta), pData(NULL) {};
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
        public:
            LADSPAAudioPort(const port_t *meta) : LADSPAPort(meta) { }
            virtual ~LADSPAAudioPort() { };

        public:
            virtual void *getBuffer() { return pData; };
    };

    class LADSPAInputPort: public LADSPAPort
    {
        private:
            float   fPrev;
            float   fValue;

        public:
            LADSPAInputPort(const port_t *meta) : LADSPAPort(meta)
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
            LADSPAOutputPort(const port_t *meta) : LADSPAPort(meta)
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
