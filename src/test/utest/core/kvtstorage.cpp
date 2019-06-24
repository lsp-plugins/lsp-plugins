/*
 * kvtstorage.cpp
 *
 *  Created on: 31 мая 2019 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/KVTStorage.h>
#include <test/utest.h>

using namespace lsp;

enum
{
    F_Attached  = 1 << 0,
    F_Detached  = 1 << 1,
    F_Created   = 1 << 2,
    F_Rejected  = 1 << 3,
    F_Changed   = 1 << 4,
    F_Removed   = 1 << 5,
    F_Accessed  = 1 << 6,
    F_Missed    = 1 << 7,
    F_Committed = 1 << 8,

    F_All   =
            F_Attached | F_Detached | F_Created |
            F_Rejected | F_Changed | F_Removed |
            F_Accessed | F_Missed | F_Committed
};

UTEST_BEGIN("core", kvtstorage)
    const char *ndstr;
    char *dstr;
    kvt_blob_t ndblob;
    kvt_blob_t dblob;

    void dump_parameter(const char *prefix, const kvt_param_t *param)
    {
        printf("%s ", prefix);
        switch (param->type)
        {
            case KVT_INT32:     printf("i32(0x%lx)\n", long(param->i32)); break;
            case KVT_UINT32:    printf("u32(0x%lx)\n", (unsigned long)(param->u32)); break;
            case KVT_INT64:     printf("i64(0x%llx)\n", (long long)(param->i64)); break;
            case KVT_UINT64:    printf("i64(0x%llx)\n", (unsigned long long)(param->u64)); break;
            case KVT_FLOAT32:   printf("f32(%f)\n", param->f32); break;
            case KVT_FLOAT64:   printf("f64(%f)\n", param->f64); break;
            case KVT_STRING:    printf("str(%s)\n", param->str); break;
            case KVT_BLOB:
                printf("blob(size=%d, type=(%s), data=(", int(param->blob.size), param->blob.ctype);
                if (param->blob.data != NULL)
                {
                    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(param->blob.data);
                    for (size_t i=0; i<param->blob.size; ++i)
                    {
                        if (i) printf(" %02x", int(ptr[i]));
                        else printf("%02x", int(ptr[i]));
                    }
                    printf(")\n");
                }
                else
                    printf("nil))\n");
                break;
            default:
                printf("Unexpected parameter type: %d\n", int(param->type));
                exit(1);
                break;
        }
    }

    class TestListener: public KVTListener
    {
        private:
            test_type_t *test;

            size_t      nAttached;
            size_t      nDetached;
            size_t      nCreated;
            size_t      nRejected;
            size_t      nChanged;
            size_t      nRemoved;
            size_t      nAccessed;
            size_t      nCommitted;
            size_t      nMissed;

            bool        bVerbose;

        public:
            explicit TestListener(test_type_t *tst): test(tst)
            {
                clear();
                bVerbose = true;
            }

            virtual ~TestListener() {}

        public:
            void clear()
            {
                nAttached   = 0;
                nDetached   = 0;
                nCreated    = 0;
                nRejected   = 0;
                nChanged    = 0;
                nRemoved    = 0;
                nAccessed   = 0;
                nCommitted  = 0;
                nMissed     = 0;
            }

            void verbose(bool verbose)
            {
                bVerbose = verbose;
            }

            bool check(size_t flags, size_t value)
            {
                if ((flags & F_Attached) && (nAttached != value))
                    return false;
                if ((flags & F_Detached) && (nDetached != value))
                    return false;
                if ((flags & F_Created) && (nCreated != value))
                    return false;
                if ((flags & F_Rejected) && (nRejected != value))
                    return false;
                if ((flags & F_Changed) && (nChanged != value))
                    return false;
                if ((flags & F_Removed) && (nRemoved != value))
                    return false;
                if ((flags & F_Accessed) && (nAccessed != value))
                    return false;
                if ((flags & F_Missed) && (nMissed != value))
                    return false;
                if ((flags & F_Committed) && (nCommitted != value))
                    return false;
                return true;
            }

            virtual void attached(KVTStorage *storage)
            {
                if (bVerbose)
                    test->printf("Attached storage %p\n", storage);
                ++nAttached;
            }

            virtual void detached(KVTStorage *storage)
            {
                if (bVerbose)
                    test->printf("Detached storage %p\n", storage);
                ++nDetached;
            }

            virtual void created(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
            {
                if (bVerbose)
                {
                    test->printf("Created parameter %s\n", id);
                    test->dump_parameter("  created   = ", param);
                }
                ++nCreated;
            }

            virtual void rejected(KVTStorage *storage, const char *id, const kvt_param_t *rej, const kvt_param_t *curr, size_t pending)
            {
                if (bVerbose)
                {
                    test->printf("Parameter %s has been rejected\n", id);
                    test->dump_parameter("  rejected  = ", rej);
                    test->dump_parameter("  current   = ", curr);
                }
                ++nRejected;
            }

            virtual void changed(KVTStorage *storage, const char *id, const kvt_param_t *oval, const kvt_param_t *nval, size_t pending)
            {
                if (bVerbose)
                {
                    test->printf("Parameter %s has been changed\n", id);
                    test->dump_parameter("  old       = ", oval);
                    test->dump_parameter("  new       = ", nval);
                }
                ++nChanged;
            }

            virtual void removed(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
            {
                if (bVerbose)
                {
                    test->printf("Parameter %s has been removed\n", id);
                    test->dump_parameter("  removed   = ", param);
                }
                ++nRemoved;
            }

            virtual void access(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
            {
                if (bVerbose)
                {
                    test->printf("Parameter %s has been accessed\n", id);
                    test->dump_parameter("  accessed  = ", param);
                }
                ++nAccessed;
            }

            virtual void commit(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
            {
                if (bVerbose)
                {
                    test->printf("Parameter %s has been committed\n", id);
                    test->dump_parameter("  committed = ", param);
                }
                ++nCommitted;
            }

            virtual void missed(KVTStorage *storage, const char *id)
            {
                if (bVerbose)
                    test->printf("Parameter %s is missing\n", id);
                ++nMissed;
            }
    };

    UTEST_INIT
    {
        ndstr = "Test non-delegated string";
        dstr      = strdup("Test delegated string");

        ndblob.ctype    = "text/html";
        ndblob.data     = "Some non-delegated blob data";
        ndblob.size     = strlen("Some non-delegated blob data") + 1;

        dblob.ctype     = strdup("application/text");
        dblob.data      = strdup("Some delegated blob data");
        dblob.size      = strlen("Some delegated blob data") + 1;
    }


    void test_bind(KVTStorage &s, TestListener &l)
    {
        // Bind listener
        UTEST_ASSERT(l.check(F_All, 0));
        UTEST_ASSERT(s.listeners() == 0);
        UTEST_ASSERT(s.bind(&l) == STATUS_OK);
        UTEST_ASSERT(s.bind(&l) == STATUS_ALREADY_BOUND);
        UTEST_ASSERT(l.check(F_All ^ F_Attached, 0));
        UTEST_ASSERT(l.check(F_Attached, 1));
        UTEST_ASSERT(s.listeners() == 1);
    }

    void test_create_entries(KVTStorage &s, TestListener &l)
    {
        // Create entries
        l.clear();
        kvt_param_t p;
        p.type  = kvt_param_type_t(1234);
        UTEST_ASSERT(s.put("/bad_type", &p, KVT_TX) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s.put("/", "fake", KVT_TX) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.put("/fake/", uint64_t(123123), KVT_TX) == STATUS_INVALID_VALUE);

        UTEST_ASSERT(s.put("/value1", uint32_t(1), KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/value3", uint64_t(3), KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/value2", int32_t(2), KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/test/value1", float(1.0f), KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/test/value2", double(2.0f), KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/test/value3", strdup("Delegated string"), KVT_TX | KVT_DELEGATE) == STATUS_OK);

        UTEST_ASSERT(s.put("/param/i32", int32_t(0x10), KVT_TX | KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/u32", uint32_t(0x20), KVT_TX | KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/i64", int64_t(0x30), KVT_TX | KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/u64", uint64_t(0x40), KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/f32", 440.0f, KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/f64", double(48000.0), KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/str", ndstr, KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/dstr", dstr, KVT_RX | KVT_DELEGATE) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/blob", &ndblob, KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/dblob", &dblob, KVT_RX | KVT_DELEGATE) == STATUS_OK);

        UTEST_ASSERT(s.put("/some/value", float(M_PI)) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/silent/value", "Math.PI") == STATUS_OK);

        UTEST_ASSERT(l.check(F_All ^ F_Created, 0));
        UTEST_ASSERT(l.check(F_Created, 18));
        UTEST_ASSERT(s.values() == 18);
        UTEST_ASSERT(s.tx_pending() == 9);
        UTEST_ASSERT(s.rx_pending() == 10);
        UTEST_ASSERT(s.nodes() == 22);

        // List modified TX entries
        {
            l.clear();
            l.verbose(false);
            KVTIterator *it = s.enum_tx_pending();
            UTEST_ASSERT(it != NULL);
            printf("Dumping the state of the TX parameters of the KVTTree\n");

            status_t res;
            size_t modified = 0;
            while ((res = it->next()) == STATUS_OK)
            {
                const char *name = it->name();
                const kvt_param_t *cp;

                if (it->exists())
                {
                    UTEST_ASSERT(it->get(&cp) == STATUS_OK)
                    dump_parameter(name, cp);
                    if (it->tx_pending())
                        ++modified;
                }
                else
                    printf("%s -> null\n", name);
            }

            UTEST_ASSERT(res == STATUS_NOT_FOUND);
            l.verbose(true);
            UTEST_ASSERT(l.check(F_Accessed, 9));
            UTEST_ASSERT(modified == 9);
        }

        // List modified RX entries
        {
            l.clear();
            l.verbose(false);
            KVTIterator *it = s.enum_rx_pending();
            UTEST_ASSERT(it != NULL);
            printf("Dumping the state of the RX parameters of the KVTTree\n");

            status_t res;
            size_t modified = 0;
            while ((res = it->next()) == STATUS_OK)
            {
                const char *name = it->name();
                const kvt_param_t *cp;

                if (it->exists())
                {
                    UTEST_ASSERT(it->get(&cp) == STATUS_OK)
                    dump_parameter(name, cp);
                    if (it->rx_pending())
                        ++modified;
                }
                else
                    printf("%s -> null\n", name);
            }

            UTEST_ASSERT(res == STATUS_NOT_FOUND);
            l.verbose(true);
            UTEST_ASSERT(l.check(F_Accessed, 10));
            UTEST_ASSERT(modified == 10);
        }
    }

    void test_replace_entries(KVTStorage &s, TestListener &l)
    {
        // Replace entries
        l.clear();
        UTEST_ASSERT(s.put("/value1", uint32_t(100)) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/test/value2", "Some string") == STATUS_OK);
        UTEST_ASSERT(s.put("/value1", uint32_t(101), KVT_KEEP) == STATUS_ALREADY_EXISTS);
        UTEST_ASSERT(s.put("/some/test/value2", double(M_PI), KVT_KEEP) == STATUS_ALREADY_EXISTS);
        UTEST_ASSERT(l.check(F_All ^ (F_Changed | F_Rejected), 0));
        UTEST_ASSERT(l.check(F_Changed, 2));
        UTEST_ASSERT(l.check(F_Rejected, 2));
        UTEST_ASSERT(s.values() == 18);
        UTEST_ASSERT(s.tx_pending() == 9);
        UTEST_ASSERT(s.rx_pending() == 10);
        UTEST_ASSERT(s.nodes() == 22);

        // Print the particular content of the tree
        l.clear();
        l.verbose(false);
        KVTIterator *it = s.enum_branch("/param", false);
        UTEST_ASSERT(it != NULL);
        printf("Dumping the state of the /param branch of KVTTree\n");

        status_t res;
        size_t modified = 0;
        while ((res = it->next()) == STATUS_OK)
        {
            const char *name = it->name();
            const kvt_param_t *cp;

            if (it->exists())
            {
                UTEST_ASSERT(it->get(&cp) == STATUS_OK)
                dump_parameter(name, cp);
                if (it->pending())
                    ++modified;

                if (!strcmp(it->id(), "f32"))
                {
                    UTEST_ASSERT(cp->f32 == 440.0f);
                    UTEST_ASSERT(it->put(880.0f) == STATUS_OK);
                    UTEST_ASSERT(cp->f32 == 440.0f);
                }
                if (!strcmp(it->id(), "f64"))
                {
                    UTEST_ASSERT(cp->f64 == double(48000.0));
                    UTEST_ASSERT(it->put(440.0f, KVT_KEEP) == STATUS_ALREADY_EXISTS);
                    UTEST_ASSERT(cp->f64 == double(48000.0));
                }
            }
            else
                printf("%s -> null\n", name);
        }

        UTEST_ASSERT(res == STATUS_NOT_FOUND);
        l.verbose(true);
        UTEST_ASSERT(l.check(F_Accessed, 10));
        UTEST_ASSERT(l.check(F_Changed, 1));
        UTEST_ASSERT(modified == 10);
    }

    void test_read_entries(KVTStorage &s, TestListener &l)
    {
        kvt_param_t p;
        const kvt_param_t *cp;
        const kvt_blob_t *cb;

        // Retrieve entries
        l.clear();
        UTEST_ASSERT(s.get("/", &cp) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.get("/value1/", &cp) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.get("/some/test/value1", &p.u32) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s.get("/some/test/value5", &p.f32) == STATUS_NOT_FOUND);

        UTEST_ASSERT(s.get("/value1", &cp) == STATUS_OK);
        UTEST_ASSERT((cp->type == KVT_UINT32) && (cp->u32 == 100));

        UTEST_ASSERT(s.get("/value2", &cp) == STATUS_OK);
        UTEST_ASSERT((cp->type == KVT_INT32) && (cp->i32 == 2));

        UTEST_ASSERT(s.get("/param/i32", &p.i32) == STATUS_OK);
        UTEST_ASSERT(p.i32 == int32_t(0x10));

        UTEST_ASSERT(s.get("/param/u32", &p.u32) == STATUS_OK);
        UTEST_ASSERT(p.u32 == uint32_t(0x20));

        UTEST_ASSERT(s.get("/param/i64", &p.i64) == STATUS_OK);
        UTEST_ASSERT(p.i64 == int64_t(0x30));

        UTEST_ASSERT(s.get("/param/u64", &p.u64) == STATUS_OK);
        UTEST_ASSERT(p.u64 == uint64_t(0x40));

        UTEST_ASSERT(s.get("/param/f32", &p.f32) == STATUS_OK);
        UTEST_ASSERT(p.f32 == 880.0f);

        UTEST_ASSERT(s.get("/param/f64", &p.f64) == STATUS_OK);
        UTEST_ASSERT(p.f64 == double(48000.0));

        UTEST_ASSERT(s.get("/param/str", &p.str) == STATUS_OK);
        UTEST_ASSERT(p.str != ndstr);
        UTEST_ASSERT(strcmp(p.str, ndstr) == 0);

        UTEST_ASSERT(s.get("/param/dstr", &p.str) == STATUS_OK);
        UTEST_ASSERT(p.str == dstr);
        UTEST_ASSERT(strcmp(p.str, dstr) == 0);

        UTEST_ASSERT(s.get("/param/blob", &cb) == STATUS_OK);
        UTEST_ASSERT(cb->size == ndblob.size);
        UTEST_ASSERT(cb->ctype != ndblob.ctype);
        UTEST_ASSERT(strcmp(cb->ctype, ndblob.ctype) == 0);
        UTEST_ASSERT(cb->data != ndblob.data);
        UTEST_ASSERT(memcmp(cb->data, ndblob.data, cb->size) == 0);

        UTEST_ASSERT(s.get("/param/dblob", &cb) == STATUS_OK);
        UTEST_ASSERT(cb->size == dblob.size);
        UTEST_ASSERT(cb->ctype == dblob.ctype);
        UTEST_ASSERT(strcmp(cb->ctype, dblob.ctype) == 0);
        UTEST_ASSERT(cb->data == dblob.data);
        UTEST_ASSERT(memcmp(cb->data, dblob.data, cb->size) == 0);

        UTEST_ASSERT(l.check(F_All ^ (F_Accessed | F_Missed), 0));
        UTEST_ASSERT(l.check(F_Accessed, 12));
        UTEST_ASSERT(l.check(F_Missed, 1));
        UTEST_ASSERT(s.values() == 18);
        UTEST_ASSERT(s.tx_pending() == 9);
        UTEST_ASSERT(s.rx_pending() == 10);
        UTEST_ASSERT(s.nodes() == 22);

        // Print the whole content of the tree
        l.clear();
        l.verbose(false);
        KVTIterator *it = s.enum_branch("/", true);
        UTEST_ASSERT(it != NULL);
        printf("Dumping the whole state of the KVTTree\n");

        status_t res;
        size_t modified = 0;
        while ((res = it->next()) == STATUS_OK)
        {
            const char *name = it->name();
            const kvt_param_t *cp;

            if (it->exists())
            {
                UTEST_ASSERT(it->get(&cp) == STATUS_OK)
                dump_parameter(name, cp);
                if (it->pending())
                    ++modified;
            }
            else
                printf("%s -> null\n", name);
        }

        UTEST_ASSERT(res == STATUS_NOT_FOUND);
        l.verbose(true);
        UTEST_ASSERT(l.check(F_All ^ (F_Accessed | F_Missed), 0));
        UTEST_ASSERT(l.check(F_Accessed, 18));
        UTEST_ASSERT(l.check(F_Missed, 4));
        UTEST_ASSERT(modified == 16);
    }

    void test_unexisting_branch(KVTStorage &s, TestListener &l)
    {
        // Print the whole content of the tree
        l.clear();
        l.verbose(false);
        KVTIterator *it = s.enum_branch("/some/branch", true);
        UTEST_ASSERT(it != NULL);
        printf("Dumping the state of unexisting branch of KVTTree\n");

        status_t res;
        size_t modified = 0;
        while ((res = it->next()) == STATUS_OK)
        {
            const char *name = it->name();
            const kvt_param_t *cp;

            if (it->exists())
            {
                UTEST_ASSERT(it->get(&cp) == STATUS_OK)
                dump_parameter(name, cp);
                if (it->pending())
                    ++modified;
            }
            else
                printf("%s -> null\n", name);
        }

        UTEST_ASSERT(res == STATUS_NOT_FOUND);
        l.verbose(true);
        UTEST_ASSERT(l.check(F_All ^ F_Missed, 0));
        UTEST_ASSERT(l.check(F_Missed, 1));
    }


    void test_commit_entries(KVTStorage &s, TestListener &l)
    {
        // Commit
        l.clear();

        UTEST_ASSERT(s.commit("/some/unexisting/value", KVT_RX | KVT_TX) == STATUS_NOT_FOUND);
        UTEST_ASSERT(s.commit("/value2", KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/i32", KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/u32", KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/i64", KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/u64", KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/f32", KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/f64", KVT_TX) == STATUS_OK);

        UTEST_ASSERT(l.check(F_All ^ (F_Committed | F_Missed), 0));
        UTEST_ASSERT(l.check(F_Committed, 9));
        UTEST_ASSERT(l.check(F_Missed, 1));
        UTEST_ASSERT(s.values() == 18);
        UTEST_ASSERT(s.tx_pending() == 5);
        UTEST_ASSERT(s.rx_pending() == 5);
        UTEST_ASSERT(s.nodes() == 22);

        // List all entries
        l.clear();
        l.verbose(false);
        KVTIterator *it = s.enum_all();
        UTEST_ASSERT(it != NULL);
        printf("Computing number of modified items with full-list iterator\n");

        status_t res;
        size_t modified = 0;
        while ((res = it->next()) == STATUS_OK)
        {
            if (it->pending())
            {
                printf("Modified entry: %s\n", it->name());
                ++modified;
            }
        }

        UTEST_ASSERT(res == STATUS_NOT_FOUND);
        l.verbose(true);
        UTEST_ASSERT(modified == 10);
    }

    void test_existense_entries(KVTStorage &s, TestListener &l)
    {
        // Check for existence
        l.clear();
        UTEST_ASSERT(s.exists("/param/i32"));
        UTEST_ASSERT(!s.exists("/param/u32", KVT_INT32));
        UTEST_ASSERT(!s.exists("/param/blob", KVT_STRING));
        UTEST_ASSERT(!s.exists("/"));
        UTEST_ASSERT(!s.exists("/some/unexisting/parameter"));

        UTEST_ASSERT(l.check(F_All ^ F_Missed, 0));
        UTEST_ASSERT(l.check(F_Missed, 1));
        UTEST_ASSERT(s.values() == 18);
        UTEST_ASSERT(s.tx_pending() == 5);
        UTEST_ASSERT(s.rx_pending() == 5);
        UTEST_ASSERT(s.nodes() == 22);
    }

    void test_remove_entries(KVTStorage &s, TestListener &l)
    {
        kvt_param_t p;
        const kvt_param_t *cp;
        const kvt_blob_t *cb;

        // Remove entries
        l.clear();
        UTEST_ASSERT(s.remove("/", &cp) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.remove("/value1/", &cp) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.remove("/some/test/value1", &p.u32) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s.remove("/some/test/value5", &p.f32) == STATUS_NOT_FOUND);

        UTEST_ASSERT(s.remove("/value2", &cp) == STATUS_OK);
        UTEST_ASSERT((cp->type == KVT_INT32) && (cp->i32 == 2));

        UTEST_ASSERT(s.remove("/value3") == STATUS_OK);
        UTEST_ASSERT((cp->type == KVT_INT32) && (cp->i32 == 2));

        UTEST_ASSERT(s.remove("/param/i32", &p.i32) == STATUS_OK);
        UTEST_ASSERT(p.i32 == int32_t(0x10));

        UTEST_ASSERT(s.remove("/param/u32", &p.u32) == STATUS_OK);
        UTEST_ASSERT(p.u32 == uint32_t(0x20));

        UTEST_ASSERT(s.remove("/param/i64", &p.i64) == STATUS_OK);
        UTEST_ASSERT(p.i64 == int64_t(0x30));

        UTEST_ASSERT(s.remove("/param/u64", &p.u64) == STATUS_OK);
        UTEST_ASSERT(p.u64 == uint64_t(0x40));

        UTEST_ASSERT(s.remove("/param/f32", &p.f32) == STATUS_OK);
        UTEST_ASSERT(p.f32 == 880.0f);

        UTEST_ASSERT(s.remove("/param/f64", &p.f64) == STATUS_OK);
        UTEST_ASSERT(p.f64 == double(48000.0));

        UTEST_ASSERT(s.remove("/param/str", &p.str) == STATUS_OK);
        UTEST_ASSERT(p.str != ndstr);
        UTEST_ASSERT(strcmp(p.str, ndstr) == 0);

        UTEST_ASSERT(s.remove("/param/dstr", &p.str) == STATUS_OK);
        UTEST_ASSERT(p.str == dstr);
        UTEST_ASSERT(strcmp(p.str, dstr) == 0);

        UTEST_ASSERT(s.remove("/param/blob", &cb) == STATUS_OK);
        UTEST_ASSERT(cb->size == ndblob.size);
        UTEST_ASSERT(cb->ctype != ndblob.ctype);
        UTEST_ASSERT(strcmp(cb->ctype, ndblob.ctype) == 0);
        UTEST_ASSERT(cb->data != ndblob.data);
        UTEST_ASSERT(memcmp(cb->data, ndblob.data, cb->size) == 0);

        UTEST_ASSERT(s.remove("/param/dblob", &cb) == STATUS_OK);
        UTEST_ASSERT(cb->size == dblob.size);
        UTEST_ASSERT(cb->ctype == dblob.ctype);
        UTEST_ASSERT(strcmp(cb->ctype, dblob.ctype) == 0);
        UTEST_ASSERT(cb->data == dblob.data);
        UTEST_ASSERT(memcmp(cb->data, dblob.data, cb->size) == 0);

        UTEST_ASSERT(l.check(F_All ^ (F_Removed | F_Missed), 0));
        UTEST_ASSERT(l.check(F_Removed, 12));
        UTEST_ASSERT(l.check(F_Missed, 1));
        UTEST_ASSERT(s.values() == 6);
        UTEST_ASSERT(s.tx_pending() == 4);
        UTEST_ASSERT(s.rx_pending() == 0);
    }

    void test_remove_branch(KVTStorage &s, TestListener &l)
    {
        // Clear tree
        printf("Cleaning up tree...\n");
        l.clear();
        s.clear();
        UTEST_ASSERT(l.check(F_All ^ F_Removed, 0));
        UTEST_ASSERT(l.check(F_Removed, 6));
        UTEST_ASSERT(s.tx_pending() == 0);
        UTEST_ASSERT(s.rx_pending() == 0);
        UTEST_ASSERT(s.values() == 0);
        UTEST_ASSERT(s.nodes() == 0);

        // Prepare tree
        printf("Initializing new tree...\n");
        l.clear();
        UTEST_ASSERT(s.put("/tree/a0", 1, KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/b0", 2, KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/c0", 3, KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/x0", 4, KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/y0", 5, KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/z0", 6, KVT_RX | KVT_TX) == STATUS_OK);

        UTEST_ASSERT(s.put("/tree/t0", 7, KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t1", 7, KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t0/some/param1", 8, KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t1/some/param1", 8, KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t0/some/param2", 9) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t1/some/param2", 9) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t0/some/param3", 10) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t1/some/param3", 10) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t0/some/other/param", 11) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t1/some/other/param", 11) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t0/other", 12) == STATUS_OK);
        UTEST_ASSERT(s.put("/tree/t1/other", 12) == STATUS_OK);

        UTEST_ASSERT(l.check(F_All ^ F_Created, 0));
        UTEST_ASSERT(l.check(F_Created, 18));
        UTEST_ASSERT(s.values() == 18)
        UTEST_ASSERT(s.tx_pending() == 6)
        UTEST_ASSERT(s.rx_pending() == 6)

        // Remove entries
        l.clear();
        UTEST_ASSERT(s.remove_branch("/tree/t0") == STATUS_OK);
        UTEST_ASSERT(l.check(F_All ^ F_Removed, 0));
        UTEST_ASSERT(l.check(F_Removed, 6));
        UTEST_ASSERT(!s.exists("/tree/t0"));
        UTEST_ASSERT(s.values() == 12);
        UTEST_ASSERT(s.tx_pending() == 5)
        UTEST_ASSERT(s.rx_pending() == 5)

        // Remove entries with iterator
        l.clear();
        l.verbose(false);
        KVTIterator *it = s.enum_branch("/tree", false);
        UTEST_ASSERT(it != NULL);
        printf("Dumping the state of the /tree branch of KVTTree and removing /tree/t1\n");

        status_t res;
        while ((res = it->next()) == STATUS_OK)
        {
            const char *name = it->name();
            const kvt_param_t *cp;

            if (it->exists())
            {
                UTEST_ASSERT(it->get(&cp) == STATUS_OK)
                dump_parameter(name, cp);
            }
            else
                printf("%s -> null\n", name);

            if (!strcmp(it->id(), "t0"))
            {
                UTEST_FAIL_MSG("/tree/t0 is still visible after removal");
            }
            if (!strcmp(it->id(), "t1"))
            {
                UTEST_ASSERT(it->remove_branch() == STATUS_OK);
            }
        }

        UTEST_ASSERT(res == STATUS_NOT_FOUND);
        l.verbose(true);
        UTEST_ASSERT(l.check(F_All ^ F_Removed ^ F_Accessed, 0));
        UTEST_ASSERT(l.check(F_Accessed, 7));
        UTEST_ASSERT(l.check(F_Removed, 6));
        UTEST_ASSERT(s.values() == 6);
        UTEST_ASSERT(s.tx_pending() == 5);
        UTEST_ASSERT(s.rx_pending() == 3);

        // Commit changes
        l.clear();
        UTEST_ASSERT(s.commit_all(KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.commit_all(KVT_RX) == STATUS_OK);

        UTEST_ASSERT(l.check(F_All ^ F_Committed, 0));
        UTEST_ASSERT(l.check(F_Committed, 8));
        UTEST_ASSERT(s.tx_pending() == 0);
        UTEST_ASSERT(s.rx_pending() == 0);
    }

    void test_perform_gc(KVTStorage &s, TestListener &l)
    {
        // Print the whole content of the tree
        l.clear();
        l.verbose(false);
        KVTIterator *it = s.enum_branch("/", true);
        UTEST_ASSERT(it != NULL);
        printf("Dumping the whole state of the KVTTree\n");

        status_t res;
        while ((res = it->next()) == STATUS_OK)
        {
            const char *name = it->name();
            const kvt_param_t *cp;

            if (it->exists())
            {
                UTEST_ASSERT(it->get(&cp) == STATUS_OK)
                dump_parameter(name, cp);
            }
            else
                printf("%s -> null\n", name);
        }

        UTEST_ASSERT(res == STATUS_NOT_FOUND);
        l.verbose(true);

        // Perform a touch
        l.clear();
        UTEST_ASSERT(s.touch("/tree/a0", KVT_TX) == STATUS_OK);
        UTEST_ASSERT(s.touch("/tree/b0", KVT_RX) == STATUS_OK);
        UTEST_ASSERT(s.touch("/tree/c0", KVT_RX | KVT_TX) == STATUS_OK);
        UTEST_ASSERT(l.check(F_All ^ F_Changed, 0));
        UTEST_ASSERT(l.check(F_Changed, 4));

        // Perform GC
        l.clear();
        UTEST_ASSERT(s.gc() == STATUS_OK);
        UTEST_ASSERT(l.check(F_All, 0));
        UTEST_ASSERT(s.values() == 6);
        UTEST_ASSERT(s.tx_pending() == 2);
        UTEST_ASSERT(s.tx_pending() == 2);
    }

    void test_unbind(KVTStorage &s, TestListener &l)
    {
        // Unbind listener
        l.clear();
        UTEST_ASSERT(s.unbind(&l) == STATUS_OK);
        UTEST_ASSERT(l.check(F_All ^ F_Detached, 0));
        UTEST_ASSERT(l.check(F_Detached, 1));
        UTEST_ASSERT(s.listeners() == 0);
    }

    UTEST_MAIN
    {
        TestListener l(this);
        KVTStorage s;

        test_bind(s, l);
        test_create_entries(s, l);
        test_replace_entries(s, l);
        test_read_entries(s, l);
        test_unexisting_branch(s, l);
        test_commit_entries(s, l);
        test_existense_entries(s, l);
        test_remove_entries(s, l);
        test_remove_branch(s, l);
        test_perform_gc(s, l);
        test_unbind(s, l);

        // Destroy storage
        s.destroy();
    }
UTEST_END;


