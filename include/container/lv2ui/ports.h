/*
 * lv2ui.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2UI_PORTS_H_
#define CONTAINER_LV2UI_PORTS_H_


namespace lsp
{
    // Specify port classes
    class LV2UIPort: public IUIPort
    {
        protected:
            ssize_t                 nID;
            LV2Extensions          *pExt;
            LV2_URID                urid;

        public:
            explicit LV2UIPort(const port_t *meta, LV2Extensions *ext) : IUIPort(meta)
            {
                nID         = -1;
                pExt        = ext;
                urid        = (meta != NULL) ? pExt->map_port(meta->id) : -1;
            }

            virtual ~LV2UIPort()
            {
            }

        public:
            inline void write_data(
                    uint32_t         buffer_size,
                    uint32_t         port_protocol,
                    const void*      buffer)
            {
                if (nID >= 0)
                {
                    lsp_trace("write_data(%d, %d, %d, %p)", int(nID), int(buffer_size), int(port_protocol), buffer);
                    pExt->write_data(nID, buffer_size, port_protocol, buffer);
                }
            }

            virtual void notify(const void *buffer, size_t protocol, size_t size)
            {
                notifyAll();
            }

            inline LV2_URID         get_urid() const    { return urid; };
            inline const char      *get_uri() const     { return (pExt->unmap_urid(urid)); };
            inline void             set_id(ssize_t id)  { nID = id; };
            inline ssize_t          get_id() const      { return nID; };
    };

    class LV2UIFloatPort: public LV2UIPort
    {
        protected:
            float   fValue;

        public:
            explicit LV2UIFloatPort(const port_t *meta, LV2Extensions *ext) :
                LV2UIPort(meta, ext)
            {
                fValue  =   meta->start;
            }
            virtual ~LV2UIFloatPort() { fValue  =   metadata()->start; };

        public:
            virtual float getValue() { return fValue; }

            virtual void setValue(float value)
            {
                fValue      = value;
                pExt->write_data(nID, sizeof(float), 0, &fValue);
            }

            virtual void notify(const void *buffer, size_t protocol, size_t size)
            {
                if (size == sizeof(float))
                {
                    fValue = *(reinterpret_cast<const float *>(buffer));
                    LV2UIPort::notify(buffer, protocol, size);
                }
            }
    };

    class LV2UIPeakPort: public LV2UIFloatPort
    {
        public:
            explicit LV2UIPeakPort(const port_t *meta, LV2Extensions *ext) :
                LV2UIFloatPort(meta, ext) {}
            virtual ~LV2UIPeakPort() {};

        public:
            virtual void notify(const void *buffer, size_t protocol, size_t size)
            {
                if (size == sizeof(LV2UI_Peak_Data))
                {
                    fValue = (reinterpret_cast<const LV2UI_Peak_Data *>(buffer))->peak;
                    notifyAll();
                    return;
                }
                LV2UIFloatPort::notify(buffer, protocol, size);
            }
    };

}


#endif /* CONTAINER_LV2UI_PORTS_H_ */
