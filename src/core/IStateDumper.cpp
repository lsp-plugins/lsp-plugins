/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 8 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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

    void IStateDumper::begin_object(const char *name, const void *ptr, size_t szof)
    {
    }

    void IStateDumper::begin_object(const void *ptr, size_t szof)
    {
    }

    void IStateDumper::end_object()
    {
    }

    void IStateDumper::begin_array(const char *name, const void *ptr, size_t count)
    {
    }

    void IStateDumper::begin_array(const void *ptr, size_t count)
    {
    }

    void IStateDumper::end_array()
    {
    }

    void IStateDumper::write(const void *value)
    {
    }

    void IStateDumper::write(const char *value)
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

    void IStateDumper::write(const char *name, const char *value)
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

    void IStateDumper::writev(const void * const *value, size_t count)
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

    void IStateDumper::writev(const char *name, const void * const *value, size_t count)
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
