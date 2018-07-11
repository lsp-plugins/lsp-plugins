/*
 * lv2.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_TRANSPORT_H_
#define CONTAINER_LV2_TRANSPORT_H_

namespace lsp
{
    class LV2AtomTransport;

    class LV2AtomVirtualPort: public LV2Port
    {
        protected:
            LV2AtomTransport   *pTr;
            LV2_URID            uridType;

        public:
            LV2AtomVirtualPort(const port_t *meta, LV2AtomTransport *tr, LV2_URID type);

            virtual ~LV2AtomVirtualPort();

            virtual bool pending()      { return false; };

            virtual void serialize()    { };

            virtual void deserialize(const LV2_Atom *atom) {};

            inline LV2_URID get_type_urid() { return uridType; };
    };

    class LV2AtomTransportInput: public LV2AtomPort
    {
        protected:
            LV2AtomTransport   *pTr;

        public:
            LV2AtomTransportInput(const port_t *meta, LV2AtomTransport *ext);
            virtual ~LV2AtomTransportInput();

            virtual bool pre_process();
    };

    class LV2AtomTransportOutput: public LV2AtomPort
    {
        protected:
            LV2AtomTransport   *pTr;

        public:
            LV2AtomTransportOutput(const port_t *meta, LV2AtomTransport *ext);
            virtual ~LV2AtomTransportOutput();

            virtual void post_process();
    };

    class LV2AtomTransport
    {
        protected:
            LV2Extensions          *pExt;
            plugin_t               *pPlugin;
            LV2AtomPort            *pIn;
            LV2AtomPort            *pOut;
            ssize_t                 nTriggered;

            cvector<LV2AtomVirtualPort> vPorts;

        public:
            LV2AtomTransport(const port_t *meta, LV2Extensions *ext, plugin_t *plugin)
            {
                pExt        = ext;
                pPlugin     = plugin;
                nTriggered  = 0;

                pIn         = new LV2AtomTransportInput(&meta[0], this);
                pOut        = new LV2AtomTransportOutput(&meta[1], this);
            }

            ~LV2AtomTransport()
            {
                lsp_trace("destroy");
                nTriggered  = 0;
                pExt        = NULL;

                vPorts.clear();

                if (pOut != NULL)
                {
                    delete pOut;
                    pOut    = NULL;
                }

                if (pIn != NULL)
                {
                    delete pIn;
                    pIn     = NULL;
                }
            };

        public:
            inline void add_port(LV2AtomVirtualPort *port)
            {
                vPorts.add(port);
            }

            inline void trigger_on() { nTriggered++; };

            inline bool trigger_off()
            {
                if (nTriggered <= 0)
                    return false;
                nTriggered--;
                return true;
            };

            inline LV2Extensions *extensions() { return pExt; }

            size_t pending()
            {
                size_t count = 0;
                if (nTriggered > 0)
                    count++;
                for (size_t i=0; i<vPorts.size(); ++i)
                    if (vPorts[i]->pending())
                        count++;
                return count;
            }

            inline plugin_t *get_plugin() { return pPlugin; }

            void    deserialize(const LV2_Atom_Object *obj);

            void    serialize();

            inline LV2AtomPort *in()    { return pIn;   }
            inline LV2AtomPort *out()   { return pOut;  }
    };

    LV2AtomTransportInput::~LV2AtomTransportInput()
    {
        lsp_trace("destroy");
            pTr = NULL;
//        }
    }

    LV2AtomTransportOutput::~LV2AtomTransportOutput()
    {
        lsp_trace("destroy");
//        if (pTr != NULL)
//        {
//            pTr->unbind();
            pTr = NULL;
//        }
    }

    LV2AtomVirtualPort::LV2AtomVirtualPort(const port_t *meta, LV2AtomTransport *tr, LV2_URID type):
            LV2Port(meta, tr->extensions())
    {
        pTr         = tr;
        uridType    = type;
        tr->add_port(this);
    }

    LV2AtomVirtualPort::~LV2AtomVirtualPort()
    {
//        if (pTr != NULL)
//        {
//            pTr->unbind();
            pTr     = NULL;
//        }
    }

    LV2AtomTransportInput::LV2AtomTransportInput(const port_t *meta, LV2AtomTransport *tr):
        LV2AtomPort(meta, tr->extensions())
    {
        pTr         = tr;
    }

    LV2AtomTransportOutput::LV2AtomTransportOutput(const port_t *meta, LV2AtomTransport *tr):
        LV2AtomPort(meta, tr->extensions())
    {
        pTr         = tr;
    }

    bool LV2AtomTransportInput::pre_process()
    {
        if (pSequence == NULL)
            return false;

        const LV2_Atom_Event* ev = lv2_atom_sequence_begin(&pSequence->body);

//        lsp_trace("pSequence->atom.type (%d) = %s", int(pSequence->atom.type), pExt->unmap_urid(pSequence->atom.type));
//        lsp_trace("pSequence->atom.size = %d", int(pSequence->atom.size));

        while (!lv2_atom_sequence_is_end(&pSequence->body, pSequence->atom.size, ev))
        {
            lsp_trace("ev->body.type (%d) = %s", int(ev->body.type), pExt->unmap_urid(ev->body.type));

            // If the event is an object
            if (ev->body.type == pExt->uridObject)
            {
                const LV2_Atom_Object *obj = reinterpret_cast<const LV2_Atom_Object*>(&ev->body);
                lsp_trace("obj->body.otype (%d) = %s", int(obj->body.otype), pExt->unmap_urid(obj->body.otype));
                lsp_trace("obj->body.id (%d) = %s", int(obj->body.id), pExt->unmap_urid(obj->body.id));

                if ((obj->body.otype == pExt->uridStateType) && (obj->body.id == pExt->uridState))
                {
                    lsp_trace("triggered state request");
                    pTr->trigger_on();
                }
                else
                    pTr->deserialize(obj);
            }
            ev = lv2_atom_sequence_next(ev);
        }

        return false;
    }

    void LV2AtomTransportOutput::post_process()
    {
        if (pSequence == NULL)
            return;
        if (pTr->pending() <= 0)
            return;

        // Initialize forge
        pExt->forge_set_buffer(pSequence, pSequence->atom.size);

        // Forge sequence header
        LV2_Atom_Forge_Frame    seq;
        pExt->forge_sequence_head(&seq, 0);

        // Serialize state of control/meter ports if state request is pending
        if (pTr->trigger_off())
        {
            lsp_trace("Serializing state");

            LV2_Atom_Forge_Frame    frame;
            pExt->forge_frame_time(0); // Event header
            pExt->forge_object(&frame, pExt->uridState, pExt->uridStateType);

            plugin_t *p = pTr->get_plugin();
            for (size_t port_id = 0; ; ++port_id)
            {
                // Get port instance
                IPort   *src    = p->port(port_id);
                if (src == NULL)
                    break;

                // Get port metadata
                const port_t *port  = src->metadata();

                // Serialize only output ports
                if (!(port->flags & F_OUT))
                    continue;

                LV2Port *p      = static_cast<LV2Port *>(src);

                // Analyze port role
                switch (port->role)
                {
                    case R_CONTROL:
                    case R_METER:
                    {
                        LV2_URID urid   = p->get_urid();
                        float value     = p->getValue();
                        lsp_trace("Serializing port urid=%d, uri=%s", int(urid), p->get_uri());
                        lsp_trace("  id=%s, value=%f", port->id, value);
                        pExt->forge_key(urid);
                        pExt->forge_float(value);
                        break;
                    }

                    default:
                        break;
                }
            }

            // Complete event
            pExt->forge_pop(&frame);

            lsp_trace("State has been serialized");
        }

        // Serialize state of virtual ports
        pTr->serialize();

        // Complete sequence
        pExt->forge_pop(&seq);
    }

    void LV2AtomTransport::serialize()
    {
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            if (!vPorts[i]->pending())
                continue;

            // Serialize object
            LV2_Atom_Forge_Frame    frame;

            pExt->forge_frame_time(0);  // Event header
            pExt->forge_object(&frame, vPorts[i]->get_urid(), vPorts[i]->get_type_urid());
            vPorts[i]->serialize();

            // Complete event
            pExt->forge_pop(&frame);
        }
    }

    void LV2AtomTransport::deserialize(const LV2_Atom_Object *obj)
    {
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            if (obj->body.otype != vPorts[i]->get_type_urid())
                continue;
            if (obj->body.id != vPorts[i]->get_urid())
                continue;

            // Deserialize object
            vPorts[i]->deserialize(reinterpret_cast<const LV2_Atom *>(obj + 1));
        }
    }

}


#endif /* CONTAINER_LV2_TRANSPORT_H_ */
