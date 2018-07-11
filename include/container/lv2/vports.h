/*
 * lv2.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_VPORTS_H_
#define CONTAINER_LV2_VPORTS_H_

namespace lsp
{
    class LV2MeshPort: public LV2AtomVirtualPort
    {
        private:
            LV2Mesh                 sMesh;

        public:
            LV2MeshPort(const port_t *meta, LV2AtomTransport *tr): LV2AtomVirtualPort(meta, tr, tr->extensions()->uridMeshType)
            {
                sMesh.init(meta, tr->extensions());
            }

            virtual ~LV2MeshPort()
            {
            };

        public:
            virtual void *getBuffer()
            {
                return sMesh.pMesh;
            }

            virtual bool pending()
            {
                return sMesh.nBuffers > 0;
            };

            virtual void serialize()
            {
                mesh_t *mesh = sMesh.pMesh;
//                lsp_trace("serializing mesh items=%d buffers=%d", int(mesh->nItems), int(mesh->nBuffers));

                // Forge number of vectors (dimensions)
                pExt->forge_key(sMesh.uridDimensions);
                pExt->forge_int(sMesh.nBuffers);

                // Forge number of items per vector
                pExt->forge_key(sMesh.uridItems);
                pExt->forge_int(sMesh.pMesh->nItems);

                // Forge vectors
                for (size_t i=0; i < sMesh.nBuffers; ++i)
                {
                    pExt->forge_key(sMesh.pUrids[i]);
                    pExt->forge_vector(sizeof(float), pExt->forge.Float, mesh->nItems, mesh->pvData[i]);
                }

                // Clear mesh state
                mesh->nBuffers  = 0;
                mesh->nItems    = 0;

//                lsp_trace("successfully serialized mesh");
            }
    };

}


#endif /* CONTAINER_LV2_VPORTS_H_ */
