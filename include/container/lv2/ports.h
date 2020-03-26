/*
 * lv2.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_PORTS_H_
#define CONTAINER_LV2_PORTS_H_

namespace lsp
{
    // Specify port classes
    class LV2Port: public IPort
    {
        protected:
            LV2Extensions          *pExt;
            LV2_URID                urid;
            ssize_t                 nID;

        public:
            explicit LV2Port(const port_t *meta, LV2Extensions *ext): IPort(meta)
            {
                pExt            =   ext;
                urid            =   (meta != NULL) ? pExt->map_port(meta->id) : -1;
                nID             =   -1;
            }
            virtual ~LV2Port()
            {
                pExt            =   NULL;
                urid            =   -1;
                nID             =   -1;
            }

        public:
            /** Bind generic port to generic data pointer
             *
             * @param data data pointer
             */
            virtual void bind(void *data)               { };

            /** Save state of the port to LV2 state
             *
             */
            virtual void save()                         { };

            /** Restore state of the port from LV2 state
             *
             */
            virtual void restore()                      { };

            /** Serialize state of the port to LV2 Atom
             *
             */
            virtual void serialize()                    { };

            /** Deserialize state of the port from LV2 Atom
             * @param flags additional flags
             */
            virtual void deserialize(const void *data, size_t flags)  { };

            /** Get type of the LV2 port in terms of Atom
             *
             * @return type of the LV2 port in terms of Atom
             */
            virtual LV2_URID get_type_urid()            { return 0;         };

            /** Check that the port is pending for transmission
             *
             * @return true if the port is pending for transmission
             */
            virtual bool tx_pending()                   { return false;     };

            /**
             * Callback: UI has connected to backend
             */
            virtual void ui_connected()                 { };

            /** Get the URID of the port in terms of Atom
             *
             * @return UIRD of the port
             */
            inline LV2_URID         get_urid() const    { return urid; }

            /** Get the URI of the port
             *
             * @return URI of the port
             */
            inline const char      *get_uri() const     { return (pExt->unmap_urid(urid)); }

            /** Get port ID
             *
             * @return poirt ID
             */
            inline ssize_t          get_id() const      { return nID;   }

            /** Set port ID
             *
             * @param id port ID
             */
            inline void             set_id(size_t id)   { nID = id;     }
    };

    class LV2PortGroup: public LV2Port
    {
        protected:
            float                   nCurrRow;
            size_t                  nCols;
            size_t                  nRows;

        public:
            explicit LV2PortGroup(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext)
            {
                nCurrRow            = meta->start;
                nCols               = port_list_size(meta->members);
                nRows               = list_size(meta->items);
            }

            virtual ~LV2PortGroup()
            {
                nCurrRow            = 0;
                nCols               = 0;
                nRows               = 0;
            }

        public:
            virtual float getValue()
            {
                return nCurrRow;
            }

            virtual void setValue(float value)
            {
                nCurrRow            = value;
            }

            virtual void serialize()
            {
                // Serialize and reset pending flag
                pExt->forge_int(nCurrRow);
            }

            virtual void deserialize(const void *data, size_t flags)
            {
                const LV2_Atom_Int *atom = reinterpret_cast<const LV2_Atom_Int *>(data);
                if ((atom->body >= 0) && (atom->body < int32_t(nRows)))
                    nCurrRow        = atom->body;
            }

            virtual void save()
            {
                if (nID >= 0)
                    return;
                int32_t value   = nCurrRow;
                lsp_trace("save port id=%s, urid=%d (%s), value=%d", pMetadata->id, urid, get_uri(), int(value));
                pExt->store_value(urid, pExt->forge.Int, &value, sizeof(float));
            }

            virtual void restore()
            {
                if (nID >= 0)
                    return;
                lsp_trace("restore port id=%s, urid=%d (%s)", pMetadata->id, urid, get_uri());
                size_t count            = 0;
                const int32_t *data     = reinterpret_cast<const int32_t *>(pExt->restore_value(urid, pExt->forge.Int, &count));
                if ((count != sizeof(int32_t)) || (data == NULL))
                    return;

                if (((*data) >= 0) && ((*data) < int32_t(nRows)))
                    nCurrRow        = *data;
            }

            virtual LV2_URID get_type_urid()
            {
                return pExt->forge.Int;
            }

        public:
            inline size_t rows() const  { return nRows; }
            inline size_t cols() const  { return nCols; }
    };

    class LV2RawPort: public LV2Port
    {
        protected:
            void       *pBuffer;

        public:
            explicit LV2RawPort(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext), pBuffer(NULL) { }
            virtual ~LV2RawPort() { pBuffer = NULL; };

        public:
            virtual void *getBuffer() { return pBuffer; };

            virtual void bind(void *data)
            {
                pBuffer = data;
            };
    };

    class LV2AudioPort: public LV2RawPort
    {
        protected:
            float      *pSanitized;
            float      *pFrame;

        public:
            explicit LV2AudioPort(const port_t *meta, LV2Extensions *ext) : LV2RawPort(meta, ext)
            {
                pSanitized  = NULL;
                pFrame      = NULL;

                if (IS_IN_PORT(pMetadata))
                {
                    size_t length = pExt->nMaxBlockLength;
                    pSanitized = reinterpret_cast<float *>(::malloc(sizeof(float) * length));
                    if (pSanitized != NULL)
                        dsp::fill_zero(pSanitized, length);
                    else
                        lsp_warn("Failed to allocate sanitize buffer for port %s", pMetadata->id);
                }
            }

            virtual ~LV2AudioPort()
            {
                if (pSanitized != NULL)
                {
                    ::free(pSanitized);
                    pSanitized = NULL;
                }
            };

            virtual void *getBuffer() { return pFrame; };

            // Should be always called at least once after bind() and before processing
            void sanitize(size_t off, size_t samples)
            {
                pFrame  = reinterpret_cast<float *>(pBuffer) + off;
                if (pSanitized != NULL)
                {
                    dsp::sanitize2(pSanitized, reinterpret_cast<float *>(pFrame), samples);
                    pFrame      = pSanitized;
                }
            }
    };

    class LV2InputPort: public LV2Port
    {
        protected:
            const float    *pData;
            float           fValue;
            float           fPrev;

        public:
            explicit LV2InputPort(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext)
            {
                pData       = NULL;
                fValue      = meta->start;
                fPrev       = meta->start;
            }

            virtual ~LV2InputPort()
            {
                pData       = NULL;
                fValue      = pMetadata->start;
                fPrev       = pMetadata->start;
            }

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual void setValue(float value)
            {
                fValue      = value;
            }

            virtual void bind(void *data)
            {
                pData = reinterpret_cast<const float *>(data);
            };

            virtual bool pre_process(size_t samples)
            {
                if ((nID >= 0) && (pData != NULL))
                    fValue      = limit_value(pMetadata, *pData);
                float old       = fPrev;
                fPrev           = fValue;
                #ifdef LSP_TRACE
                if (fPrev != old)
                    lsp_trace("Port %s has been changed, value=%f", pMetadata->id, fValue);
                #endif
                return fPrev != old; // Value has changed?
            }

//            virtual void post_process(size_t samples)
//            {
//                fPrev       = fValue;
//            };

            virtual void save()
            {
                if (nID >= 0)
                    return;
                lsp_trace("save port id=%s, urid=%d (%s), value=%f", pMetadata->id, urid, get_uri(), fValue);
                pExt->store_value(urid, pExt->forge.Float, &fValue, sizeof(float));
            }

            virtual void restore()
            {
                if (nID >= 0)
                    return;
                lsp_trace("restore port id=%s, urid=%d (%s)", pMetadata->id, urid, get_uri());
                size_t count            = 0;
                const void *data        = pExt->restore_value(urid, pExt->forge.Float, &count);
                if ((count == sizeof(float)) && (data != NULL))
                    fValue      = limit_value(pMetadata, *(reinterpret_cast<const float *>(data)));
            }

            virtual void deserialize(const void *data, size_t flags)
            {
                const LV2_Atom_Float *atom = reinterpret_cast<const LV2_Atom_Float *>(data);
                fValue      = atom->body;
            }

            virtual void serialize()
            {
                // Serialize and reset pending flag
                pExt->forge_float(fValue);
                fPrev       = fValue;
            }

            virtual LV2_URID get_type_urid()
            {
                return pExt->forge.Float;
            }
    };

    class LV2BypassPort: public LV2InputPort
    {
        public:
            explicit LV2BypassPort(const port_t *meta, LV2Extensions *ext) : LV2InputPort(meta, ext) { }

            virtual ~LV2BypassPort() {}

        public:
            virtual float getValue()
            {
                return pMetadata->max - fValue;
            }

            virtual void setValue(float value)
            {
                fValue      = pMetadata->max - value;
            }

            virtual void save()
            {
                if (nID >= 0)
                    return;
                float value = pMetadata->max - fValue;
                lsp_trace("save port id=%s, urid=%d (%s), value=%f", pMetadata->id, urid, get_uri(), value);
                pExt->store_value(urid, pExt->forge.Float, &value, sizeof(float));
            }

            virtual void restore()
            {
                if (nID >= 0)
                    return;
                lsp_trace("restore port id=%s, urid=%d (%s)", pMetadata->id, urid, get_uri());
                size_t count            = 0;
                const void *data        = pExt->restore_value(urid, pExt->forge.Float, &count);
                if ((count == sizeof(float)) && (data != NULL))
                    fValue      = limit_value(pMetadata, pMetadata->max - *(reinterpret_cast<const float *>(data)));
            }

            virtual void deserialize(const void *data, size_t flags)
            {
                const LV2_Atom_Float *atom = reinterpret_cast<const LV2_Atom_Float *>(data);
                fValue      = pMetadata->max - atom->body;
            }
    };

    class LV2OutputPort: public LV2Port
    {
        protected:
            float  *pData;
            float   fPrev;
            float   fValue;

        public:
            explicit LV2OutputPort(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext)
            {
                pData       = NULL;
                fPrev       = meta->start;
                fValue      = meta->start;
            }

            virtual ~LV2OutputPort()
            {
                pData = NULL;
            };

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual void setValue(float value)
            {
                value       = limit_value(pMetadata, value);
                if (pMetadata->flags & F_PEAK)
                {
                    if (fabs(fValue) < fabs(value))
                        fValue = value;
                }
                else
                    fValue = value;
            }

            virtual void bind(void *data)
            {
                pData       = reinterpret_cast<float *>(data);
            };

            virtual bool pre_process(size_t samples)
            {
                if (pMetadata->flags & F_PEAK)
                    fValue      = 0.0f;
                return false;
            }

            virtual void post_process(size_t samples)
            {
                // Store data i the port
                if (pData != NULL)
                    *pData      = fValue;

                // Serialize data and reset tx_pending flag
                fPrev       = fValue;

//                // Update data according to peak protocol, only for direct-mapped ports
//                if ((nID >= 0) && (pMetadata->flags & F_PEAK))
//                    fValue      = 0.0f;
            }

            virtual bool tx_pending()
            {
                if (fValue != fPrev)
                    lsp_trace("pending_value id=%s, prev=%f, value=%f", pMetadata->id, fPrev, fValue);
                return fValue != fPrev;
            }

            virtual void serialize()
            {
                // Serialize and reset pending flag
                pExt->forge_float(fValue);

                // Update data according to peak protocol, only for Atom transport ports
                if ((nID < 0) && (pMetadata->flags & F_PEAK))
                    fValue      = 0.0f;
            }

            virtual LV2_URID get_type_urid()
            {
                return pExt->forge.Float;
            }
    };

    class LV2MeshPort: public LV2Port
    {
        protected:
            LV2Mesh                 sMesh;

        public:
            explicit LV2MeshPort(const port_t *meta, LV2Extensions *ext): LV2Port(meta, ext)
            {
                sMesh.init(meta, ext);
            }

            virtual ~LV2MeshPort()
            {
            };

        public:
            virtual LV2_URID get_type_urid()        { return pExt->uridMeshType; };

            virtual void *getBuffer()
            {
                return sMesh.pMesh;
            }

            virtual bool tx_pending()
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
                pExt->forge_key(pExt->uridMeshDimensions);
                pExt->forge_int(mesh->nBuffers);

                // Forge number of items per vector
                pExt->forge_key(pExt->uridMeshItems);
                pExt->forge_int(mesh->nItems);

                // Forge vectors
                for (size_t i=0; i < mesh->nBuffers; ++i)
                {
                    pExt->forge_key(pExt->uridMeshData);
                    pExt->forge_vector(sizeof(float), pExt->forge.Float, mesh->nItems, mesh->pvData[i]);
                }

                // Set mesh waiting until next frame is allowed
                mesh->setWaiting();
            }
    };

    class LV2FrameBufferPort: public LV2Port
    {
        protected:
            frame_buffer_t      sFB;
            size_t              nRowID;

        public:
            explicit LV2FrameBufferPort(const port_t *meta, LV2Extensions *ext): LV2Port(meta, ext)
            {
                sFB.init(meta->start, meta->step);
                nRowID = 0;
            }

            virtual ~LV2FrameBufferPort()
            {
            };

        public:
            virtual LV2_URID get_type_urid()        { return pExt->uridFrameBufferType; };

            virtual void *getBuffer()
            {
                return &sFB;
            }

            virtual bool tx_pending()
            {
                return sFB.next_rowid() != nRowID;
            }

            virtual void ui_connected()
            {
                // We need to replay buffer contents for the connected client
                lsp_trace("UI connected event");
                nRowID      = sFB.next_rowid() - sFB.rows();
            }

            virtual void serialize()
            {
                // Serialize not more than 4 rows
                size_t delta = sFB.next_rowid() - nRowID;
                uint32_t first_row = (delta > sFB.rows()) ? sFB.next_rowid() - sFB.rows() : nRowID;
                if (delta > FRAMEBUFFER_BULK_MAX)
                    delta = FRAMEBUFFER_BULK_MAX;
                uint32_t last_row = first_row + delta;

                lsp_trace("id = %s, first=%d, last=%d", pMetadata->id, int(first_row), int(last_row));

                // Forge frame buffer parameters
                pExt->forge_key(pExt->uridFrameBufferRows);
                pExt->forge_int(sFB.rows());
                pExt->forge_key(pExt->uridFrameBufferCols);
                pExt->forge_int(sFB.cols());
                pExt->forge_key(pExt->uridFrameBufferFirstRowID);
                pExt->forge_int(first_row);
                pExt->forge_key(pExt->uridFrameBufferLastRowID);
                pExt->forge_int(last_row);

                // Forge vectors
                while (first_row != last_row)
                {
                    pExt->forge_key(pExt->uridFrameBufferData);
                    pExt->forge_vector(sizeof(float), pExt->forge.Float, sFB.cols(), sFB.get_row(first_row++));
                }

                // Update current RowID
                nRowID = first_row;
            }
    };

    class LV2PathPort: public LV2Port
    {
        protected:
            lv2_path_t          sPath;
            atomic_t            nLastChange;

            inline void set_string(const char *string, size_t len, size_t flags)
            {
                lsp_trace("submitting path to '%s' (length = %d), flags=0x%x", string, int(len), int(flags));
                sPath.submit(string, len, flags);
            }

        public:
            explicit LV2PathPort(const port_t *meta, LV2Extensions *ext): LV2Port(meta, ext)
            {
                sPath.init();
                nLastChange = sPath.nChanges;
            }

        public:
            virtual void *getBuffer()
            {
                return static_cast<path_t *>(&sPath);
            }

            virtual void save()
            {
                const char *path = sPath.sPath;

                lsp_trace("save port id=%s, urid=%d (%s), value=%s", pMetadata->id, urid, get_uri(), path);

                if (::strlen(path) > 0)
                {
                    char *mapped = NULL;

                    // We need to translate absolute path to relative path?
                    if ((pExt->mapPath != NULL) && (::strstr(path, LSP_BUILTIN_PREFIX) != path))
                    {
                        mapped = pExt->mapPath->abstract_path(pExt->mapPath->handle, path);
                        if (mapped != NULL)
                        {
                            lsp_trace("mapped path: %s -> %s", path, mapped);
                            path = mapped;
                        }
                    }

                    // Store the actual value of the path
                    pExt->store_value(urid, pExt->uridPathType, path, ::strlen(path) + sizeof(char));

                    if (mapped != NULL)
                        ::free(mapped);
                }
            }

            void tx_request()
            {
                lsp_trace("tx_request");
                atomic_add(&sPath.nChanges, 1);
            }

            virtual void restore()
            {
                lsp_trace("restore port id=%s, urid=%d (%s)", pMetadata->id, urid, get_uri());
                size_t count            = 0;
                uint32_t type           = -1;

                const char *path        = reinterpret_cast<const char *>(pExt->retrieve_value(urid, &type, &count));
                char *mapped            = NULL;
                if (path != NULL)
                {
                    if (type == pExt->forge.URID)
                    {
                        const LV2_URID *urid    = reinterpret_cast<const LV2_URID *>(path);
                        path                = pExt->unmap_urid(*urid);
                        if (path != NULL)
                            count               = ::strnlen(path, PATH_MAX-1);
                    }
                    else if ((type != pExt->uridPathType) && (type != pExt->forge.String))
                    {
                        if (path != NULL)
                            lsp_trace("Invalid type: %d = %s", int(type), pExt->unmap_urid(type));
                        path                    = NULL;
                    }
                }

                if ((path != NULL) && (count > 0))
                {
                    // Save path as temporary variable
                    char tmp_path[PATH_MAX];
                    ::strncpy(tmp_path, path, count);
                    tmp_path[count] = '\0';
                    path        = tmp_path;

                    // We need to translate relative path to absolute path?
                    if ((pExt->mapPath != NULL) && (::strstr(path, LSP_BUILTIN_PREFIX) != path))
                    {
                        mapped = pExt->mapPath->absolute_path(pExt->mapPath->handle, path);
                        if (mapped != NULL)
                        {
                            lsp_trace("unmapped path: %s -> %s", path, mapped);
                            path  = mapped;
                            count = ::strnlen(path, PATH_MAX-1);
                        }
                    }

                    // Restore the actual value of the path
                    set_string(path, count, PF_STATE_IMPORT);
                }
                else
                    set_string("", 0, PF_STATE_IMPORT);
                tx_request();

                if (mapped != NULL)
                    ::free(mapped);
            }

            virtual bool tx_pending()
            {
                return sPath.nChanges != nLastChange;
            }

            void reset_tx_pending()
            {
                lsp_trace("reset_tx_pending");
                nLastChange     = sPath.nChanges;
            }

            virtual void serialize()
            {
                pExt->forge_path(sPath.get_path());
                reset_tx_pending();
            }

            virtual void deserialize(const void *data, size_t flags)
            {
                const LV2_Atom *atom = reinterpret_cast<const LV2_Atom *>(data);
                if (atom->type != pExt->uridPathType)
                    return;
                set_string(reinterpret_cast<const char *>(atom + 1), atom->size, flags);
            }

            virtual LV2_URID get_type_urid()    { return pExt->uridPathType; }

            virtual bool pre_process(size_t samples)
            {
                return sPath.pending();
            }
    };

    class LV2MidiPort: public LV2Port
    {
        protected:
            midi_t      sQueue;

        public:
            explicit LV2MidiPort(const port_t *meta, LV2Extensions *ext): LV2Port(meta, ext)
            {
                sQueue.clear();
            }

        public:
            virtual void *getBuffer()
            {
                return &sQueue;
            }
    };

    class LV2OscPort: public LV2Port
    {
        protected:
            osc_buffer_t     *pFB;

        public:
            explicit LV2OscPort(const port_t *meta, LV2Extensions *ext) : LV2Port(meta, ext)
            {
                pFB     = NULL;
            }

            virtual ~LV2OscPort()
            {
            }

        public:
            virtual void *getBuffer()
            {
                return pFB;
            }

            virtual int init()
            {
                pFB = osc_buffer_t::create(OSC_BUFFER_MAX);
                return (pFB == NULL) ? STATUS_NO_MEM : STATUS_OK;
            }

            virtual void destroy()
            {
                if (pFB != NULL)
                {
                    osc_buffer_t::destroy(pFB);
                    pFB     = NULL;
                }
            }
    };

}


#endif /* CONTAINER_LV2_PORTS_H_ */
