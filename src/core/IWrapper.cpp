/*
 * IWrapper.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/IWrapper.h>
#include <core/system.h>
#include <core/JsonDumper.h>
#include <core/plugin.h>
#include <metadata/metadata.h>
#include <time.h>

namespace lsp
{
    IWrapper::IWrapper(plugin_t *plugin)
    {
        pPlugin     = plugin;
    }

    IWrapper::~IWrapper()
    {
    }

    ipc::IExecutor *IWrapper::get_executor()
    {
        return NULL;
    }

    void IWrapper::query_display_draw()
    {
    }

    const position_t *IWrapper::position()
    {
        return NULL;
    }

    ICanvas *IWrapper::create_canvas(ICanvas *&cv, size_t width, size_t height)
    {
        return NULL;
    }

    KVTStorage *IWrapper::kvt_lock()
    {
        return NULL;
    }

    KVTStorage *IWrapper::kvt_trylock()
    {
        return NULL;
    }

    bool IWrapper::kvt_release()
    {
        return false;
    }

    void IWrapper::state_changed()
    {
    }

    void IWrapper::dump_plugin_state()
    {
        if (pPlugin == NULL)
            return;

        io::Path path;
        status_t res;
        if ((res = system::get_temporary_dir(&path)) != STATUS_OK)
        {
            lsp_warn("Could not obtain temporary directory: %d", int(res));
            return;
        }
        if ((res = path.append_child(LSP_ARTIFACT_ID "-dumps")) != STATUS_OK)
        {
            lsp_warn("Could not form path to directory: %d", int(res));
            return;
        }
        if ((res = path.mkdir(true)) != STATUS_OK)
        {
            lsp_warn("Could not create directory %s: %d", path.as_utf8(), int(res));
            return;
        }

        // Get current time
        struct timespec stime;
        struct tm *t;
        clock_gettime(CLOCK_REALTIME, &stime);
    #if defined(_POSIX_C_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
        struct tm ctime;
        t           = localtime_r(&stime.tv_sec, &ctime);
    #else
        t           = localtime(&stime.tv_sec);
    #endif

        const plugin_metadata_t *meta = pPlugin->get_metadata();
        if (meta == NULL)
            return;

        // Build the file name
        LSPString fname;
        if (!fname.fmt_ascii("%04d%02d%02d-%02d%02d%02d-%03d-%s.json",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, int(stime.tv_nsec / 1000000),
                meta->lv2_uid
            ))
        {
            lsp_warn("Could not format the file name");
            return;
        }

        if ((res = path.append_child(&fname)) != STATUS_OK)
        {
            lsp_warn("Could not form the file name: %d", int(res));
            return;
        }

        lsp_info("Dumping plugin state to file:\n%s...", path.as_utf8());

        JsonDumper v;
        if ((res = v.open(&path)) != STATUS_OK)
        {
            lsp_warn("Could not create file %s: %d", path.as_utf8(), int(res));
            return;
        }

        v.begin_raw_object();
        {
            LSPString tmp;

            v.write("name", meta->name);
            v.write("description", meta->description);
            v.write("package", LSP_MAIN_VERSION);
            tmp.fmt_ascii("%d.%d.%d",
                    int(LSP_VERSION_MAJOR(meta->version)),
                    int(LSP_VERSION_MINOR(meta->version)),
                    int(LSP_VERSION_MICRO(meta->version))
                );
            v.write("version", tmp.get_utf8());
            tmp.fmt_ascii("%s%s", LSP_URI(lv2), meta->lv2_uid);
            v.write("lv2_uri", tmp.get_utf8());
            v.write("vst_id", meta->vst_uid);
            v.write("ladspa_id", meta->ladspa_id);
            v.write("this", pPlugin);
            v.begin_raw_object("data");
            {
                pPlugin->dump(&v);
            }
            v.end_raw_object();
        }

        v.end_raw_object();
        v.close();

        lsp_info("State has been dumped to file:\n%s", path.as_utf8());
    }

} /* namespace lsp */
