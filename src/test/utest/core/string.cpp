/*
 * string.cpp
 *
 *  Created on: 28 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/LSPString.h>
#include <test/utest.h>

using namespace lsp;

UTEST_BEGIN("core", lspstring)
    UTEST_MAIN
    {
        LSPString s1, s2, s3, s4, s5;

        // Settings
        UTEST_ASSERT(s1.set_ascii("This is some text"));
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s2.set_utf8("Всем привет!"));
        UTEST_ASSERT(s2.get_native() != NULL);
        printf("s2 = %s\n", s2.get_native());

        UTEST_ASSERT(s3.set_utf8("涼宮ハルヒの憂鬱：第一章."));
        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());

        UTEST_ASSERT(s4.set_ascii("Всем привет!"));
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s5.set(&s1));
        UTEST_ASSERT(s5.get_native() != NULL);
        printf("s5 = %s\n", s5.get_native());


        UTEST_ASSERT(s1.length() == 17);
        UTEST_ASSERT(s2.length() == 12);
        UTEST_ASSERT(s3.length() == 13);
        UTEST_ASSERT(s4.length() == 22);
        UTEST_ASSERT(s5.length() == s1.length());

        UTEST_ASSERT(s3.set(&s1, 8, 12)); // "some"
        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());

        UTEST_ASSERT(s4.set(&s1, -9, 12)); // "some"
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s5.set(&s1, -4, -10)); // empty because -5 < -9
        UTEST_ASSERT(s5.get_native() != NULL);
        printf("s5 = %s\n", s5.get_native());
        UTEST_ASSERT(s3.equals(&s4));

        UTEST_ASSERT(s5.length() == 0);

        s3.toupper(); // "SOME"
        printf("s3 = %s\n", s3.get_native());
        UTEST_ASSERT(s3.get_native() != NULL);

        s1.toupper(); // "THIS IS SOME TEXT"
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        s4.clear(); // ""
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s4.is_empty());
        UTEST_ASSERT(s1.index_of(&s3) == 8);
        s1.swap(&s3); // s1 = "SOME", s3 = "THIS IS SOME TEXT"

        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());
        UTEST_ASSERT(s3.index_of(&s1) == 8);

        s4.truncate(); // ""
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());
        UTEST_ASSERT(s4.is_empty());
        UTEST_ASSERT(s4.capacity() == s4.length());

        // Search and insert
        UTEST_ASSERT(s1.set_ascii("ABAABBAAABBBAAAABBBB")); // "ABAABBAAABBBAAAABBBB"
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s2.set_ascii("CCCC")); // "CCCC"
        UTEST_ASSERT(s2.get_native() != NULL);
        printf("s2 = %s\n", s2.get_native());

        UTEST_ASSERT(s3.set_ascii("CC")); // "CC"
        UTEST_ASSERT(s3.get_native() != NULL);
        printf("s3 = %s\n", s3.get_native());

        UTEST_ASSERT(s4.set_ascii("abaaccccbbccccaaabbbaaaabbbb")); // "abaaccccbbccccaaabbbaaaabbbb"
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s1.insert(4, &s2)); // ABAACCCCBBAAABBBAAAABBBB
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s1.insert(10, &s2)); // ABAACCCCBBCCCCAAABBBAAAABBBB
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());
        UTEST_ASSERT(s1.equals_nocase(&s4));

        UTEST_ASSERT(s1.index_of(&s3) == 4);
        UTEST_ASSERT(s1.index_of(5, &s3) == 5);
        UTEST_ASSERT(s1.index_of(6, &s3) == 6);
        UTEST_ASSERT(s1.index_of(7, &s3) == 10);
        UTEST_ASSERT(s1.index_of(-21, &s3) == 10);
        UTEST_ASSERT(s1.rindex_of(&s3) == 12);
        UTEST_ASSERT(s1.rindex_of(-17, &s3) == 6);

        UTEST_ASSERT(s5.set_ascii("BBBBCCCC"));
        UTEST_ASSERT(s5.get_native() != NULL);
        printf("s5 = %s\n", s5.get_native());

        UTEST_ASSERT(s4.set_ascii("CCABAACCCCBB"));
        UTEST_ASSERT(s4.get_native() != NULL);
        printf("s4 = %s\n", s4.get_native());

        UTEST_ASSERT(s1.append(&s2));
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s1.prepend(&s3));
        UTEST_ASSERT(s1.get_native() != NULL);
        printf("s1 = %s\n", s1.get_native());

        UTEST_ASSERT(s1.starts_with(&s4));
        UTEST_ASSERT(s1.ends_with(&s5));
    }
UTEST_END;




