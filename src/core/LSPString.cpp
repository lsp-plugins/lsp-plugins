/*
 * LSPString.cpp
 *
 *  Created on: 30 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <stdlib.h>
#include <iconv.h>
#include <locale.h>
#include <errno.h>
#include <wctype.h>
#include <stdarg.h>
#include <core/io/charset.h>

#define GRANULARITY     0x20
#define BUF_SIZE        0x200
//#define BUF_SIZE        16

#define XSAFE_TRANS(index, length, result) \
    if (index < 0) \
    { \
        if ((index += (length)) < 0) \
            return result; \
    } \
    else if (size_t(index) > (length)) \
        return result;

#define XSAFE_ITRANS(index, length, result) \
    if (index < 0) \
    { \
        if ((index += (length)) < 0) \
            return result; \
    } \
    else if (size_t(index) >= (length)) \
        return result;

namespace lsp
{
    static bool is_space(lsp_wchar_t c)
    {
        switch (c)
        {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '\v':
                return true;
            default:
                return false;
        }
    }

    LSPString::LSPString()
    {
        nLength     = 0;
        nCapacity   = 0;
        pData       = NULL;
        pTemp       = NULL;
    }

    LSPString::~LSPString()
    {
        truncate();
    }

    int LSPString::xcasecmp(const lsp_wchar_t *a, const lsp_wchar_t *b, size_t n)
    {
        while (n--)
        {
            int retval = int(towlower(*(a++))) - int(towlower(*(b++)));
            if (retval != 0)
                return retval;
        }
        return 0;
    }

    void LSPString::acopy(lsp_wchar_t *dst, const char *src, size_t n)
    {
        while (n--)
            *(dst++) = *(src++);
    }

    void LSPString::drop_temp()
    {
        if (pTemp == NULL)
            return;

        if (pTemp->pData != NULL)
            free(pTemp->pData);

        free(pTemp);
        pTemp = NULL;
    }

    void LSPString::clear()
    {
        drop_temp();
        nLength     = 0;
    }

    void LSPString::truncate()
    {
        drop_temp();

        nLength     = 0;
        nCapacity   = 0;
        if (pData == NULL)
            return;

        xfree(pData);
        pData = NULL;
    }

    void LSPString::truncate(size_t size)
    {
        drop_temp();
        if (size > nCapacity)
            return;
        if (nLength > size)
            nLength = size;
        if (size > 0)
        {
            lsp_wchar_t *v = xrealloc(pData, size);
            if (v == NULL)
                return;
            pData       = v;
            nCapacity   = size;
        }
        else
        {
            free(pData);
            pData       = NULL;
            nLength     = 0;
            nCapacity   = 0;
        }
    }

    bool LSPString::reserve(size_t size)
    {
        if (size < nCapacity)
            return true;
        lsp_wchar_t *v = xrealloc(pData, size);
        if (v == NULL)
            return false;

        pData       = v;
        nCapacity   = size;
        return true;
    }

    inline bool LSPString::cap_reserve(size_t size)
    {
        return reserve((size + (GRANULARITY-1)) & (~(GRANULARITY-1)));
    }

    void LSPString::reduce()
    {
        drop_temp();
        if (nCapacity <= nLength)
            return;
        lsp_wchar_t *v = xrealloc(pData, nLength);
        if (v == NULL)
            return;
        pData       = v;
        nCapacity   = nLength;
    }

    void LSPString::trim()
    {
        if ((pData == NULL) || (nLength <= 0))
            return;

        // Cut tail first
        lsp_wchar_t *p = &pData[nLength];
        while (nLength > 0)
        {
            if (!is_space(*(--p)))
                break;
            nLength--;
        }
        if (nLength <= 0)
            return;

        // Cut head
        p = pData;
        while (true)
        {
            if (!is_space(*p))
                break;
            p++;
        }
        if (p > pData)
            nLength -= (p - pData);
        if (nLength <= 0)
            return;

        xmove(pData, p, nLength);
    }

    void LSPString::swap(LSPString *src)
    {
        size_t len      = src->nLength;
        size_t cap      = src->nCapacity;
        lsp_wchar_t *c  = src->pData;

        src->nLength    = nLength;
        src->nCapacity  = nCapacity;
        src->pData      = pData;

        nLength         = len;
        nCapacity       = cap;
        pData           = c;
    }

    bool LSPString::swap(ssize_t idx1, ssize_t idx2)
    {
        XSAFE_ITRANS(idx1, nLength, false);
        XSAFE_ITRANS(idx2, nLength, false);
        if (idx1 == idx2)
            return true;

        // Swap characters
        lsp_wchar_t c   = pData[idx1];
        pData[idx1]     = pData[idx2];
        pData[idx2]     = c;

        return true;
    }

    void LSPString::take(LSPString *src)
    {
        drop_temp();
        if (pData != NULL)
            xfree(pData);

        nLength         = src->nLength;
        nCapacity       = src->nCapacity;
        pData           = src->pData;

        src->nLength    = 0;
        src->nCapacity  = 0;
        src->pData      = NULL;
    }

    LSPString *LSPString::copy() const
    {
        LSPString *s = new LSPString();
        if (s == NULL)
            return s;

        s->nLength      = nLength;
        s->nCapacity    = nLength;
        if (s->nLength > 0)
        {
            s->pData        = xmalloc(nLength);
            if (s->pData == NULL)
            {
                delete s;
                return NULL;
            }

            xcopy(s->pData, pData, nLength);
        }
        else
            s->pData        = NULL;

        return s;
    }

    lsp_wchar_t LSPString::at(ssize_t index) const
    {
        if (index < 0)
        {
            if ((index += nLength) < 0)
                return 0;
        }
        else if (size_t(index) >= nLength)
            return 0;

        return pData[index];
    }

    bool LSPString::set(lsp_wchar_t ch)
    {
        drop_temp();

        if (nCapacity == 0)
        {
            lsp_wchar_t *v = xmalloc(GRANULARITY);
            if (v == NULL)
                return false;
            v[0]        = ch;
            pData       = v;
            nCapacity   = GRANULARITY;
        }
        else
            pData[0]    = ch;

        nLength     = 1;
        return true;
    }

    bool LSPString::set(const lsp_wchar_t *arr, size_t n)
    {
        drop_temp();

        if (!cap_reserve(n))
            return false;

        xcopy(pData, arr, n);
        nLength     = n;
        return true;
    }

    bool LSPString::set(ssize_t first, lsp_wchar_t ch)
    {
        XSAFE_ITRANS(first, nLength, false);
        pData[first]    = ch;
        return true;
    }

    bool LSPString::set(const LSPString *src)
    {
        if (src == this)
            return true;
        drop_temp();

        if (!cap_reserve(src->nLength))
            return false;
        if (src->nLength > 0)
            xcopy(pData, src->pData, src->nLength);
        nLength     = src->nLength;
        return true;
    }

    bool LSPString::set(const LSPString *src, ssize_t first)
    {
        drop_temp();

        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;

        if (length > 0)
        {
            if (!cap_reserve(length))
                return false;
            xcopy(pData, &src->pData[first], length);
            nLength     = length;
        }
        else
            nLength     = 0;
        return true;
    }

    bool LSPString::set(const LSPString *src, ssize_t first, ssize_t last)
    {
        drop_temp();

        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);

        ssize_t length = last - first;
        if (length > 0)
        {
            if (!cap_reserve(length))
                return false;
            xcopy(pData, &src->pData[first], length);
            nLength     = length;
        }
        else
            nLength     = 0;

        return true;
    }

    bool LSPString::insert(ssize_t pos, lsp_wchar_t ch)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (!cap_reserve(nLength + 1))
            return false;

        ssize_t length = nLength - pos;
        if (length > 0)
            xmove(&pData[pos+1], &pData[pos], length);

        pData[pos] = ch;
        nLength++;
        return true;
    }

    bool LSPString::insert(ssize_t pos, const lsp_wchar_t *arr, ssize_t n)
    {
        XSAFE_TRANS(pos, nLength, false);
        if (!cap_reserve(nLength + n))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+n], &pData[pos], count);
        xcopy(&pData[pos], arr, n);
        nLength    += n;

        return true;
    }

    bool LSPString::insert(ssize_t pos, const LSPString *src)
    {
        if (src->nLength <= 0)
            return true;

        XSAFE_TRANS(pos, nLength, false);
        if (!cap_reserve(nLength + src->nLength))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+src->nLength], &pData[pos], count);
        xcopy(&pData[pos], src->pData, src->nLength);
        nLength    += src->nLength;

        return true;
    }

    bool LSPString::insert(ssize_t pos, const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;
        if (length <= 0)
            return true;

        XSAFE_TRANS(pos, nLength, false);
        if (!cap_reserve(nLength + length))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+length], &pData[pos], count);
        xcopy(&pData[pos], &src->pData[first], length);
        nLength    += length;

        return true;
    }

    bool LSPString::insert(ssize_t pos, const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        XSAFE_TRANS(pos, nLength, false);
        if (!cap_reserve(nLength + length))
            return false;

        ssize_t count = nLength - pos;
        if (count > 0)
            xmove(&pData[pos+length], &pData[pos], count);
        xcopy(&pData[pos], &src->pData[first], length);
        nLength    += length;

        return true;
    }

    bool LSPString::append(lsp_wchar_t ch)
    {
        if (!cap_reserve(nLength + 1))
            return false;
        pData[nLength++] = ch;
        return true;
    }

    bool LSPString::append(const lsp_wchar_t *arr, size_t n)
    {
        if (!cap_reserve(nLength + n))
            return false;
        xcopy(&pData[nLength], arr, n);
        nLength += n;
        return true;
    }

    bool LSPString::append_ascii(const char *arr, size_t n)
    {
        if (!cap_reserve(nLength + n))
            return false;
        acopy(&pData[nLength], arr, n);
        nLength += n;
        return true;
    }

    bool LSPString::append_utf8(const char *arr, size_t n)
    {
        if (nLength <= 0)
            return set_utf8(arr, n);

        LSPString tmp;
        if (!tmp.set_utf8(arr, n))
            return false;
        return append(&tmp);
    }

    bool LSPString::append(const LSPString *src)
    {
        if (src->nLength <= 0)
            return true;
        if (!cap_reserve(nLength + src->nLength))
            return false;
        xcopy(&pData[nLength], src->pData, src->nLength);
        nLength += src->nLength;
        return true;
    }

    bool LSPString::append(const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;
        if (length <= 0)
            return true;

        if (!cap_reserve(nLength + length))
            return false;
        xcopy(&pData[nLength], &src->pData[first], length);
        nLength += length;
        return true;
    }

    bool LSPString::append(const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        if (!cap_reserve(nLength + length))
            return false;
        xcopy(&pData[nLength], &src->pData[first], length);
        nLength += length;
        return true;
    }

    bool LSPString::prepend(lsp_wchar_t ch)
    {
        if (!cap_reserve(nLength + 1))
            return false;
        if (nLength > 0)
            xmove(&pData[1], pData, nLength);
        pData[0]    = ch;
        nLength     ++;
        return true;
    }

    bool LSPString::prepend(const lsp_wchar_t *arr, size_t n)
    {
        if (n <= 0)
            return true;
        if (!cap_reserve(nLength + n))
            return false;
        if (nLength > 0)
            xmove(&pData[n], pData, nLength);
        xcopy(pData, arr, n);
        nLength += n;
        return true;
    }

    bool LSPString::prepend_ascii(const char *arr, size_t n)
    {
        if (n <= 0)
            return true;
        if (!cap_reserve(nLength + n))
            return false;
        if (nLength > 0)
            xmove(&pData[n], pData, nLength);
        acopy(pData, arr, n);
        nLength += n;
        return true;
    }

    bool LSPString::prepend_utf8(const char *arr, size_t n)
    {
        if (nLength <= 0)
            return set_utf8(arr, n);

        LSPString tmp;
        if (!tmp.set_utf8(arr, n))
            return false;
        return prepend(&tmp);
    }

    bool LSPString::prepend(const LSPString *src)
    {
        if (src->nLength <= 0)
            return true;
        if (!cap_reserve(nLength + src->nLength))
            return false;
        if (nLength > 0)
            xmove(&pData[src->nLength], pData, nLength);
        xcopy(pData, src->pData, src->nLength);
        nLength += src->nLength;
        return true;
    }

    bool LSPString::prepend(const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;
        if (length <= 0)
            return true;

        if (!cap_reserve(nLength + length))
            return false;
        if (nLength > 0)
            xmove(&pData[length], pData, nLength);
        xcopy(pData, &src->pData[first], length);
        nLength += length;
        return true;
    }

    bool LSPString::prepend(const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, src->nLength, false);
        XSAFE_TRANS(last, src->nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        if (!cap_reserve(nLength + length))
            return false;
        if (nLength > 0)
            xmove(&pData[length], pData, nLength);
        xcopy(pData, &src->pData[first], length);
        nLength += length;
        return true;
    }

    bool LSPString::ends_with(lsp_wchar_t ch) const
    {
        if (nLength <= 0)
            return false;
        return pData[nLength-1] == ch;
    }

    bool LSPString::ends_with_nocase(lsp_wchar_t ch) const
    {
        if (nLength <= 0)
            return false;
        return towlower(pData[nLength-1]) == towlower(ch);
    }

    bool LSPString::ends_with(const LSPString *src) const
    {
        if (src->nLength <= 0)
            return true;

        ssize_t offset = nLength - src->nLength;
        if (offset < 0)
            return false;

        return xcmp(&pData[offset], src->pData, src->nLength) == 0;
    }

    bool LSPString::ends_with_nocase(const LSPString *src) const
    {
        if (src->nLength <= 0)
            return true;

        ssize_t offset = nLength - src->nLength;
        if (offset < 0)
            return false;

        return xcasecmp(&pData[offset], src->pData, src->nLength) == 0;
    }

    bool LSPString::starts_with(lsp_wchar_t ch) const
    {
        if (nLength <= 0)
            return false;
        return pData[0] == ch;
    }

    bool LSPString::starts_with_nocase(lsp_wchar_t ch) const
    {
        if (nLength <= 0)
            return false;
        return towlower(pData[0]) == towlower(ch);
    }

    bool LSPString::starts_with(const LSPString *src) const
    {
        if (src->nLength <= 0)
            return true;

        if (nLength < src->nLength)
            return false;

        return xcasecmp(pData, src->pData, src->nLength) == 0;
    }

    bool LSPString::starts_with_nocase(const LSPString *src) const
    {
        if (src->nLength <= 0)
            return true;

        if (nLength < src->nLength)
            return false;

        return xcasecmp(pData, src->pData, src->nLength) == 0;
    }

    bool LSPString::remove()
    {
        drop_temp();
        nLength = 0;
        return true;
    }

    bool LSPString::remove(ssize_t first)
    {
        XSAFE_TRANS(first, nLength, false);
        nLength = first;
        return true;
    }

    bool LSPString::remove(ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t length = last - first;
        if (length <= 0)
            return true;

        ssize_t count = nLength - last;
        if (count > 0)
            xmove(&pData[first], &pData[last], count);

        nLength -= length;
        return true;
    }

    void LSPString::reverse()
    {
        drop_temp();

        size_t n = (nLength >> 1);
        lsp_wchar_t *h = pData, *t = &pData[nLength];
        while (n--)
        {
            lsp_wchar_t c = *h;
            *(h++)  = *(--t);
            *t      = c;
        }
    }

    void LSPString::shuffle()
    {
        if (nLength < 2)
            return;

        size_t n = nLength * 2;
        size_t idx1 = rand() % nLength, idx2;

        while (n--)
        {
            // Generate random indexes
            idx1 = (idx1 + rand()) % nLength;
            idx2 = (idx1 + rand()) % nLength;
            if (idx1 == idx2)
                continue;

            // Swap characters
            lsp_wchar_t c   = pData[idx1];
            pData[idx1]     = pData[idx2];
            pData[idx2]     = c;
        }
    }

    bool LSPString::replace(ssize_t pos, lsp_wchar_t ch)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (size_t(pos) < nLength)
        {
            pData[pos]  = ch;
            nLength     = pos;

            return true;
        }

        // Append at the tail
        return append(ch);
    }

    bool LSPString::replace(ssize_t pos, const lsp_wchar_t *arr, size_t n)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (!cap_reserve(pos + n))
            return false;

        xcopy(&pData[pos], arr, n);
        nLength =  pos + n;
        return true;
    }

    bool LSPString::replace(ssize_t pos, const LSPString *src)
    {
        XSAFE_TRANS(pos, nLength, false);

        if (!cap_reserve(pos + src->nLength))
            return false;

        xcopy(&pData[pos], src->pData, src->nLength);
        nLength =  pos + src->nLength;
        return true;
    }

    bool LSPString::replace(ssize_t pos, const LSPString *src, ssize_t first)
    {
        XSAFE_TRANS(pos, nLength, false);
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;

        if (length > 0)
        {
            if (!cap_reserve(pos + length))
                return false;

            xcopy(&pData[pos], &src->pData[first], length);
        }
        nLength =  pos + length;
        return true;
    }

    bool LSPString::replace(ssize_t pos, const LSPString *src, ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(pos, nLength, false);
        XSAFE_TRANS(first, src->nLength, false);
        ssize_t length = src->nLength - first;

        if (!cap_reserve(pos + length))
            return false;

        xcopy(&pData[pos], &src->pData[first], length);
        nLength =  pos + length;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, lsp_wchar_t ch)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = last - first;
        if (count < 0)
            count = 0;

        if (!cap_reserve(nLength - count + 1))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + 1], &pData[tail], nLength - tail);
        pData[first]    = ch;

        nLength = nLength - count + 1;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const lsp_wchar_t *arr, size_t n)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = last - first;
        if (count < 0)
            count = 0;

        if (!cap_reserve(nLength - count + n))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + n], &pData[tail], nLength - tail);
        if (n > 0)
            xcopy(&pData[first], arr, n);
        nLength = nLength - count + n;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const LSPString *src)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = last - first;
        if (count < 0)
            count = 0;

        if (!cap_reserve(nLength - count + src->nLength))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + src->nLength], &pData[tail], nLength - tail);
        if (src->nLength > 0)
            xcopy(&pData[first], src->pData, src->nLength);
        nLength = nLength - count + src->nLength;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const LSPString *src, ssize_t sfirst)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = last - first;
        if (count < 0)
            count = 0;

        XSAFE_TRANS(sfirst, src->nLength, false);
        ssize_t scount = src->nLength - sfirst;

        if (!cap_reserve(nLength - count + scount))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + scount], &pData[tail], nLength - tail);
        if (scount > 0)
            xcopy(&pData[first], &src->pData[sfirst], scount);
        nLength = nLength - count + scount;
        return true;
    }

    bool LSPString::replace(ssize_t first, ssize_t last, const LSPString *src, ssize_t sfirst, ssize_t slast)
    {
        XSAFE_TRANS(first, nLength, false);
        XSAFE_TRANS(last, nLength, false);
        ssize_t count = last - first;
        if (count < 0)
            count = 0;

        XSAFE_TRANS(sfirst, src->nLength, false);
        XSAFE_TRANS(slast, src->nLength, false);
        ssize_t scount = slast - sfirst;
        if (scount < 0)
            scount = 0;

        if (!cap_reserve(nLength - count + scount))
            return false;

        ssize_t tail = nLength - first - count;
        if (tail > 0)
            xmove(&pData[first + scount], &pData[tail], nLength - tail);
        if (scount > 0)
            xcopy(&pData[first], &src->pData[sfirst], scount);
        nLength = nLength - count + scount;
        return true;
    }

    ssize_t LSPString::index_of(ssize_t start, const LSPString *str) const
    {
        XSAFE_TRANS(start, nLength, -1);
        if (str->nLength <= 0)
            return start;

        ssize_t last = nLength - str->nLength;
        while (start < last)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start ++;
        }
        return -1;
    }

    ssize_t LSPString::index_of(const LSPString *str) const
    {
        if (str->nLength <= 0)
            return 0;

        ssize_t start = 0, last = nLength - str->nLength;
        while (start < last)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start ++;
        }
        return -1;
    }

    ssize_t LSPString::index_of(ssize_t start, lsp_wchar_t ch) const
    {
        XSAFE_TRANS(start, nLength, -1);

        ssize_t length = nLength;
        while (start < length)
        {
            if (pData[start] == ch)
                return start;
            start ++;
        }

        return -1;
    }

    ssize_t LSPString::index_of(lsp_wchar_t ch) const
    {
        size_t start = 0;

        while (start < nLength)
        {
            if (pData[start] == ch)
                return start;
            start ++;
        }

        return -1;
    }

    ssize_t LSPString::rindex_of(ssize_t start, const LSPString *str) const
    {
        XSAFE_ITRANS(start, nLength, -1);
        if (str->nLength <= 0)
            return start;

        start -= str->nLength;
        while (start >= 0)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start --;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of(const LSPString *str) const
    {
        if (str->nLength <= 0)
            return 0;

        ssize_t start = nLength - str->nLength - 1;
        while (start >= 0)
        {
            if (xcmp(&pData[start], str->pData, str->nLength) == 0)
                return start;
            start --;
        }
        return -1;
    }

    ssize_t LSPString::rindex_of(ssize_t start, lsp_wchar_t ch) const
    {
        XSAFE_ITRANS(start, nLength, -1);

        while (start >= 0)
        {
            if (pData[start] == ch)
                return start;
            start --;
        }

        return -1;
    }

    ssize_t LSPString::rindex_of(lsp_wchar_t ch) const
    {
        ssize_t start = nLength - 1;
        while (start >= 0)
        {
            if (pData[start] == ch)
                return start;
            start --;
        }

        return -1;
    }

    LSPString *LSPString::substring(ssize_t first) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        ssize_t length = nLength - first;

        LSPString *s = new LSPString();
        if (s == NULL)
            return s;

        s->nLength      = length;
        s->nCapacity    = length;

        if (length > 0)
        {
            s->pData        = xmalloc(length);
            if (s->pData == NULL)
            {
                delete s;
                return NULL;
            }

            xcopy(s->pData, &pData[first], length);
        }
        else
            s->pData        = NULL;

        return s;
    }

    LSPString *LSPString::substring(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        ssize_t length  = last - first;
        if (length < 0)
            length          = 0;

        LSPString *s = new LSPString();
        if (s == NULL)
            return s;

        s->nLength      = length;
        s->nCapacity    = length;

        if (length > 0)
        {
            s->pData        = xmalloc(length);
            if (s->pData == NULL)
            {
                delete s;
                return NULL;
            }

            xcopy(s->pData, &pData[first], length);
        }
        else
            s->pData        = NULL;

        return s;
    }

    int LSPString::compare_to(const LSPString *src) const
    {
        ssize_t n = (nLength > src->nLength) ? src->nLength : nLength;
        const lsp_wchar_t *a = pData, *b = src->pData;

        while (n--)
        {
            int retval = int(*(a++)) - int(*(b++));
            if (retval != 0)
                return retval;
        }

        if (a < &pData[nLength])
            return int(*a);
        else if (b < &src->pData[src->nLength])
            return -int(*b);

        return 0;
    }

    int LSPString::compare_to_nocase(const LSPString *src) const
    {
        ssize_t n = (nLength > src->nLength) ? src->nLength : nLength;
        const lsp_wchar_t *a = pData, *b = src->pData;

        while (n--)
        {
            int retval = int(towlower(*(a++))) - int(towlower(*(b++)));
            if (retval != 0)
                return retval;
        }

        if (a < &pData[nLength])
            return int(*a);
        else if (b < &src->pData[src->nLength])
            return -int(*b);

        return 0;
    }

    size_t LSPString::tolower()
    {
        for (size_t i=0; i<nLength; ++i)
            pData[i] = towlower(pData[i]);
        return nLength;
    }

    size_t LSPString::tolower(ssize_t first)
    {
        XSAFE_TRANS(first, nLength, 0);
        ssize_t n = nLength - first;
        lsp_wchar_t *ptr = &pData[first];
        for (ssize_t i=0; i<n; ++i)
            ptr[i] = towlower(ptr[i]);
        return (n >= 0) ? n : 0;
    }

    size_t LSPString::tolower(ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, nLength, 0);
        XSAFE_TRANS(last, nLength, 0);
        if (last < first)
        {
            ssize_t tmp = last;
            last = first;
            first = tmp;
        }
        ssize_t n = last - first;
        lsp_wchar_t *ptr = &pData[first];
        for (; first < last; ++first)
            ptr[first] = towlower(ptr[first]);
        return n;
    }

    size_t LSPString::toupper()
    {
        for (size_t i=0; i<nLength; ++i)
            pData[i] = towupper(pData[i]);
        return nLength;
    }

    size_t LSPString::toupper(ssize_t first)
    {
        XSAFE_TRANS(first, nLength, 0);
        ssize_t n = nLength - first;
        lsp_wchar_t *ptr = &pData[first];
        for (ssize_t i=0; i<n; ++i)
            ptr[i] = towupper(ptr[i]);
        return (n >= 0) ? n : 0;
    }

    size_t LSPString::toupper(ssize_t first, ssize_t last)
    {
        XSAFE_TRANS(first, nLength, 0);
        XSAFE_TRANS(last, nLength, 0);
        if (last < first)
        {
            ssize_t tmp = last;
            last = first;
            first = tmp;
        }
        ssize_t n = last - first;
        lsp_wchar_t *ptr = &pData[first];
        for (; first < last; ++first)
            ptr[first] = towupper(ptr[first]);
        return n;
    }

    bool LSPString::equals(const LSPString *src) const
    {
        if (nLength != src->nLength)
            return false;
        if (nLength <= 0)
            return true;

        return xcmp(pData, src->pData, nLength) == 0;
    }

    bool LSPString::equals_nocase(const LSPString *src) const
    {
        if (nLength != src->nLength)
            return false;

        const lsp_wchar_t *a = pData, *b = src->pData;
        for (size_t i=nLength; i>0; --i)
        {
            if (towlower(*(a++)) != towlower(*(b++)))
                return false;
        }

        return true;
    }

    bool LSPString::set_utf8(const char *s, size_t n)
    {
        const char *l   = &s[n];
        LSPString   tmp;

        while (s < l)
        {
            uint8_t v   = *(s++);
            if (v <= 0x7f) // 1 byte: 0xxxxxxx
            {
                if (!tmp.append(v))
                    return false;
            }
            else if ((v & 0xe0) == 0xc0) // 2 bytes: 110xxxxx 10xxxxxx
            {
                if (s >= l)
                    return false;

                lsp_wchar_t ch  = ((v & 0x1f) << 6);
                ch             |= ((*(s++)) & 0x3f);
                if (!tmp.append(ch))
                    return false;
            }
            else if ((v & 0xf0) == 0xe0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
            {
                if ((l - s) < 2)
                    return false;

                lsp_wchar_t ch  = ((v & 0x0f) << 12);
                ch             |= ((*(s++)) & 0x3f) << 6;
                ch             |= ((*(s++)) & 0x3f);
                if (!tmp.append(ch))
                    return false;
            }
            else
                return false;
        }

        take(&tmp);
        return true;
    }

    bool LSPString::set_native(const char *s, ssize_t n, const char *charset)
    {
        char buf[BUF_SIZE];
        LSPString temp;

        // Open conversion
        iconv_t cd = init_iconv_to_wchar_t(charset);
        if (cd == iconv_t(-1))
            return false;

        size_t insize   = (n < 0) ? strlen(s) : n;
        size_t outsize  = BUF_SIZE;
        char *inbuf     = const_cast<char *>(s);
        char *outbuf    = buf;

        while (insize > 0)
        {
            // Do the conversion
            size_t nconv = iconv(cd, &inbuf, &insize, &outbuf, &outsize);

            if (nconv == (size_t) -1)
            {
                switch (errno)
                {
                    case E2BIG:
                    case EINVAL:
                        break;
                    default:
                        iconv_close(cd);
                        return false;
                }
            }

            // Append set of converted characters to string
            ssize_t n_chars = (BUF_SIZE - outsize) / sizeof(lsp_wchar_t);
            if (n_chars > 0)
            {
                if (!temp.append(reinterpret_cast<lsp_wchar_t *>(&buf[0]), n_chars))
                {
                    iconv_close(cd);
                    return false;
                }
            }

            size_t right = n_chars * sizeof(lsp_wchar_t);
            ssize_t tail = (outsize - right) % sizeof(lsp_wchar_t);
            if (tail > 0)
            {
                // If there is a tail, copy it to the start of buffer
                ::memmove(buf, &buf[right], tail);
                outbuf  = &buf[tail];
                outsize = BUF_SIZE - tail;
            }
            else
            {
                // Otherwise just reset buffer's pointer
                outbuf  = buf;
                outsize = BUF_SIZE;
            }
        }

        // Close descriptor
        iconv_close(cd);
        take(&temp);
        return true;
    }

    bool LSPString::set_ascii(const char *s, size_t n)
    {
        LSPString   tmp;
        if (!tmp.reserve(n))
            return false;

        acopy(tmp.pData, s, n);
        take(&tmp);
        nLength = n;
        return true;
    }

    const char *LSPString::get_utf8(ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first >= last)
            return (last == first) ? "" : NULL;

        if (pTemp != NULL)
            pTemp->nOffset      = 0;

        char temp[BUF_SIZE + 16];
        char *th = temp, *tt = &temp[BUF_SIZE];

        for (ssize_t i=first; i<last; ++i)
        {
            lsp_wchar_t ch = pData[i];

            if (ch < 0x80) // 1 byte
                *(th++) = ch;
            else if (ch >= 0x800) // 3 bytes
            {
                th[0]   = (ch >> 12) | 0xe0;
                th[1]   = ((ch >> 6) & 0x3f) | 0x80;
                th[2]   = (ch & 0x3f) | 0x80;
                th += 3;
            }
            else // 2 bytes
            {
                th[0]   = (ch >> 6) | 0xc0;
                th[1]   = (ch & 0x3f) | 0x80;
                th += 2;
            }

            if (th >= tt)
            {
                if (!append_temp(temp, th - temp))
                    return NULL;
                th  = temp;
            }
        }

        *(th++) = '\0';
        if (!append_temp(temp, th - temp))
            return NULL;

        return pTemp->pData;
    }

    const char *LSPString::get_utf8(ssize_t first) const
    {
        return get_utf8(first, nLength);
    }

    const char *LSPString::get_utf8() const
    {
        return get_utf8(0, nLength);
    }

    const char *LSPString::get_ascii() const
    {
        if (!resize_temp(nLength + 1))
            return NULL;

        lsp_wchar_t *p  = pData;
        size_t n        = nLength;
        char *dst       = pTemp->pData;

        while (n--)
        {
            lsp_wchar_t c   = *(p++);
            *(dst++)        = (c <= 0x7f) ? c : 0xff;
        }

        *(dst++)        = '\0';
        pTemp->nOffset  = dst - pTemp->pData;

        return pTemp->pData;
    }

    const char *LSPString::get_native(const char *charset) const
    {
        return get_native(0, nLength, charset);
    }

    const char *LSPString::get_native(ssize_t first, const char *charset) const
    {
        return get_native(first, nLength, charset);
    }

    const char *LSPString::get_native(ssize_t first, ssize_t last, const char *charset) const
    {
        XSAFE_TRANS(first, nLength, NULL);
        XSAFE_TRANS(last, nLength, NULL);
        if (first >= last)
            return (last == first) ? "" : NULL;

        // Open conversion
        iconv_t cd = init_iconv_from_wchar_t(charset);
        if (cd == iconv_t(-1))
            return NULL;

        // Analyze temp
        size_t outsize  = 0;
        char *outbuf    = NULL;
        if (pTemp != NULL)
        {
            pTemp->nOffset      = 0;
            outsize             = pTemp->nLength;
            outbuf              = pTemp->pData;
        }

        size_t insize   = (last - first) * sizeof(lsp_wchar_t);
        char *inbuf     = reinterpret_cast<char *>(const_cast<lsp_wchar_t *>(&pData[first]));

        while (insize > 0)
        {
            // Reserve space if there is not enough space
            if (outsize < 16)
            {
                // Try to grow the temprary buffer
                if (!grow_temp(BUF_SIZE))
                {
                    iconv_close(cd);
                    return NULL;
                }

                // Initialize location of buffers to store data
                outsize         = pTemp->nLength - pTemp->nOffset;
                outbuf          = &pTemp->pData[pTemp->nOffset];
            }

            // Do the conversion
            size_t nconv = iconv(cd, &inbuf, &insize, &outbuf, &outsize);
            if (nconv == (size_t) -1)
            {
                switch (errno)
                {
                    case E2BIG:
                    case EINVAL:
                        break;
                    default:
                        iconv_close (cd);
                        return NULL;
                }
            }

            // Update pointer
            pTemp->nOffset      = pTemp->nLength - outsize;
        }

        // Close the iconv descriptor
        iconv_close(cd);

        // Append zeros at the end to make compatible with C-strings
        if (!append_temp("\x00\x00\x00\x00", 4))
            return NULL;

        return pTemp->pData;
    }

    bool LSPString::append_temp(const char *p, size_t n) const
    {
        ssize_t free = (pTemp != NULL) ? pTemp->nLength - pTemp->nOffset : -1;

        if (free < ssize_t(n))
        {
            size_t resize   = n + (n >> 1);
            if (pTemp != NULL)
                resize         += pTemp->nLength;

            if (!resize_temp(resize))
                return false;
        }

        memcpy(&pTemp->pData[pTemp->nOffset], p, n * sizeof(char));
        pTemp->nOffset     += n;

        return true;
    }

    bool LSPString::grow_temp(size_t n) const
    {
        if (pTemp == NULL)
        {
            pTemp = static_cast<buffer_t *>(malloc(sizeof(buffer_t)));
            if (pTemp == NULL)
                return false;
            pTemp->nLength  = 0;
            pTemp->nOffset  = 0;
            pTemp->pData    = 0;
        }

        char *xc        = static_cast<char *>(realloc(pTemp->pData, (pTemp->nLength + n)*sizeof(char)));
        if (xc == NULL)
            return false;

        pTemp->pData    = xc;
        pTemp->nLength += n;
        return true;
    }

    bool LSPString::resize_temp(size_t n) const
    {
        if (pTemp == NULL)
        {
            pTemp = static_cast<buffer_t *>(malloc(sizeof(buffer_t)));
            if (pTemp == NULL)
                return false;
            pTemp->nLength  = 0;
            pTemp->nOffset  = 0;
            pTemp->pData    = 0;
        }

        char *xc        = static_cast<char *>(realloc(pTemp->pData, n*sizeof(char)));
        if (xc == NULL)
            return false;

        pTemp->pData    = xc;
        pTemp->nLength  = n;
        return true;
    }

    size_t LSPString::count(lsp_wchar_t ch) const
    {
        size_t n = 0;
        for (size_t i=0; i<nLength; ++i)
            if (pData[i] == ch)
                ++n;
        return n;
    }

    size_t LSPString::count(lsp_wchar_t ch, ssize_t first) const
    {
        XSAFE_TRANS(first, nLength, 0);

        size_t n = 0;
        for (size_t i=first; i<nLength; ++i)
            if (pData[i] == ch)
                ++n;
        return n;
    }

    size_t LSPString::count(lsp_wchar_t ch, ssize_t first, ssize_t last) const
    {
        XSAFE_TRANS(first, nLength, 0);
        XSAFE_TRANS(last, nLength, 0);

        size_t n = 0;
        if (first < last)
        {
            for (ssize_t i=first; i<last; ++i)
                if (pData[i] == ch)
                    ++n;
        }
        else
        {
            for (ssize_t i=last; i<first; ++i)
                if (pData[i] == ch)
                    ++n;
        }
        return n;
    }

    bool LSPString::fmt_append_native(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;

        va_start(vl, fmt);
        bool res = tmp.vfmt_native(fmt, vl);
        va_end(vl);
        if (res)
            res = append(&tmp);
        return res;
    }

    bool LSPString::fmt_preend_native(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;

        va_start(vl, fmt);
        bool res = tmp.vfmt_native(fmt, vl);
        va_end(vl);
        if (res)
            res = prepend(&tmp);
        return res;
    }

    bool LSPString::fmt_native(const char *fmt...)
    {
        va_list vl;
        va_start(vl, fmt);
        bool res = vfmt_native(fmt, vl);
        va_end(vl);

        return res;
    }

    bool LSPString::vfmt_append_native(const char *fmt, va_list args)
    {
        LSPString tmp;
        if (!tmp.vfmt_native(fmt, args))
            return false;
        return append(&tmp);
    }

    bool LSPString::vfmt_preend_native(const char *fmt, va_list args)
    {
        LSPString tmp;
        if (!tmp.vfmt_native(fmt, args))
            return false;
        return prepend(&tmp);
    }

    bool LSPString::vfmt_native(const char *fmt, va_list args)
    {
        char *ptr = NULL;
        int count = vasprintf(&ptr, fmt, args);
        if (ptr == NULL)
            return false;

        bool res = set_native(ptr, count);
        free(ptr);
        return res;
    }

    bool LSPString::fmt_append_ascii(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;

        va_start(vl, fmt);
        bool res = tmp.vfmt_ascii(fmt, vl);
        va_end(vl);
        if (res)
            res = append(&tmp);
        return res;
    }

    bool LSPString::fmt_prepend_ascii(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;

        va_start(vl, fmt);
        bool res = tmp.vfmt_ascii(fmt, vl);
        va_end(vl);
        if (res)
            res = prepend(&tmp);
        return res;
    }

    bool LSPString::fmt_ascii(const char *fmt...)
    {
        va_list vl;
        va_start(vl, fmt);
        bool res = vfmt_ascii(fmt, vl);
        va_end(vl);

        return res;
    }

    bool LSPString::vfmt_append_ascii(const char *fmt, va_list args)
    {
        LSPString tmp;
        if (!tmp.vfmt_ascii(fmt, args))
            return false;
        return append(&tmp);
    }

    bool LSPString::vfmt_prepend_ascii(const char *fmt, va_list args)
    {
        LSPString tmp;
        if (!tmp.vfmt_ascii(fmt, args))
            return false;
        return prepend(&tmp);
    }

    bool LSPString::vfmt_ascii(const char *fmt, va_list args)
    {
        char *ptr = NULL;
        int count = vasprintf(&ptr, fmt, args);
        if (ptr == NULL)
            return false;

        bool res = set_ascii(ptr, count);
        free(ptr);
        return res;
    }

    bool LSPString::fmt_append_utf8(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;

        va_start(vl, fmt);
        bool res = tmp.vfmt_utf8(fmt, vl);
        va_end(vl);
        if (res)
            res = append(&tmp);
        return res;
    }

    bool LSPString::fmt_prepend_utf8(const char *fmt...)
    {
        LSPString tmp;
        va_list vl;

        va_start(vl, fmt);
        bool res = tmp.vfmt_utf8(fmt, vl);
        va_end(vl);
        if (res)
            res = prepend(&tmp);
        return res;
    }

    bool LSPString::fmt_utf8(const char *fmt...)
    {
        va_list vl;
        va_start(vl, fmt);
        bool res = vfmt_utf8(fmt, vl);
        va_end(vl);

        return res;
    }

    bool LSPString::vfmt_append_utf8(const char *fmt, va_list args)
    {
        LSPString tmp;
        if (!tmp.vfmt_utf8(fmt, args))
            return false;
        return append(&tmp);
    }

    bool LSPString::vfmt_prepend_utf8(const char *fmt, va_list args)
    {
        LSPString tmp;
        if (!tmp.vfmt_utf8(fmt, args))
            return false;
        return prepend(&tmp);
    }

    bool LSPString::vfmt_utf8(const char *fmt, va_list args)
    {
        char *ptr = NULL;
        int count = vasprintf(&ptr, fmt, args);
        if (ptr == NULL)
            return false;

        bool res = set_utf8(ptr, count);
        free(ptr);
        return res;
    }
} /* namespace lsp */
