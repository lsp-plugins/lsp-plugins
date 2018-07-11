/*
 * lv2ui.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2UI_TRANSPORT_H_
#define CONTAINER_LV2UI_TRANSPORT_H_

namespace lsp
{
    class LV2UIAtomTransport;

    class LV2UIAtomVirtualPort: public LV2UIPort
    {
        protected:
            LV2UIAtomTransport *pTr;
            LV2_URID            uridType;

        public:
            LV2UIAtomVirtualPort(const port_t *meta, LV2UIAtomTransport *tr, LV2_URID type);

            virtual ~LV2UIAtomVirtualPort();

            virtual void serialize() {};

            virtual void deserialize(const LV2_Atom_Object *obj) {};

            inline LV2_URID get_type_urid() { return uridType; };
    };

    class LV2UIAtomTransportInput: public LV2UIPort
    {
        protected:
            LV2UIAtomTransport *pTr;

        public:
            LV2UIAtomTransportInput(const port_t *meta, LV2UIAtomTransport *tr);
            virtual ~LV2UIAtomTransportInput();

            virtual void notify(const void *buffer, size_t protocol, size_t size);
    };

    class LV2UIAtomTransportOutput: public LV2UIPort
    {
        protected:
            LV2UIAtomTransport *pTr;

        public:
            LV2UIAtomTransportOutput(const port_t *meta, LV2UIAtomTransport *tr);
            virtual ~LV2UIAtomTransportOutput();

            virtual void send(LV2UIAtomVirtualPort *port);
    };

    class LV2UIAtomTransport
    {
        protected:
            LV2Extensions              *pExt;
            plugin_ui                  *pUI;
            LV2UIAtomTransportInput    *pIn;
            LV2UIAtomTransportOutput   *pOut;
            ssize_t                     nRefs;
            size_t                      nBufSize;
            uint8_t                    *pBuffer;

            cvector<LV2UIAtomVirtualPort> vPorts;

        public:
            LV2UIAtomTransport(const port_t *meta, LV2Extensions *ext, plugin_ui *ui)
            {
                ext             ->bind();
                pExt            = ext;
                pUI             = ui;
                nRefs           = 3;
                nBufSize        = lv2_all_port_sizes(pUI->metadata(), F_OUT);
                pBuffer         = new uint8_t[nBufSize];

                pIn             = new LV2UIAtomTransportInput(&meta[0], this);
                pOut            = new LV2UIAtomTransportOutput(&meta[1], this);
            }

            ~LV2UIAtomTransport()
            {
                lsp_trace("destroy");
                nRefs       = 0;

                if (pExt != NULL)
                {
                    pExt        ->unbind();
                    pExt        = NULL;
                }

                if (pBuffer != NULL)
                {
                    delete [] pBuffer;
                    pBuffer = NULL;
                }

                vPorts.clear();
            };

        public:
            bool bind(LV2UIAtomVirtualPort *port)
            {
                if (!vPorts.add(port))
                    return false;
                nRefs++;
                return true;
            }

            void unbind()
            {
                if ((--nRefs) <= 0)
                    delete this;
            }

            inline LV2Extensions *extensions() { return pExt; }

            inline plugin_ui *get_ui() { return pUI; }

            void    notify(const LV2_Atom_Object *obj);

            void    serialize();

            inline LV2UIPort *in()    { return pIn;   }
            inline LV2UIPort *out()   { return pOut;  }

            void    query_state();
    };

    LV2UIAtomTransportInput::LV2UIAtomTransportInput(const port_t *meta, LV2UIAtomTransport *tr):
        LV2UIPort(meta, -1, tr->extensions())
    {
        pTr = tr;
    }

    LV2UIAtomTransportInput::~LV2UIAtomTransportInput()
    {
        lsp_trace("destroy");
        if (pTr != NULL)
        {
            pTr->unbind();
            pTr     = NULL;
        }
    }

    void LV2UIAtomTransportInput::notify(const void *buffer, size_t protocol, size_t size)
    {
//        lsp_trace("received notification = %p, %d, %d", buffer, int(protocol), int(size));

        // validate protocol
//        lsp_trace("protocol (%d) = %s", int(protocol), pExt->unmap_urid(protocol));
        if (protocol != pExt->uridEventTransfer)
            return;

        // Check that event is an object
        const LV2_Atom* atom = reinterpret_cast<const LV2_Atom*>(buffer);
//        lsp_trace("atom.type (%d) = %s", int(atom->type), pExt->unmap_urid(atom->type));
        if (atom->type != pExt->uridObject)
            return;

        pTr->notify(reinterpret_cast<const LV2_Atom_Object *>(atom));
    }

    LV2UIAtomTransportOutput::LV2UIAtomTransportOutput(const port_t *meta, LV2UIAtomTransport *tr):
        LV2UIPort(meta, -1, tr->extensions())
    {
        pTr                 = tr;
    }

    LV2UIAtomTransportOutput::~LV2UIAtomTransportOutput()
    {
        lsp_trace("destroy");
        if (pTr != NULL)
        {
            pTr->unbind();
            pTr     = NULL;
        }
    }

    LV2UIAtomVirtualPort::LV2UIAtomVirtualPort(const port_t *meta, LV2UIAtomTransport *tr, LV2_URID type):
            LV2UIPort(meta, -1, tr->extensions())
    {
        pTr         = tr;
        uridType    = type;
        tr->bind(this);
    }

    LV2UIAtomVirtualPort::~LV2UIAtomVirtualPort()
    {
        if (pTr != NULL)
        {
            pTr->unbind();
            pTr     = NULL;
        }
    }

    void LV2UIAtomTransportOutput::send(LV2UIAtomVirtualPort *port)
    {
    }

    void LV2UIAtomTransport::query_state()
    {
        LV2_Atom_Forge_Frame    frame;

        lsp_trace("querying state buffer=%p, size=%d", pBuffer, int(nBufSize));

        // Serialize message
        pExt->forge_set_buffer(pBuffer, nBufSize);
        LV2_Atom *msg       = pExt->forge_object(&frame, pExt->uridState, pExt->uridStateType);
        pExt->forge_pop(&frame);

        pOut->write_data(lv2_atom_total_size(msg), pExt->uridEventTransfer, msg);
        lsp_trace("state request has been written");
    }

    void LV2UIAtomTransport::notify(const LV2_Atom_Object *obj)
    {
        // Check that event type is a plugin state object
        lsp_trace("body.id (%d) = %s", int(obj->body.id), pExt->unmap_urid(obj->body.id));
        lsp_trace("body.otype (%d) = %s", int(obj->body.otype), pExt->unmap_urid(obj->body.otype));

        if ((obj->body.id == pExt->uridState) && (obj->body.otype == pExt->uridStateType))
        {
            lsp_trace("received UI_STATE object");

            // Parse atom body
            LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);

            while (!lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
            {
                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                // Scan all ports
                for (size_t port_id=0; ; ++port_id)
                {
                    IUIPort *p    = pUI->port(port_id);
                    if (p == NULL)
                    {
                        lsp_trace("Unknown port for urid %d (%s)", body->key, pExt->unmap_urid(body->key));
                        break;
                    }

                    // Check that port's URID matches URID of the atom
                    LV2UIPort *l2p  = static_cast<LV2UIPort *>(p);
                    if (l2p->get_urid() == body->key)
                    {
                        // Port matched, analyze
                        const port_t *port = l2p->metadata();
                        if (!(port->flags & F_OUT))
                            continue;

                        // Analyze port role
                        switch (port->role)
                        {
                            case R_CONTROL:
                            case R_METER:
                                if (body->value.type == pExt->forge.Float)
                                {
                                    const LV2_Atom_Float *f_atom = reinterpret_cast<LV2_Atom_Float *>(&body->value);
                                    lsp_trace("%s = %f", port->id, f_atom->body);
                                    l2p->notify(&f_atom->body, 0, f_atom->atom.size);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                }

                lsp_trace("call lv2_atom_object_next");
                body = lv2_atom_object_next(body);
            }

            lsp_trace("complete read state");
        }
        else
        {
            for (size_t i=0; i<vPorts.size(); ++i)
            {
                if ((obj->body.id == vPorts[i]->get_urid()) && (obj->body.otype == vPorts[i]->get_type_urid()))
                    vPorts[i]->deserialize(obj);
            }
        }
    }

}


#endif /* CONTAINER_LV2UI_TRANSPORT_H_ */
