/*
 * CtlSwitchedPort.cpp
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <ui/ui.h>
#include <ui/plugin_ui.h>

namespace lsp
{
    namespace ctl
    {
        CtlSwitchedPort::CtlSwitchedPort(plugin_ui *ui): CtlPort(NULL)
        {
            pUI             = ui;
            nDimensions     = 0;
            vControls       = NULL;
            pReference      = NULL;
            sName           = NULL;
            sTokens         = NULL;
        }

        CtlSwitchedPort::~CtlSwitchedPort()
        {
            destroy();
        }

        CtlSwitchedPort::token_t *CtlSwitchedPort::tokenize(const char *path)
        {
            buffer_t buf;
            if (!init_buf(&buf))
                return NULL;

            while (path != NULL)
            {
                char c = *path;
                if (c == '\0')
                    return reinterpret_cast<token_t *>(buf.pString);

                if (c == '[')
                {
                    path    ++;
                    const char *end   = strchr(path, ']');
                    if (end == NULL)
                    {
                        destroy_buf(&buf);
                        return NULL;
                    }

                    if (!append_buf(&buf, char(TT_INDEX)))
                        break;
                    if (!append_buf(&buf, path, end - path))
                        break;
                    if (!append_buf(&buf, char('\0')))
                        break;
                    path    = end + 1;
                }
                else
                {
                    const char *end   = strchr(path + 1, '[');
                    if (end == NULL)
                        end         = path + strlen(path);

                    if (!append_buf(&buf, char(TT_STRING)))
                        break;
                    if (!append_buf(&buf, path, end - path))
                        break;
                    if (!append_buf(&buf, char('\0')))
                        break;
                    path    = end;
                }
            }

            destroy_buf(&buf);
            return NULL;
        }

        CtlSwitchedPort::token_t *CtlSwitchedPort::next_token(token_t *token)
        {
            if (token == NULL)
                return NULL;
            size_t len  = strlen(token->data);
            return reinterpret_cast<token_t *>(reinterpret_cast<char *>(token) + len + 2);
        }

        void CtlSwitchedPort::rebind()
        {
            // Unbind from referenced ports
            if (pReference != NULL)
            {
                pReference->unbind(this);
                pMetadata       = NULL;
            }

            // Initialize buffer
            buffer_t buf;
            if (!init_buf(&buf))
                return;

            // Generate port name
            size_t ctl_id   = 0;
            token_t *tok    = sTokens;
            while (tok->type != TT_END)
            {
                if (tok->type == TT_INDEX)
                {
                    char tmp_buf[32];
                    int index   = vControls[ctl_id]->get_value();
                    snprintf(tmp_buf, sizeof(tmp_buf), "_%d", index);
                    if (!append_buf(&buf, tmp_buf))
                    {
                        destroy_buf(&buf);
                        return;
                    }
                    ctl_id      ++;
                }
                else if (tok->type == TT_STRING)
                {
                    if (!append_buf(&buf, tok->data))
                    {
                        destroy_buf(&buf);
                        return;
                    }
                }
                else
                    break;
                tok     = next_token(tok);
            }

            // Now fetch port by name
            pReference  = pUI->port(buf.pString);
            if (pReference != NULL)
            {
                pMetadata       = pReference->metadata();
                pReference->bind(this);
            }

            // Destroy buffer
            destroy_buf(&buf);
        }

        void CtlSwitchedPort::destroy()
        {
            if (pReference != NULL)
            {
                pReference->unbind(this);
                pReference  = NULL;
            }
            if (vControls != NULL)
            {
                delete [] vControls;
                vControls = NULL;
            }
            if (sName != NULL)
            {
                lsp_free(sName);
                sName       = NULL;
            }
            if (sTokens != NULL)
            {
                lsp_free(sTokens);
                sTokens     = NULL;
            }
            pMetadata       = NULL;
        }

        bool CtlSwitchedPort::compile(const char *id)
        {
            destroy();

            sTokens = tokenize(id);
            if (sTokens != NULL)
            {
                sName   = lsp_strdup(id);
                if (sName != NULL)
                {
                    // Calculate number of control ports
                    nDimensions     = 0;
                    token_t *tok    = sTokens;
                    while (tok->type != TT_END)
                    {
                        if (tok->type == TT_INDEX)
                            nDimensions++;
                        tok     = next_token(tok);
                    }

                    // Bind control ports
                    vControls       = new CtlPort *[nDimensions];
                    if (vControls != NULL)
                    {
                        size_t index    = 0;
                        tok             = sTokens;
                        while (tok->type != TT_END)
                        {
                            if (tok->type == TT_INDEX)
                            {
                                CtlPort *sw         = pUI->port(tok->data);
                                if (sw != NULL)
                                {
                                    sw->bind(this);
                                    vControls[index++]  = sw;
                                }
                            }
                            tok     = next_token(tok);
                        }

                        rebind();
                        return true;
                    }
                }
            }

            destroy();
            return false;
        }

        void CtlSwitchedPort::write(const void *buffer, size_t size)
        {
            CtlPort *p  = current();
            if (p != NULL)
                p->write(buffer, size);
        }

        void *CtlSwitchedPort::get_buffer()
        {
            CtlPort *p  = current();
            return (p != NULL) ? p->get_buffer() : NULL;
        }

        float CtlSwitchedPort::get_value()
        {
            CtlPort *p  = current();
            return (p != NULL) ? p->get_value() : 0.0f;
        }

        float CtlSwitchedPort::get_default_value()
        {
            CtlPort *p  = current();
            return (p != NULL) ? p->get_default_value() : 0.0f;
        }

        void CtlSwitchedPort::set_value(float value)
        {
            CtlPort *p  = current();
            if (p != NULL)
                p->set_value(value);
        }

        void CtlSwitchedPort::notify_all()
        {
            CtlPort *p  = current();
            if (p != NULL)
                p->notify_all(); // We will receive notify() as subscribers
            else
                CtlPort::notify_all();
        }

        const char *CtlSwitchedPort::id() const
        {
            return sName;
        }

        void CtlSwitchedPort::notify(CtlPort *port)
        {
            // Check that event is not from dimension-control port
            for (size_t i=0; i<nDimensions; ++i)
            {
                if (port == vControls[i])
                {
                    rebind();
                    notify_all();
                    return;
                }
            }

            // Proxy notify() event only for active port
            CtlPort *p  = current();
            if ((p == NULL) || (port != p))
                return;

            // Notify all subscribers
            CtlPort::notify_all();
        }
    
    } /* namespace ctl */
} /* namespace lsp */
