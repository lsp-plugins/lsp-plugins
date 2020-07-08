/*
 * JsonDumper.h
 *
 *  Created on: 8 июл. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_JSONDUMPER_H_
#define CORE_JSONDUMPER_H_

#include <core/IStateDumper.h>
#include <core/files/json/Serializer.h>

namespace lsp
{
    class JsonDumper: public IStateDumper
    {
        private:
            JsonDumper & operator = (const JsonDumper &);

        protected:
            json::Serializer sOut;

        public:
            explicit JsonDumper();
            virtual ~JsonDumper();

        public:
            status_t    open(const char *path);
            status_t    open(const LSPString *path);
            status_t    close();

        public:
            virtual void start_object(const char *name, const void *ptr, size_t szof);
            virtual void start_object(const void *ptr, size_t szof);
            virtual void end_object();

            virtual void start_array(const char *name, const void *ptr, size_t length);
            virtual void start_array(const void *ptr, size_t length);
            virtual void end_array();

            virtual void write(const void *value);
            virtual void write(bool value);
            virtual void write(uint8_t value);
            virtual void write(int8_t value);
            virtual void write(uint16_t value);
            virtual void write(int16_t value);
            virtual void write(uint32_t value);
            virtual void write(int32_t value);
            virtual void write(uint64_t value);
            virtual void write(int64_t value);
            virtual void write(float value);
            virtual void write(double value);

            virtual void write(const char *name, const void *value);
            virtual void write(const char *name, bool value);
            virtual void write(const char *name, uint8_t value);
            virtual void write(const char *name, int8_t value);
            virtual void write(const char *name, uint16_t value);
            virtual void write(const char *name, int16_t value);
            virtual void write(const char *name, uint32_t value);
            virtual void write(const char *name, int32_t value);
            virtual void write(const char *name, uint64_t value);
            virtual void write(const char *name, int64_t value);
            virtual void write(const char *name, float value);
            virtual void write(const char *name, double value);

            virtual void writev(const bool *value, size_t count);
            virtual void writev(const uint8_t *value, size_t count);
            virtual void writev(const int8_t *value, size_t count);
            virtual void writev(const uint16_t *value, size_t count);
            virtual void writev(const int16_t *value, size_t count);
            virtual void writev(const uint32_t *value, size_t count);
            virtual void writev(const int32_t *value, size_t count);
            virtual void writev(const uint64_t *value, size_t count);
            virtual void writev(const int64_t *value, size_t count);
            virtual void writev(const float *value, size_t count);
            virtual void writev(const double *value, size_t count);

            virtual void writev(const char *name, const bool *value, size_t count);
            virtual void writev(const char *name, const uint8_t *value, size_t count);
            virtual void writev(const char *name, const int8_t *value, size_t count);
            virtual void writev(const char *name, const uint16_t *value, size_t count);
            virtual void writev(const char *name, const int16_t *value, size_t count);
            virtual void writev(const char *name, const uint32_t *value, size_t count);
            virtual void writev(const char *name, const int32_t *value, size_t count);
            virtual void writev(const char *name, const uint64_t *value, size_t count);
            virtual void writev(const char *name, const int64_t *value, size_t count);
            virtual void writev(const char *name, const float *value, size_t count);
            virtual void writev(const char *name, const double *value, size_t count);
    };

} /* namespace lsp */

#endif /* CORE_JSONDUMPER_H_ */
