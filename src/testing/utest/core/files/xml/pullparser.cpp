/*
 * pullparser.cpp
 *
 *  Created on: 28 окт. 2019 г.
 *      Author: sadko
 */

#include <locale.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <core/stdlib/math.h>
#include <core/files/xml/PullParser.h>

using namespace lsp;
using namespace lsp::xml;

UTEST_BEGIN("core.files.xml", pullparser)

    void check_document_start(PullParser &p, xml_version_t xversion, const char *version, const char *encoding, bool standalone)
    {
        printf("  START_DOCUMENT: xml_version=%d, version=%s, encoding=%s, standalone=%s\n",
            int(xversion), version, encoding, (standalone) ? "yes" : "no");

        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_START_DOCUMENT);
        UTEST_ASSERT(p.xml_version() == xversion);
        UTEST_ASSERT((version == NULL) ? (p.version() == NULL) : p.version()->equals_ascii(version));
        UTEST_ASSERT((encoding == NULL) ? (p.encoding() == NULL) : p.encoding()->equals_ascii(encoding));
        UTEST_ASSERT(p.is_standalone() == standalone);
        UTEST_ASSERT(p.name() == NULL);
        UTEST_ASSERT(p.value() == NULL);
    }

    void check_doctype(PullParser &p, const char *type, const char *pub, const char *sys)
    {
        printf("  DOCTYPE: type=%s, pub=%s, sys=%s\n", type, pub, sys);
        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_DTD);
        UTEST_ASSERT((p.doctype() != NULL) && (p.doctype()->equals_ascii(type)));
        UTEST_ASSERT((p.pub_literal() != NULL) && (p.pub_literal()->equals_ascii(pub)));
        UTEST_ASSERT((p.sys_literal() != NULL) && (p.sys_literal()->equals_ascii(sys)));
    }

    void check_document_end(PullParser &p)
    {
        printf("  END_DOCUMENT\n");
        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_END_DOCUMENT);
        UTEST_ASSERT(p.name() == NULL);
        UTEST_ASSERT(p.value() == NULL);
    }

    void check_comment(PullParser &p, const char *comment)
    {
        printf("  COMMENT: %s\n", comment);
        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_COMMENT);
        UTEST_ASSERT(p.name() == NULL);
        UTEST_ASSERT(p.value()->equals_ascii(comment));
    }

    status_t check_references(PullParser &p)
    {
        LSPString tmp;
        while (true)
        {
            status_t token = p.read_next();
            if (token != XT_ENTITY_RESOLVE)
                return token;

            UTEST_ASSERT(tmp.set(p.name()));
            UTEST_ASSERT(p.value() == NULL);
            tmp.toupper();
            printf("  REFERENCE_RESOLVE: %s -> %s\n", p.name()->get_utf8(), tmp.get_utf8());
            UTEST_ASSERT(p.set_value(&tmp) == STATUS_OK);
        }
    }

    void check_text(PullParser &p, const char *text)
    {
        status_t token = check_references(p);
        printf("  CHARACTERS: %s\n", text);
        UTEST_ASSERT(token == XT_CHARACTERS);
        UTEST_ASSERT(p.name() == NULL);
        UTEST_ASSERT(p.value()->equals_ascii(text));
    }

    void check_cdata(PullParser &p, const char *cdata)
    {
        printf("  CDATA: %s\n", cdata);
        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_CDATA);
        UTEST_ASSERT(p.name() == NULL);
        UTEST_ASSERT(p.value()->equals_ascii(cdata));
    }

    void check_attribute(PullParser &p, const char *name, const char *value)
    {
        LSPString tmp;
        status_t token = check_references(p);
        printf("  ATTRIBUTE: name=%s, value=%s\n", name, value);
        UTEST_ASSERT(token == XT_ATTRIBUTE);
        UTEST_ASSERT(tmp.set_utf8(value));
        UTEST_ASSERT(p.name()->equals_ascii(name));
        UTEST_ASSERT(p.value()->equals(&tmp));
    }

    void check_start_element(PullParser &p, const char *name)
    {
        printf("  START_ELEMENT: name=%s\n", name);
        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_START_ELEMENT);
        UTEST_ASSERT(p.name()->equals_ascii(name));
        UTEST_ASSERT(p.value() == NULL);
    }

    void check_end_element(PullParser &p, const char *name)
    {
        printf("  END_ELEMENT: name=%s\n", name);
        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_END_ELEMENT);
        UTEST_ASSERT(p.name()->equals_ascii(name));
        UTEST_ASSERT(p.value() == NULL);
    }

    void check_pi(PullParser &p, const char *name, const char *value)
    {
        printf("  PROCESSING_INSTRUCTION: name=%s, value=%s\n", name, value);
        status_t token = p.read_next();
        UTEST_ASSERT(token == XT_PROCESSING_INSTRUCTION);
        UTEST_ASSERT(p.name()->equals_ascii(name));
        UTEST_ASSERT(p.value()->equals_ascii(value));
    }

    status_t read_xml_fully(const char *xml, ssize_t last_token)
    {
        LSPString text, tmp;
        status_t res;

        printf("Reading document: %s\n", xml);

        if (!text.set_utf8(xml))
            return STATUS_NO_MEM;

        PullParser p;
        if ((res = p.wrap(&text)) != STATUS_OK)
            return res;

        bool start = false, end = false;
        status_t last = -1;

        while (true)
        {
            // Read event
            if ((res = p.read_next()) < 0)
            {
                if (res == -STATUS_EOF)
                {
                    if (end)
                        return p.close();

                    printf("  No END_DOCUMENT triggered\n");
                    p.close();
                    return STATUS_CORRUPTED;
                }
                if ((last_token >= 0) && (last_token != last))
                {
                    printf("  Last token %d does not match required %d\n", int(last), int(last_token));
                    return STATUS_CORRUPTED;
                }
                return -res;
            }

            // Require START_DOCUMENT
            if ((!start) && (res != XT_START_DOCUMENT))
            {
                printf("  No START_DOCUMENT triggered\n");
                return STATUS_CORRUPTED;
            }

            last = res;     // Store last token
            switch (res)
            {
                case XT_START_DOCUMENT:
                    printf("  START_DOCUMENT: xml_version=%d", int(p.xml_version()));
                    if (p.version() != NULL)
                        printf(" version='%s'", p.version()->get_utf8());
                    if (p.encoding() != NULL)
                        printf(" encoding='%s'", p.encoding()->get_utf8());
                    printf(" standalone='%s'\n", p.is_standalone() ? "yes" : "no");
                    start = true;
                    break;

                case XT_END_DOCUMENT:
                    printf("  END_DOCUMENT\n");
                    end = true;
                    break;

                case XT_ATTRIBUTE:
                    printf("  ATTRIBUTE: name='%s', value='%s'\n", p.name()->get_utf8(), p.value()->get_utf8());
                    break;

                case XT_CDATA:
                    printf("  CDATA: value='%s'\n", p.value()->get_utf8());
                    break;

                case XT_CHARACTERS:
                    printf("  CHARACTERS: value='%s'\n", p.value()->get_utf8());
                    break;

                case XT_COMMENT:
                    printf("  COMMENT: value='%s'\n", p.value()->get_utf8());
                    break;

                case XT_END_ELEMENT:
                    printf("  END_ELEMENT: name='%s'\n", p.name()->get_utf8());
                    break;

                case XT_ENTITY_RESOLVE:
                    if (!tmp.set(p.name()))
                        return STATUS_NO_MEM;
                    tmp.toupper();
                    printf("  REFERENCE_RESOLVE: name='%s', resolving to '%s'\n", p.name()->get_utf8(), tmp.get_utf8());
                    if ((res = p.set_value(&tmp)) != STATUS_OK)
                        return res;
                    break;

                case XT_PROCESSING_INSTRUCTION:
                    printf("  PROCESSING_INSTRUCTION: value='%s'\n", p.value()->get_utf8());
                    break;

                case XT_START_ELEMENT:
                    printf("  START_ELEMENT: name='%s'\n", p.name()->get_utf8());
                    break;

                case XT_DTD:
                    printf("  DOCTYPE: type=%s, public=%s, system=%s\n",
                            (p.doctype() != NULL) ? p.doctype()->get_utf8() : NULL,
                            (p.pub_literal() != NULL) ? p.pub_literal()->get_utf8() : NULL,
                            (p.sys_literal() != NULL) ? p.sys_literal()->get_utf8() : NULL
                        );
                    break;

                default:
                    printf("Unknown event: %d\n", int(res));
                    return STATUS_UNKNOWN_ERR;
            }
        }
    }

    void test_simple_valid_xml()
    {
        static const char *xml[] =
        {
            "",

            "<?xml version='1.0'?>",
            "<?xml version='1.1' encoding='UTF-8'?>",
            "<?xml version='1.1' encoding='UTF-8' standalone='yes' ?>",
            "<?xml version='1.0' standalone='yes' ?>",
            "<?xml \n\t\r version='1.1' \n\t\r encoding='UTF-8' \n\t\r  standalone='yes' \n\t\r ?>",

            "<!-- Simple comment -->",
            "<!--- comment - with - dash -->",
            "<!-- Simple comment 1 -->\n<!-- Simple comment 2 -->",

            "<!DOCTYPE greeting>",
            "<!DOCTYPE greeting >",
            "<!DOCTYPE greeting SYSTEM \"hello.dtd\">",
            "<!DOCTYPE greeting SYSTEM 'hello.dtd'>",
            "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">",

            "<?xml version='1.0'?><!-- comment after header -->",
            "<?xml version='1.0'?><!-- c1 --><?pi some text ?><!-- c2 -->",

            "<?xml version='1.0'?><?pi?>",
            "<?xml version='1.0'?><?pi arg?>",
            "<?xml version='1.0'?><?pi arg arg   ?>",

            "<?xml version='1.0'?><root/>",
            "<?xml version='1.0'?><root />",
            "<?xml version='1.0'?><root></root>",
            "<?xml version='1.0'?><root>&#x00000000000020;</root>",
            "<?xml version='1.0'?><root>&reference1;</root>",
            "<?xml version='1.0'?><root><![CDATA[data]]data]]></root>",

            "<?xml version='1.0'?><root id='1' ID='2'></root>",
            "<?xml version='1.0'?><root att1=\"value1\"></root>",
            "<?xml version='1.0'?><root att1=\"value1\" ns:attr2='value2'  ></root>",
            "<?xml version='1.0'?><root><t1><t2></t2></t1></root>",
            "<?xml version='1.0'?><root att1=\"value1\" ><nested id=\"&amp;&amp;&amp;\" /><nested2 x='123'><?pi2 args?><!-- comment --></nested2></root>",
            "<?xml version='1.0'?><root>text &quot;&lt;<![CDATA[character&amp;data]]>&gt;&quot;</root>",
            "<?xml version='1.0'?><root>&unknown_entity;<nested att='&unknown_entity2;' /></root>",
            NULL
        };

        for (const char **p=xml; *p != NULL; ++p)
            UTEST_ASSERT(read_xml_fully(*p, XT_END_DOCUMENT) == STATUS_OK);
    }

    void test_simple_invalid_xml()
    {
        static const char *xml[] =
        {
            "<?xml?>",
            "<?xml vesion='1.0'>",
            " <?xml vesion='1.0'?>",
            "<?xml encoding='UTF-8'?>",
            "<?xml standalone='yes'?>",
            "<?xml version='3.1' ?>",
            "<?xml version='1.0' encoding='#$%$^#' ?>",
            "<?xml version='1.0' standalone='true' ?>",
            "<?xml version='1.0' standalone='NO' ?>",

            "<!DOCTYPE greeting 123>",
            "<!DOCTYPE greeting xml>",
            "<!DOCTYPE greeting []>",
            "<!DOCTYPE greeting SYSTEM>",
            "<!DOCTYPE greeting PUBLIC>",
            "<!DOCTYPE greeting SYSTEM >",
            "<!DOCTYPE greeting PUBLIC >",
            "<!DOCTYPE greeting SYSTEM 'привет.dtd' []>",
            "<!DOCTYPE greeting SYSTEM \"greeting.dtd\" []>",
            "<!DOCTYPE HTML PUBLIC \"привет\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">",
            "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\" []>",
            "<!DOCTYPE greeting><!DOCTYPE xml>",

            "<?xml version='1.0'?><![CDATA[1234]]>",

            "<?xml version='1.0'?><!---- invalid comment -->",
            "<?xml version='1.0'?><!-- invalid -- comment -->",
            "<?xml version='1.0'?><!-- invalid --->",
            "<?xml version='1.0'?><!-- invalid ",
            "<!-- valid --><?xml version='1.0'?>",

            "<?xml version='1.0'?>&amp;",
            "<?xml version='1.0'?>&#x100;",
            "<?xml version='1.0'?>< ? pi?>",
            "<?xml version='1.0'?><? pi?>",
            "<?xml version='1.0'?><?pi? >",

            "<?xml version='1.0'?><root>",
            "<?xml version='1.0'?><root/ >",
            "<?xml version='1.0'?><root>< /root>",
            "<?xml version='1.0'?><root></ root>",
            "<?xml version='1.0'?><root1/><root2/>",
            "<?xml version='1.0'?><root1></root1><root2></root2>",

            "<?xml version='1.0'?><root att1></root>",
            "<?xml version='1.0'?><root att=value></root>",
            "<?xml version='1.0'?><root 1att='value'></root>",
            "<?xml version='1.0'?><root att='value' att2></root>",
            "<?xml version='1.0'?><root att='value1' att='value2'></root>",

            "<?xml version='1.0'?><root><tag></root>",
            "<?xml version='1.0'?><root><1tag /></root>",
            "<?xml version='1.0'?><root><tag/ ></root>",
            "<?xml version='1.0'?><root>&reference1</root>",
            "<?xml version='1.0'?><root>&#xde00;</root>",
            "<?xml version='1.0'?><root>&#xdf00;</root>",
            "<?xml version='1.0'?><root>&#x110000;</root>",

            "<?xml version='1.0'?><root><t1><t2></t1></t2></root>",

            NULL
        };

        for (const char **p=xml; *p != NULL; ++p)
            UTEST_ASSERT(read_xml_fully(*p, -1) != STATUS_OK);
    }

    void test_xml_parsing()
    {
        const char *xml =
            "<?xml version='1.1' encoding='US-ASCII' standalone='yes'?>\n"
            "<!DOCTYPE TEST PUBLIC 'lsp-plugin' 'http://lsp-plug.in/test.dtd'>\n"
            "<!--c1-->\n"
            "<?pi v1?>\n"
            "<!--c2-->\n"
            "<root:root xmlns:test=\"http://some test namespace\">\n"
                "<?pi v2?>\n"
                "<!--c3-->\n"
                "<?pi v3?>\n"
                "<tag1 p1='10' p2='20'>"
                    "<?pi v4?>\n"
                    "<!--c4-->\n"
                    "some text characters\n"
                    "<?pi v5?>\n"
                "</tag1>\n"
                "&reference1;\n"
                "<test:tag2 attr='&amp;&quot;&apos;&lt;&gt;' attr2=\"&#x041F;&#x0440;&#x0438;&#x0432;&#x0435;&#x0442;\" />\n"
                "<tag3 param='&#65;-&#90;' ref=\"&reference2;-&reference3;\">\n"
                    "<!--cdata-->\n"
                    "<![CDATA[175bf801fddd9ea5ded1da4e26a75ad3]]>\n"
                    "<?xi v5?>\n"
                "</tag3>\n"
            "</root:root>\n\n"
            "<?pi v6?>\n"
            "<!-- end -->\n"
            ;

        LSPString text;
        printf("Reading document: %s\n", xml);
        UTEST_ASSERT(text.set_utf8(xml));

        PullParser p;
        UTEST_ASSERT((p.wrap(&text)) == STATUS_OK);

        check_document_start(p, XML_VERSION_1_1, "1.1", "US-ASCII", true);
        check_doctype(p, "TEST", "lsp-plugin", "http://lsp-plug.in/test.dtd");
        check_comment(p, "c1");
        check_pi(p, "pi", "v1");
        check_comment(p, "c2");
        UTEST_ASSERT(p.level() == 0);

        check_start_element(p, "root:root");
        check_attribute(p, "xmlns:test", "http://some test namespace");
        check_text(p, "\n");
            UTEST_ASSERT(p.level() == 1);
            check_pi(p, "pi", "v2");
            check_text(p, "\n");
            check_comment(p, "c3");
            check_text(p, "\n");
            check_pi(p, "pi", "v3");
            check_text(p, "\n");

            check_start_element(p, "tag1");
            check_attribute(p, "p1", "10");
            check_attribute(p, "p2", "20");
                UTEST_ASSERT(p.level() == 2);
                check_pi(p, "pi", "v4");
                check_text(p, "\n");
                check_comment(p, "c4");
                check_text(p, "\nsome text characters\n");
                check_pi(p, "pi", "v5");
                check_text(p, "\n");
            check_end_element(p, "tag1");
            check_text(p, "\nREFERENCE1\n");
            UTEST_ASSERT(p.level() == 1);

            check_start_element(p, "test:tag2");
            UTEST_ASSERT(p.level() == 2);
            check_attribute(p, "attr", "&\"\'<>");
            check_attribute(p, "attr2", "Привет");
            check_end_element(p, "test:tag2");
            UTEST_ASSERT(p.level() == 1);
            check_text(p, "\n");

            check_start_element(p, "tag3");
            check_attribute(p, "param", "A-Z");
            check_attribute(p, "ref", "REFERENCE2-REFERENCE3");
                UTEST_ASSERT(p.level() == 2);
                check_text(p, "\n");
                check_comment(p, "cdata");
                check_text(p, "\n");
                check_cdata(p, "175bf801fddd9ea5ded1da4e26a75ad3");
                check_text(p, "\n");
                check_pi(p, "xi", "v5");
                check_text(p, "\n");
            check_end_element(p, "tag3");
            check_text(p, "\n");
            UTEST_ASSERT(p.level() == 1);
        check_end_element(p, "root:root");
        UTEST_ASSERT(p.level() == 0);
        check_pi(p, "pi", "v6");
        check_comment(p, " end ");
    }


    UTEST_MAIN
    {
        printf("Testing simple valid XML parsing...\n");
        test_simple_valid_xml();
        printf("Testing simple invalid XML parsing...\n");
        test_simple_invalid_xml();
        printf("Testing XML document parsing...\n");
        test_xml_parsing();
    }

UTEST_END
