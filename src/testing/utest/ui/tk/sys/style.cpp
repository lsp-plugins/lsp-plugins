/*
 * style.cpp
 *
 *  Created on: 4 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <test/utest.h>
#include <test/helpers.h>
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

    class Circle: public IStyleListener
    {
        private:
            test_type_t    *test;
            LSPStyle       *style;
            ui_atom_t       p_r;
            ui_atom_t       p_phi;
            ui_atom_t       p_x;
            ui_atom_t       p_y;

            float           r;
            float           phi;
            float           x;
            float           y;

            enum mask {
                F_R         = 1 << 0,
                F_PHI       = 1 << 1,
                F_X         = 1 << 2,
                F_Y         = 1 << 3
            };

            void sync(size_t mask)
            {
                style->begin();
                if (mask & F_R)
                    style->set_float(p_r, r);
                if (mask & F_PHI)
                    style->set_float(p_phi, phi);
                if (mask & F_X)
                    style->set_float(p_x, x);
                if (mask & F_Y)
                    style->set_float(p_y, y);
                style->end();
            }

        public:
            explicit Circle(test_type_t *t)
            {
                test        = t;
                style       = NULL;
                p_r         = -1;
                p_phi       = -1;
                p_x         = -1;
                p_y         = -1;

                r           = 1.0f;
                phi         = 0.0f;
                x           = 1.0f;
                y           = 0.0f;
            }

            status_t init(LSPStyle *s, ui_atom_t r, ui_atom_t phi, ui_atom_t x, ui_atom_t y)
            {
                style       = s;
                p_r         = r;
                p_phi       = phi;
                p_x         = x;
                p_y         = y;

                status_t res = STATUS_OK;

                if (res == STATUS_OK)
                    res = s->bind(r, PT_FLOAT, this);
                if (res == STATUS_OK)
                    res = s->bind(phi, PT_FLOAT, this);
                if (res == STATUS_OK)
                    res = s->bind(x, PT_FLOAT, this);
                if (res == STATUS_OK)
                    res = s->bind(y, PT_FLOAT, this);

                return res;
            }

            virtual void notify(ui_atom_t property)
            {
                float v;
                if (property == p_r)
                {
                    style->get_float(p_r, &v);
                    set_r(v);
                }
                else if (property == p_phi)
                {
                    style->get_float(p_phi, &v);
                    set_phi(v);
                }
                else if (property == p_x)
                {
                    style->get_float(p_x, &v);
                    set_x(v);
                }
                else if (property == p_y)
                {
                    style->get_float(p_y, &v);
                    set_y(v);
                }
            }

            inline float get_r() const { return r; }
            inline float get_phi() const { return phi; }

            void set_r(float vr)
            {
                if (r == vr)
                    return;

                r = vr;
                style->get_float(p_phi, &phi);
                x = r * cosf(phi);
                y = r * sinf(phi);
                sync(F_R | F_X | F_Y);
            }

            void set_phi(float vphi)
            {
                if (phi == vphi)
                    return;

                style->get_float(p_r, &r);
                phi = vphi;
                x = r * cosf(phi);
                y = r * sinf(phi);
                sync(F_PHI | F_X | F_Y);
            }

            void set_x(float vx)
            {
                if (x == vx)
                    return;

                x       = vx;
                style->get_float(p_y, &y);
                r       = sqrtf(x*x + y*y);
                phi     = (y >= 0.0f) ? acos(x / r) : M_PI + acos(-x / r);

                sync(F_R | F_PHI | F_X);
            }

            void set_y(float vy)
            {
                if (y == vy)
                    return;

                style->get_float(p_x, &x);
                y       = vy;
                r       = sqrtf(x*x + y*y);
                phi     = (y >= 0.0f) ? acos(x / r) : M_PI + acos(-x / r);

                sync(F_R | F_PHI | F_Y);
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
        printf("Building root style...\n");
        UTEST_ASSERT(s.set_int(atom("int_value"), 10) == STATUS_OK);
        UTEST_ASSERT(s.set_int(atom("dfl_int_value"), 123) == STATUS_OK);
        UTEST_ASSERT(s.set_int(atom("count"), 42) == STATUS_OK);

        UTEST_ASSERT(s.set_bool(atom("bool_value"), false) == STATUS_OK);
        UTEST_ASSERT(s.set_bool(atom("dfl_bool_value"), false) == STATUS_OK);

        UTEST_ASSERT(s.set_string(atom("string_value"), "Some string value") == STATUS_OK);
        UTEST_ASSERT(s.set_string(atom("dfl_string_value"), "Default value") == STATUS_OK);

        UTEST_ASSERT(s.set_float(atom("circle.r"), 2.0f) == STATUS_OK);
        UTEST_ASSERT(s.set_float(atom("circle.phi"), M_PI) == STATUS_OK);
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
        printf("Binding listeners...\n");
        UTEST_ASSERT(s1.bind(var1, PT_INT, &l1) == STATUS_OK);
        UTEST_ASSERT(s2.bind(var1, PT_INT, &l2) == STATUS_OK);
        UTEST_ASSERT(s3.bind(var1, PT_INT, &l3) == STATUS_OK);
        UTEST_ASSERT(s4.bind(var1, PT_FLOAT, &l4) == STATUS_OK);
        UTEST_ASSERT(s3.bind(var2, PT_INT, &l3) == STATUS_OK);
        UTEST_ASSERT(s4.bind(var2, PT_INT, &l4) == STATUS_OK);

        // Initialize
        printf("Initializing...\n");
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
        printf("Setting inheritance...\n");
        UTEST_ASSERT(s1.add_parent(&s1) == STATUS_BAD_HIERARCHY);
        UTEST_ASSERT(s1.add_parent(&s) == STATUS_OK);
        UTEST_ASSERT(s1.add_parent(&s) == STATUS_ALREADY_EXISTS);
        UTEST_ASSERT(s3.add_parent(&s1) == STATUS_OK);
        UTEST_ASSERT(s.add_child(&s) == STATUS_BAD_HIERARCHY);
        UTEST_ASSERT(s.add_child(&s2) == STATUS_OK);
        UTEST_ASSERT(s.add_child(&s2) == STATUS_ALREADY_EXISTS);
        UTEST_ASSERT(s2.add_child(&s4) == STATUS_OK);

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
        printf("Checking relations...\n");
        UTEST_ASSERT(s1.has_parent(&s));
        UTEST_ASSERT(s2.has_parent(&s));
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
        printf("Changing root property 'count'...\n");
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
        printf("Changing root property 'int_value'...\n");
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
        UTEST_ASSERT(s1.remove_parent(&s) == STATUS_OK);
        UTEST_ASSERT(!s.has_child(&s1, false));
        UTEST_ASSERT(!s.has_child(&s1, true));
        UTEST_ASSERT(!s1.has_parent(&s, false));
        UTEST_ASSERT(!s1.has_parent(&s, true));
        UTEST_ASSERT(s1.parents() == 0);

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
        UTEST_ASSERT(s.remove_child(&s2) == STATUS_OK);
        UTEST_ASSERT(!s.has_child(&s2, false));
        UTEST_ASSERT(!s.has_child(&s2, true));
        UTEST_ASSERT(!s2.has_parent(&s, false));
        UTEST_ASSERT(!s2.has_parent(&s, true));
        UTEST_ASSERT(s2.parents() == 0);

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
        printf("Destroying styles s1 and s2...\n");
        s1.destroy();
        s2.destroy();
        UTEST_ASSERT(!s1.has_child(&s3, true));
        UTEST_ASSERT(!s2.has_child(&s4, true));
        UTEST_ASSERT(!s.has_child(&s1, true));
        UTEST_ASSERT(!s.has_child(&s2, true));
        UTEST_ASSERT(!s.has_child(&s3, true));
        UTEST_ASSERT(!s.has_child(&s4, true));

        UTEST_ASSERT(l1.cl_get(var1) == 0);
        UTEST_ASSERT(l2.cl_get(var1) == 0);
        UTEST_ASSERT(l3.cl_get(var1) == 1);
        UTEST_ASSERT(l4.cl_get(var1) == 0);
        UTEST_ASSERT(l1.cl_get(var2) == 0);
        UTEST_ASSERT(l2.cl_get(var2) == 0);
    }

    void test_function(LSPStyle &s)
    {
        ui_atom_t r = atom("circle.r");
        ui_atom_t phi = atom("circle.phi");
        ui_atom_t x = atom("circle.x");
        ui_atom_t y = atom("circle.y");
        float v;

        printf("Initializing circle...\n");
        Circle circle(this);
        UTEST_ASSERT(circle.init(&s, r, phi, x, y) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(circle.get_r(), 2.0f));
        UTEST_ASSERT(float_equals_relative(circle.get_phi(), M_PI));

        UTEST_ASSERT(s.get_float(r, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 2.0f));
        UTEST_ASSERT(s.get_float(phi, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, M_PI));
        UTEST_ASSERT(s.get_float(x, &v) == STATUS_OK); // Affected by init()
        UTEST_ASSERT(float_equals_relative(v, -2.0f));
        UTEST_ASSERT(s.get_float(y, &v) == STATUS_OK); // Affected by init()
        UTEST_ASSERT(float_equals_relative(v, 0.0f));

        printf("Testing circle.set_phi...\n");
        circle.set_phi(0.0f);
        UTEST_ASSERT(s.get_float(r, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 2.0f));
        UTEST_ASSERT(s.get_float(phi, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 0.0f));
        UTEST_ASSERT(s.get_float(x, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 2.0f));
        UTEST_ASSERT(s.get_float(y, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 0.0f));

        printf("Testing circle.set_y...\n");
        circle.set_y(2.0f);
        UTEST_ASSERT(s.get_float(r, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 2.0f * M_SQRT2));
        UTEST_ASSERT(s.get_float(phi, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, M_PI * 0.25f));
        UTEST_ASSERT(s.get_float(x, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 2.0f));
        UTEST_ASSERT(s.get_float(y, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, 2.0f));

        printf("Setting x...\n");
        UTEST_ASSERT(s.set_float(x, -2.0f) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(circle.get_r(), 2.0f * M_SQRT2));
        UTEST_ASSERT(float_equals_relative(circle.get_phi(), 0.75f * M_PI));

        printf("Setting r...\n");
        UTEST_ASSERT(s.set_float(r, 2.0f) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(circle.get_r(), 2.0f));
        UTEST_ASSERT(float_equals_relative(circle.get_phi(), 0.75f * M_PI));
        UTEST_ASSERT(s.get_float(x, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, M_SQRT2));
        UTEST_ASSERT(s.get_float(y, &v) == STATUS_OK);
        UTEST_ASSERT(float_equals_relative(v, M_SQRT2));
    }

    void test_multiple_parents()
    {
        LSPStyle p1, p2;
        LSPStyle c1, c2;
        ChangeListener l1(this, "c1"), l2(this, "c2");

        ui_atom_t v1 = atom("value1");
        ui_atom_t v2 = atom("value2");
        ssize_t v;

        printf("Initializing style inheritance...\n");
        UTEST_ASSERT(p1.init() == STATUS_OK);
        UTEST_ASSERT(p2.init() == STATUS_OK);
        UTEST_ASSERT(c1.init() == STATUS_OK);
        UTEST_ASSERT(c2.init() == STATUS_OK);

        UTEST_ASSERT(c1.bind_int(v1, &l1) == STATUS_OK);
        UTEST_ASSERT(c1.bind_int(v2, &l1) == STATUS_OK);
        UTEST_ASSERT(c2.bind_int(v1, &l2) == STATUS_OK);
        UTEST_ASSERT(c2.bind_int(v2, &l2) == STATUS_OK);

        UTEST_ASSERT(c1.add_parent(&p1) == STATUS_OK);
        UTEST_ASSERT(c1.add_parent(&p2) == STATUS_OK);
        UTEST_ASSERT(c2.add_parent(&p1) == STATUS_OK);

        printf("Setting values...\n");
        UTEST_ASSERT(p1.set_int(v1, 1) == STATUS_OK);
        UTEST_ASSERT(p1.set_int(v2, 2) == STATUS_OK);
        UTEST_ASSERT(p2.set_int(v1, 100) == STATUS_OK);
        UTEST_ASSERT(p2.set_int(v2, 200) == STATUS_OK);

        UTEST_ASSERT(l1.cl_get(v1) == 3);
        UTEST_ASSERT(l1.cl_get(v2) == 3);
        UTEST_ASSERT(l2.cl_get(v1) == 2);
        UTEST_ASSERT(l2.cl_get(v2) == 2);

        UTEST_ASSERT(c1.get_int(v1, &v) == STATUS_OK);
        UTEST_ASSERT(v == 100);
        UTEST_ASSERT(c1.get_int(v2, &v) == STATUS_OK);
        UTEST_ASSERT(v == 200);
        UTEST_ASSERT(c2.get_int(v1, &v) == STATUS_OK);
        UTEST_ASSERT(v == 1);
        UTEST_ASSERT(c2.get_int(v2, &v) == STATUS_OK);
        UTEST_ASSERT(v == 2);

        printf("Overriding values...\n");
        UTEST_ASSERT(p1.set_int(v1, 10) == STATUS_OK);
        UTEST_ASSERT(p1.set_int(v2, 20) == STATUS_OK);

        UTEST_ASSERT(l1.cl_get(v1) == 0);
        UTEST_ASSERT(l1.cl_get(v2) == 0);
        UTEST_ASSERT(l2.cl_get(v1) == 1);
        UTEST_ASSERT(l2.cl_get(v2) == 1);

        UTEST_ASSERT(c1.get_int(v1, &v) == STATUS_OK);
        UTEST_ASSERT(v == 100);
        UTEST_ASSERT(c1.get_int(v2, &v) == STATUS_OK);
        UTEST_ASSERT(v == 200);
        UTEST_ASSERT(c2.get_int(v1, &v) == STATUS_OK);
        UTEST_ASSERT(v == 10);
        UTEST_ASSERT(c2.get_int(v2, &v) == STATUS_OK);
        UTEST_ASSERT(v == 20);
    }

    UTEST_MAIN
    {
        LSPStyle root;
        init_style(root);
        test_binding(root);
        test_function(root);
        test_multiple_parents();
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


