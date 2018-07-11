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
            size_t                  nSamples;
            size_t                  nFrameSize;

        public:
            LV2MeshPort(const port_t *meta, LV2AtomTransport *tr): LV2AtomVirtualPort(meta, tr, tr->extensions()->uridMeshType)
            {
                sMesh.init(meta, tr->extensions());
                nSamples            = 0;
                nFrameSize          = 0;
            }

            virtual ~LV2MeshPort()
            {
                nSamples            = 0;
                nFrameSize          = 0;
            };

        public:
            virtual void *getBuffer()
            {
                return sMesh.pMesh;
            }

            virtual void set_sample_rate(long sr)
            {
                nFrameSize      = sr / 25; // 25 frames per second
            }

            virtual bool pre_process(size_t samples)
            {
                // Get mesh
                mesh_t *mesh = sMesh.pMesh;
                if ((mesh == NULL) || (nFrameSize <= 0))
                    return false;

                // Increment number of pending samples
                nSamples       += samples;

                // Mesh is waiting for data request, process pending samples
                if (mesh->isWaiting())
                {
                    if (nSamples >= nFrameSize)
                    {
                        mesh->cleanup();
                        nSamples           -= nFrameSize; // Decrement number of pending samples
                    }
                }

                return false;
            }

            virtual bool pending()
            {
                mesh_t *mesh = sMesh.pMesh;
                if (mesh == NULL)
                    return false;

                // Return true only if mesh contains data
                return mesh->containsData();
            };

            virtual void serialize()
            {
                mesh_t *mesh = sMesh.pMesh;

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

                // Set mesh waiting until next frame is allowed
                mesh->setWaiting();
            }
    };

}


#endif /* CONTAINER_LV2_VPORTS_H_ */
