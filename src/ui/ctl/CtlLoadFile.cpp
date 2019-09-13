/*
 * CtlLoadFile.cpp
 *
 *  Created on: 7 мая 2019 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlLoadFile::metadata = { "CtlLoadFile", &CtlWidget::metadata };

        CtlLoadFile::CtlLoadFile(CtlRegistry *reg, LSPLoadFile *load):
            CtlWidget(reg, load)
        {
            pClass          = &metadata;
            pFile           = NULL;
            pPath           = NULL;
            pStatus         = NULL;
            pCommand        = NULL;
            pProgress       = NULL;

            pPathID         = NULL;
        }

        CtlLoadFile::~CtlLoadFile()
        {
            if (pPathID != NULL)
            {
                free(pPathID);
                pPathID = NULL;
            }
        }

        void CtlLoadFile::update_state()
        {
            LSPLoadFile *load   = widget_cast<LSPLoadFile>(pWidget);
            if (load == NULL)
                return;
            if (pStatus == NULL)
                return;

            size_t status = pStatus->get_value();
            if (status == STATUS_UNSPECIFIED)
                load->set_state(LFS_SELECT);
            else if (status == STATUS_OK)
            {
                load->set_state(LFS_LOADED);
                if (pCommand != NULL)
                {
                    pCommand->set_value(0.0f);
                    pCommand->notify_all();
                }
            }
            else if (status == STATUS_LOADING)
            {
                load->set_state(LFS_LOADING);
                if (pProgress != NULL)
                    load->set_progress(pProgress->get_value());
            }
            else
            {
                load->set_state(LFS_ERROR);
                if (pCommand != NULL)
                {
                    pCommand->set_value(0.0f);
                    pCommand->notify_all();
                }
            }
        }

        status_t CtlLoadFile::slot_on_file_submit(LSPWidget *sender, void *ptr, void *data)
        {
            CtlLoadFile *_this   = static_cast<CtlLoadFile *>(ptr);
            return (_this != NULL) ? _this->commit_state() : STATUS_BAD_ARGUMENTS;
        }

        status_t CtlLoadFile::commit_state()
        {
            LSPLoadFile *load   = widget_cast<LSPLoadFile>(pWidget);
            if (load == NULL)
                return STATUS_OK;

            const char *path = load->file_name();
            if (pFile != NULL)
            {
                pFile->write(path, strlen(path));
                pFile->notify_all();
            }
            if (pCommand != NULL)
            {
                pCommand->set_value(1.0f);
                pCommand->notify_all();
            }

            return STATUS_OK;
        }

        void CtlLoadFile::set(widget_attribute_t att, const char *value)
        {
            LSPLoadFile *load   = widget_cast<LSPLoadFile>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pFile, value);
                    break;
                case A_PATH_ID:
                    if (pPathID != NULL)
                        free(pPathID);
                    pPathID = (value != NULL) ? strdup(value) : NULL;
                    break;
                case A_STATUS_ID:
                    BIND_PORT(pRegistry, pStatus, value);
                    break;
                case A_COMMAND_ID:
                    BIND_PORT(pRegistry, pCommand, value);
                    break;
                case A_PROGRESS_ID:
                    BIND_PORT(pRegistry, pProgress, value);
                    break;
                case A_FORMAT:
                    if (load != NULL)
                        parse_file_formats(value, load->filter());
                    break;
                case A_FORMAT_ID:
                    BIND_EXPR(sFormat, value);
                    break;
                case A_SIZE:
                    if (load != NULL)
                        PARSE_INT(value, load->set_size(__) );
                    break;
                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlLoadFile::end()
        {
            LSPLoadFile *load   = widget_cast<LSPLoadFile>(pWidget);
            if (load != NULL)
            {
                load->slots()->bind(LSPSLOT_ACTIVATE, slot_on_activate, this);
                load->slots()->bind(LSPSLOT_SUBMIT, slot_on_file_submit, this);
                load->slots()->bind(LSPSLOT_CLOSE, slot_on_close, this);
            }

            const char *path = (pPathID != NULL) ? pPathID : DEFAULT_PATH_PORT;
            BIND_PORT(pRegistry, pPath, path);

            update_state();

            CtlWidget::end();
        }

        status_t CtlLoadFile::slot_on_activate(LSPWidget *sender, void *ptr, void *data)
        {
            CtlLoadFile *ctl    = static_cast<CtlLoadFile *>(ptr);
            if ((ctl == NULL) || (ctl->pPath == NULL))
                return STATUS_BAD_ARGUMENTS;
            LSPLoadFile *load   = widget_cast<LSPLoadFile>(ctl->pWidget);
            if (load == NULL)
                return STATUS_BAD_STATE;

            load->set_path(ctl->pPath->get_buffer<char>());
            return STATUS_OK;
        }

        status_t CtlLoadFile::slot_on_close(LSPWidget *sender, void *ptr, void *data)
        {
            CtlLoadFile *ctl = static_cast<CtlLoadFile *>(ptr);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            ctl->update_path();
            return STATUS_OK;
        }

        void CtlLoadFile::update_path()
        {
            LSPLoadFile *load   = widget_cast<LSPLoadFile>(pWidget);
            if ((load == NULL) || (pPath == NULL))
                return;

            LSPString path;
            if (load->get_path(&path) != STATUS_OK)
                return;

            if (path.length() <= 0)
                return;

            pPath->write(path.get_native(), path.length());
            pPath->notify_all();
        }

        void CtlLoadFile::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if ((port == pStatus) ||
                (port == pProgress))
                update_state();

            LSPLoadFile *load   = widget_cast<LSPLoadFile>(pWidget);
            if (load == NULL)
                return;

            if (sFormat.valid())
                load->filter()->set_default(sFormat.evaluate());
        }
    } /* namespace ctl */
} /* namespace lsp */
