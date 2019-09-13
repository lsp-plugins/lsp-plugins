/*
 * CtlSaveFile.cpp
 *
 *  Created on: 20 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlSaveFile::metadata = { "CtlSaveFile", &CtlWidget::metadata };

        CtlSaveFile::CtlSaveFile(CtlRegistry *reg, LSPSaveFile *save):
            CtlWidget(reg, save)
        {
            pClass          = &metadata;
            pFile           = NULL;
            pPath           = NULL;
            pStatus         = NULL;
            pCommand        = NULL;
            pProgress       = NULL;

            pPathID         = NULL;
        }

        CtlSaveFile::~CtlSaveFile()
        {
            if (pPathID != NULL)
            {
                free(pPathID);
                pPathID = NULL;
            }
        }

        void CtlSaveFile::update_state()
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save == NULL)
                return;
            if (pStatus == NULL)
                return;

            size_t status = pStatus->get_value();
            if (status == STATUS_UNSPECIFIED)
                save->set_state(SFS_SELECT);
            else if (status == STATUS_OK)
            {
                save->set_state(SFS_SAVED);
                if (pCommand != NULL)
                {
                    pCommand->set_value(0.0f);
                    pCommand->notify_all();
                }
            }
            else if (status == STATUS_LOADING)
            {
                save->set_state(SFS_SAVING);
                if (pProgress != NULL)
                    save->set_progress(pProgress->get_value());
            }
            else
            {
                save->set_state(SFS_ERROR);
                if (pCommand != NULL)
                {
                    pCommand->set_value(0.0f);
                    pCommand->notify_all();
                }
            }
        }

        status_t CtlSaveFile::slot_on_file_submit(LSPWidget *sender, void *ptr, void *data)
        {
            CtlSaveFile *_this   = static_cast<CtlSaveFile *>(ptr);
            return (_this != NULL) ? _this->commit_state() : STATUS_BAD_ARGUMENTS;
        }

        status_t CtlSaveFile::commit_state()
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save == NULL)
                return STATUS_OK;

            const char *path = save->file_name();
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

        void CtlSaveFile::set(widget_attribute_t att, const char *value)
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);

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
                    if (save != NULL)
                        parse_file_formats(value, save->filter());
                    break;
                case A_FORMAT_ID:
                    BIND_EXPR(sFormat, value);
                    break;
                case A_SIZE:
                    if (save != NULL)
                        PARSE_INT(value, save->set_size(__) );
                    break;
                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlSaveFile::end()
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save != NULL)
            {
                save->slots()->bind(LSPSLOT_ACTIVATE, slot_on_activate, this);
                save->slots()->bind(LSPSLOT_SUBMIT, slot_on_file_submit, this);
                save->slots()->bind(LSPSLOT_CLOSE, slot_on_close, this);
            }

            const char *path = (pPathID != NULL) ? pPathID : DEFAULT_PATH_PORT;
            BIND_PORT(pRegistry, pPath, path);

            update_state();

            CtlWidget::end();
        }

        status_t CtlSaveFile::slot_on_activate(LSPWidget *sender, void *ptr, void *data)
        {
            CtlSaveFile *ctl    = static_cast<CtlSaveFile *>(ptr);
            if ((ctl == NULL) || (ctl->pPath == NULL))
                return STATUS_BAD_ARGUMENTS;
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(ctl->pWidget);
            if (save == NULL)
                return STATUS_BAD_STATE;

            save->set_path(ctl->pPath->get_buffer<char>());
            return STATUS_OK;
        }

        status_t CtlSaveFile::slot_on_close(LSPWidget *sender, void *ptr, void *data)
        {
            CtlSaveFile *ctl = static_cast<CtlSaveFile *>(ptr);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            ctl->update_path();
            return STATUS_OK;
        }

        void CtlSaveFile::update_path()
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if ((save == NULL) || (pPath == NULL))
                return;

            LSPString path;
            if (save->get_path(&path) != STATUS_OK)
                return;

            if (path.length() <= 0)
                return;

            pPath->write(path.get_native(), path.length());
            pPath->notify_all();
        }

        void CtlSaveFile::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if ((port == pStatus) ||
                (port == pProgress))
                update_state();

            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save == NULL)
                return;

            if (sFormat.valid())
                save->filter()->set_default(sFormat.evaluate());
        }
    } /* namespace ctl */
} /* namespace lsp */
