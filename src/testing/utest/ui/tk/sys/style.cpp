/*
 * style.cpp
 *
 *  Created on: 4 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <test/utest.h>
#include <data/cvector.h>
#include <data/cstorage.h>

using namespace lsp::tk;

UTEST_BEGIN("ui.tk.sys", style)

    class ChangeListener: public IStyleListener
    {
        private:
            cstorage<size_t>    counters;
            test_type_t        *test;
            const char         *id;

        public:
            explicit ChangeListener(test_type_t *test, const char *id)
            {
                this->test = test;
                this->id = id;
            }

            virtual ~ChangeListener()
            {
            }

        public:
            virtual void notify(ui_atom_t property)
            {
                while (ssize_t(counters.size()) <= property)
                    counters.add(size_t(0));
                size_t *cnt = counters.get(property);
                ++(*cnt);
                test->printf("%s[%d]++ -> %d\n", id, int(property), int(*cnt));
            }

            ssize_t get(ui_atom_t property)
            {
                size_t *cnt = counters.get(property);
                return (cnt == NULL) ? -1 : *cnt;
            }

            ssize_t cl_get(ui_atom_t property)
            {
                size_t *cnt = counters.get(property);
                ssize_t val = (cnt == NULL) ? -1 : *cnt;
                *cnt = 0;
                test->printf("%s[%d] = %d\n", id, int(property), int(*cnt));
                return val;
            }
    };

    cvector<char> atoms;

    ui_atom_t atom(const char *name)
    {
        // Find the atom
        for (size_t i=0, n=atoms.size(); i<n; ++i)
        {
            char *aname = atoms.at(i);
            if ((aname != NULL) && (!::strcmp(aname, name)))
                return i;
        }

        // Create new atom
        int idx = atoms.size();
        char *aname = ::strdup(name);
        UTEST_ASSERT(aname != NULL);
        UTEST_ASSERT(atoms.add(aname));

        return idx;
    }

    void init_style(LSPStyle &s)
    {
        union
        {
            ssize_t     iValue;
            float       fValue;
            bool        bValue;
            const char *sValue;
        } v;

        UTEST_ASSERT(s.init() == STATUS_OK);

        // Set initial values
        UTEST_ASSERT(s.set_int(atom("int_value"), 10) == STATUS_OK);
        UTEST_ASSERT(s.set_int(atom("dfl_int_value"), 123) == STATUS_OK);
        UTEST_ASSERT(s.set_int(atom("count"), 42) == STATUS_OK);

        UTEST_ASSERT(s.set_bool(atom("bool_value"), false) == STATUS_OK);
        UTEST_ASSERT(s.set_bool(atom("dfl_bool_value"), false) == STATUS_OK);

        UTEST_ASSERT(s.set_string(atom("string_value"), "Some string value") == STATUS_OK);
        UTEST_ASSERT(s.set_string(atom("dfl_string_value"), "Default value") == STATUS_OK);

        UTEST_ASSERT(s.set_float(atom("circle.rho"), 1.0f) == STATUS_OK);
        UTEST_ASSERT(s.set_float(atom("circle.theta"), 0.0f) == STATUS_OK);
        UTEST_ASSERT(s.set_float(atom("circle.x"), 0.5f) == STATUS_OK);
        UTEST_ASSERT(s.set_float(atom("circle.y"), 0.5f) == STATUS_OK);
        UTEST_ASSERT(s.set_float(atom("dfl_float_value"), 440.0f) == STATUS_OK);

        UTEST_ASSERT(s.listeners() == 0);
        UTEST_ASSERT(s.properties() == 12);

        // Override initial values
        UTEST_ASSERT(s.set_int(atom("dfl_int_value"), 321) == STATUS_OK);
        UTEST_ASSERT(s.get_int(atom("dfl_int_value"), &v.iValue) == STATUS_OK);
        UTEST_ASSERT(v.iValue == 321);

        UTEST_ASSERT(s.set_bool(atom("dfl_bool_value"), true) == STATUS_OK);
        UTEST_ASSERT(s.get_bool(atom("dfl_bool_value"), &v.bValue) == STATUS_OK);
        UTEST_ASSERT(v.bValue == true);

        UTEST_ASSERT(s.set_string(atom("dfl_string_value"), "Replacement string") == STATUS_OK);
        UTEST_ASSERT(s.get_string(atom("dfl_string_value"), &v.sValue) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sValue, "Replacement string") == 0);

        UTEST_ASSERT(s.set_float(atom("dfl_float_value"), 48000.0) == STATUS_OK);
        UTEST_ASSERT(s.get_float(atom("dfl_float_value"), &v.fValue) == STATUS_OK);
        UTEST_ASSERT(v.fValue == 48000.0);

        // Obtain unexisting and bad-typed values
        UTEST_ASSERT(s.get_int(atom("unexisting_value"), &v.iValue) == STATUS_OK);
        UTEST_ASSERT(v.iValue == 0);
        UTEST_ASSERT(s.get_bool(atom("unexisting_value"), &v.bValue) == STATUS_OK);
        UTEST_ASSERT(v.bValue == false);
        UTEST_ASSERT(s.get_float(atom("unexisting_value"), &v.fValue) == STATUS_OK);
        UTEST_ASSERT(v.fValue == 0.0f);
        UTEST_ASSERT(s.get_string(atom("unexisting_value"), &v.sValue) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sValue, "") == 0);

        UTEST_ASSERT(s.get_int(atom("dfl_string_value"), &v.iValue) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s.get_bool(atom("dfl_int_value"), &v.bValue) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s.get_float(atom("dfl_int_value"), &v.fValue) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s.get_string(atom("dfl_int_value"), &v.sValue) == STATUS_BAD_TYPE);

        // Set to defaults
        UTEST_ASSERT(s.set_default(atom("dfl_int_value")) == STATUS_OK);
        UTEST_ASSERT(s.get_int(atom("dfl_int_value"), &v.iValue) == STATUS_OK);
        UTEST_ASSERT(v.iValue == 0);

        UTEST_ASSERT(s.set_default(atom("dfl_bool_value")) == STATUS_OK);
        UTEST_ASSERT(s.get_bool(atom("dfl_bool_value"), &v.bValue) == STATUS_OK);
        UTEST_ASSERT(v.bValue == false);

        UTEST_ASSERT(s.set_default(atom("dfl_float_value")) == STATUS_OK);
        UTEST_ASSERT(s.get_float(atom("dfl_float_value"), &v.fValue) == STATUS_OK);
        UTEST_ASSERT(v.fValue == 0.0f);

        UTEST_ASSERT(s.set_default(atom("dfl_string_value")) == STATUS_OK);
        UTEST_ASSERT(s.get_string(atom("dfl_string_value"), &v.sValue) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sValue, "") == 0);

        UTEST_ASSERT(s.listeners() == 0);
        UTEST_ASSERT(s.properties() == 12);
    }

    void test_binding(LSPStyle &s)
    {
        ChangeListener l1(this, "s1"), l2(this, "s2"), l3(this, "s3"), l4(this, "s4");
        LSPStyle s1, s2, s3, s4;
        ssize_t iv = -1;

        ui_atom_t var1 = atom("count");
        ui_atom_t var2 = atom("int_value");

        //  Style inheritance:
        //  s <- s1 <- s3
        //  s <- s2 <- s4

        UTEST_ASSERT(s1.init() == STATUS_OK);
        UTEST_ASSERT(s2.init() == STATUS_OK);
        UTEST_ASSERT(s3.init() == STATUS_OK);
        UTEST_ASSERT(s4.init() == STATUS_OK);

        // Bind listeners
        UTEST_ASSERT(s1.bind(var1, PT_INT, &l1) == STATUS_OK);
        UTEST_ASSERT(s2.bind(var1, PT_INT, &l2) == STATUS_OK);
        UTEST_ASSERT(s3.bind(var1, PT_INT, &l3) == STATUS_OK);
        UTEST_ASSERT(s4.bind(var1, PT_FLOAT, &l4) == STATUS_OK);
        UTEST_ASSERT(s3.bind(var2, PT_INT, &l3) == STATUS_OK);
        UTEST_ASSERT(s4.bind(var2, PT_INT, &l4) == STATUS_OK);

        // Initialize
        UTEST_ASSERT(s1.set_int(var1, 256) == STATUS_OK); // Should be notified only l1
        UTEST_ASSERT(s4.set_int(var2, 99) == STATUS_OK); // Should be notified only l4
        UTEST_ASSERT(l1.cl_get(var1) == 2);
        UTEST_ASSERT(l2.cl_get(var1) == 1);
        UTEST_ASSERT(l3.cl_get(var1) == 1);
        UTEST_ASSERT(l4.cl_get(var1) == 1);
        UTEST_ASSERT(l3.cl_get(var2) == 1);
        UTEST_ASSERT(l4.cl_get(var2) == 2);

        UTEST_ASSERT(s1.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s2.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 0);
        UTEST_ASSERT(s3.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 0);
        UTEST_ASSERT(s4.get_int(var1, &iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s3.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 0);
        UTEST_ASSERT(s4.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 99);

        // Set inheritance
        UTEST_ASSERT(s1.set_parent(&s1) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(s1.set_parent(&s) == STATUS_OK);
        UTEST_ASSERT(s1.set_parent(&s) == STATUS_OK);
        UTEST_ASSERT(s2.set_parent(&s) == STATUS_OK);
        UTEST_ASSERT(s3.set_parent(&s1) == STATUS_OK);
        UTEST_ASSERT(s4.set_parent(&s2) == STATUS_OK);

        UTEST_ASSERT(l1.cl_get(var1) == 0);
        UTEST_ASSERT(l2.cl_get(var1) == 1);
        UTEST_ASSERT(l3.cl_get(var1) == 1);
        UTEST_ASSERT(l4.cl_get(var1) == 0);
        UTEST_ASSERT(l3.cl_get(var2) == 1);
        UTEST_ASSERT(l4.cl_get(var2) == 0);

        UTEST_ASSERT(s1.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s2.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 42);
        UTEST_ASSERT(s3.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s4.get_int(var1, &iv) == STATUS_BAD_TYPE);

        UTEST_ASSERT(s1.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 10);
        UTEST_ASSERT(s2.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 10);
        UTEST_ASSERT(s3.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 10);
        UTEST_ASSERT(s4.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 99);

        // Check relations
        UTEST_ASSERT(s1.parent() == &s);
        UTEST_ASSERT(s2.parent() == &s);
        UTEST_ASSERT(!s3.has_parent(&s, false));
        UTEST_ASSERT(s3.has_parent(&s, true));
        UTEST_ASSERT(!s4.has_parent(&s, false));
        UTEST_ASSERT(s4.has_parent(&s, true));

        UTEST_ASSERT(s.has_child(&s1, false));
        UTEST_ASSERT(s.has_child(&s2, false));
        UTEST_ASSERT(!s.has_child(&s3, false));
        UTEST_ASSERT(s.has_child(&s3, true));
        UTEST_ASSERT(!s.has_child(&s4, false));
        UTEST_ASSERT(s.has_child(&s4, true));
        UTEST_ASSERT(!s3.has_parent(&s4, true));
        UTEST_ASSERT(!s4.has_child(&s3, true));

        // Change root property
        UTEST_ASSERT(s.set_int(var1, 256) == STATUS_OK);
        UTEST_ASSERT(l1.cl_get(var1) == 0);
        UTEST_ASSERT(l2.cl_get(var1) == 1);
        UTEST_ASSERT(l3.cl_get(var1) == 0);
        UTEST_ASSERT(l4.cl_get(var1) == 0);

        UTEST_ASSERT(s1.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s2.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s3.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s4.get_int(var1, &iv) == STATUS_BAD_TYPE);

        // Change another property
        UTEST_ASSERT(s.set_int(var2, 199) == STATUS_OK);
        UTEST_ASSERT(l1.cl_get(var2) == 0);
        UTEST_ASSERT(l2.cl_get(var2) == 0);
        UTEST_ASSERT(l3.cl_get(var2) == 1);
        UTEST_ASSERT(l4.cl_get(var2) == 0);

        UTEST_ASSERT(s1.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 199);
        UTEST_ASSERT(s2.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 199);
        UTEST_ASSERT(s3.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 199);
        UTEST_ASSERT(s4.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 99);

        // Unlink style s1
        printf("Unlinking style s1...\n");
        UTEST_ASSERT(s1.set_parent(NULL) == STATUS_OK);
        UTEST_ASSERT(!s.has_child(&s1, false));
        UTEST_ASSERT(!s.has_child(&s1, true));
        UTEST_ASSERT(!s1.has_parent(&s, false));
        UTEST_ASSERT(!s1.has_parent(&s, true));
        UTEST_ASSERT(s1.parent() == NULL);

        UTEST_ASSERT(l1.cl_get(var1) == 0);
        UTEST_ASSERT(l3.cl_get(var1) == 0);
        UTEST_ASSERT(l1.cl_get(var2) == 0);
        UTEST_ASSERT(l3.cl_get(var2) == 1);

        UTEST_ASSERT(s1.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s3.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 256);
        UTEST_ASSERT(s1.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 0);
        UTEST_ASSERT(s3.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 0);

        // Unlink style s2
        printf("Unlinking style s2...\n");
        UTEST_ASSERT(s.remove(&s2) == STATUS_OK);
        UTEST_ASSERT(!s.has_child(&s2, false));
        UTEST_ASSERT(!s.has_child(&s2, true));
        UTEST_ASSERT(!s2.has_parent(&s, false));
        UTEST_ASSERT(!s2.has_parent(&s, true));
        UTEST_ASSERT(s2.parent() == NULL);

        UTEST_ASSERT(l2.cl_get(var1) == 1);
        UTEST_ASSERT(l4.cl_get(var1) == 0);
        UTEST_ASSERT(l1.cl_get(var2) == 0);
        UTEST_ASSERT(l2.cl_get(var2) == 0);

        UTEST_ASSERT(s2.get_int(var1, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 0);
        UTEST_ASSERT(s4.get_int(var1, &iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(s2.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 0);
        UTEST_ASSERT(s4.get_int(var2, &iv) == STATUS_OK);
        UTEST_ASSERT(iv == 99);

        // Destroy styles
        s3.destroy();
        s4.destroy();
        UTEST_ASSERT(!s1.has_child(&s3, true));
        UTEST_ASSERT(!s2.has_child(&s4, true));
        UTEST_ASSERT(l1.cl_get(var1) == 0);
        UTEST_ASSERT(l2.cl_get(var1) == 0);
        UTEST_ASSERT(l1.cl_get(var2) == 0);
        UTEST_ASSERT(l2.cl_get(var2) == 0);
    }

    UTEST_MAIN
    {
        LSPStyle root;
        init_style(root);
        test_binding(root);
    }

    UTEST_DESTROY
    {
        for (size_t i=0, n=atoms.size(); i<n; ++i)
        {
            char *aname = atoms.at(i);
            if (aname != NULL)
                ::free(aname);
        }
        atoms.flush();
    }

UTEST_END


