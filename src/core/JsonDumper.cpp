/*
 * JsonDumper.cpp
 *
 *  Created on: 8 июл. 2020 г.
 *      Author: sadko
 */

#include <core/JsonDumper.h>
#include <core/stdlib/stdio.h>

namespace lsp
{
    
    JsonDumper::JsonDumper()
    {
    }
    
    JsonDumper::~JsonDumper()
    {
        close();
    }

    status_t JsonDumper::open(const char *path)
    {
        json::serial_flags_t flags;
        flags.version       = json::JSON_LEGACY;
        flags.identifiers   = false;
        flags.ident         = ' ';
        flags.padding       = 4;
        flags.separator     = true;
        flags.multiline     = true;

        return sOut.open(path, &flags);
    }

    status_t JsonDumper::open(const LSPString *path)
    {
        json::serial_flags_t flags;
        flags.version       = json::JSON_LEGACY;
        flags.identifiers   = false;
        flags.ident         = ' ';
        flags.padding       = 4;
        flags.separator     = true;
        flags.multiline     = true;

        return sOut.open(path, &flags);
    }

    status_t JsonDumper::close()
    {
        return sOut.close();
    }

    void JsonDumper::start_object(const char *name, const void *ptr, size_t szof)
    {
        sOut.write_property(name);
        sOut.start_object();
        write("this", ptr);
        write("sizeof", szof);
        sOut.write_property("data");
        sOut.start_object();
    }

    void JsonDumper::start_object(const void *ptr, size_t szof)
    {
        sOut.start_object();
        write("this", ptr);
        write("sizeof", szof);
        sOut.write_property("data");
        sOut.start_object();
    }

    void JsonDumper::end_object()
    {
        sOut.end_object();
    }

    void JsonDumper::start_array(const char *name, const void *ptr, size_t count)
    {
        sOut.write_property(name);
        sOut.start_object();
        write("this", ptr);
        write("length", count);
        sOut.write_property("data");
        sOut.start_array();
    }

    void JsonDumper::start_array(const void *ptr, size_t count)
    {
        sOut.start_object();
        write("this", ptr);
        write("length", count);
        sOut.write_property("data");
        sOut.start_array();
    }

    void JsonDumper::end_array()
    {
        sOut.end_array();
        sOut.end_object();
    }

    void JsonDumper::write(const void *value)
    {
        if (value == NULL)
        {
            sOut.write_null();
            return;
        }

        char buf[0x40];
        snprintf(buf, sizeof(buf), "*%p", value);
        sOut.write_string(buf);
    }

    void JsonDumper::write(bool value)
    {
        sOut.write_bool(value);
    }

    void JsonDumper::write(uint8_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(int8_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(uint16_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(int16_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(uint32_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(int32_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(uint64_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(int64_t value)
    {
        sOut.write_int(value);
    }

    void JsonDumper::write(float value)
    {
        sOut.write_double(value, "%g");
    }

    void JsonDumper::write(double value)
    {
        sOut.write_double(value, "%g");
    }

    void JsonDumper::write(const char *name, const void *value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, bool value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, uint8_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, int8_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, uint16_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, int16_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, uint32_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, int32_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, uint64_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, int64_t value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, float value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::write(const char *name, double value)
    {
        sOut.write_property(name);
        write(value);
    }

    void JsonDumper::writev(const bool *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const uint8_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const int8_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const uint16_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const int16_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const uint32_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const int32_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const uint64_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const int64_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const float *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const double *value, size_t count)
    {
        if (value == NULL)
        {
            write(value);
            return;
        }

        start_array(value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const bool *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const uint8_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const int8_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const uint16_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const int16_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const uint32_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const int32_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const uint64_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const int64_t *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const float *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

    void JsonDumper::writev(const char *name, const double *value, size_t count)
    {
        if (value == NULL)
        {
            write(name, value);
            return;
        }
        start_array(name, value, count);
        for (size_t i=0; i<count; ++i)
            write(value[i]);
        end_array();
    }

} /* namespace lsp */
