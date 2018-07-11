/*
 * lv2ui.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2UI_PORTS_H_
#define CONTAINER_LV2UI_PORTS_H_


namespace lsp
{
    // Specify port classes
    class LV2UIPort: public IUIPort, public LV2Serializable
    {
        protected:
            ssize_t                 nID;

        public:
            explicit LV2UIPort(const port_t *meta, LV2Extensions *ext) : IUIPort(meta), LV2Serializable(ext)
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
    };

    class LV2UIPortGroup: public LV2UIPort
    {
        private:
            size_t                  nRows;
            size_t                  nCols;
            size_t                  nCurrRow;

        public:
            LV2UIPortGroup(const port_t *meta, LV2Extensions *ext) : LV2UIPort(meta, ext)
            {
                nCurrRow            = meta->start;
                nRows               = list_size(meta->items);
                nCols               = port_list_size(meta->members);
            }

            virtual ~LV2UIPortGroup()
            {
            }

        public:
            virtual float getValue()
            {
                return nCurrRow;
            }

            virtual void setValue(float value)
            {
                size_t new_value = value;
                if ((new_value >= 0) && (new_value < nRows) && (new_value != nCurrRow))
                {
                    nCurrRow        = new_value;
                    if (urid > 0)
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
            float   fValue;

        public:
            explicit LV2UIFloatPort(const port_t *meta, LV2Extensions *ext) :
                LV2UIPort(meta, ext)
            {
                fValue  =   meta->start;
            }
            virtual ~LV2UIFloatPort() { fValue  =   pMetadata->start; };

        public:
            virtual float getValue() { return fValue; }

            virtual void setValue(float value)
            {
                fValue      = limit_value(pMetadata, value);
                if (nID >= 0)
                {
                    lsp_trace("write(%d, %d, %d, %f)", int(nID), int(sizeof(float)), int(0), fValue);
                    pExt->write_data(nID, sizeof(float), 0, &fValue);
                }
                else if (urid > 0)
                    pExt->ui_write_state(this);
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
    };

    class LV2UIPeakPort: public LV2UIFloatPort
    {
        public:
            explicit LV2UIPeakPort(const port_t *meta, LV2Extensions *ext) :
                LV2UIFloatPort(meta, ext) {}
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
            }
    };

    class LV2UIMeshPort: public LV2UIPort
    {
        protected:
            LV2Mesh                 sMesh;
            bool                    bParsed;

        public:
            explicit LV2UIMeshPort(const port_t *meta, LV2Extensions *ext) : LV2UIPort(meta, ext)
            {
                sMesh.init(meta, ext);
                bParsed = false;
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

                if ((body->key != sMesh.uridDimensions) || (body->value.type != pExt->forge.Int))
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

                if ((body->key != sMesh.uridItems) || (body->value.type != pExt->forge.Int))
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
                    dsp::copy_saturated(sMesh.pMesh->pvData[i], reinterpret_cast<const float *>(v + 1), v_items);
//                    memcpy(sMesh.pMesh->pvData[i], reinterpret_cast<const float *>(v + 1), v_items * sizeof(float));
                }

                // Update mesh parameters
//                lsp_trace("Mesh was successful parsed dimensions=%d, items=%d", int(sMesh.pMesh->nBuffers), int(sMesh.pMesh->nItems));
                bParsed                 = true;
            }

            virtual void *getBuffer()
            {
                if (!bParsed)
                    return NULL;

                return sMesh.pMesh;
            }
    };

    class LV2UIPathPort: public LV2UIPort
    {
        protected:
            char            sPath[PATH_MAX];

        public:
            explicit LV2UIPathPort(const port_t *meta, LV2Extensions *ext) :  LV2UIPort(meta, ext)
            {
                sPath[0]    = '\0';
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
                    memcpy(sPath, str, len);
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
            }

            virtual LV2_URID        get_type_urid() const   { return pExt->uridPathType; };

            virtual void serialize()
            {
                pExt->forge_path(sPath);
            }

            virtual void write(const void* buffer, size_t size)
            {
                set_string(reinterpret_cast<const char *>(buffer), size);
                if (nID >= 0)
                    pExt->ui_write_patch(this);
                else
                    pExt->ui_write_state(this);
            }

            virtual void *getBuffer()
            {
                return sPath;
            }
    };
}


#endif /* CONTAINER_LV2UI_PORTS_H_ */
