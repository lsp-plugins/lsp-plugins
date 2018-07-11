/*
 * common.h
 *
 *  Created on: 07 апр. 2016 г.
 *      Author: sadko
 */

#ifndef TEST_COMMON_H_
#define TEST_COMMON_H_

#include <stdio.h>

namespace test
{
    void dump_data(const float *data, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.3f", data[i]);
        }
        printf("\n");
    }
}

#endif /* TEST_COMMON_H_ */
