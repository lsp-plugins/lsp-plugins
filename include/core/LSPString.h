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
            bool            size_reserve(size_t size);
            inline bool     cap_reserve(size_t size);
            inline bool     cap_grow(size_t delta);
            void            drop_temp();
            bool            append_temp(const char *p, size_t n) const;
            bool            resize_temp(size_t n) const;
            bool            grow_temp(size_t n) const;

            static inline lsp_wchar_t *xmalloc(size_t size) { return reinterpret_cast<lsp_wchar_t *>(::malloc(size * sizeof(lsp_wchar_t))); }
            static inline lsp_wchar_t *xrealloc(lsp_wchar_t * ptr, size_t size) { return reinterpret_cast<lsp_wchar_t *>(::realloc(ptr, size * sizeof(lsp_wchar_t))); };
            static inline void xfree(lsp_wchar_t *ptr) { ::free(ptr); }
            static inline void xmove(lsp_wchar_t *dst, const lsp_wchar_t *src, size_t n) { ::memmove(dst, src, n * sizeof(lsp_wchar_t)); }
            static inline size_t xlen(const lsp_wchar_t *s);

#ifdef ARCH_LE
            static inline int xcmp(const lsp_wchar_t *a, const lsp_wchar_t *b, size_t n) { return ::memcmp(a, b, n * sizeof(lsp_wchar_t)); }
#else
            static int xcmp(const lsp_wchar_t *a, const lsp_wchar_t *b, size_t n);
