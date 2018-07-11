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
                nFrameSize      = sr / MESH_REFRESH_RATE;
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

    class LV2PathPort: public LV2AtomVirtualPort
    {
        private:
            lv2_path_t      sPath;

            inline void set_string(const char *string, size_t len)
            {
                lsp_trace("submitting path to %s (lenght = %d)", string, int(len));
                sPath.submit(string, len);
            }

        public:
            LV2PathPort(const port_t *meta, LV2AtomTransport *tr): LV2AtomVirtualPort(meta, tr, tr->extensions()->uridPathType)
            {
                sPath.init();
            }

        public:
            virtual void *getBuffer()
            {
                return static_cast<path_t *>(&sPath);
            }

            virtual void deserialize(const LV2_Atom *atom)
            {
                if (atom->type != pExt->uridPathType)
                    return;
                set_string(reinterpret_cast<const char *>(atom + 1), atom->size);
            }

            virtual void save()
            {
                lsp_trace("save port id=%s, urid=%d (%s), value=%s", pMetadata->id, urid, get_uri(), sPath.sPath);
                pExt->store_value(urid, pExt->uridPathType, sPath.sPath, strlen(sPath.sPath) + sizeof(char));
            }

            virtual void restore()
            {
                lsp_trace("restore port id=%s, urid=%d (%s)", pMetadata->id, urid, get_uri());
                size_t count            = 0;
                const char *path        = reinterpret_cast<const char *>(pExt->restore_value(urid, pExt->uridPathType, &count));
                if (path != NULL)
                    set_string(path, count);
                else
                    set_string("", 0);
            }

            virtual bool pre_process(size_t samples)
            {
                return sPath.pending();
            }
    };

}


#endif /* CONTAINER_LV2_VPORTS_H_ */
