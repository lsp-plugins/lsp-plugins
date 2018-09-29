/*
 * LSPString.h
 *
 *  Created on: 30 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPSTRING_H_
#define UI_TK_UTIL_LSPSTRING_H_

#include <core/types.h>
#include <stdarg.h>

namespace lsp
{
    /**
     * String class
     */
    class LSPString
    {
        protected:
            typedef struct buffer_t
            {
                size_t      nOffset;
                size_t      nLength;
                char       *pData;
            } buffer_t;

        protected:
            size_t              nLength;
            size_t              nCapacity;
            lsp_wchar_t        *pData;
            mutable buffer_t   *pTemp;

        protected:
            inline bool cap_reserve(size_t size);
            void drop_temp();
            bool append_temp(const char *p, size_t n) const;
            bool resize_temp(size_t n) const;
            bool grow_temp(size_t n) const;

            static inline lsp_wchar_t *xmalloc(size_t size) { return reinterpret_cast<lsp_wchar_t *>(malloc(size * sizeof(lsp_wchar_t))); }
            static inline lsp_wchar_t *xrealloc(lsp_wchar_t * ptr, size_t size) { return reinterpret_cast<lsp_wchar_t *>(realloc(ptr, size * sizeof(lsp_wchar_t))); };
            static inline void xfree(lsp_wchar_t *ptr) { free(ptr); }
            static inline void xmove(lsp_wchar_t *dst, const lsp_wchar_t *src, size_t n) { memmove(dst, src, n * sizeof(lsp_wchar_t)); }
            static inline void xcopy(lsp_wchar_t *dst, const lsp_wchar_t *src, size_t n) { memcpy(dst, src, n * sizeof(lsp_wchar_t)); }
            static inline int xcmp(const lsp_wchar_t *a, const lsp_wchar_t *b, size_t n) { return memcmp(a, b, n * sizeof(lsp_wchar_t)); }
            static int xcasecmp(const lsp_wchar_t *a, const lsp_wchar_t *b, size_t n);
            static inline void acopy(lsp_wchar_t *dst, const char *src, size_t n);

        public:
            explicit LSPString();
            ~LSPString();

        public:
            /** Get the length of the string
             *
             * @return the length of the string
             */
            inline size_t length() const { return nLength; }

            /** Get the capacity of the string
             *
             * @return capacity of the string
             */
            inline size_t capacity() const { return nCapacity; }

            /** Check whether the string is emtpy
             *
             * @return true if string is empty
             */
            inline bool is_empty() const { return nLength <= 0; }

            /** Get pointer to internal non-zero-terminated characters array
             * This method is unsafe and is provided only for low-level optimizations
             * You should know what you're doing when calling this method
             *
             * @return pointer to internal to internal non-zero-terminated characters array
             */
            inline const lsp_wchar_t *characters() const { return pData; }

            /** Reserve additional capacity at the tail for further modifications
             *
             * @param size number of characters to use as capacity
             * @return true on success
             */
            bool reserve(size_t size);

            /**
             * Clear the string without deallocating internal buffer
             */
            void clear();

            /** Truncate the string (and remove extra capacity)
             *
             */
            void truncate();
            void truncate(size_t size);

            /** Try to reduce memory allocated by the string (remove extra capacity)
             *
             */
            void reduce();

            /**
             * Trim spaces at the start of line and at the end of line
             */
            void trim();

            /** Swap internal contents with other string
             *
             * @param src string to perform the swap
             */
            void swap(LSPString *src);

            /** Swap two characters
             *
             * @param idx1 character at first position
             * @param idx2 character at second position
             * @return true on success
             */
            bool swap(ssize_t idx1, ssize_t idx2);

            /** Take data from another string and empty that string
             *
             * @param src source string
             */
            void take(LSPString *src);

            /** Give data to another string and empty self
             *
             * @param dst target string
             */
            inline void give(LSPString *dst) { dst->take(this); }

            /** Copy this string
             *
             * @return copy of the string or NULL on error
             */
            LSPString *copy() const;
            inline LSPString *clone() const { return copy(); }

            /** Insert data at specified position
             *
             * @param pos position
             * @param src source string to insert
             * @return true on success
             */
            bool insert(ssize_t pos, lsp_wchar_t ch);
            bool insert(ssize_t pos, const lsp_wchar_t *arr, ssize_t n);
            bool insert(ssize_t pos, const LSPString *src);
            bool insert(ssize_t pos, const LSPString *src, ssize_t first);
            bool insert(ssize_t pos, const LSPString *src, ssize_t first, ssize_t last);

            /** Append data at the tail of string
             *
             * @param pos position
             * @param src source string to insert
             * @return true on success
             */
            bool append(lsp_wchar_t ch);
            bool append_ascii(const char *arr, size_t n);
            bool append_utf8(const char *arr, size_t n);
            bool append(const lsp_wchar_t *arr, size_t n);
            bool append(const LSPString *src);
            bool append(const LSPString *src, ssize_t first);
            bool append(const LSPString *src, ssize_t first, ssize_t last);
            inline bool append_ascii(const char *arr) { return append_ascii(arr, strlen(arr)); };
            inline bool append_utf8(const char *arr) { return append_utf8(arr, strlen(arr)); };

            bool prepend(lsp_wchar_t ch);
            bool prepend(const lsp_wchar_t *arr, size_t n);
            bool prepend_utf8(const char *arr, size_t n);
            bool prepend_ascii(const char *arr, size_t n);
            bool prepend(const LSPString *src);
            bool prepend(const LSPString *src, ssize_t first);
            bool prepend(const LSPString *src, ssize_t first, ssize_t last);
            inline bool prepend_ascii(const char *arr) { return prepend_ascii(arr, strlen(arr)); };
            inline bool prepend_utf8(const char *arr) { return prepend_ascii(arr, strlen(arr)); };

            /** Get unicode character at the specified position
             *
             * @param index index
             * @return character or 0 on error
             */
            lsp_wchar_t     at(ssize_t index) const;
            lsp_wchar_t     char_at(ssize_t index) const { return at(index); }

            /** Reverse the order of characters
             *
             */
            void reverse();

            /** Shuffle characters
             *
             */
            void shuffle();

            /** Copy contents from another string
             * @param src source string
             */
            bool set(lsp_wchar_t ch);
            bool set(ssize_t pos, lsp_wchar_t ch);
            bool set(const lsp_wchar_t *arr, size_t n);
            bool set(const LSPString *src);
            bool set(const LSPString *src, ssize_t first);
            bool set(const LSPString *src, ssize_t first, ssize_t last);

            /** Different conversion routines
             *
             * @param s pointer to UTF-8 string
             * @param n number of octets in the string
             * @return true on success
             */
            bool set_utf8(const char *s, size_t n);
            bool set_ascii(const char *s, size_t n);
            bool set_native(const char *s, ssize_t n, const char *charset = NULL);
            inline bool set_utf8(const char *s) { return set_utf8(s, strlen(s)); };
            inline bool set_ascii(const char *s) { return set_ascii(s, strlen(s)); };
            inline bool set_native(const char *s, const char *charset) { return set_native(s, strlen(s), charset); };
            inline bool set_native(const char *s) { return set_native(s, strlen(s), NULL); };

            const char *get_utf8() const;
            const char *get_utf8(ssize_t first) const;
            const char *get_utf8(ssize_t first, ssize_t last) const;

            const char *get_ascii() const;
            const char *get_native(const char *charset =  NULL) const;
            const char *get_native(ssize_t first, const char *charset =  NULL) const;
            const char *get_native(ssize_t first, ssize_t last, const char *charset =  NULL) const;

            inline size_t temporal_size() const     { return (pTemp != NULL) ? pTemp->nOffset : 0; };
            inline size_t temporal_capacity() const { return (pTemp != NULL) ? pTemp->nLength : 0; };

            /** Replace the contents of the string
             *
             */
            bool replace(ssize_t pos, lsp_wchar_t ch);
            bool replace(ssize_t pos, const lsp_wchar_t *arr, size_t n);
            bool replace(ssize_t pos, const LSPString *src);
            bool replace(ssize_t pos, const LSPString *src, ssize_t start);
            bool replace(ssize_t pos, const LSPString *src, ssize_t first, ssize_t last);

            bool replace(ssize_t first, ssize_t last, lsp_wchar_t ch);
            bool replace(ssize_t first, ssize_t last, const lsp_wchar_t *arr, size_t n);
            bool replace(ssize_t first, ssize_t last, const LSPString *src);
            bool replace(ssize_t first, ssize_t last, const LSPString *src, ssize_t sfirst);
            bool replace(ssize_t first, ssize_t last, const LSPString *src, ssize_t sfirst, ssize_t slast);


            /** Check ending and start
             *
             */
            bool ends_with(lsp_wchar_t ch) const;
            bool ends_with(const LSPString *src) const;
            bool ends_with_nocase(lsp_wchar_t ch) const;
            bool ends_with_nocase(const LSPString *src) const;

            bool starts_with(lsp_wchar_t ch) const;
            bool starts_with(const LSPString *src) const;
            bool starts_with_nocase(lsp_wchar_t ch) const;
            bool starts_with_nocase(const LSPString *src) const;

            /** Delete character sequence from the string
             *
             */
            bool remove();
            bool remove(ssize_t first);
            bool remove(ssize_t first, ssize_t last);

            /** Try to find substring in a string
             *
             * @param start start location to begin search
             * @param str string to search
             * @return index of character or negative value
             */
            ssize_t index_of(ssize_t start, const LSPString *str) const;
            ssize_t index_of(const LSPString *str) const;
            ssize_t rindex_of(ssize_t start, const LSPString *str) const;
            ssize_t rindex_of(const LSPString *str) const;

            ssize_t index_of(ssize_t start, lsp_wchar_t ch) const;
            ssize_t index_of(lsp_wchar_t ch) const;
            ssize_t rindex_of(ssize_t start, lsp_wchar_t ch) const;
            ssize_t rindex_of(lsp_wchar_t ch) const;

            /** Produce new object as substring of a string
             *
             * @param start start character
             * @param last end character
             * @return
             */
            LSPString *substring(ssize_t first, ssize_t last) const;
            LSPString *substring(ssize_t first) const;

            /** Compare to another string
             *
             * @param src string to compare to
             * @return result of comparison
             */
            int compare_to(const LSPString *src) const;
            int compare_to_nocase(const LSPString *src) const;

            size_t tolower();
            size_t tolower(ssize_t first);
            size_t tolower(ssize_t first, ssize_t last);

            size_t toupper();
            size_t toupper(ssize_t first);
            size_t toupper(ssize_t first, ssize_t last);

            /** Check that string equals
             *
             * @param src string to compare
             * @return true if equals
             */
            bool equals(const LSPString *src) const;
            bool equals_nocase(const LSPString *src) const;

            /** Calculate number of character occurences
             *
             * @param ch character
             * @return number of character occurences
             */
            size_t count(lsp_wchar_t ch) const;
            size_t count(lsp_wchar_t ch, ssize_t first) const;
            size_t count(lsp_wchar_t ch, ssize_t first, ssize_t last) const;

            /**
             * Formatting functions, internally asprintf functin is used
             */
            bool fmt_append_native(const char *fmt...);
            bool fmt_preend_native(const char *fmt...);
            bool fmt_native(const char *fmt...);

            bool vfmt_append_native(const char *fmt, va_list args);
            bool vfmt_preend_native(const char *fmt, va_list args);
            bool vfmt_native(const char *fmt, va_list args);

            bool fmt_append_ascii(const char *fmt...);
            bool fmt_prepend_ascii(const char *fmt...);
            bool fmt_ascii(const char *fmt...);

            bool vfmt_append_ascii(const char *fmt, va_list args);
            bool vfmt_prepend_ascii(const char *fmt, va_list args);
            bool vfmt_ascii(const char *fmt, va_list args);

            bool fmt_append_utf8(const char *fmt...);
            bool fmt_prepend_utf8(const char *fmt...);
            bool fmt_utf8(const char *fmt...);

            bool vfmt_append_utf8(const char *fmt, va_list args);
            bool vfmt_prepend_utf8(const char *fmt, va_list args);
            bool vfmt_utf8(const char *fmt, va_list args);
    };
    
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPSTRING_H_ */
