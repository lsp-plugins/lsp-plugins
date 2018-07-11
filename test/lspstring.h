#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/LSPString.h>

namespace lspstring_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        LSPString s;
        s.set_utf8("Всем привет! Давно не виделись! 涼宮ハルヒの憂鬱：第一章.");
        lsp_trace("String length: %d", int(s.length()));
        lsp_trace("String data: %s", s.get_utf8());

        lsp_trace("Modifying string...");
        LSPString s2;
        s2.set_utf8("APPENDED");
        s.append(&s2);
        s2.set_utf8("PREPENDED");
        s.prepend(&s2);
        s2.set_utf8("INSERTED");
        s.insert(s.length() / 2, &s2);
        lsp_trace("String length: %d", int(s.length()));
        lsp_trace("String data: %s", s.get_utf8());

        lsp_trace("Getting substring...");
        LSPString *sub = s.substring(10, -10);
        lsp_trace("String length: %d", int(sub->length()));
        lsp_trace("String data: %s", sub->get_utf8());
        delete sub;

        lsp_trace("Shuffling string...");
        s.shuffle();
        lsp_trace("String length: %d", int(s.length()));
        lsp_trace("String data: %s", s.get_utf8());

        return 0;
    }
    
}
