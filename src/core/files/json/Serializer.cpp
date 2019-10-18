/*
 * Serializer.cpp
 *
 *  Created on: 17 окт. 2019 г.
 *      Author: sadko
 */

#include <core/io/File.h>
#include <core/io/OutFileStream.h>
#include <core/io/OutStringSequence.h>
#include <core/io/OutSequence.h>
#include <core/files/json/Serializer.h>

namespace lsp
{
    namespace json
    {
        
        Serializer::Serializer()
        {
            pOut            = NULL;
            nWFlags         = 0;
            sState.mode     = WRITE_ROOT;
            sState.flags    = 0;
            sState.ident    = 0;
        }
        
        Serializer::~Serializer()
        {
            close();
        }

        status_t Serializer::open(const char *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        status_t Serializer::open(const LSPString *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        status_t Serializer::open(const io::Path *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        void Serializer::copy_settings(const serial_flags_t *flags)
        {
            if (flags != NULL)
            {
                sSettings       = *flags;
                return;
            }

            sSettings.version           = JSON_LEGACY;
            sSettings.identifiers       = false;
            sSettings.ident             = ' ';
            sSettings.padding           = 0;
            sSettings.separator         = false;
            sSettings.multiline         = false;
        }

        status_t Serializer::wrap(LSPString *str, const serial_flags_t *settings)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence *seq = new io::OutStringSequence(str, false);
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = wrap(seq, settings, WRAP_CLOSE | WRAP_DELETE);
            if (res == STATUS_OK)
                return res;

            seq->close();
            delete seq;

            return res;
        }

        status_t Serializer::wrap(io::IOutStream *os, const serial_flags_t *settings, size_t flags, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (os == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutSequence *seq = new io::OutSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(os, flags, charset);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, settings, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t Serializer::wrap(io::IOutSequence *seq, const serial_flags_t *settings, size_t flags)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (seq == NULL)
                return STATUS_BAD_ARGUMENTS;

            pOut            = seq;
            nWFlags         = flags;
            sState.mode     = WRITE_ROOT;
            sState.flags    = 0;
            copy_settings(settings);

            return STATUS_OK;
        }

        status_t Serializer::close()
        {
            status_t res = STATUS_OK;

            if (pOut != NULL)
            {
                if (nWFlags & WRAP_CLOSE)
                {
                    status_t xres = pOut->close();
                    if (res == STATUS_OK)
                        res = xres;
                }

                if (nWFlags & WRAP_DELETE)
                    delete pOut;

                pOut = NULL;
            }

            return res;
        }

        status_t Serializer::push_state(pmode_t mode)
        {
            if (!sStack.add(&sState))
                return STATUS_NO_MEM;

            sState.mode     = mode;
            sState.flags    = 0;
            sState.ident   += sSettings.padding;
            return STATUS_OK;
        }

        status_t Serializer::pop_state()
        {
            state_t *st = sStack.last();
            if (st == NULL)
                return STATUS_BAD_STATE;
            sState          = *st;
            return (sStack.remove_last()) ? STATUS_OK : STATUS_BAD_STATE;
        }

        status_t Serializer::new_line()
        {
            status_t res = (sSettings.multiline) ? pOut->write('\n') : STATUS_OK;
            if (res != STATUS_OK)
                return res;
            for (size_t i=0, n=sState.ident; i<n; ++i)
            {
                if ((res = pOut->write(sSettings.ident)) != STATUS_OK)
                    break;
            }
            return res;
        }
    
        status_t Serializer::write(const event_t *event)
        {
            if (event == NULL)
                return STATUS_BAD_ARGUMENTS;

            switch (event->type)
            {
                case JE_OBJECT_START:   return start_object();
                case JE_OBJECT_END:     return end_object();
                case JE_ARRAY_START:    return start_array();
                case JE_ARRAY_END:      return end_array();
                case JE_PROPERTY:       return write_property(&event->sValue);
                case JE_STRING:         return write_string(&event->sValue);
                case JE_INTEGER:        return write_int(event->iValue);
                case JE_DOUBLE:         return write_double(event->fValue);
                case JE_BOOL:           return write_bool(event->bValue);
                case JE_NULL:           return write_null();
                default:                break;
            }

            return STATUS_BAD_ARGUMENTS;
        }

        status_t Serializer::write_raw(const char *buf, int len)
        {
            status_t res;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_CORRUPTED;
                    break;
                case WRITE_ARRAY:
                    res = (sState.flags & SF_VALUE) ? pOut->write(',') : STATUS_OK;
                    if (res == STATUS_OK)
                        res = new_line();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_CORRUPTED;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            if (res != STATUS_OK)
                return res;
            sState.flags   |= SF_VALUE;

            // Output data to the sequence
            return (res == STATUS_OK) ? pOut->write_ascii(buf, len) : res;
        }

        status_t Serializer::write_int(ssize_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%lld", (long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_hex(ssize_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_BAD_ARGUMENTS;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "0x%llx", (long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_double(double value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%f", value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_double(double value, const char *fmt)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char *buf = NULL;
            int len = ::asprintf(&buf, fmt, value);
            if (buf == NULL)
                return STATUS_NO_MEM;

            status_t res = (len <= 0) ? write_raw(buf, len) : STATUS_NO_DATA;
            ::free(buf);
            return res;
        }

        status_t Serializer::write_bool(bool value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            return (value) ? write_raw("true", 4) : write_raw("false", 5);
        }

        status_t Serializer::write_null()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            return write_raw("null", 4);
        }

        status_t Serializer::write_string(const char *value, const char *charset)
        {
            if (value == NULL)
                return write_null();

            LSPString tmp;
            return (tmp.set_native(value, charset)) ? write_string(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_string(const LSPString *value)
        {
            if (value == NULL)
                return write_null();
            else if (pOut == NULL)
                return STATUS_BAD_STATE;

            // TODO
            return STATUS_OK;
        }

        status_t Serializer::write_comment(const char *value, const char *charset)
        {
            LSPString tmp;
            return (tmp.set_native(value, charset)) ? write_comment(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_comment(const LSPString *value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_BAD_ARGUMENTS;

            return STATUS_OK;
        }

        status_t Serializer::write_property(const char *name, const char *charset)
        {
            LSPString tmp;
            return (tmp.set_native(name, charset)) ? write_property(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_property(const LSPString *name)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Serializer::start_object()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_CORRUPTED;
                    break;
                case WRITE_ARRAY:
                    res = (sState.flags & SF_VALUE) ? pOut->write(',') : STATUS_OK;
                    if (res == STATUS_OK)
                        res = new_line();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_CORRUPTED;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            if (res != STATUS_OK)
                return res;
            sState.flags   |= SF_VALUE;

            res = pOut->write('{');
            return (res == STATUS_OK) ? push_state(WRITE_OBJECT) : res;
        }

        status_t Serializer::end_object()
        {
            if ((pOut == NULL) || (sState.mode != WRITE_OBJECT) || (sState.flags & SF_PROPERTY))
                return STATUS_BAD_STATE;

            status_t res = pop_state();
            if ((res == STATUS_OK) && (sState.flags & SF_VALUE))
                res         = new_line();
            return (res == STATUS_OK) ? pOut->write('}') : res;
        }

        status_t Serializer::start_array()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_CORRUPTED;
                    break;
                case WRITE_ARRAY:
                    res = (sState.flags & SF_VALUE) ? pOut->write(',') : STATUS_OK;
                    if (res == STATUS_OK)
                        res = new_line();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_CORRUPTED;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            if (res != STATUS_OK)
                return res;
            sState.flags   |= SF_VALUE;

            res = pOut->write('[');
            return (res == STATUS_OK) ? push_state(WRITE_ARRAY) : res;
        }

        status_t Serializer::end_array()
        {
            if ((pOut == NULL) || (sState.mode != WRITE_OBJECT))
                return STATUS_BAD_STATE;

            status_t res = pop_state();
            if ((res == STATUS_OK) && (sState.flags & SF_VALUE))
                res         = new_line();
            return (res == STATUS_OK) ? pOut->write('}') : res;
        }

    } /* namespace json */
} /* namespace lsp */
