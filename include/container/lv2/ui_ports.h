/*
 * lv2ui.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_UI_PORTS_H_
#define CONTAINER_LV2_UI_PORTS_H_

#include <core/resource.h>

namespace lsp
{
    // Specify port classes
    class LV2UIPort: public CtlPort, public LV2Serializable
    {
        protected:
            ssize_t                 nID;

        public:
            explicit LV2UIPort(const port_t *meta, LV2Extensions *ext) : CtlPort(meta), LV2Serializable(ext)
            {
                nID         = -1;
                urid        = (meta != NULL) ? pExt->map_port(meta->id) : -1;
            }

            virtual ~LV2UIPort()
            {
            }

        public:
            virtual void notify(const void *buffer, size_t protocol, size_t size)
            {
            }

            inline const char      *get_uri() const         { return (pExt->unmap_urid(urid)); };
            virtual LV2_URID        get_type_urid() const   { return 0; };
            inline void             set_id(ssize_t id)      { nID = id; };
            inline ssize_t          get_id() const          { return nID; };
            virtual bool            sync()                  { return false; };
    };

    class LV2UIPortGroup: public LV2UIPort
    {
        protected:
            size_t      nRows;
            size_t      nCols;
            size_t      nCurrRow;
            LV2Port    *pPort;

        public:
            LV2UIPortGroup(const port_t *meta, LV2Extensions *ext, LV2Port *port) : LV2UIPort(meta, ext)
            {
                nCurrRow            = meta->start;
                nRows               = list_size(meta->items);
                nCols               = port_list_size(meta->members);
                pPort               = port;

                if (port != NULL)
                {
                    lsp_trace("Connected direct group port id=%s", port->metadata()->id);
                    nCurrRow            = port->getValue();
                }
            }

            virtual ~LV2UIPortGroup()
            {
            }

        public:
            virtual float get_value()
            {
                return nCurrRow;
            }

            virtual void set_value(float value)
            {
                size_t new_value = value;
                if ((new_value >= 0) && (new_value < nRows) && (new_value != nCurrRow))
                {
                    nCurrRow        = new_value;
                    if (pPort != NULL)
                    {
                        lsp_trace("Directly writing group port id=%s, value=%d",
                            pPort->metadata()->id, int(nCurrRow));
                        pPort->setValue(nCurrRow);
                    }
                    else if (urid > 0)
                        pExt->ui_write_state(this);
                }
            }

            virtual void serialize()
            {
                // Serialize and reset pending flag
                pExt->forge_int(nCurrRow);
            }

            virtual void deserialize(const void *data)
            {
                const LV2_Atom_Int *atom = reinterpret_cast<const LV2_Atom_Int *>(data);
                if ((atom->body >= 0) && (atom->body < int32_t(nRows)))
                    nCurrRow        = atom->body;
            }

            virtual LV2_URID get_type_urid() const
            {
                return pExt->forge.Int;
            }

        public:
            inline size_t rows() const  { return nRows; }
            inline size_t cols() const  { return nCols; }
    };

    class LV2UIFloatPort: public LV2UIPort
    {
        protected:
            float       fValue;
            bool        bForce;
            LV2Port    *pPort;

        public:
            explicit LV2UIFloatPort(const port_t *meta, LV2Extensions *ext, LV2Port *port) :
                LV2UIPort(meta, ext)
            {
                fValue      = meta->start;
                pPort       = port;
                if (port != NULL)
                {
                    lsp_trace("Connected direct float port id=%s", port->metadata()->id);
                    fValue      = port->getValue();
                }
                bForce      = port != NULL;
            }
            virtual ~LV2UIFloatPort() { fValue  =   pMetadata->start; };

        public:
            virtual float get_value() { return fValue; }

            virtual void set_value(float value)
            {
                fValue      = limit_value(pMetadata, value);
                if (nID >= 0)
                {
                    // Use standard mechanism to access port
                    lsp_trace("write(%d, %d, %d, %f)", int(nID), int(sizeof(float)), int(0), fValue);
                    pExt->write_data(nID, sizeof(float), 0, &fValue);
                }
                else
                {
                    if (pPort != NULL)
                    {
                        lsp_trace("Directly writing float port id=%s, value=%f",
                            pPort->metadata()->id, fValue);
                        pPort->setValue(fValue);
                    }
                    else if (urid > 0)
                        pExt->ui_write_state(this);
                }
            }

            virtual LV2_URID        get_type_urid() const   { return pExt->forge.Float; };

            virtual void serialize()
            {
                pExt->forge_float(fValue);
            };

            virtual void deserialize(const void *data)
            {
                const LV2_Atom_Float *atom = reinterpret_cast<const LV2_Atom_Float *>(data);
                fValue      = limit_value(pMetadata, atom->body);
            }

            virtual void notify(const void *buffer, size_t protocol, size_t size)
            {
                if (size == sizeof(float))
                    fValue = limit_value(pMetadata, *(reinterpret_cast<const float *>(buffer)));
            }

            virtual bool sync()
            {
                if ((pPort == NULL) || (nID >= 0))
                    return false;

                float old   = fValue;
                fValue      = limit_value(pMetadata, pPort->getValue());
                bool synced = (fValue != old) || bForce;
                bForce      = false;

                #ifdef LSP_TRACE
                    if (synced)
                        lsp_trace("Directly received float port id=%s, value=%f",
                            pPort->metadata()->id, fValue);
                #endif
                return synced;
            }
    };

    class LV2UIBypassPort: public LV2UIFloatPort
    {
        public:
            explicit LV2UIBypassPort(const port_t *meta, LV2Extensions *ext, LV2Port *port) :
                LV2UIFloatPort(meta, ext, port)
            {
            }

            virtual ~LV2UIBypassPort() { };

        public:
            virtual void set_value(float value)
            {
                fValue      = limit_value(pMetadata, value);
                if (nID >= 0)
                {
                    // Use standard mechanism to access port
                    float value = pMetadata->max - fValue;
                    lsp_trace("write(%d, %d, %d, %f)", int(nID), int(sizeof(float)), int(0), value);
                    pExt->write_data(nID, sizeof(float), 0, &value);
                }
                else
                {
                    if (pPort != NULL)
                    {
                        lsp_trace("Directly writing float port id=%s, value=%f",
                            pPort->metadata()->id, fValue);
                        pPort->setValue(fValue);
                    }
                    else if (urid > 0)
                        pExt->ui_write_state(this);
                }
            }

            virtual void serialize()
            {
                pExt->forge_float(pMetadata->max - fValue);
            };

            virtual void deserialize(const void *data)
            {
                const LV2_Atom_Float *atom = reinterpret_cast<const LV2_Atom_Float *>(data);
                fValue      = limit_value(pMetadata, pMetadata->max - atom->body);
            }

            virtual void notify(const void *buffer, size_t protocol, size_t size)
            {
                if (size == sizeof(float))
                    fValue = limit_value(pMetadata, pMetadata->max - *(reinterpret_cast<const float *>(buffer)));
                lsp_trace("set value of port %s = %f", pMetadata->id, fValue);
            }
    };

    class LV2UIPeakPort: public LV2UIFloatPort
    {
        public:
            explicit LV2UIPeakPort(const port_t *meta, LV2Extensions *ext, LV2Port *port) :
                LV2UIFloatPort(meta, ext, port) {}
            virtual ~LV2UIPeakPort() {};

        public:
            virtual void notify(const void *buffer, size_t protocol, size_t size)
            {
                if (size == sizeof(LV2UI_Peak_Data))
                {
                    fValue = limit_value(pMetadata, (reinterpret_cast<const LV2UI_Peak_Data *>(buffer))->peak);
                    return;
                }
                LV2UIFloatPort::notify(buffer, protocol, size);
//                lsp_trace("id=%s, value=%f", pMetadata->id, fValue);
            }
    };

    class LV2UIMeshPort: public LV2UIPort
    {
        protected:
            LV2Mesh                 sMesh;
            bool                    bParsed;
            LV2MeshPort            *pPort;

        public:
            explicit LV2UIMeshPort(const port_t *meta, LV2Extensions *ext, LV2Port *xport) : LV2UIPort(meta, ext)
            {
                sMesh.init(meta, ext);
                bParsed     = false;
                pPort       = NULL;

                lsp_trace("id=%s, ext=%p, xport=%p", meta->id, ext, xport);

                // Try to perform direct access to the port using LV2:Instance interface
                const port_t *xmeta = (xport != NULL) ? xport->metadata() : NULL;
                if ((xmeta != NULL) && (xmeta->role == R_MESH))
                {
                    pPort               = static_cast<LV2MeshPort *>(xport);
                    mesh_t *mesh        = static_cast<mesh_t *>(pPort->getBuffer());
                    mesh->cleanup();  // Mark mesh as empty to force the DSP to write data to mesh
                    lsp_trace("Connected direct mesh port id=%s", xmeta->id);
                }
            }

            virtual ~LV2UIMeshPort()
            {
            };

        public:
            virtual LV2_URID        get_type_urid() const   { return pExt->uridMeshType; };

            virtual void deserialize(const void *data)
            {
                const LV2_Atom_Object* obj = reinterpret_cast<const LV2_Atom_Object *>(data);

                // Parse atom body
                bParsed     = false;
                LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);

                // Get number of vectors (dimensions)
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    return;
//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key != pExt->uridMeshDimensions) || (body->value.type != pExt->forge.Int))
                    return;
                ssize_t dimensions = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
//                lsp_trace("dimensions=%d", int(dimensions));
                if (dimensions > ssize_t(sMesh.nBuffers))
                    return;
                sMesh.pMesh->nBuffers   = dimensions;

                // Get size of each vector
                body = lv2_atom_object_next(body);
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    return;

//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key != pExt->uridMeshItems) || (body->value.type != pExt->forge.Int))
                    return;
                ssize_t vector_size = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
//                lsp_trace("vector size=%d", int(vector_size));
                if ((vector_size < 0) || (vector_size > ssize_t(sMesh.nMaxItems)))
                    return;
                sMesh.pMesh->nItems     = vector_size;

                // Now parse each vector
                for (ssize_t i=0; i < dimensions; ++i)
                {
                    // Read vector as array of floats
                    body = lv2_atom_object_next(body);
                    if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                        return;

//                    lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                    lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));
//                    lsp_trace("sMesh.pUrids[%d] (%d) = %s", int(i), int(sMesh.pUrids[i]), pExt->unmap_urid(sMesh.pUrids[i]));

                    if ((body->key != pExt->uridMeshData) || (body->value.type != pExt->forge.Vector))
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
                    dsp::copy_saturated(sMesh.pMesh->pvData[i], reinterpret_cast<const float *>(v + 1), v_items);
//                    memcpy(sMesh.pMesh->pvData[i], reinterpret_cast<const float *>(v + 1), v_items * sizeof(float));
                }

                // Update mesh parameters
//                lsp_trace("Mesh was successful parsed dimensions=%d, items=%d", int(sMesh.pMesh->nBuffers), int(sMesh.pMesh->nItems));
                bParsed                 = true;
            }

            virtual void *get_buffer()
            {
                if (!bParsed)
                    return NULL;

                return sMesh.pMesh;
            }

            virtual bool sync()
            {
                if (pPort == NULL)
                    return false;

                mesh_t *mesh = reinterpret_cast<mesh_t *>(pPort->getBuffer());
                if ((mesh == NULL) || (!mesh->containsData()))
                    return false;

                // Copy mesh data
                for (size_t i=0; i < mesh->nBuffers; ++i)
                    dsp::copy_saturated(sMesh.pMesh->pvData[i], mesh->pvData[i], mesh->nItems);
                sMesh.pMesh->data(mesh->nBuffers, mesh->nItems);
//                lsp_trace("Directly received mesh port id=%s, buffers=%d, items=%d",
//                        pPort->metadata()->id, int(sMesh.pMesh->nBuffers), int(sMesh.pMesh->nItems));

                // Clean source mesh
                mesh->cleanup();
                bParsed = true;
                return sMesh.pMesh->containsData();
            }
    };

    class LV2UIFrameBufferPort: public LV2UIPort
    {
        protected:
            frame_buffer_t          sFB;
            LV2FrameBufferPort     *pPort;

        public:
            explicit LV2UIFrameBufferPort(const port_t *meta, LV2Extensions *ext, LV2Port *xport) : LV2UIPort(meta, ext)
            {
                sFB.init(meta->start, meta->step);
                pPort       = NULL;

                lsp_trace("id=%s, ext=%p, xport=%p", meta->id, ext, xport);

                // Try to perform direct access to the port using LV2:Instance interface
                const port_t *xmeta = (xport != NULL) ? xport->metadata() : NULL;
                if ((xmeta != NULL) && (xmeta->role == R_FBUFFER))
                {
                    pPort               = static_cast<LV2FrameBufferPort *>(xport);
                    lsp_trace("Connected direct framebuffer port id=%s", xmeta->id);
                }
            }

            virtual ~LV2UIFrameBufferPort()
            {
            };

        public:
            virtual LV2_URID        get_type_urid() const   { return pExt->uridMeshType; };

            virtual void deserialize(const void *data)
            {
                const LV2_Atom_Object* obj = reinterpret_cast<const LV2_Atom_Object *>(data);

//                lsp_trace("id = %s", pMetadata->id);

                // Parse atom body
                LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);

                // Get number of vectors (dimensions)
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    return;
//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key != pExt->uridFrameBufferRows) || (body->value.type != pExt->forge.Int))
                    return;
                uint32_t rows = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
                if (rows != sFB.rows())
                {
//                    lsp_trace("Row count does not match: %d vs %d", int(rows), int(sFB.rows()));
                    return;
                }

                // Get size of each vector
                body = lv2_atom_object_next(body);
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                {
//                    lsp_trace("unexpected end of object");
                    return;
                }
//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key != pExt->uridFrameBufferCols) || (body->value.type != pExt->forge.Int))
                    return;
                uint32_t cols = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
                if (cols != sFB.cols())
                {
//                    lsp_trace("Column count does not match: %d vs %d", int(cols), int(sFB.cols()));
                    return;
                }

                // Get first row
                body = lv2_atom_object_next(body);
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                {
//                    lsp_trace("unexpected end of object");
                    return;
                }

//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));
                if ((body->key != pExt->uridFrameBufferFirstRowID) || (body->value.type != pExt->forge.Int))
                    return;
                uint32_t first_row = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
//                lsp_trace("first_row = %d", int(first_row));

                // Get last row
                body = lv2_atom_object_next(body);
                if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                {
//                    lsp_trace("unexpected end of object");
                    return;
                }

//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key != pExt->uridFrameBufferLastRowID) || (body->value.type != pExt->forge.Int))
                    return;
                uint32_t last_row = (reinterpret_cast<const LV2_Atom_Int *>(& body->value))->body;
//                lsp_trace("last_row = %d", int(last_row));

                lsp_trace("first_row = %d, last_row = %d", int(first_row), int(last_row));

                // Validate
                uint32_t delta = last_row - first_row;
                if (delta > FRAMEBUFFER_BULK_MAX)
                {
//                    lsp_trace("delta too large: %d", int(delta));
                    return;
                }

                // Now parse each vector
                while (first_row != last_row)
                {
                    // Read vector as array of floats
                    body = lv2_atom_object_next(body);
                    if (lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                    {
//                        lsp_trace("unexpected end of object");
                        return;
                    }

//                    lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                    lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                    if ((body->key != pExt->uridFrameBufferData) || (body->value.type != pExt->forge.Vector))
                        return;
                    const LV2_Atom_Vector *v = reinterpret_cast<const LV2_Atom_Vector *>(&body->value);

//                    lsp_trace("body->child_size = %d, body->child_type (%d) = %s", int(v->body.child_size), int(v->body.child_type), pExt->unmap_urid(v->body.child_type));

                    if ((v->body.child_size != sizeof(float)) || (v->body.child_type != pExt->forge.Float))
                        return;
                    ssize_t v_items     = (v->atom.size - sizeof(LV2_Atom_Vector_Body)) / sizeof(float);
                    if (v_items != ssize_t(cols))
                    {
//                        lsp_trace("vector items does not match columns count: %d vs %d", int(v_items), int(cols));
                        return;
                    }

                    sFB.write_row(first_row++, reinterpret_cast<const float *>(v + 1));
                }
                sFB.seek(first_row);
            }

            virtual void *get_buffer()
            {
                return &sFB;
            }

            virtual bool sync()
            {
                // Check if there is data for viewing
                frame_buffer_t *fb = reinterpret_cast<frame_buffer_t *>(pPort->getBuffer());
                return (fb != NULL) ? sFB.sync(fb) : false;
            }
    };

    class LV2UIPathPort: public LV2UIPort
    {
        protected:
            LV2PathPort    *pPort;
            char            sPath[PATH_MAX];

        public:
            explicit LV2UIPathPort(const port_t *meta, LV2Extensions *ext, LV2Port *xport) :  LV2UIPort(meta, ext)
            {
                sPath[0]    = '\0';
                pPort       = NULL;

                lsp_trace("id=%s, ext=%p, xport=%p", meta->id, ext, xport);

                // Try to perform direct access to the port using LV2:Instance interface
                const port_t *xmeta = (xport != NULL) ? xport->metadata() : NULL;
                if ((xmeta != NULL) && (xmeta->role == R_PATH))
                {
                    pPort               = static_cast<LV2PathPort *>(xport);
                    pPort->tx_request();
                    lsp_trace("Connected direct path port id=%s", xmeta->id);
                }
            }

            virtual ~LV2UIPathPort()
            {
            };

        protected:
            void set_string(const char *str, size_t len)
            {
                if ((str != NULL) && (len > 0))
                {
                    size_t copy     = (len >= PATH_MAX) ? PATH_MAX-1 : len;
                    ::memcpy(sPath, str, len);
                    sPath[copy]     = '\0';
                }
                else
                    sPath[0]        = '\0';
            }

        public:
            virtual void deserialize(const void *data)
            {
                // Read path value
                const LV2_Atom *atom = reinterpret_cast<const LV2_Atom *>(data);
                set_string(reinterpret_cast<const char *>(atom + 1), atom->size);

                lsp_trace("mapPath = %p, path = %s", pExt->mapPath, sPath);
                if ((pExt->mapPath != NULL) && (::strstr(sPath, LSP_BUILTIN_PREFIX) != sPath))
                {
                    char *unmapped_path = pExt->mapPath->absolute_path(pExt->mapPath->handle, sPath);
                    if (unmapped_path != NULL)
                    {
                        lsp_trace("unmapped path: %s -> %s", sPath, unmapped_path);
                        set_string(unmapped_path, ::strlen(unmapped_path));
                        ::free(unmapped_path);
                    }
                }
            }

            virtual LV2_URID        get_type_urid() const   { return pExt->uridPathType; };

            virtual void serialize()
            {
                lsp_trace("mapPath = %p, path = %s", pExt->mapPath, sPath);
                if ((pExt->mapPath != NULL) && (::strstr(sPath, LSP_BUILTIN_PREFIX) != sPath))
                {
                    char* mapped_path = pExt->mapPath->abstract_path(pExt->mapPath->handle, sPath);
                    if (mapped_path != NULL)
                    {
                        lsp_trace("mapped path: %s -> %s", sPath, mapped_path);
                        pExt->forge_path(mapped_path);
                        ::free(mapped_path);
                    }
                    else
                        pExt->forge_path(sPath);
                }
                else
                    pExt->forge_path(sPath);
            }

            virtual void write(const void* buffer, size_t size, size_t flags)
            {
                set_string(reinterpret_cast<const char *>(buffer), size);

                // Try to perform direct access to the port using LV2:Instance interface
                lv2_path_t *path    = (pPort != NULL) ? static_cast<lv2_path_t *>(pPort->getBuffer()) : NULL;
                if (path != NULL)
                {
                    lsp_trace("Directly writing path port id=%s, path=%s (%d)",
                            pPort->metadata()->id, static_cast<const char *>(buffer), int(size));
                    path->submit(static_cast<const char *>(buffer), size, flags);
                    return;
                }

                // Write data using atom port
                if ((nID >= 0) && (flags == 0))
                    pExt->ui_write_patch(this);
                else
                    pExt->ui_write_state(this, flags);
            }

            virtual void write(const void* buffer, size_t size)
            {
                write(buffer, size, 0);
            }

            virtual bool sync()
            {
                if (!pPort->tx_pending())
                    return false;
                pPort->reset_tx_pending();

                path_t *path        = static_cast<path_t *>(pPort->getBuffer());
                ::strncpy(sPath, path->get_path(), PATH_MAX); // Copy current contents
                sPath[PATH_MAX-1]   = '\0';

                lsp_trace("Directly received path port id=%s, path=%s",
                        pPort->metadata()->id, sPath);

                return true;
            }

            virtual void *get_buffer()
            {
                return sPath;
            }
    };
}


#endif /* CONTAINER_LV2_UI_PORTS_H_ */
