/*
 * ui_ports.h
 *
 *  Created on: 18 мая 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_JACK_UI_PORTS_H_
#define CONTAINER_JACK_UI_PORTS_H_

namespace lsp
{
    class JACKUIPort: public CtlPort
    {
        protected:
            JACKPort               *pPort;

        public:
            JACKUIPort(JACKPort *port) : CtlPort(port->metadata())
            {
                pPort       = port;
            }

            virtual ~JACKUIPort()
            {
                pPort       = NULL;
            }

        public:
            virtual bool sync()         { return false; };

            virtual void resync()       { };
    };

    class JACKUIPortGroup: public JACKUIPort
    {
        private:
            JACKPortGroup          *pPG;

        public:
            JACKUIPortGroup(JACKPortGroup *port) : JACKUIPort(port)
            {
                pPG                 = port;
            }

            virtual ~JACKUIPortGroup()
            {
            }

        public:
            virtual float get_value()
            {
                return pPort->getValue();
            }

            virtual void set_value(float value)
            {
                pPort->setValue(value);
            }

        public:
            inline size_t rows() const  { return pPG->rows(); }
            inline size_t cols() const  { return pPG->cols(); }
    };

    class JACKUIControlPort: public JACKUIPort
    {
        protected:
            float           fValue;

        public:
            JACKUIControlPort(JACKPort *port): JACKUIPort(port)
            {
                fValue      = port->getValue();
            }

            virtual ~JACKUIControlPort()
            {
                fValue      = pMetadata->start;
            }

        public:
            virtual float get_value()
            {
                return fValue;
            }

            virtual void set_value(float value)
            {
                fValue  = limit_value(pMetadata, value);
                static_cast<JACKControlPort *>(pPort)->updateValue(fValue);
            }

            virtual void write(const void *buffer, size_t size)
            {
                if (size == sizeof(float))
                {
                    fValue  = *reinterpret_cast<const float *>(buffer);
                    static_cast<JACKControlPort *>(pPort)->updateValue(fValue);
                }
            }
    };

    class JACKUIMeterPort: public JACKUIPort
    {
        private:
            float   fValue;

        public:
            JACKUIMeterPort(JACKPort *port): JACKUIPort(port)
            {
                fValue      = port->getValue();
            }

            virtual ~JACKUIMeterPort()
            {
                fValue      = pMetadata->start;
            }

        public:
            virtual float get_value()
            {
                return fValue;
            }

            virtual bool sync()
            {
                float value = fValue;

                if (pMetadata->flags & F_PEAK)
                {
                    JACKMeterPort *mp = static_cast<JACKMeterPort *>(pPort);
                    fValue      = mp->syncValue();
                }
                else
                    fValue      = pPort->getValue();

                return fValue != value;
            }
    };

    class JACKUIInPort: public JACKUIPort
    {
        private:
            float   fValue;

        public:
            JACKUIInPort(JACKPort *port): JACKUIPort(port)
            {
                fValue      = port->getValue();
            }

            virtual ~JACKUIInPort()
            {
                fValue      = pMetadata->start;
            }

        public:
            virtual float get_value()
            {
                return fValue;
            }

            virtual bool sync()
            {
                float value = fValue;
                fValue      = pPort->getValue();
                return fValue != value;
            }
    };

    class JACKUIMeshPort: public JACKUIPort
    {
        private:
            mesh_t      *pMesh;

        public:
            JACKUIMeshPort(JACKPort *port): JACKUIPort(port)
            {
                pMesh       = jack_create_mesh(port->metadata());
            }

            virtual ~JACKUIMeshPort()
            {
                jack_destroy_mesh(pMesh);
                pMesh = NULL;
            }

        public:
            virtual bool sync()
            {
                mesh_t *mesh = pPort->getBuffer<mesh_t>();
                if ((mesh == NULL) || (!mesh->containsData()))
                    return false;

                // Copy mesh data
                for (size_t i=0; i < mesh->nBuffers; ++i)
                    dsp::copy_saturated(pMesh->pvData[i], mesh->pvData[i], mesh->nItems);
//                    dsp::copy(pMesh->pvData[i], mesh->pvData[i], mesh->nItems);
                pMesh->data(mesh->nBuffers, mesh->nItems);

                // Clean source mesh
                mesh->cleanup();
                return true;
            }

            virtual void *get_buffer()
            {
                return pMesh;
            }
    };

    class JACKUIFrameBufferPort: public JACKUIPort
    {
        private:
            frame_buffer_t     sFB;

        public:
            JACKUIFrameBufferPort(JACKPort *port): JACKUIPort(port)
            {
                sFB.init(pMetadata->start, pMetadata->step);
            }

            virtual ~JACKUIFrameBufferPort()
            {
                sFB.destroy();
            }

        public:
            virtual bool sync()
            {
                // Check if there is data for viewing
                frame_buffer_t *fb = pPort->getBuffer<frame_buffer_t>();
                return (fb != NULL) ? sFB.sync(fb) : false;
            }

            virtual void *get_buffer()
            {
                return &sFB;
            }
    };

    class JACKUIPathPort: public JACKUIPort
    {
        private:
            jack_path_t    *pPath;
            char            sPath[PATH_MAX];

        public:
            JACKUIPathPort(JACKPort *port): JACKUIPort(port)
            {
                path_t *path    = reinterpret_cast<path_t *>(pPort->getBuffer());
                if (path != NULL)
                    pPath           = static_cast<jack_path_t *>(path);
                else
                    pPath           = NULL;
                sPath[0]            = '\0';
            }

            virtual ~JACKUIPathPort()
            {
                pPath       = NULL;
            }

        public:
            virtual void *get_buffer()
            {
                return sPath;
            }

            virtual void write(const void *buffer, size_t size)
            {
                // Store path string
                if (size >= PATH_MAX)
                    size = PATH_MAX - 1;
                memcpy(sPath, buffer, size);
                sPath[size] = '\0';

                // Submit path string to DSP
                if (pPath != NULL)
                    pPath->submit(sPath);
            }
    };

}

#endif /* CONTAINER_JACK_UI_PORTS_H_ */
