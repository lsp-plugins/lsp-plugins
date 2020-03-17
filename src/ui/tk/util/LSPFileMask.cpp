/*
 * LSPFileMask.cpp
 *
 *  Created on: 9 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <string.h>
#include <wctype.h>
#include <locale.h>

namespace lsp
{
    namespace tk
    {
        LSPFileMask::LSPFileMask()
        {
            pBuffer     = NULL;
            nFlags      = 0;
        }

        LSPFileMask::~LSPFileMask()
        {
            clear();
        }

        LSPFileMask::simplemask_t *LSPFileMask::parse_simple(cstorage<simplemask_t> *dst, biter_t *bi)
        {
            // Check that iterator is valid
            lsp_wchar_t *ptr  = bi->pHead;
            lsp_wchar_t *tail = bi->pTail;
            if (ptr >= tail)
                return NULL;

            // Create new mask object
            simplemask_t *mask  = dst->append();
            if (mask == NULL)
                return NULL;
            mask->pHead         = ptr;
            mask->bInvert       = false;

            // Eliminate duplicate wildcard '*' characters
            do
            {
                // Check that there is not the mask separator
                lsp_wchar_t ch =  *ptr;
                if (ch == '|')
                    break;
                else if (ch == '*')
                {
                    ssize_t diff = tail - ptr;
                    if ((diff >= 2) && (ptr[1] == '*'))
                    {
                        ::memmove(ptr, &ptr[1], diff - 1);
                        tail--;
                    }
                    else
                        ptr++;
                }
                else
                    ptr++;
            } while (ptr < tail);

            // Update parameters
            mask->pTail     = ptr;
            bi->pHead       = ptr;
            bi->pTail       = tail;

            return mask;
        }

        bool LSPFileMask::check_simple_case(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail)
        {
            // Check that there is end of pattern
            if (head >= tail)
                return (shead != NULL) && (stail >= shead);

            // Wildcard set of characters ?
            if ((*head) == '*')
            {
                // Last character in mask ?
                if ((++head) >= tail)
                    return (shead != NULL) && (stail >= shead);

                // Analyze tail mask
                do
                {
                    if (check_simple_case(head, tail, shead, stail))
                        return true;
                } while ((++head) < tail);

                return false;
            }

            // Non-wildcard set of characters
            do
            {
                // Get source character of the mask
                lsp_wchar_t sch = *head;
                if (sch == '*')
                    return check_simple_case(head, tail, shead, stail);
                else if ((shead == NULL) || (shead >= stail))
                    return false;
                else if (sch == '?') // Wildcard character
                {
                    head        ++;
                    shead       ++;
                    continue;
                }

                // Get target character of the mask
                if (*(shead++) != sch)
                    return false;
            } while ((++head) < tail);

            // Return success
            return (shead >= stail);
        }

        bool LSPFileMask::check_simple_nocase(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail)
        {
            // Check that there is end of pattern
            if (head >= tail)
                return (shead != NULL) && (stail >= shead);

            // Wildcard set of characters ?
            if ((*head) == '*')
            {
                // Last character in mask ?
                if ((++head) >= tail)
                    return (shead != NULL) && (stail >= shead);

                // Analyze tail mask
                do
                {
                    if (check_simple_nocase(head, tail, shead, stail))
                        return true;
                } while ((++shead) < stail);

                return false;
            }

            // Non-wildcard set of characters
            do
            {
                // Get source character of the mask
                lsp_wchar_t sch = *head;
                if (sch == '*')
                    return check_simple_nocase(head, tail, shead, stail);
                else if ((shead == NULL) || (shead >= stail))
                    return false;
                else if (sch == '?') // Wildcard character
                {
                    head        ++;
                    shead       ++;
                    continue;
                }

                // Get target character of the mask
                if (towlower(*(shead++)) != towlower(sch))
                    return false;
            } while ((++head) < tail);

            // Return success
            return (shead >= stail);
        }

        bool LSPFileMask::check_mask(simplemask_t *mask, const lsp_wchar_t *s, size_t len)
        {
            if (nFlags & CASE_SENSITIVE)
                return check_simple_case(mask->pHead, mask->pTail, s, &s[len]) ^ mask->bInvert;
            else
                return check_simple_nocase(mask->pHead, mask->pTail, s, &s[len]) ^ mask->bInvert;
        }

        void LSPFileMask::swap(LSPFileMask *mask)
        {
            sMask.swap(&mask->sMask);
            vMasks.swap(&mask->vMasks);

            size_t flags        = mask->nFlags;
            mask->nFlags        = nFlags;
            nFlags              = flags;

            lsp_wchar_t *buf    = mask->pBuffer;
            mask->pBuffer       = pBuffer;
            pBuffer             = buf;
        }

        status_t LSPFileMask::parse(const LSPString *pattern, size_t flags)
        {
            size_t n = pattern->length();

            if (n <= 0)
            {
                clear();
                return STATUS_OK;
            }

            // Allocate buffer
            lsp_wchar_t *buf        = lsp_tmalloc(lsp_wchar_t, n);
            if (buf == NULL)
                return STATUS_NO_MEM;
            ::memcpy(buf, pattern->characters(), n*sizeof(lsp_wchar_t));

            // Create iterator
            biter_t bi;
            cstorage<simplemask_t> masks;
            bi.pHead                = buf;
            bi.pTail                = &buf[n];
            bi.bInvert              = false;

            // Parse mask
            while (bi.pHead < bi.pTail)
            {
                char ch = *bi.pHead;
                if (ch == '!')
                {
                    bi.bInvert  = !bi.bInvert;
                    bi.pHead    ++;
                }
                else
                {
                    if (parse_simple(&masks, &bi) == NULL)
                    {
                        lsp_free(buf);  // Free buffer
                        masks.flush();
                        return STATUS_NO_MEM;
                    }
                    bi.bInvert  = false;
                    bi.pHead    ++;
                }
            }

            // OK, mask has been parsed into simple tokens
            // Update mask
            if (!sMask.set(pattern))
            {
                lsp_free(buf);
                return STATUS_NO_MEM;
            }

            // Remove previously used buffer
            if (pBuffer != NULL)
                lsp_free(pBuffer);
            pBuffer     = buf;

            // Swap context and free previously used context
            vMasks.swap(&masks);
            masks.flush();

            // Store flags
            nFlags      = flags & (INVERSIVE | CASE_SENSITIVE);

            return STATUS_OK;
        }

        status_t LSPFileMask::parse(const char *pattern, size_t flags)
        {
            LSPString tmp;
            if (!tmp.set_native(pattern))
                return STATUS_NO_MEM;
            return parse(&tmp, flags);
        }

        void LSPFileMask::set_flags(size_t flags)
        {
            nFlags      = flags & (INVERSIVE | CASE_SENSITIVE);
        }

        status_t LSPFileMask::set(const LSPFileMask *mask)
        {
            return parse(&mask->sMask, mask->nFlags);
        }

        bool LSPFileMask::matched(const LSPString *str)
        {
            if (sMask.length() <= 0)
                return !(nFlags & INVERSIVE);

            // Get raw string data
            const lsp_wchar_t *chars = str->characters();
            size_t len = str->length();

            // Validate mask
            size_t n    = vMasks.size();
            if (nFlags & INVERSIVE)
            {
                for (size_t i=0; i<n; ++i)
                {
                    simplemask_t *m = vMasks.at(i);
                    if (!check_mask(m, chars, len))
                        return false;
                }

                return true;
            }
            else // Normal conditions
            {
                for (size_t i=0; i<n; ++i)
                {
                    simplemask_t *m = vMasks.at(i);
                    if (check_mask(m, chars, len))
                        return true;
                }

                return false;
            }
        }

        bool LSPFileMask::matched(const char *text)
        {
            LSPString tmp;
            if (!tmp.set_native(text))
                return false;
            return matched(&tmp);
        }

        void LSPFileMask::clear()
        {
            sMask.truncate();
            vMasks.flush();
            if (pBuffer != NULL)
            {
                lsp_free(pBuffer);
                pBuffer     = NULL;
            }
        }

        bool LSPFileMask::valid_file_name(const char *fname)
        {
            LSPString tmp;
            if (!tmp.set_native(fname))
                return false;
            return valid_file_name(&tmp);
        }

        bool LSPFileMask::valid_path_name(const char *fname)
        {
            LSPString tmp;
            if (!tmp.set_native(fname))
                return false;
            return valid_path_name(&tmp);
        }

        bool LSPFileMask::valid_file_name(const LSPString *fname)
        {
            if (fname == NULL)
                return false;
            size_t n = fname->length();
            if (n <= 0)
                return false;

            const lsp_wchar_t *chars = fname->characters();
            for (size_t i=0; i<n; ++i)
            {
                lsp_wchar_t ch = *(chars++);
                if ((ch == '*') || (ch == '?'))
                    return false;
                if ((ch == FILE_SEPARATOR_C) || (ch == '\0'))
                    return false;
                #ifdef PLATFORM_WINDOWS
                if ((ch == '/') || (ch == ':') || (ch== '|') || (ch == '<') || (ch == '>'))
                    return false;
                #endif /* PLATFORM_WINDOWS */
            }

            return true;
        }

        bool LSPFileMask::valid_path_name(const LSPString *fname)
        {
            if (fname == NULL)
                return false;
            size_t n = fname->length();
            if (n <= 0)
                return false;

            const lsp_wchar_t *chars = fname->characters();
            for (size_t i=0; i<n; ++i)
            {
                lsp_wchar_t ch = *(chars++);
                if ((ch == '*') || (ch == '?'))
                    return false;
                if (ch == '\0')
                    return false;
                #ifdef PLATFORM_WINDOWS
                if ((ch == '/') || (ch== '|') || (ch == '<') || (ch == '>'))
                    return false;
                #endif /* PLATFORM_WINDOWS */
            }

            #ifdef PLATFORM_WINDOWS
            ssize_t semicolon = fname->index_of(':');
            if (semicolon > 0)
            {
                ssize_t next = fname->index_of(':', semicolon + 1);
                if (next >= 0)
                    return false;
                if ((semicolon + 1) < fname->length()) // Should be "?:\"
                {
                    if (fname->char_at(semicolon + 1) != FILE_SEPARATOR_C)
                        return false;
                }

                // Check disk name
                const lsp_wchar_t *chars = fname->characters();
                while ((semicolon--) > 0)
                {
                    lsp_wchar_t ch = *(chars++);
                    if ((ch >= 'a') && (ch <= 'z'))
                        continue;
                    if ((ch >= 'A') && (ch <= 'Z'))
                        continue;
                    return false;
                }
            }
            else if (semicolon == 0)
                return false;
            #endif /* PLATFORM_WINDOWS */

            return true;
        }

        status_t LSPFileMask::append_path(LSPString *path, const char *fname)
        {
            return append_path(path, path, fname);
        }

        status_t LSPFileMask::append_path(LSPString *path, const LSPString *fname)
        {
            return append_path(path, path, fname);
        }

        status_t LSPFileMask::append_path(LSPString *dst, const LSPString *path, const char *fname)
        {
            LSPString tmp;
            if (!tmp.set_native(fname))
                return STATUS_NO_MEM;

            return append_path(dst, path, &tmp);
        }

        status_t LSPFileMask::append_path(LSPString *dst, const LSPString *path, const LSPString *fname)
        {
            LSPString result;
            if (!result.set(path))
                return STATUS_NO_MEM;
            if (!result.ends_with(FILE_SEPARATOR_C))
            {
                if (!result.append(FILE_SEPARATOR_C))
                    return STATUS_NO_MEM;
            }

            if (!result.append(fname))
                return STATUS_NO_MEM;

            result.swap(dst);
            return STATUS_OK;
        }

        bool LSPFileMask::is_dot(const LSPString *fname)
        {
            if (fname == NULL)
                return false;
            if (fname->length() != 1)
                return false;
            return fname->char_at(0) == '.';
        }

        bool LSPFileMask::is_dotdot(const LSPString *fname)
        {
            if (fname == NULL)
                return false;
            if (fname->length() != 2)
                return false;
            return (fname->char_at(0) == '.') && (fname->char_at(1) == '.');
        }

        bool LSPFileMask::is_dots(const LSPString *fname)
        {
            if (fname == NULL)
                return false;
            size_t len = fname->length();
            if ((len <= 0) || (len > 2))
                return false;
            const lsp_wchar_t *ch = fname->characters();
            while (len--)
            {
                if ((*ch) != '.')
                    return false;
            }
            return true;
        }
    } /* namespace tk */
} /* namespace lsp */
