/*
 * ext.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_CONTAINER_LV2EXT_H_
#define CORE_CONTAINER_LV2EXT_H_

// LV2 includes
#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#ifdef LSP_UI_SIDE
    #include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#endif /* LSP_UI_SIDE */

#ifndef LV2_ATOM__Object
    #define LV2_ATOM__Object        LV2_ATOM_PREFIX "Object"
#endif /* LV2_ATOM__Object */

namespace lsp
{
    #define LSP_LV2_ATOM_KEY_SIZE       (sizeof(uint32_t) * 2)
    #define LSP_LV2_SIZE_PAD(size)      ((size_t(size) + 0x1ff) & (~size_t(0xff)))

    typedef struct LV2Extensions
    {
//        private:
//            ssize_t                 nRefs;

        public:
            LV2_Atom_Forge          forge;
            LV2_URID_Map           *map;
            LV2_URID_Unmap         *unmap;

            const char             *uriPlugin;
            LV2_URID                uridPlugin;

            LV2_URID                uridAtomTransfer;
            LV2_URID                uridEventTransfer;
            LV2_URID                uridObject;
            LV2_URID                uridState;
            LV2_URID                uridStateType;
            LV2_URID                uridMeshType;

#ifdef LSP_UI_SIDE
            LV2UI_Controller        ctl;
            LV2UI_Write_Function    wf;
#endif /* LSP_UI_SIDE */

        public:
#ifdef LSP_UI_SIDE
            inline LV2Extensions(const LV2_Feature* const* feat, const char *uri, LV2UI_Controller lv2_ctl, LV2UI_Write_Function lv2_write)
#else
            inline LV2Extensions(const LV2_Feature* const* feat, const char *uri)
#endif /* LSP_UI_SIDE */
            {
//                nRefs               = 1;
                map                 = NULL;
                unmap               = NULL;

                if (feat != NULL)
                {
                    for (size_t i=0; feat[i]; ++i)
                    {
                        const LV2_Feature *f = feat[i];

                        if (!strcmp(f->URI, LV2_URID__map))
                            map = reinterpret_cast<LV2_URID_Map *>(f->data);
                        if (!strcmp(f->URI, LV2_URID__unmap))
                            unmap = reinterpret_cast<LV2_URID_Unmap *>(f->data);
                    }
                }

                uriPlugin           = uri;
                uridPlugin          = (map != NULL) ? map->map(map->handle, uriPlugin) : -1;

#ifdef LSP_UI_SIDE
                ctl                 = lv2_ctl;
                wf                  = lv2_write;
#endif /* LSP_UI_SIDE */
                if (map != NULL)
                    lv2_atom_forge_init(&forge, map);

                uridAtomTransfer    = map_uri(LV2_ATOM__atomTransfer);
                uridEventTransfer   = map_uri(LV2_ATOM__eventTransfer);
                uridObject          = map_uri(LV2_ATOM__Object);
                uridState           = map_primitive("state");
                uridMeshType            = map_type("Mesh");
                uridStateType       = map_type("State");
            }

            ~LV2Extensions()
            {
                lsp_trace("destroy");
            }

//            inline void bind()      { nRefs ++; };
//            inline void unbind()
//            {
//                if ((--nRefs) <= 0)
//                    delete this;
//            }

        public:
            inline bool atom_supported() const
            {
                return map != NULL;
            }

            inline const char *unmap_urid(LV2_URID urid)
            {
                return (unmap != NULL) ? unmap->unmap(unmap->handle, urid) : NULL;
            }

#ifdef LSP_UI_SIDE
            inline void write_data(
                    uint32_t         port_index,
                    uint32_t         buffer_size,
                    uint32_t         port_protocol,
                    const void*      buffer)
            {
                wf(ctl, port_index, buffer_size, port_protocol, buffer);
            }
#endif /* LSP_UI_SIDE */

            inline LV2_Atom *forge_object(LV2_Atom_Forge_Frame* frame, LV2_URID id, LV2_URID otype)
            {
                const LV2_Atom_Object a = {
                    { sizeof(LV2_Atom_Object_Body), uridObject },
                    { id, otype }
                };
                return reinterpret_cast<LV2_Atom *>(
                    lv2_atom_forge_push(&forge, frame, lv2_atom_forge_write(&forge, &a, sizeof(a)))
                );
            }

            inline LV2_Atom_Vector *forge_vector(
                    uint32_t        child_size,
                    uint32_t        child_type,
                    uint32_t        n_elems,
                    const void*     elems)
            {
                return reinterpret_cast<LV2_Atom_Vector *>(
                    lv2_atom_forge_vector(&forge, child_size, child_type, n_elems, elems)
                );
            }


            inline LV2_Atom_Forge_Ref forge_property_head(LV2_URID key, LV2_URID context)
            {
                return lv2_atom_forge_property_head(&forge, key, context);
            }

            inline LV2_Atom_Forge_Ref forge_sequence_head(LV2_Atom_Forge_Frame* frame, uint32_t unit)
            {
                return lv2_atom_forge_sequence_head(&forge, frame, unit);
            }

            inline LV2_Atom_Forge_Ref forge_key(LV2_URID key)
            {
                const LV2_Atom_Property_Body a = { key, 0, { 0, 0 } };
                return lv2_atom_forge_write(&forge, &a, 2 * (uint32_t)sizeof(uint32_t));
            }

            inline void forge_pop(LV2_Atom_Forge_Frame* frame)
            {
                lv2_atom_forge_pop(&forge, frame);
            }

            inline LV2_Atom_Forge_Ref forge_float(float val)
            {
                return lv2_atom_forge_float(&forge, val);
            }

            inline LV2_Atom_Forge_Ref forge_int(size_t val)
            {
                return lv2_atom_forge_int(&forge, int32_t(val));
            }

            inline void forge_set_buffer(void* buf, size_t size)
            {
                lv2_atom_forge_set_buffer(&forge, reinterpret_cast<uint8_t *>(buf), size);
            }

            inline LV2_Atom_Forge_Ref forge_frame_time(int64_t frames)
            {
                return lv2_atom_forge_write(&forge, &frames, sizeof(frames));
            }

            inline LV2_URID map_uri(const char *fmt...)
            {
                if (map == NULL)
                    return -1;

                va_list vl;
                char tmpbuf[1024];

                va_start(vl, fmt);
                vsnprintf(tmpbuf, sizeof(tmpbuf), fmt, vl);
                va_end(vl);

                LV2_URID res = map->map(map->handle, tmpbuf);
                lsp_trace("URID for <%s> is %d", tmpbuf, int(res));
                return res;
            }

            inline LV2_URID map_port(const char *id)
            {
                return map_uri("%s/ports#%s", uriPlugin, id);
            }

            inline LV2_URID map_type(const char *id)
            {
                return map_uri("%s/ports#%s", LSP_TYPE_URI(lv2), id);
            }

            inline LV2_URID map_primitive(const char *id)
            {
                return map_uri("%s/%s", uriPlugin, id);
            }
    } LV2AtomForge;

