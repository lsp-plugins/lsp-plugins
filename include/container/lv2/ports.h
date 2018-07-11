/*
 * lv2.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_PORTS_H_
#define CONTAINER_LV2_PORTS_H_

namespace lsp
{
    // Specify port classes
    class LV2Port: public IPort
    {
        protected:
            LV2Extensions          *pExt;
            LV2_URID                urid;

        public:
            LV2Port(const port_t *meta, LV2Extensions *ext): IPort(meta)
            {
                ext             ->  bind();
                pExt            =   ext;
                urid            =   (meta != NULL) ? pExt->map_port(meta->id) : -1;
            }
            virtual ~LV2Port()
            {
                pExt            ->  unbind();
                pExt            =   NULL;
                urid            =   -1;
            }

        public:
            virtual void bind(void *data)
            {
            }

            inline LV2_URID         get_urid() const { return urid; }
            inline const char      *get_uri() const { return (pExt->unmap_urid(urid)); }
            inline LV2Extensions   *extensions() const { return pExt; }
    };

    class LV2AudioPort: public LV2Port
    {
        private:
            void *pBuffer;

        public:
            LV2AudioPort(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext), pBuffer(NULL) { }
            virtual ~LV2AudioPort() { pBuffer = NULL; };

        public:
            virtual void *getBuffer() { return pBuffer; };

            virtual void bind(void *data)
            {
                pBuffer = data;
            };
    };

    class LV2InputPort: public LV2Port
    {
        private:
            float  *pData;
            float   fValue;
            float   fPrev;

        public:
            LV2InputPort(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext)
            {
                pData       = NULL;
                fValue      = meta->start;
                fPrev       = meta->start;
            }
            virtual ~LV2InputPort()
            {
                pData       = NULL;
                fValue      = NAN;
                fPrev       = NAN;
            }

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual bool changed()
            {
                return fValue != fPrev;
            };

            virtual void bind(void *data)
            {
                pData = reinterpret_cast<float *>(data);
            };

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

            virtual void update()
            {
                fPrev = fValue;
            };
    };

    class LV2OutputPort: public LV2Port
    {
        private:
            float  *pData;
            float   fValue;

        public:
            LV2OutputPort(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext)
            {
                pData       = NULL;
                fValue      = meta->start;
            }
            virtual ~LV2OutputPort() { pData = NULL; };

        public:
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

                if (pData != NULL)
                    *pData      = value;
            }

            virtual void bind(void *data)
            {
                pData = reinterpret_cast<float *>(data);
            };
    };

    class LV2LatencyPort: public LV2Port
    {
        private:
            float  *pData;
            plugin *pPlugin;

        public:
            LV2LatencyPort(const port_t *meta, LV2Extensions *ext, plugin *p) : LV2Port(meta, ext)
            {
                pData       = NULL;
                pPlugin     = p;
            }
            virtual ~LV2LatencyPort()
            {
                pData       = NULL;
                pPlugin     = NULL;
            };

        public:
            virtual void bind(void *data)
            {
                lsp_trace("bind %p", data);
                pData = reinterpret_cast<float *>(data);
            };

            virtual void post_process()
            {
                float value = (pPlugin != NULL) ? pPlugin->get_latency() : 0.0f;
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

                lsp_trace("pPlugin = %p, value = %.3f, pData = %p", pPlugin, value, pData);

                if (pData != NULL)
                    *pData      = value;
            }
    };

    class LV2AtomPort: public LV2Port
    {
        protected:
            LV2_Atom_Sequence      *pSequence;

        public:
            LV2AtomPort(const port_t *meta, LV2Extensions *ext): LV2Port(meta, ext)
            {
                pSequence   =   NULL;
            }

            virtual ~LV2AtomPort()
            {
                pSequence   =   NULL;
            };

            virtual void bind(void *data)
            {
                pSequence       = reinterpret_cast<LV2_Atom_Sequence *>(data);
            }

            inline LV2_Atom_Sequence  *sequence() { return pSequence; };
    };

}


#endif /* CONTAINER_LV2_PORTS_H_ */
