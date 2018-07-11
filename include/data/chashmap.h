///*
// * chashmap.h
// *
// *  Created on: 14 апр. 2016 г.
// *      Author: sadko
// */
//
//#ifndef DATA_CHASHMAP_H_
//#define DATA_CHASHMAP_H_
//
//#include <stddef.h>
//
//namespace lsp
//{
//    class basic_chashmap
//    {
//        protected:
//            static const size_t HASH_SEED   = 16;
//
//            typedef struct item_t
//            {
//                item_t     *next;
//                item_t     *prev;
//                size_t      key;
//                void       *value;
//            } item_t;
//
//            typedef struct list_t
//            {
//                size_t      nItems;
//                item_t      *pHead;
//            } list_t;
//
//        protected:
//            size_t      nNodes;
//            size_t      nItems;
//            list_t    **vNodes;
//
//        protected:
//            void *do_get(size_t key)
//            {
//                if (nNodes <= 0)
//                    return NULL;
//                list_t *list = vNodes[key % nNodes];
//                key         /= nNodes;
//                if (list == NULL)
//                    return NULL;
//                for (size_t i=0; i<list->nItems; ++i)
//                {
//                    if (list->vItems[i].key == key)
//                        return list->vItems[i].value;
//                }
//                return NULL;
//            }
//
//            bool resize()
//            {
//                list_t **nodes      = new list_t *[nNodes << 1];
//                for (size_t i=0; i<nNodes; ++i)
//                {
//                    if (vNodes[i] == NULL)
//                    {
//                        nodes[i]            = NULL;
//                        nodes[i + nNodes]   = NULL;
//                    }
//
//                    size_t lr           = 0;
//
//
//                    list_t split        = new list_t;
//                    nodes[i]            = vNodes[i];
//                    nodes[i + nNodes]   = new list_t;
//                    if (nodes[i] == NULL)
//                        continue;
//
//
//                }
//            }
//
//            bool do_put(size_t key, void *value, void **prev)
//            {
//                // Check root array
//                if (vNodes == NULL)
//                {
//                    vNodes      = new list_t * [HASH_SEED];
//                    if (vNodes == NULL)
//                        return false;
//                    for (size_t i=0; i<HASH_SEED; ++i)
//                        vNodes[i]   = NULL;
//                    nNodes      = HASH_SEED;
//                }
//
//                // Get list
//                size_t seed  = key % nNodes;
//                list_t *list = vNodes[seed];
//                if (list != NULL)
//                {
//                    size_t tmp_k    = key / nNodes;
//                    for (size_t i=0; i<list->nItems; ++i)
//                    {
//                        // Replace previous value
//                        if (list->vItems[i].key == tmp_k)
//                        {
//                            if (prev != NULL)
//                                *prev = list->vItems[i].value;
//                            list->vItems[i].value = value;
//                            return true;
//                        }
//                    }
//                }
//
//                // Extend hash until list is NULL or list size < HASH_SEED
//                while ((list != NULL) && (list->nItems >= HASH_SEED))
//                {
//                    if (!resize())
//                        return false;
//                    size_t seed  = key % nNodes;
//                    list = vNodes[seed];
//                }
//
//                if (list == NULL)
//                {
//                    list        = new list_t;
//                    if (list == NULL)
//                        return false;
//                    list->nItems            = 0;
//                    vNodes[seed]            = list;
//                }
//
//                size_t tmp_k            = key / nNodes;
//                list->vItems[0].key     = tmp_k;
//                list->vItems[0].value   = value;
//                list->nItems            ++;
//                if (prev != NULL)
//                    *prev                   = NULL;
//                nItems ++;
//                return true;
//            }
//
//        public:
//            basic_chashmap()
//            {
//                nNodes          = 0;
//                nItems          = 0;
//                vNodes          = NULL;
//            }
//
//            ~basic_chashmap()
//            {
//                flush();
//            }
//
//            inline void clear()
//            {
//                for (size_t i=0; i<nNodes; ++i)
//                {
//                    list_t *list = vNodes[i];
//                    if (list != NULL)
//                        list->nItems    = 0;
//                }
//                nItems = 0;
//            }
//
//            inline size_t size() const  { return nItems; }
//
//            void flush()
//            {
//                if (vNodes != NULL)
//                {
//                    for (size_t i=0; i<nNodes; ++i)
//                    {
//                        if (vNodes[i] != NULL)
//                        {
//                            delete vNodes[i];
//                            vNodes[i]   = NULL;
//                        }
//                    }
//                    delete [] vNodes;
//                    vNodes      = NULL;
//                }
//
//                nItems      = 0;
//            }
//    };
//
//    template <class T>
//        class chashmap: public basic_chashmap
//        {
//            public:
//                inline T *operator[](size_t key)
//                {
//                    return reinterpret_cast<T *>(do_get(key));
//                }
//
//                inline T *get(size_t key)
//                {
//                    return reinterpret_cast<T *>(do_get(key));
//                }
//
//                inline bool put(size_t key, T *value)
//                {
//                    return do_put(key, value, NULL);
//                }
//
//                inline bool put(size_t key, T *value, T **prev)
//                {
//                    return do_put(key, value, reinterpret_cast<void **>(prev));
//                }
//        };
//
//}
//
//
//#endif /* DATA_CHASHMAP_H_ */
