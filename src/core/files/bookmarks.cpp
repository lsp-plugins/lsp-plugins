/*
 * bookmarks.cpp
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#include <core/files/bookmarks.h>
#include <core/io/InFileStream.h>
#include <core/io/InSequence.h>

namespace lsp
{
    namespace bookmarks
    {
        static inline int decode_hex(lsp_wchar_t c)
        {
            if ((c >= '0') && (c <= '9'))
                return c - '0';
            else if ((c >= 'a') && (c <= 'f'))
                return c - 'a' + 10;
            else if ((c >= 'A') && (c <= 'F'))
                return c - 'A' + 10;
            return -1;
        }

        void destroy_bookmarks(cvector<bookmark_t> *list)
        {
            if (list == NULL)
                return;

            for (size_t i=0, n=list->size(); i<n; ++i)
            {
                bookmark_t *ptr = list->at(i);
                if (ptr != NULL)
                    delete ptr;
            }
            list->flush();
        }

        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, const char *path, const char *charset)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            cvector<bookmark_t> tmp;
            io::InSequence is;
            status_t res;
            if ((res = is.open(path, charset)) == STATUS_OK)
            {
                res = read_bookmarks_gtk3(&tmp, &is);
                if (res == STATUS_OK)
                    res = is.close();
                else
                    is.close();
            }

            if (res == STATUS_OK)
                dst->swap_data(&tmp);
            destroy_bookmarks(&tmp);

            return res;
        }

        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, const LSPString *path, const char *charset)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            cvector<bookmark_t> tmp;
            io::InSequence is;
            status_t res;
            if ((res = is.open(path, charset)) == STATUS_OK)
            {
                res = read_bookmarks_gtk3(&tmp, &is);
                if (res == STATUS_OK)
                    res = is.close();
                else
                    is.close();
            }

            if (res == STATUS_OK)
                dst->swap_data(&tmp);
            destroy_bookmarks(&tmp);

            return res;
        }

        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, const io::Path *path, const char *charset)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            cvector<bookmark_t> tmp;
            io::InSequence is;
            status_t res;
            if ((res = is.open(path, charset)) == STATUS_OK)
            {
                res = read_bookmarks_gtk3(&tmp, &is);
                if (res == STATUS_OK)
                    res = is.close();
                else
                    is.close();
            }

            if (res == STATUS_OK)
                dst->swap_data(&tmp);
            destroy_bookmarks(&tmp);

            return res;
        }

        status_t decode_gtk3_path(LSPString *dst, const LSPString *src, size_t off)
        {
            char *buf = NULL;
            int code;
            size_t bcap=0, blen=0;
            LSPString tmp;

            for (size_t len=src->length(); off < len; )
            {
                lsp_wchar_t c = src->char_at(off);
                if (c == '%')
                {
                    // Read hex codes
                    do
                    {
                        // Check availability
                        if ((len - off) < 3)
                        {
                            if (buf != NULL)
                                ::free(buf);
                            return STATUS_CORRUPTED;
                        }

                        // Fetch byte code
                        code    = decode_hex(src->char_at(++off)) << 4;
                        code   |= decode_hex(src->char_at(++off));
                        if (code < 0)
                        {
                            if (buf != NULL)
                                ::free(buf);
                            return STATUS_CORRUPTED;
                        }

                        // Append code to buffer
                        if (blen >= bcap)
                        {
                            bcap           += ((blen + 0x10) & ~0xf);
                            char *nbuf      = reinterpret_cast<char *>(::realloc(buf, bcap * sizeof(char)));
                            if (nbuf == NULL)
                            {
                                if (buf != NULL)
                                    ::free(buf);
                                return STATUS_NO_MEM;
                            }
                            buf             = nbuf;
                        }
                        buf[blen++]     = code;

                        // Read next character
                        c = src->char_at(++off);
                    } while (c == '%');

                    // Perform encoding
                    if (!tmp.set_utf8(buf, blen))
                    {
                        if (buf != NULL)
                            ::free(buf);
                        return STATUS_CORRUPTED;
                    }

                    // Append data
                    if (!dst->append(&tmp))
                    {
                        if (buf != NULL)
                            ::free(buf);
                        return STATUS_NO_MEM;
                    }

                    // Reset buffer length
                    blen    = 0;
                }
                else if (!dst->append(c))
                {
                    if (buf != NULL)
                        ::free(buf);
                    return STATUS_NO_MEM;
                }
                else
                    ++off;
            }

            // Destroy buffer
            if (buf != NULL)
                ::free(buf);

            return STATUS_OK;
        }

        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, io::IInSequence *in)
        {
            cvector<bookmark_t> vtmp;
            LSPString tmp;
            status_t res;

            while (true)
            {
                // Read line
                res = in->read_line(&tmp, true);
                if (res != STATUS_OK)
                {
                    if (res == STATUS_EOF)
                        break;
                    destroy_bookmarks(&vtmp);
                    return res;
                }

                // Analyze line
                if (!tmp.starts_with_ascii_nocase("file://"))
                    continue;

                // Create bookmark
                bookmark_t *bm  = new bookmark_t;
                bm->source      = BM_GTK3;
                if (bm == NULL)
                {
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }

                if ((res = decode_gtk3_path(&bm->path, &tmp, 7)) != STATUS_OK)
                {
                    delete bm;
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }

                // Obtain the last name in path
                ssize_t idx = bm->path.rindex_of(FILE_SEPARATOR_C);
                if (idx < 0)
                    idx = -1;
                if (!bm->name.set(&bm->path, idx+1))
                {
                    delete bm;
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }

                // Add to list
                if (!vtmp.add(bm))
                {
                    delete bm;
                    destroy_bookmarks(&vtmp);
                    return STATUS_NO_MEM;
                }
            }

            dst->swap_data(&vtmp);
            destroy_bookmarks(&vtmp);

            return STATUS_OK;
        }
    }
}