#endif /* ARCH_LE */
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

            /**
             * Set the length of the string, allows only to cut data from tail
             * @param length the length of the string
             * @return the length of the string after applied operarion
             */
            size_t set_length(size_t length);

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
             * @return pointer to internal to internal non-null-terminated characters array
             */
            inline const lsp_wchar_t *characters() const { return pData; }

            /** Reserve additional capacity at the tail for further modifications
             *
             * @param size number of characters to use as capacity
             * @return true on success
             */
            inline bool reserve(size_t size) { return (size > nCapacity) ? size_reserve(size) : true; };

            /**
             * Clear the string without deallocating internal buffer
             */
            void clear();

            /** Truncate the string (and remove extra capacity)
             *
             */
            void truncate();
            bool truncate(size_t size);

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
            LSPString *copy(ssize_t first) const;
            LSPString *copy(ssize_t first, ssize_t last) const;
            inline LSPString *clone() const { return copy(); }
            inline LSPString *clone(ssize_t first) const { return copy(first); }
            inline LSPString *clone(ssize_t first, ssize_t last) const { return copy(first, last); }

            /** Allocate instance of string and drain self data to this instance
             *
             * @return not null pointer is there is enough memory
             */
            LSPString *release();

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
            bool append(char ch);
            bool append(lsp_wchar_t ch);
            bool append(lsp_swchar_t ch);
            bool append_ascii(const char *arr, size_t n);
            bool append_utf8(const char *arr, size_t n);
            bool append(const lsp_wchar_t *arr, size_t n);
            bool append(const LSPString *src);
            bool append(const LSPString *src, ssize_t first);
            bool append(const LSPString *src, ssize_t first, ssize_t last);
            inline bool append_ascii(const char *arr) { return append_ascii(arr, ::strlen(arr)); };
            inline bool append_utf8(const char *arr) { return append_utf8(arr, ::strlen(arr)); };

            bool prepend(lsp_wchar_t ch);
            bool prepend(const lsp_wchar_t *arr, size_t n);
            bool prepend_utf8(const char *arr, size_t n);
            bool prepend_ascii(const char *arr, size_t n);
            bool prepend(const LSPString *src);
            bool prepend(const LSPString *src, ssize_t first);
            bool prepend(const LSPString *src, ssize_t first, ssize_t last);
            inline bool prepend_ascii(const char *arr) { return prepend_ascii(arr, ::strlen(arr)); };
            inline bool prepend_utf8(const char *arr) { return prepend_ascii(arr, ::strlen(arr)); };

            /** Get unicode character at the specified position
             *
             * @param index index
             * @return character or 0 on error
             */
            lsp_wchar_t     at(ssize_t index) const;
            lsp_wchar_t     char_at(ssize_t index) const { return at(index); }

            /**
             * Get first character
             * @return first character on 0 on error
             */
            lsp_wchar_t     first() const;

            /**
             * Get last character
             * @return last character on 0 on error
             */
            lsp_wchar_t     last() const;

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
            inline bool set(char ch)        { return set(lsp_wchar_t(uint8_t(ch))); };
            bool set(ssize_t pos, lsp_wchar_t ch);
            inline bool set_at(ssize_t pos, lsp_wchar_t ch) { return set(pos, ch); }
            bool set(const lsp_wchar_t *arr);
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
            inline bool set_utf8(const char *s) { return set_utf8(s, ::strlen(s)); };

            bool set_utf16(const lsp_utf16_t *s, size_t n);
            bool set_utf16(const lsp_utf16_t *s);

            bool set_ascii(const char *s, size_t n);
            inline bool set_ascii(const char *s) { return set_ascii(s, ::strlen(s)); };

            bool set_native(const char *s, size_t n, const char *charset = NULL);
            inline bool set_native(const char *s, const char *charset) { return set_native(s, ::strlen(s), charset); };
            inline bool set_native(const char *s) { return set_native(s, ::strlen(s), NULL); };

            const char *get_utf8(ssize_t first, ssize_t last) const;
            inline const char *get_utf8(ssize_t first) const { return get_utf8(first, nLength); };
            const char *get_utf8() const { return get_utf8(0, nLength); };

            const lsp_utf16_t *get_utf16(ssize_t first, ssize_t last) const;
            inline const lsp_utf16_t *get_utf16(ssize_t first) const { return get_utf16(first, nLength); };
            const lsp_utf16_t *get_utf16() const { return get_utf16(0, nLength); };

            const char *get_ascii(ssize_t first, ssize_t last) const;
            inline const char *get_ascii(ssize_t first) const { return get_ascii(first, nLength); };
            inline const char *get_ascii() const { return get_ascii(0, nLength); };

            const char *get_native(ssize_t first, ssize_t last, const char *charset =  NULL) const;
            inline const char *get_native(const char *charset = NULL) const { return get_native(0, nLength, charset); }
            inline const char *get_native(ssize_t first, const char *charset =  NULL) const { return get_native(first, nLength, charset); }

            inline size_t temporal_size() const     { return (pTemp != NULL) ? pTemp->nOffset : 0; };
            inline size_t temporal_capacity() const { return (pTemp != NULL) ? pTemp->nLength : 0; };

            /**
             * Clone string as set of UTF-8 characters. 1 character at the tail will contain additional end-of-line.
             */
            char *clone_utf8(size_t *bytes, ssize_t first, ssize_t last) const;
            inline char *clone_utf8(size_t *bytes, ssize_t first) const { return clone_utf8(bytes, first, nLength); };
            inline char *clone_utf8(size_t *bytes) const { return clone_utf8(bytes, 0, nLength); };
            inline char *clone_utf8(ssize_t first, ssize_t last) const { return clone_utf8(NULL, first, last); };
            inline char *clone_utf8(ssize_t first) const { return clone_utf8(NULL, first, nLength); };
            inline char *clone_utf8() const { return clone_utf8(NULL, 0, nLength); };

            /**
             * Clone string as set of UTF-16 characters. 1 UTF-16 character at the tail will contain additional end-of-line.
             */
            lsp_utf16_t *clone_utf16(size_t *bytes, ssize_t first, ssize_t last) const;
            inline lsp_utf16_t *clone_utf16(size_t *bytes, ssize_t first) const { return clone_utf16(bytes, first, nLength); };
            inline lsp_utf16_t *clone_utf16(size_t *bytes) const { return clone_utf16(bytes, 0, nLength); };
            inline lsp_utf16_t *clone_utf16(ssize_t first, ssize_t last) const { return clone_utf16(NULL, first, last); };
            inline lsp_utf16_t *clone_utf16(ssize_t first) const { return clone_utf16(NULL, first, nLength); };
            inline lsp_utf16_t *clone_utf16() const { return clone_utf16(NULL, 0, nLength); };

            /**
             * Clone string as set of UTF-16 characters. 1 ascii character at the tail will contain additional end-of-line.
             */
            char *clone_ascii(size_t *bytes, ssize_t first, ssize_t last) const;
            inline char *clone_ascii(size_t *bytes, ssize_t first) const { return clone_ascii(bytes, first, nLength); };
            inline char *clone_ascii(size_t *bytes) const { return clone_ascii(bytes, 0, nLength); };
            inline char *clone_ascii(ssize_t first, ssize_t last) const { return clone_ascii(NULL, first, last); };
            inline char *clone_ascii(ssize_t first) const { return clone_ascii(NULL, first, nLength); };
            inline char *clone_ascii() const { return clone_ascii(NULL, 0, nLength); };

            /**
             * Clone string as set of UTF-16 characters. 4 ascii characters at the tail will contain additional end-of-line.
             */
            char *clone_native(size_t *bytes, ssize_t first, ssize_t last, const char *charset = NULL) const;
            inline char *clone_native(size_t *bytes, ssize_t first, const char *charset =  NULL) const { return clone_native(bytes, first, nLength, charset); }
            inline char *clone_native(size_t *bytes, const char *charset = NULL) const { return clone_native(bytes, 0, nLength, charset); }
            inline char *clone_native(ssize_t first, ssize_t last, const char *charset = NULL) const { return clone_native(NULL, first, last, charset); }
            inline char *clone_native(const char *charset = NULL) const { return clone_native(NULL, 0, nLength, charset); }
            inline char *clone_native(ssize_t first, const char *charset =  NULL) const { return clone_native(NULL, first, nLength, charset); }

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

            /**
             * Replace all characters in the string with the replacement
             * @param ch character to replace
             * @param rep replacement character
             * @return number of replacements
             */
            size_t replace_all(lsp_wchar_t ch, lsp_wchar_t rep);
            inline size_t replace_all(char ch, char rep) { return replace_all(lsp_wchar_t(uint8_t(ch)), lsp_wchar_t(uint8_t(rep))); };
            inline size_t replace_all(char ch, lsp_wchar_t rep) { return replace_all(lsp_wchar_t(uint8_t(ch)), rep); };
            inline size_t replace_all(lsp_wchar_t ch, char rep) { return replace_all(ch, lsp_wchar_t(uint8_t(rep))); };

            /** Check ending and start
             *
             */
            bool ends_with(lsp_wchar_t ch) const;
            inline bool ends_with(char ch) const { return ends_with(lsp_wchar_t(uint8_t(ch))); };
            bool ends_with(const LSPString *src) const;
            bool ends_with_nocase(lsp_wchar_t ch) const;
            inline bool ends_with_nocase(char ch) const { return ends_with(lsp_wchar_t(uint8_t(ch))); };
            bool ends_with_nocase(const LSPString *src) const;

            bool starts_with(lsp_wchar_t ch) const;
            inline bool starts_with(char ch) const { return starts_with(lsp_wchar_t(uint8_t(ch))); };
            bool starts_with(lsp_wchar_t ch, size_t offset) const;
            inline bool starts_with(char ch, size_t offset) const { return starts_with(lsp_wchar_t(uint8_t(ch)), offset); };

            bool starts_with(const LSPString *src) const;
            bool starts_with_ascii(const char *src) const;
            bool starts_with(const LSPString *src, size_t offset) const;
            bool starts_with_ascii(const char *src, size_t offsset) const;

            bool starts_with_nocase(lsp_wchar_t ch) const;
            inline bool starts_with_nocase(char ch) const { return starts_with_nocase(lsp_wchar_t(uint8_t(ch))); };
            bool starts_with_nocase(lsp_wchar_t ch, size_t offset) const;
            inline bool starts_with_nocase(char ch, size_t offset) const { return starts_with_nocase(lsp_wchar_t(uint8_t(ch)), offset); };

            bool starts_with_nocase(const LSPString *src) const;
            bool starts_with_nocase(const LSPString *src, size_t offset) const;
            bool starts_with_ascii_nocase(const char *src) const;
            bool starts_with_ascii_nocase(const char *src, size_t offset) const;

            /** Delete character sequence from the string
             *
             */
            bool remove();
            bool remove(ssize_t first);
            bool remove(ssize_t first, ssize_t last);

            bool remove_last();

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
            inline ssize_t index_of(ssize_t start, char ch) const { return index_of(start, lsp_wchar_t(uint8_t(ch))); };
            ssize_t index_of(lsp_wchar_t ch) const;
            inline ssize_t index_of(char ch) const { return index_of(lsp_wchar_t(uint8_t(ch))); };
            ssize_t rindex_of(ssize_t start, lsp_wchar_t ch) const;
            inline ssize_t rindex_of(ssize_t start, char ch) const { return rindex_of(start, lsp_wchar_t(uint8_t(ch))); };
            ssize_t rindex_of(lsp_wchar_t ch) const;
            inline ssize_t rindex_of(char ch) const { return rindex_of(lsp_wchar_t(uint8_t(ch))); };

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
            int compare_to(const lsp_wchar_t *src) const;
            int compare_to(const lsp_wchar_t *src, size_t n) const;
            inline int compare_to(const LSPString *src) const { return compare_to(src->pData, src->nLength); };
            int compare_to_ascii(const char *src) const;
            int compare_to_utf8(const char *src) const;

            int compare_to_nocase(const lsp_wchar_t *src) const;
            int compare_to_nocase(const lsp_wchar_t *src, size_t n) const;
            inline int compare_to_nocase(const LSPString *src) const { return compare_to_nocase(src->pData, src->nLength); };
            int compare_to_ascii_nocase(const char *src) const;
            int compare_to_utf8_nocase(const char *src) const;

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
            bool equals(const lsp_wchar_t *src) const;
            bool equals(const lsp_wchar_t *src, size_t len) const;
            inline bool equals(const LSPString *src) const { return equals(src->pData, src->nLength); };

            bool equals_nocase(const lsp_wchar_t *src) const;
            bool equals_nocase(const lsp_wchar_t *src, size_t len) const;
            inline bool equals_nocase(const LSPString *src) const { return equals_nocase(src->pData, src->nLength); };

            inline bool equals_ascii(const char *src) const { return compare_to_ascii(src) == 0; };
            inline bool equals_ascii_nocase(const char *src) const { return compare_to_ascii_nocase(src) == 0; };

            inline bool equals_utf8(const char *src) const { return compare_to_utf8(src) == 0; };
            inline bool equals_utf8_nocase(const char *src) const { return compare_to_utf8_nocase(src) == 0; };

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
