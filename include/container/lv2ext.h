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
#include <lv2/lv2plug.in/ns/ext/port-groups/port-groups.h>
#include <lv2/lv2plug.in/ns/ext/resize-port/resize-port.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/port-props/port-props.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/extensions/units/units.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

// Include common definitions
#include <container/const.h>

#ifndef LV2_ATOM__Object
    #define LV2_ATOM__Object        LV2_ATOM_PREFIX "Object"
#endif /* LV2_ATOM__Object */

namespace lsp
{
    #define LSP_LV2_ATOM_KEY_SIZE       (sizeof(uint32_t) * 2)
    #define LSP_LV2_SIZE_PAD(size)      ((size_t(size) + 0x1ff) & (~size_t(0xff)))

    struct LV2Extensions
    {
        public:
            LV2_Atom_Forge          forge;

            // Extension interfaces
            LV2_URID_Map           *map;
            LV2_URID_Unmap         *unmap;
            LV2_Worker_Schedule    *sched;

            // State interface
            LV2_State_Store_Function    hStore;
            LV2_State_Retrieve_Function hRetrieve;
            LV2_State_Handle            hHandle;

            // Plugin URI and URID
            const char             *uriPlugin;
            LV2_URID                uridPlugin;

            // Miscellaneous URIDs
            LV2_URID                uridAtomTransfer;
            LV2_URID                uridEventTransfer;
            LV2_URID                uridObject;
            LV2_URID                uridState;
            LV2_URID                uridStateType;
            LV2_URID                uridMeshType;
            LV2_URID                uridPathType;
            LV2_URID                uridPatchGet;
            LV2_URID                uridPatchSet;
            LV2_URID                uridPatchMessage;
            LV2_URID                uridPatchResponse;
            LV2_URID                uridPatchProperty;
            LV2_URID                uridPatchValue;
            LV2_URID                uridAtomUrid;
            LV2_URID                uridChunk;

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
                map                 = NULL;
                unmap               = NULL;
                sched               = NULL;

                if (feat != NULL)
                {
                    for (size_t i=0; feat[i]; ++i)
                    {
                        const LV2_Feature *f = feat[i];

                        if (!strcmp(f->URI, LV2_URID__map))
                            map = reinterpret_cast<LV2_URID_Map *>(f->data);
                        if (!strcmp(f->URI, LV2_URID__unmap))
                            unmap = reinterpret_cast<LV2_URID_Unmap *>(f->data);
                        if (!strcmp(f->URI, LV2_WORKER__schedule))
                            sched = reinterpret_cast<LV2_Worker_Schedule *>(f->data);
                    }
                }

#ifdef LSP_UI_SIDE
                ctl                 = lv2_ctl;
                wf                  = lv2_write;
#endif /* LSP_UI_SIDE */

                uriPlugin           = uri;
                uridPlugin          = (map != NULL) ? map->map(map->handle, uriPlugin) : -1;

                if (map != NULL)
                    lv2_atom_forge_init(&forge, map);

                uridAtomTransfer    = map_uri(LV2_ATOM__atomTransfer);
                uridEventTransfer   = map_uri(LV2_ATOM__eventTransfer);
                uridObject          = map_uri(LV2_ATOM__Object);
                uridStateType       = map_type("State");
                uridState           = map_primitive("state");
                uridMeshType        = map_type("Mesh");
                uridPathType        = map_uri(LV2_ATOM__Path);
                uridPatchGet        = map_uri(LV2_PATCH__Get);
                uridPatchSet        = map_uri(LV2_PATCH__Set);
                uridPatchMessage    = map_uri(LV2_PATCH__Message);
                uridPatchResponse   = map_uri(LV2_PATCH__Response);
                uridPatchProperty   = map_uri(LV2_PATCH__property);
                uridPatchValue      = map_uri(LV2_PATCH__value);
                uridAtomUrid        = map_uri(LV2_ATOM__URID);
                uridChunk           = map_uri(LV2_ATOM__Chunk);
            }

