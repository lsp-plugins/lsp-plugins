/*
 * wrappers.h
 *
 *  Created on: 6 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_WRAPPERS_H_
#define CORE_FILES_JAVA_WRAPPERS_H_

#include <core/files/java/const.h>
#include <core/files/java/Object.h>

namespace lsp
{
    namespace java
    {
        #define WRAPPER_DEF(name, type_t) \
            class name: public Object \
            { \
                public: \
                    static const char *CLASS_NAME; \
                \
                protected: \
                    virtual status_t to_string_padded(LSPString *dst, size_t pad); \
                \
                public: \
                    explicit name(); \
                    virtual ~name(); \
                \
                public: \
                    type_t value() const; \
                    status_t get_value(type_t *dst) const; \
            }

        WRAPPER_DEF(Byte, byte_t);
        WRAPPER_DEF(Short, short_t);
        WRAPPER_DEF(Integer, int_t);
        WRAPPER_DEF(Long, long_t);
        WRAPPER_DEF(Double, double_t);
        WRAPPER_DEF(Float, float_t);
        WRAPPER_DEF(Boolean, bool_t);
        WRAPPER_DEF(Character, char_t);

        #undef WRAPPER_DEF
    }
}


#endif /* CORE_FILES_JAVA_WRAPPERS_H_ */
