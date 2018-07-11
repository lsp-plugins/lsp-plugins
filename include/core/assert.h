/*
 * assert.h
 *
 *  Created on: 6 окт. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_ASSERT_H_
#define CORE_ASSERT_H_


#define LSP_STATUS_ASSERT(x, ...) { status_t __res__ = (x); if (__res__ != STATUS_OK) { __VA_ARGS__; return __res__; } }
#define LSP_VSTATUS_ASSERT(x, ...) { status_t __res__ = (x); if (__res__ != STATUS_OK) { __VA_ARGS__; return; } }
#define LSP_BOOL_ASSERT(x, res, ...) { if (!(x)) { __VA_ARGS__; return res; } }


#endif /* CORE_ASSERT_H_ */