            ~LV2Extensions()
            {
                lsp_trace("destroy");
            }

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

            inline LV2_Atom_Forge_Ref forge_sequence_head(LV2_Atom_Forge_Frame* frame, uint32_t unit)
            {
                return lv2_atom_forge_sequence_head(&forge, frame, unit);
            }

            inline LV2_Atom_Forge_Ref forge_key(LV2_URID key)
            {
                const uint32_t body[] = { key, 0 };
                return lv2_atom_forge_write(&forge, body, sizeof(body));
            }

            inline LV2_Atom_Forge_Ref forge_urid(LV2_URID urid)
            {
                const LV2_Atom_URID a = { { sizeof(LV2_URID), forge.URID }, urid };
                return lv2_atom_forge_write(&forge, &a, sizeof(LV2_Atom_URID));
            }

            inline LV2_Atom_Forge_Ref forge_path(const char *str)
            {
                size_t len = strlen(str);
                return lv2_atom_forge_typed_string(&forge, forge.Path, str, len);
            }

            inline void forge_pop(LV2_Atom_Forge_Frame* frame)
            {
                lv2_atom_forge_pop(&forge, frame);
            }

            inline LV2_Atom_Forge_Ref forge_float(float val)
            {
                const LV2_Atom_Float a = { { sizeof(float), forge.Float }, val };
                return lv2_atom_forge_primitive(&forge, &a.atom);
            }

            inline LV2_Atom_Forge_Ref forge_int(int32_t val)
            {
                const LV2_Atom_Int a    = { { sizeof(int32_t), forge.Int }, val };
                return lv2_atom_forge_primitive(&forge, &a.atom);
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

            // Sa
            inline void init_state_context(
                LV2_State_Store_Function    store,
                LV2_State_Retrieve_Function retrieve,
                LV2_State_Handle            handle,
                uint32_t                    flags,
                const LV2_Feature *const *  features
            )
            {
                hStore          = store;
                hRetrieve       = retrieve;
                hHandle         = handle;
            }

            inline void store_value(LV2_URID urid, LV2_URID type, const void *data, size_t size)
            {
                if ((hStore == NULL) || (hHandle == NULL))
                    return;
                hStore(hHandle, urid, data, size, type, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
            }

            inline const void *restore_value(LV2_URID urid, LV2_URID type, size_t *size)
            {
                if ((hRetrieve == NULL) || (hHandle == NULL))
                    return NULL;

                size_t t_size;
                uint32_t t_type, t_flags;
                const void *ptr   = hRetrieve(hHandle, urid, &t_size, &t_type, &t_flags);
                lsp_trace("retrieved ptr = %p", ptr);
                if (ptr == NULL)
                    return NULL;

                lsp_trace("retrieved type = %d (%s)", int(t_type), unmap_urid(t_type));
                if (t_type != type)
                    return NULL;

                *size    = t_size;
                return ptr;
            }

            inline void reset_state_context()
            {
                hStore          = NULL;
                hRetrieve       = NULL;
                hHandle         = NULL;
            }
    };

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
            switch (p->role)
            {
                case R_CONTROL:
                case R_METER:
                    if ((p->flags & F_OUT) == type)
                    {
                        state_size      += sizeof(LV2_Atom_Float);
                        state_size      += LSP_LV2_ATOM_KEY_SIZE;
                    }
                    break;
                case R_MESH:
                    if ((p->flags & F_OUT) == type)
                        size            += LV2Mesh::size_of_port(p);
                    break;
                case R_PATH: // Both sizes: IN and OUT
                    size            += sizeof(LV2_Atom_Property) + sizeof(LV2_Atom_URID) + sizeof(LV2_Atom) + PATH_MAX;
                    break;
                default:
                    break;
            }
        }

        // Update state size
        return LSP_LV2_SIZE_PAD(size + state_size); // Add some extra bytes
    }
}


#endif /* CORE_CONTAINER_LV2EXT_H_ */
