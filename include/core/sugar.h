/*
 * sugar.h
 *
 *  Created on: 17 марта 2016 г.
 *      Author: sadko
 */

#ifndef CORE_SUGAR_H_
#define CORE_SUGAR_H_

// Alignment
#define DEFAULT_ALIGN                   0x10
#define ALIGN_SIZE(x, size)             (((x) + size - 1) & (~size_t(size - 1)))

// Array management
#define DROP_ARRAY(ptr)     \
    if (ptr != NULL) \
    { \
        delete [] ptr; \
        ptr = NULL; \
    }



#endif /* CORE_SUGAR_H_ */
