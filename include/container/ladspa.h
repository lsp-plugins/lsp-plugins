/*
 * LADSPA.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LADSPA_H_
#define CONTAINER_LADSPA_H_

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

            virtual void pre_process()
            {
                if (pData == NULL)
                    return;
                fValue      = *pData;
                if (pMetadata->flags & F_UPPER)
                {
                    if (fValue > pMetadata->max)
                        fValue = pMetadata->max;
                }
                if (pMetadata->flags & F_LOWER)
                {
                    if (fValue < pMetadata->min)
                        fValue = pMetadata->min;
                }
            }

            virtual bool changed() { return fValue != fPrev; };

            virtual void update() { fPrev = fValue; };
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

                if (pData != NULL)
                    *pData      = value;
            };

            virtual void bind(void *data) { pData = reinterpret_cast<float *>(data); };
    };

}


#endif /* CONTAINER_LADSPA_H_ */
