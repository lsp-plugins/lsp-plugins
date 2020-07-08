/*
 * IStateDumper.cpp
 *
 *  Created on: 8 июл. 2020 г.
 *      Author: sadko
 */

#include <core/IStateDumper.h>

namespace lsp
{
    
    IStateDumper::IStateDumper()
    {
    }
    
    IStateDumper::~IStateDumper()
    {
    }

    void IStateDumper::start_object(const char *name, const void *ptr, size_t szof)
    {
    }

    void IStateDumper::start_object(const void *ptr, size_t szof)
    {
    }

    void IStateDumper::end_object()
    {
    }

    void IStateDumper::start_array(const char *name, const void *ptr, size_t count)
    {
    }

    void IStateDumper::start_array(const void *ptr, size_t count)
    {
    }

    void IStateDumper::end_array()
    {
    }

    void IStateDumper::write(const void *value)
    {
    }

    void IStateDumper::write(bool value)
    {
    }

    void IStateDumper::write(uint8_t value)
    {
    }

    void IStateDumper::write(int8_t value)
    {
    }

    void IStateDumper::write(uint16_t value)
    {
    }

    void IStateDumper::write(int16_t value)
    {
    }

    void IStateDumper::write(uint32_t value)
    {
    }

    void IStateDumper::write(int32_t value)
    {
    }

    void IStateDumper::write(uint64_t value)
    {
    }

    void IStateDumper::write(int64_t value)
    {
    }

    void IStateDumper::write(float value)
    {
    }

    void IStateDumper::write(double value)
    {
    }

    void IStateDumper::write(const char *name, const void *value)
    {
    }

    void IStateDumper::write(const char *name, bool value)
    {
    }

    void IStateDumper::write(const char *name, uint8_t value)
    {
    }

    void IStateDumper::write(const char *name, int8_t value)
    {
    }

    void IStateDumper::write(const char *name, uint16_t value)
    {
    }

    void IStateDumper::write(const char *name, int16_t value)
    {
    }

    void IStateDumper::write(const char *name, uint32_t value)
    {
    }

    void IStateDumper::write(const char *name, int32_t value)
    {
    }

    void IStateDumper::write(const char *name, uint64_t value)
    {
    }

    void IStateDumper::write(const char *name, int64_t value)
    {
    }

    void IStateDumper::write(const char *name, float value)
    {
    }

    void IStateDumper::write(const char *name, double value)
    {
    }

    void IStateDumper::writev(const bool *value, size_t count)
    {
    }

    void IStateDumper::writev(const uint8_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const int8_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const uint16_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const int16_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const uint32_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const int32_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const uint64_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const int64_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const float *value, size_t count)
    {
    }

    void IStateDumper::writev(const double *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const bool *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const uint8_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const int8_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const uint16_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const int16_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const uint32_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const int32_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const uint64_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const int64_t *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const float *value, size_t count)
    {
    }

    void IStateDumper::writev(const char *name, const double *value, size_t count)
    {
    }


} /* namespace lsp */
