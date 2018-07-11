/*
 * ui_ports.h
 *
 *  Created on: 16 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_VST_UI_PORTS_H_
#define CONTAINER_VST_UI_PORTS_H_

namespace lsp
{
    class VSTUIPort: public IUIPort
    {
        protected:
            VSTPort                *pPort;

        public:
            VSTUIPort(const port_t *meta, VSTPort *port):
                IUIPort(meta)
            {
                pPort       = port;
            }

            virtual ~VSTUIPort()
            {
                pPort       = NULL;
            }

        public:
            inline VSTPort *getPort() { return pPort; };

            virtual void sync()         { };
    };

    class VSTUIParameterPort: public VSTUIPort
    {
        protected:
            float           fValue;
            vst_serial_t    nSID;

        public:
            VSTUIParameterPort(const port_t *meta, VSTParameterPort *port):
                VSTUIPort(meta, port)
            {
                fValue      = meta->start;
                nSID        = port->getSID() - 1;
            }

            virtual ~VSTUIParameterPort()
            {
                fValue      = pMetadata->start;
            }

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual void setValue(float value)
            {
                fValue = value;
                if (pPort != NULL)
                    pPort->writeValue(value);
            }

            virtual void sync()
            {
                vst_serial_t sid = static_cast<VSTParameterPort *>(pPort)->getSID();
                if (sid != nSID)
                {
                    fValue      = pPort->getValue();
                    nSID        = sid;
                    notifyAll();
                }
            }

            virtual void *getBuffer()
            {
                return pPort->getBuffer();
            }
    };

    class VSTUIMeterPort: public VSTUIPort
    {
        private:
            float   fValue;

        public:
            VSTUIMeterPort(const port_t *meta, VSTPort *port):
                VSTUIPort(meta, port)
            {
                fValue      = meta->start;
            }

            virtual ~VSTUIMeterPort()
            {
                fValue      = pMetadata->start;
            }

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual void sync()
            {
                float value = pPort->getValue();
                if (value != fValue)
                {
                    fValue      = value;
                    notifyAll();
                }
            }
    };

    class VSTUIMeshPort: public VSTUIPort
    {
        private:
            mesh_t      *pMesh;

        public:
            VSTUIMeshPort(const port_t *meta, VSTPort *port):
                VSTUIPort(meta, port)
            {
                pMesh       = vst_create_mesh(meta);
            }

            virtual ~VSTUIMeshPort()
            {
                vst_destroy_mesh(pMesh);
                pMesh = NULL;
            }

        public:
            virtual void sync()
            {
                mesh_t *mesh = reinterpret_cast<mesh_t *>(pPort->getBuffer());
                if (mesh == NULL)
                    return;

                if (mesh->containsData())
                {
                    // Copy mesh data
                    for (size_t i=0; i < mesh->nBuffers; ++i)
                        dsp::copy(pMesh->pvData[i], mesh->pvData[i], mesh->nItems);
                    pMesh->data(mesh->nBuffers, mesh->nItems);

                    // Clean source mesh
                    mesh->cleanup();

                    // Notify all for changes
                    notifyAll();
                }
            }

            virtual void *getBuffer()
            {
                return pMesh;
            }
    };
}

#endif /* CONTAINER_VST_UI_PORTS_H_ */
