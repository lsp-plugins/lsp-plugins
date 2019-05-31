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
    class TestListener: public KVTListener
    {
        private:
            Test *test;

            void dump_parameter(const char *prefix, const kvt_param_t *param)
            {
                test->printf("%s ", prefix);
                switch (param->type)
                {
                    case KVT_INT32:     test->printf("i32(0x%lx)\n", long(param->i32)); break;
                    case KVT_UINT32:    test->printf("u32(0x%lx)\n", (unsigned long)(param->u32)); break;
                    case KVT_INT64:     test->printf("i64(0x%llx)\n", (long long)(param->i64)); break;
                    case KVT_UINT64:    test->printf("i64(0x%llx)\n", (unsigned long long)(param->u64)); break;
                    case KVT_FLOAT32:   test->printf("f32(%f)\n", param->f32); break;
                    case KVT_FLOAT64:   test->printf("f64(%f)\n", param->f64); break;
                    case KVT_STRING:    test->printf("str(%s)\n", param->str); break;
                    case KVT_BLOB:
                        test->printf("blob(size=%d, type=(%s), data=(", int(param->blob.size), param->blob.ctype);
                        if (param->blob.data != NULL)
                        {
                            const uint8_t *ptr = reinterpret_cast<const uint8_t *>(param->blob.data);
                            for (size_t i=0; i<param->blob.size; ++i)
                            {
                                if (i) test->printf(" %02x", int(ptr[i]));
                                else test->printf("%02x", int(ptr[i]));
                            }
                            test->printf(")\n");
                        }
                        else
                            test->printf("nil))\n");
                        break;
                    default:
                        test->printf("Unexpected parameter type: %d\n", int(param->type));
                        exit(1);
                        break;
                }
            }

            size_t      nAttached;
            size_t      nDetached;
            size_t      nCreated;
            size_t      nRejected;
            size_t      nChanged;
            size_t      nRemoved;
            size_t      nAccessed;
            size_t      nCommitted;
            size_t      nMissed;

        public:
            explicit TestListener(Test *tst): test(tst)
            {
                clear();
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
                test->printf("Attached storage %p\n", storage);
                ++nAttached;
            }

            virtual void detached(KVTStorage *storage)
            {
                test->printf("Detached storage %p\n", storage);
                ++nDetached;
            }

            virtual void created(const char *id, const kvt_param_t *param)
            {
                test->printf("Created parameter %s\n", id);
                dump_parameter("  created   = ", param);
                ++nCreated;
            }

            virtual void rejected(const char *id, const kvt_param_t *rej, const kvt_param_t *curr)
            {
                test->printf("Parameter %s has been rejected\n", id);
                dump_parameter("  rejected  = ", rej);
                dump_parameter("  current   = ", curr);
                ++nRejected;
            }

            virtual void changed(const char *id, const kvt_param_t *oval, const kvt_param_t *nval)
            {
                test->printf("Parameter %s has been changed\n", id);
                dump_parameter("  old       = ", oval);
                dump_parameter("  new       = ", nval);
                ++nChanged;
            }

            virtual void removed(const char *id, const kvt_param_t *param)
            {
                test->printf("Parameter %s has been removed\n", id);
                dump_parameter("  removed   = ", param);
                ++nRemoved;
            }

            virtual void access(const char *id, const kvt_param_t *param)
            {
                test->printf("Parameter %s has been accessed\n", id);
                dump_parameter("  accessed  = ", param);
                ++nAccessed;
            }

            virtual void commit(const char *id, const kvt_param_t *param)
            {
                test->printf("Parameter %s has been committed\n", id);
                dump_parameter("  committed = ", param);
                ++nCommitted;
            }

            virtual void missed(const char *id)
            {
                test->printf("Parameter %s is missing\n", id);
                ++nMissed;
            }
    };


    UTEST_MAIN
    {
        TestListener l(this);
        KVTStorage s;

        const char *ndstr = "Test non-delegated string";
        kvt_blob_t ndblob;
        ndblob.ctype    = "text/html";
        ndblob.data     = "Some non-delegated blob data";
        ndblob.size     = strlen("Some non-delegated blob data") + 1;

        char *dstr      = strdup("Test delegated string");
        kvt_blob_t dblob;
        dblob.ctype     = strdup("application/text");
        dblob.data      = strdup("Some delegated blob data");
        dblob.size      = strlen("Some delegated blob data") + 1;

        kvt_param_t p;
        const kvt_param_t *cp;
        const kvt_blob_t *cb;

        // Bind listener
        UTEST_ASSERT(l.check(F_All, 0));
        UTEST_ASSERT(s.listeners() == 0);
        UTEST_ASSERT(s.bind(&l) == STATUS_OK);
        UTEST_ASSERT(s.bind(&l) == STATUS_ALREADY_BOUND);
        UTEST_ASSERT(l.check(F_All ^ F_Attached, 0));
        UTEST_ASSERT(l.check(F_Attached, 1));
        UTEST_ASSERT(s.listeners() == 1);

        // Create entries
        l.clear();
        UTEST_ASSERT(s.put("/", "fake") == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.put("/fake/", uint64_t(123123)) == STATUS_INVALID_VALUE);

        UTEST_ASSERT(s.put("/value1", uint32_t(1)) == STATUS_OK);
        UTEST_ASSERT(s.put("/value3", uint64_t(3)) == STATUS_OK);
        UTEST_ASSERT(s.put("/value2", int32_t(2)) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/test/value1", float(1.0f)) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/test/value2", double(2.0f)) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/test/value3", strdup("Delegated string"), KVT_DELEGATE) == STATUS_OK);

        UTEST_ASSERT(s.put("/param/i32", int32_t(0x10)) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/u32", uint32_t(0x20)) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/i64", int64_t(0x30)) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/u64", uint64_t(0x40)) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/f32", 440.0f) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/f64", double(48000.0)) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/str", ndstr) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/dstr", dstr, KVT_DELEGATE) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/blob", &ndblob) == STATUS_OK);
        UTEST_ASSERT(s.put("/param/dblob", &dblob, KVT_DELEGATE) == STATUS_OK);

        UTEST_ASSERT(s.put("/some/value", float(M_PI), KVT_COMMIT) == STATUS_OK);
        UTEST_ASSERT(s.put("/some/silent/value", "Math.PI", KVT_COMMIT) == STATUS_OK);

        UTEST_ASSERT(l.check(F_All ^ (F_Created | F_Committed), 0));
        UTEST_ASSERT(l.check(F_Created, 16));
        UTEST_ASSERT(l.check(F_Committed, 2));
        UTEST_ASSERT(s.values() == 18);
        UTEST_ASSERT(s.modified() == 16);
        UTEST_ASSERT(s.nodes() == 22);

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
        UTEST_ASSERT(s.modified() == 16);
        UTEST_ASSERT(s.nodes() == 22);

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
        UTEST_ASSERT(p.f32 == 440.0f);

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
        UTEST_ASSERT(s.modified() == 16);
        UTEST_ASSERT(s.nodes() == 22);

        // Commit
        l.clear();

        UTEST_ASSERT(s.commit("/some/unexisting/value") == STATUS_NOT_FOUND);
        UTEST_ASSERT(s.commit("/value2") == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/i32") == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/u32") == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/i64") == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/u64") == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/f32") == STATUS_OK);
        UTEST_ASSERT(s.commit("/param/f64") == STATUS_OK);

        UTEST_ASSERT(l.check(F_All ^ (F_Committed | F_Missed), 0));
        UTEST_ASSERT(l.check(F_Committed, 7));
        UTEST_ASSERT(l.check(F_Missed, 1));
        UTEST_ASSERT(s.values() == 18);
        UTEST_ASSERT(s.modified() == 9);
        UTEST_ASSERT(s.nodes() == 22);

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
        UTEST_ASSERT(s.modified() == 9);
        UTEST_ASSERT(s.nodes() == 22);

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
        UTEST_ASSERT(p.f32 == 440.0f);

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
        UTEST_ASSERT(s.modified() == 4);

        // Perform GC
        l.clear();
        UTEST_ASSERT(s.gc() == STATUS_OK);
        UTEST_ASSERT(l.check(F_All, 0));
        UTEST_ASSERT(s.values() == 6);
        UTEST_ASSERT(s.modified() == 4);

        // Unbind listener
        l.clear();
        UTEST_ASSERT(s.unbind(&l) == STATUS_OK);
        UTEST_ASSERT(l.check(F_All ^ F_Detached, 0));
        UTEST_ASSERT(l.check(F_Detached, 1));
        UTEST_ASSERT(s.listeners() == 0);

        // Destroy storage
        s.destroy();
    }
UTEST_END;


