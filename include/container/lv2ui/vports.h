/*
 * lv2ui.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2UI_VPORTS_H_
#define CONTAINER_LV2UI_VPORTS_H_

namespace lsp
{
    class LV2UIMeshPort: public LV2UIAtomVirtualPort
    {
        protected:
            LV2Mesh                 sMesh;
            bool                    bParsed;

        public:
            explicit LV2UIMeshPort(const port_t *meta, LV2UIAtomTransport *tr) :
                LV2UIAtomVirtualPort(meta, tr, tr->extensions()->uridMeshType)
            {
                sMesh.init(meta, tr->extensions());
                bParsed = false;
            }

            virtual ~LV2UIMeshPort()
            {
            };

        public:

            virtual void deserialize(const LV2_Atom_Object* obj)
            {
                // Parse atom body
                bParsed     = false;
                LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);

                // Get number of vectors (dimensions)
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    return;
//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key != sMesh.uridDimensions) || (body->value.type != pExt->forge.Int))
                    return;
                ssize_t dimensions = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
//                lsp_trace("dimensions=%d", int(dimensions));
                if (dimensions < ssize_t(sMesh.nBuffers))
                    return;
                sMesh.pMesh->nBuffers   = dimensions;

                // Get size of each vector
                body = lv2_atom_object_next(body);
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    return;

//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key != sMesh.uridItems) || (body->value.type != pExt->forge.Int))
                    return;
                ssize_t vector_size = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
//                lsp_trace("vector size=%d", int(vector_size));
                if ((vector_size < 0) || (vector_size > ssize_t(sMesh.nMaxItems)))
                    return;
                sMesh.pMesh->nItems     = vector_size;

                // Now parse each vector
                for (size_t i=0; i < sMesh.nBuffers; ++i)
                {
                    // Read vector as array of floats
                    body = lv2_atom_object_next(body);
                    if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                        return;

//                    lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                    lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));
//                    lsp_trace("sMesh.pUrids[%d] (%d) = %s", int(i), int(sMesh.pUrids[i]), pExt->unmap_urid(sMesh.pUrids[i]));

                    if ((body->key != sMesh.pUrids[i]) || (body->value.type != pExt->forge.Vector))
                        return;
                    const LV2_Atom_Vector *v = reinterpret_cast<const LV2_Atom_Vector *>(&body->value);

//                    lsp_trace("v->body.child_type (%d) = %s", int(v->body.child_type), pExt->unmap_urid(v->body.child_type));
//                    lsp_trace("v->body.child_size = %d", int(v->body.child_size));
                    if ((v->body.child_size != sizeof(float)) || (v->body.child_type != pExt->forge.Float))
                        return;
                    ssize_t v_items     = (v->atom.size - sizeof(LV2_Atom_Vector_Body)) / sizeof(float);
//                    lsp_trace("vector items=%d", int(v_items));
                    if (v_items != vector_size)
                        return;

                    // Now we can surely copy data
//                    lsp_trace("memcpy(%p, %p, %d)", sMesh.pMesh->pvData[i], v + 1, int(v_items * sizeof(float)));
                    memcpy(sMesh.pMesh->pvData[i], reinterpret_cast<const float *>(v + 1), v_items * sizeof(float));
                }

                // Update mesh parameters
                bParsed                 = true;

//                lsp_trace("complete read mesh");

                notifyAll();
            }

            virtual void *getBuffer()
            {
                if (!bParsed)
                    return NULL;

                sMesh.pMesh->nBuffers   = sMesh.nBuffers;
                return sMesh.pMesh;
            }
    };
}


#endif /* CONTAINER_LV2UI_VPORTS_H_ */
