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

    status_t read_xml_fully(const char *xml, xml_token_t last_token)
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
                    if (!tmp.set(p.value()))
                        return STATUS_NO_MEM;
                    tmp.toupper();
                    printf("  ENTITY_RESOLVE: name='%s', resolving to '%s'\n", p.value()->get_utf8(), tmp.get_utf8());
                    if ((res = p.resolve_entity(&tmp)) != STATUS_OK)
                        return res;
                    break;

                case XT_PROCESSING_INSTRUCTION:
                    printf("  PROCESSING_INSTRUCTION: value='%s'\n", p.value()->get_utf8());
                    break;

                case XT_START_ELEMENT:
                    printf("  START_ELEMENT: name='%s'\n", p.name()->get_utf8());
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
//            "",
//            "<?xml version='1.0'?>",
//            "<?xml version='1.1' encoding='UTF-8'?>",
//            "<?xml version='1.1' encoding='UTF-8' standalone='yes' ?>",
//            "<?xml \n\t\r version='1.1' \n\t\r encoding='UTF-8' \n\t\r  standalone='yes' \n\t\r ?>",
//            "<!-- Simple comment -->",
//            "<!-- Simple comment 1 -->\n<!-- Simple comment 2 -->",
//            "<?xml version='1.0'?><!-- comment after header -->",
//            "<?xml version='1.0'?><!-- c1 --><?pi some text ?><!-- c2 -->",
//            "<?xml version='1.0'?><root></root>",
//            "<?xml version='1.0'?><root att1=\"value1\"></root>",
//            "<?xml version='1.0'?><root att1=\"value1\" ns:attr2='value2'  ></root>",
            "<?xml version='1.0'?><root att1=\"value1\" ><nested id=\"&amp;&amp;&amp;\" /><nested2 x='123'><?pi2 args?><!-- comment --></nested2></root>",
            NULL
        };

        for (const char **p=xml; *p != NULL; ++p)
            UTEST_ASSERT(read_xml_fully(*p, XT_END_DOCUMENT) == STATUS_OK);
    }

    UTEST_MAIN
    {
        printf("Testing simple valid XML parsing...\n");
        test_simple_valid_xml();
    }

UTEST_END
