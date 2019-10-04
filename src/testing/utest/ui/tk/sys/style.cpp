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

        public:
            virtual ~ChangeListener()
            {
            }

        public:
            virtual void notify(ui_atom_t property)
            {
                while (ssize_t(counters.size()) < property)
                    counters.add(size_t(0));
                counters.add(size_t(1));
            }

            ssize_t get(ui_atom_t property)
            {
                size_t *cnt = counters.get(property);
                return (cnt == NULL) ? -1 : *cnt;
            }

            ssize_t get_and_clear(ui_atom_t property)
            {
                size_t *cnt = counters.get(property);
                ssize_t val = (cnt == NULL) ? -1 : *cnt;
                *cnt = 0;
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
        UTEST_ASSERT(s.get_int(atom("unexisting_value"), &v.iValue) == STATUS_NOT_FOUND);
        UTEST_ASSERT(s.get_bool(atom("unexisting_value"), &v.bValue) == STATUS_NOT_FOUND);
        UTEST_ASSERT(s.get_float(atom("unexisting_value"), &v.fValue) == STATUS_NOT_FOUND);
        UTEST_ASSERT(s.get_string(atom("unexisting_value"), &v.sValue) == STATUS_NOT_FOUND);

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

    void test_changes(LSPStyle &s)
    {
    }

    UTEST_MAIN
    {
        LSPStyle root;
        init_style(root);
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


