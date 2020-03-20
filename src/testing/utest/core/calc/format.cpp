/*
 * format.cpp
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/calc/format.h>

using namespace lsp;
using namespace lsp::calc;

#define OK(arg)     UTEST_ASSERT((arg) == STATUS_OK)

UTEST_BEGIN("core.calc", format)

    void test_simple(Parameters *p)
    {
        LSPString out;

        OK(format(&out, "123", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("123"));

        OK(format(&out, "{}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("100500"));

        OK(format(&out, "{} {} {}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("100500 440.000000 true"));

        OK(format(&out, "{@bool} {@int} {@float} {@strA} {@strB} {@null} {@undef} {@nan} {@pinf} {@ninf}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("true 100500 440.000000 string CaMeL <null> <undef> nan inf -inf"));

        OK(format(&out, "{[0]} {[1]} {[2]} {[3]} {[4]} {[5]} {[6]} {[7]} {[8]} {[9]}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("100500 440.000000 true string CaMeL nan inf -inf <null> <undef>"));

        OK(format(&out, "\\{[0]} {} {[1]} {} {[2]}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("{[0]} 100500 440.000000 440.000000 true"));

        OK(format(&out, "{\\} {@1} {[int]} {[]} {^} {$} {>>} {||} {@int$} {^0$1[a]} {%Z} {[", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("{\\} {@1} {[int]} {[]} {^} {$} {>>} {||} {@int$} {^0$1[a]} {%Z} {["));

        OK(format(&out, "{@a@b} {[1][2]} {^0^9} {$0$9} {>|>|} {%d%d} {%.f}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("{@a@b} {[1][2]} {^0^9} {$0$9} {>|>|} {%d%d} {%.f}"));
    }

    void test_format(Parameters *p)
    {
        LSPString out;

        OK(format(&out, "{@int%d} {@neg%d} {@int%+d} {@neg%+d} {@int%b} {@int%o} {@hex%x} {@hex%X}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("100500 -1234 +100500 -1234 11000100010010100 304224 c0de C0DE"));

        OK(format(&out, "{@null%d} {@undef%d} {@null%b} {@undef%b} {@null%o} {@undef%o} {@null%x} {@undef%x}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("<null> <undef> <null> <undef> <null> <undef> <null> <undef>"));

        OK(format(&out, "{@float%f} {@float%.2f} {@float%.0f} {@float%+.2f} {@nan%f} {@pinf%f} {@ninf%f} {@pinf%+f} {@null%f} {@undef%f}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("440.000000 440.00 440 +440.00 nan inf -inf +inf <null> <undef>"));

        OK(format(&out, "{@bool%l} {@bool%L} {@bool%Ll} {@bool%lL} {@null%l} {@undef%l}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("true TRUE True tRUE <null> <undef>"));

        OK(format(&out, "{@strA%s} {@strA%t} {@strA%T} {@strA%Tt} {@strA%tT} {@null%s} {@null%t} {@undef%s} {@undef%t}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("string string STRING String sTRING <null> <null> <undef> <undef>"));

        OK(format(&out, "{@strB%s} {@strB%t} {@strB%T} {@strB%Tt} {@strB%tT}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("CaMeL camel CAMEL Camel cAMEL"));

        OK(format(&out, "{@strC%s} {@strC%t} {@strC%T} {@strC%Tt} {@strC%tT}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("    "));
    }

    void test_padding(Parameters *p)
    {
        LSPString out;

        OK(format(&out, "{>@strA^0%10s$1} {@strA^0%10s$1<} {|@strA^0%10s$1} {|>@strA^0%10s$1} {<|@strA^0%10s$1} {>|@strA^0%10s$1} {|<@strA^0%10s$1}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("0000string string1111 00string11 000string1 0string111 00string11 00string11"));

        OK(format(&out, "{>@strB^0%10s$1} {@strB^0%10s$1<} {|@strB^0%10s$1} {|>@strB^0%10s$1} {<|@strB^0%10s$1} {>|@strB^0%10s$1} {|<@strB^0%10s$1}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("00000CaMeL CaMeL11111 00CaMeL111 0000CaMeL1 0CaMeL1111 00CaMeL111 000CaMeL11"));

        OK(format(&out, "{>@strC^0%10s$1} {@strC^0%10s$1<} {|@strC^0%10s$1} {|>@strC^0%10s$1} {<|@strC^0%10s$1} {>|@strC^0%10s$1} {|<@strC^0%10s$1}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("0000000000 1111111111 0000011111 0000000011 0011111111 0000011111 0000011111"));

        OK(format(&out, "{>@null^0%10s$1} {@null^0%10s$1<} {|@null^0%10s$1} {|>@null^0%10s$1} {<|@null^0%10s$1} {>|@null^0%10s$1} {|<@null^0%10s$1}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("0000<null> <null>1111 00<null>11 000<null>1 0<null>111 00<null>11 00<null>11"));

        OK(format(&out, "{>@null%10s} {@null%10s<} {|@null%10s} {|>@null%10s} {<|@null%10s} {>|@null%10s} {|<@null%10s}", p));
        printf("out = %s\n", out.get_utf8());
        UTEST_ASSERT(out.equals_ascii("    <null> <null>       <null>      <null>   <null>      <null>     <null>  "));
    }

    UTEST_MAIN
    {
        Parameters p;
        OK(p.add_int("int", 100500));
        OK(p.add_float("float", 440.0));
        OK(p.add_bool("bool", true));
        OK(p.add_cstring("strA", "string"));
        OK(p.add_cstring("strB", "CaMeL"));
        OK(p.add_float("nan", NAN));
        OK(p.add_float("pinf", +INFINITY));
        OK(p.add_float("ninf", -INFINITY));
        OK(p.add_null("null"));
        OK(p.add_undef("undef"));
        OK(p.add_int("neg", -1234));
        OK(p.add_int("hex", 0xc0de));
        OK(p.add_cstring("strC", ""));

        printf("\nTesting simple types...\n");
        test_simple(&p);

        printf("\nTesting format specifiers...\n");
        test_format(&p);

        printf("\nTesting padding...\n");
        test_padding(&p);
    }

UTEST_END

