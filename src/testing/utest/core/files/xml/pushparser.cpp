/*
 * pushparser.cpp
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#include <locale.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <core/stdlib/math.h>
#include <core/files/xml/PushParser.h>
#include <stdarg.h>

using namespace lsp;
using namespace lsp::xml;

UTEST_BEGIN("core.files.xml", pushparser)

    class Validator: public IXMLHandler
    {
        private:
            const char **pTok;
            test_type_t *pTest;

        public:
            explicit Validator(const char **instr, test_type_t *pt): pTok(instr), pTest(pt) { }

            inline bool no_more_tokens() const { return *pTok == NULL; }

        private:
            status_t validate(const char *prefix...)
            {
                LSPString tmp, ck;
                va_list v;

                const char *p = *(pTok++);
                pTest->printf("  Validating: %s\n", p);

                va_start(v, prefix);
                if (!tmp.append_utf8(prefix))
                    return STATUS_NO_MEM;

                while (true)
                {
                    const LSPString *s = va_arg(v, const LSPString *);
                    if (s == NULL)
                        break;
                    if (!tmp.append(','))
                        return STATUS_NO_MEM;
                    if (!tmp.append(s))
                        return STATUS_NO_MEM;
                }
                va_end(v);

                if ((p != NULL) && (!ck.set_utf8(p)))
                    return STATUS_NO_MEM;

                if (tmp.equals(&ck))
                    return STATUS_OK;

                pTest->eprintf("Expected: %s, actually get: %s\n", ck.get_utf8(), tmp.get_utf8());
                return STATUS_CORRUPTED;
            }

        public:
            virtual status_t start_document(xml_version_t xversion, const LSPString *version, const LSPString *encoding, bool standalone)
            {
                if (xversion != XML_VERSION_1_1)
                {
                    pTest->eprintf("Bat XML version: %d", int(xversion));
                    return STATUS_CORRUPTED;
                }
                if (!standalone)
                {
                    pTest->eprintf("Bat standalone param: %s", (standalone) ? "true" : "false");
                    return STATUS_CORRUPTED;
                }
                return validate("START", version, encoding, NULL);
            }

            virtual status_t end_document()
            {
                return validate("END", NULL);
            }

            virtual status_t cdata(const LSPString *cdata)
            {
                return validate("CDATA", cdata, NULL);
            }

            virtual status_t characters(const LSPString *text)
            {
                return validate("CHAR", text, NULL);
            }

            virtual status_t comment(const LSPString *text)
            {
                return validate("COMM", text, NULL);
            }

            virtual status_t resolve(LSPString *value, const LSPString *name)
            {
                if (!value->set(name))
                    return STATUS_NO_MEM;
                value->toupper();
                return STATUS_OK;
            }

            virtual status_t start_element(const LSPString *name, const LSPString * const *atts)
            {
                LSPString tmp;
                if (!tmp.append(name))
                    return STATUS_NO_MEM;

                while (*atts != NULL)
                {
                    if (!tmp.append(','))
                        return STATUS_NO_MEM;
                    if (!tmp.append(*(atts++)))
                        return STATUS_NO_MEM;
                    if (!tmp.append(','))
                        return STATUS_NO_MEM;
                    if (!tmp.append(*(atts++)))
                        return STATUS_NO_MEM;
                }
                return validate("OPEN", &tmp, NULL);
            }

            virtual status_t end_element(const LSPString *name)
            {
                return validate("CLOSE", name, NULL);
            }

            virtual status_t processing(const LSPString *name, const LSPString *args)
            {
                return validate("PI", name, args, NULL);
            }

            virtual status_t doctype(const LSPString *name, const LSPString *pub, const LSPString *sys)
            {
                return validate("DTD", name, pub, sys, NULL);
            }
    };

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

        const char *events[] =
        {
            "START,1.1,US-ASCII",
            "DTD,TEST,lsp-plugin,http://lsp-plug.in/test.dtd",
            "COMM,c1",
            "PI,pi,v1",
            "COMM,c2",
            "OPEN,root:root,xmlns:test,http://some test namespace",
                "CHAR,\n",
                "PI,pi,v2",
                "CHAR,\n",
                "COMM,c3",
                "CHAR,\n",
                "PI,pi,v3",
                "CHAR,\n",
                "OPEN,tag1,p1,10,p2,20",
                    "PI,pi,v4",
                    "CHAR,\n",
                    "COMM,c4",
                    "CHAR,\nsome text characters\n",
                    "PI,pi,v5",
                    "CHAR,\n",
                "CLOSE,tag1",
                "CHAR,\nREFERENCE1\n",
                "OPEN,test:tag2,attr,&\"\'<>,attr2,Привет",
                "CLOSE,test:tag2",
                "CHAR,\n",
                "OPEN,tag3,param,A-Z,ref,REFERENCE2-REFERENCE3",
                    "CHAR,\n",
                    "COMM,cdata",
                    "CHAR,\n",
                    "CDATA,175bf801fddd9ea5ded1da4e26a75ad3",
                    "CHAR,\n",
                    "PI,xi,v5",
                    "CHAR,\n",
                "CLOSE,tag3",
                "CHAR,\n",
            "CLOSE,root:root",
            "PI,pi,v6",
            "COMM, end ",
            "END",
            NULL
        };

        LSPString text;
        printf("Reading document: %s\n", xml);
        UTEST_ASSERT(text.set_utf8(xml));

        PushParser p;
        Validator v(events, this);
        UTEST_ASSERT(p.parse_data(&v, &text) == STATUS_OK);
        UTEST_ASSERT(v.no_more_tokens());
    }

    UTEST_MAIN
    {
        printf("Testing XML document parsing...\n");
        test_xml_parsing();
    }

UTEST_END