    typedef struct LV2Mesh
    {
        size_t                  nMaxItems;
        size_t                  nBuffers;
        LV2_URID                uridItems;
        LV2_URID                uridDimensions;
        mesh_t                 *pMesh;
        LV2_URID               *pUrids;

        LV2Mesh()
        {
            nMaxItems       = 0;
            nBuffers        = 0;
            pMesh           = NULL;
            pUrids          = NULL;
            uridItems       = 0;
            uridDimensions  = 0;
        }

        ~LV2Mesh()
        {
            // Simply delete root structure
            if (pMesh != NULL)
            {
                delete [] reinterpret_cast<uint8_t *>(pMesh);
                pMesh       = NULL;
            }
            pUrids      = NULL;
        }

        void init(const port_t *meta, LV2Extensions *ext)
        {
            // Calculate sizes
            nBuffers            = meta->step;
            nMaxItems           = meta->start;
            size_t hdr_part     = sizeof(mesh_t) + sizeof(float *) * nBuffers;
            size_t urid_part    = sizeof(LV2_URID) * nBuffers;
            size_t hdr_size     = hdr_part + urid_part;
            size_t buf_size     = sizeof(float) * nMaxItems;

            lsp_trace("buffers = %d, hdr_part=%d, urid_part=%d, hdr_size=%d, buf_size=%d",
                    int(nBuffers), int(hdr_part), int(urid_part), int(hdr_size), int(buf_size));

            hdr_size            = (hdr_size + 0x3f) & (~size_t(0x3f)); // Align to 64-byte boundary
            buf_size            = (buf_size + 0x3f) & (~size_t(0x3f)); // Align to 64-byte boundary
            size_t buf_items    = buf_size / sizeof(float);

            lsp_trace("hdr_size=%d, buf_size=%d, buf_items=%d",
                                    int(hdr_size), int(buf_size), int(buf_items));

            // Initialize data
            uint8_t *ptr        = new uint8_t[hdr_size + buf_size * nBuffers];
            pMesh               = reinterpret_cast<mesh_t *>(ptr);
            pUrids              = reinterpret_cast<LV2_URID *>(&ptr[hdr_part]);
            float *bufs         = reinterpret_cast<float *>(&ptr[hdr_size]);

            lsp_trace("ptr = %p, pMesh = %p, pUrids = %p", ptr, pMesh, pUrids);

            for (size_t i=0; i<nBuffers; ++i)
            {
                lsp_trace("bufs[%d] = %p", int(i), bufs);

                pMesh->pvData[i]    = bufs;
                pUrids[i]           = ext->map_uri("%s/Mesh#dimension%d", LSP_TYPE_URI(lv2), int(i));
                bufs               += buf_items;
            }

            pMesh->nState       = M_WAIT;
            pMesh->nBuffers     = 0;
            pMesh->nItems       = 0;
            uridItems           = ext->map_uri("%s/Mesh#items", LSP_TYPE_URI(lv2));
            uridDimensions      = ext->map_uri("%s/Mesh#dimensions", LSP_TYPE_URI(lv2));

            lsp_trace("Initialized");
        }

        static size_t size_of_port(const port_t *meta)
        {
            size_t hdr_size     = sizeof(LV2_Atom_Int) + sizeof(LV2_Atom_Int) + 0x100; // Some extra bytes
            size_t prop_size    = sizeof(uint32_t) * 2;
            size_t vector_size  = prop_size + sizeof(LV2_Atom_Vector) + meta->start * sizeof(float);

            return LSP_LV2_SIZE_PAD(hdr_size + vector_size * meta->step);
        }
    } LV2Mesh;

    inline long lv2_all_port_sizes(const plugin_metadata_t *m, size_t type)
    {
        long size           = 0;
        long state_size     = 0;

        for (const port_t *p = m->ports; (p->id != NULL) && (p->name != NULL); ++p)
        {
            if ((p->flags & F_OUT) != type)
                continue;

            switch (p->role)
            {
                case R_CONTROL:
                case R_METER:
                    state_size      += sizeof(LV2_Atom_Float);
                    state_size      += LSP_LV2_ATOM_KEY_SIZE;
                    break;
                case R_MESH:
                    size            += LV2Mesh::size_of_port(p);
                    break;
                default:
                    break;
            }
        }

        // Update state size
        return size + LSP_LV2_SIZE_PAD(state_size); // Add some extra bytes
    }
}


#endif /* CORE_CONTAINER_LV2EXT_H_ */
