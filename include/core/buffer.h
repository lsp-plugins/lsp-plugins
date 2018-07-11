/*
 * buffer.h
 *
 *  Created on: 12 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_BUFFER_H_
#define CORE_BUFFER_H_

#include <core/types.h>

namespace lsp
{
    typedef struct buffer_t
    {
        char       *pString;
        size_t      nCapacity;
        size_t      nLength;
    } buffer_t;

    bool init_buf(buffer_t *buf);
    bool init_buf(buffer_t *buf, size_t reserve);
    bool append_buf(buffer_t *buf, const char *str, size_t count);
    bool append_buf(buffer_t *buf, const char *str);
    bool append_buf(buffer_t *buf, char ch);
    bool extend_buf(buffer_t *buf, size_t reserve);
    void destroy_buf(buffer_t *buf);
    void clear_buf(buffer_t *buf);
    char *release_buf(buffer_t *buf);

}

#endif /* CORE_BUFFER_H_ */
