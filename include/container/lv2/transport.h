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

            virtual void set_sample_rate(long sr)           { };

            virtual bool pending()                          { return false;     };

            virtual void serialize()                        { };

            virtual void deserialize(const LV2_Atom *atom)  { };

            inline LV2_URID get_type_urid()                 { return uridType; };
    };

    class LV2AtomTransportInput: public LV2AtomPort
    {
        protected:
            LV2AtomTransport   *pTr;

        public:
            LV2AtomTransportInput(const port_t *meta, LV2AtomTransport *ext);
            virtual ~LV2AtomTransportInput();

            virtual bool pre_process(size_t samples);
    };

    class LV2AtomTransportOutput: public LV2AtomPort
    {
        protected:
            LV2AtomTransport   *pTr;

        public:
            LV2AtomTransportOutput(const port_t *meta, LV2AtomTransport *ext);
            virtual ~LV2AtomTransportOutput();

            virtual void post_process(size_t samples);
    };

    class LV2AtomTransport
    {
        protected:
            LV2Extensions          *pExt;
            plugin_t               *pPlugin;
            ssize_t                 nStateRequests;
            ssize_t                 nPatchRequests;
            LV2AtomPort            *pIn;
            LV2AtomPort            *pOut;

            cvector<LV2AtomVirtualPort> vPorts;

        public:
            LV2AtomTransport(const port_t *meta, LV2Extensions *ext, plugin_t *plugin)
            {
                pExt                = ext;
                pPlugin             = plugin;
                nStateRequests      = 0;
                nPatchRequests      = 0;
                pIn                 = new LV2AtomTransportInput(&meta[0], this);
                pOut                = new LV2AtomTransportOutput(&meta[1], this);
            }

            ~LV2AtomTransport()
            {
                vPorts.clear();

                nStateRequests  = 0;
                pExt        = NULL;

                // Input and output transport ports are not stored in internal ports, have to be deleted here
                if (pOut != NULL)
                {
                    delete pOut;
                    pOut        = NULL;
                }
                if (pIn != NULL)
                {
                    delete pIn;
                    pIn         = NULL;
                }
            };

        public:
            void init()
            {
                long srate = pPlugin->get_sample_rate();
                for (size_t i=0; i<vPorts.size(); ++i)
                    vPorts[i]->set_sample_rate(srate);
            }

            inline void add_port(LV2AtomVirtualPort *port)
            {
                vPorts.add(port);
            }

            inline void trigger_state_request() { nStateRequests++; };
            inline void trigger_patch_request() { nPatchRequests++; };

            inline LV2Extensions *extensions() { return pExt; }

            size_t pending()
            {
                size_t count = 0;
                if ((nStateRequests + nPatchRequests) > 0)
                    count++;
                for (size_t i=0; i<vPorts.size(); ++i)
                    if (vPorts[i]->pending())
                        count++;
                return count;
            }

            inline plugin_t *get_plugin() { return pPlugin; }

            void    deserialize(LV2_URID urid, LV2_URID type, const LV2_Atom *atom);

            void    serialize_virtual_ports();
            void    serialize_state();
            void    serialize_patches();

            inline LV2AtomPort *in()    { return pIn;   }
            inline LV2AtomPort *out()   { return pOut;  }
    };
}

#include <container/lv2/vports.h>

namespace lsp
{
    LV2AtomTransportInput::~LV2AtomTransportInput()
    {
        pTr = NULL;
    }

    LV2AtomTransportOutput::~LV2AtomTransportOutput()
    {
        pTr = NULL;
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
        pTr     = NULL;
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

    bool LV2AtomTransportInput::pre_process(size_t samples)
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
                    pTr->trigger_state_request();
                }
                else if ((obj->body.otype == pExt->uridPatchGet) && (obj->body.id == pExt->uridChunk))
                {
                    lsp_trace("triggered patch request");
                    LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);

                    while (!lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    {
                        lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                        lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                        body = lv2_atom_object_next(body);
                    }

                    pTr->trigger_patch_request();
                }
                else if ((obj->body.otype == pExt->uridPatchSet) && (obj->body.id == pExt->uridChunk))
                {
                    // Parse atom body
                    LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);
                    const LV2_Atom_URID    *key     = NULL;
                    const LV2_Atom         *value   = NULL;

                    while (!lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    {
                        lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                        lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                        if ((body->key  == pExt->uridPatchProperty) && (body->value.type == pExt->uridAtomUrid))
                        {
                            key     = reinterpret_cast<const LV2_Atom_URID *>(&body->value);
                            lsp_trace("body->value.body (%d) = %s", int(key->body), pExt->unmap_urid(key->body));
                        }
                        else if (body->key   == pExt->uridPatchValue)
                            value   = &body->value;

                        if ((key != NULL) && (value != NULL))
                        {
                            pTr->deserialize(key->body, value->type, value);

                            key     = NULL;
                            value   = NULL;
                        }

                        body = lv2_atom_object_next(body);
                    }
                }
                else
                    pTr->deserialize(obj->body.id, obj->body.otype, reinterpret_cast<const LV2_Atom *>(obj + 1));
            }
            ev = lv2_atom_sequence_next(ev);
        }

        return false;
    }

    void LV2AtomTransportOutput::post_process(size_t samples)
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

        // Call serialization routines
        pTr->serialize_state();
        pTr->serialize_patches();
        pTr->serialize_virtual_ports();

        // Complete sequence
        pExt->forge_pop(&seq);
    }

    void LV2AtomTransport::serialize_patches()
    {
        // Check that there is pending request
        if (nPatchRequests <= 0)
            return;
        nPatchRequests--;

        // Serialize patches
        lsp_trace("Serializing patches");

        plugin_t *p = pPlugin;
        for (size_t port_id = 0; ; ++port_id)
        {
            // Get port instance
            IPort   *src    = p->port(port_id);
            if (src == NULL)
                break;

            // Get port metadata
            const port_t *m  = src->metadata();

            // Analyze port role
            switch (m->role)
            {
                case R_PATH:
                {
                    // Get port
                    LV2PathPort *lvp    = static_cast<LV2PathPort *>(src);
                    LV2_URID urid       = lvp->get_urid();
                    path_t *path        = reinterpret_cast<path_t *>(lvp->getBuffer());
                    lsp_trace("Serializing port urid=%d, uri=%s", int(urid), lvp->get_uri());
                    lsp_trace("  id=%s, value=%s", m->id, path->get_path());

                    // Serialize atom
                    LV2_Atom_Forge_Frame    frame;
                    pExt->forge_frame_time(0); // Event header
                    pExt->forge_object(&frame, pExt->uridPatchMessage, pExt->uridPatchSet);
                    pExt->forge_key(pExt->uridPatchProperty);
                    pExt->forge_urid(urid);
                    pExt->forge_key(pExt->uridPatchValue);
                    pExt->forge_path(path->get_path());

                    // Complete event
                    pExt->forge_pop(&frame);
                    break;
                }

                default:
                    break;
            }
        }

        lsp_trace("All patches have been serialized");
    }

    void LV2AtomTransport::serialize_state()
    {
        // Check that there is pending request
        if (nStateRequests <= 0)
            return;
        nStateRequests--;

        // Serialize state of control/meter ports if state request is pending
        lsp_trace("Serializing state");

        LV2_Atom_Forge_Frame    frame;
        pExt->forge_frame_time(0); // Event header
        pExt->forge_object(&frame, pExt->uridState, pExt->uridStateType);

        plugin_t *p = pPlugin;
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

    void LV2AtomTransport::serialize_virtual_ports()
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

    void LV2AtomTransport::deserialize(LV2_URID urid, LV2_URID type, const LV2_Atom *atom)
    {
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            lsp_trace("Check virtual port %s", vPorts[i]->metadata()->id);
            if (vPorts[i]->get_urid() != urid)
            {
                lsp_trace("vPorts[i]->get_urid() (%d) != urid (%d)",
                    int(vPorts[i]->get_type_urid()), int(urid));
                continue;
            }
            if (vPorts[i]->get_type_urid() != type)
            {
                lsp_trace("vPorts[i]->get_type_urid() (%d) != type (%d)",
                    int(vPorts[i]->get_type_urid()), int(type));
                continue;
            }

            // Deserialize object
            vPorts[i]->deserialize(atom);
        }
    }

}


#endif /* CONTAINER_LV2_TRANSPORT_H_ */
