/*
 * pluginui.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/alloc.h>

#include <ui/ui.h>
#include <ui/buffer.h>
#include <ui/UISwitchedPort.h>

#include <stdio.h>
#include <ctype.h>
#include <string.h>

namespace lsp
{
    static char *escape_characters(const char *s)
    {
        if (s == NULL)
            return NULL;

        buffer_t sbuf;
        init_buf(&sbuf, strlen(s) + 32);

        while (*s != '\0')
        {
            switch (*s)
            {
                case '\"':
                    append_buf(&sbuf, "\\\"", 2);
                    break;
                case '\\':
                    append_buf(&sbuf, "\\\\", 2);
                    break;
                case '\n':
                    append_buf(&sbuf, "\\n", 2);
                    break;
                default:
                    append_buf(&sbuf, *s);
                    break;
            }
            s++;
        }

        return release_buf(&sbuf);
    }

    static ssize_t read_line(FILE *fd, buffer_t *buf)
    {
        while(true)
        {
            size_t avail    = buf->nCapacity - buf->nLength;
            if (avail < 1)
            {
                if (!extend_buf(buf, 4096))
                    return -1;
                avail    = buf->nCapacity - buf->nLength;
            }
            if (fgets(&buf->pString[buf->nLength], avail, fd) == NULL)
                return -2;

            size_t slen     = strlen(&buf->pString[buf->nLength]);
            if (memchr(&buf->pString[buf->nLength], '\n', slen) != NULL)
            {
                buf->nLength   += slen;
                return 0;
            }

            buf->nLength   += slen;
        }
        return -3;
    }

    static bool parse_line(buffer_t *buf, char **key, char **value)
    {
        const char *src = buf->pString;
        char *dst       = buf->pString;

        // Read the key
        *key            = dst;
        *value          = NULL;
        size_t k_chars  = 0;
        while (*src != '\0')
        {
            char ch = *(src++);
            if ((ch == ' ') || (ch == '\t'))
            {
                if (k_chars > 0)
                {
                    while (*src != '\0')
                    {
                        switch (*(src++))
                        {
                            case '=':
                                goto end_key;
                            case ' ':
                            case '\t':
                                break;
                            case '\n':
                            case '#':
                                *dst    = '\0';
                                *value  = dst;
                                return true;
                            default:
                                return false;
                        }
                    }
                    break;
                }
            }
            else if (ch == '=')
            {
                if (k_chars <= 0)
                    return false;
                break;
            }
            else if ((ch == '#') || (ch == '\n'))
            {
                if (k_chars <= 0)
                    return false;
                *dst    = '\0';
                *value  = dst;
                return true;
            }
            else if ((isalnum(ch)) || (ch == '_'))
            {
                *(dst++)    = ch;
                k_chars     ++;
            }
            else
                return false;
        }
        end_key:
        *(dst++) = '\0';

        // Read the value
        *value          = dst;
        size_t v_chars  = 0;
        bool quoted     = false;
        bool protector  = false;

        while (*src != '\0')
        {
            char ch = *(src++);
            switch (ch)
            {
                case '\"':
                    if (protector)
                    {
                        *(dst++)    = ch;
                        v_chars     ++;
                        protector   = false;
                    }
                    else if (!quoted)
                    {
                        if (v_chars > 0)
                            return false;
                        quoted      = true;
                    }
                    else
                        goto parse_end;
                    break;

                case '\\':
                    if (protector)
                    {
                        *(dst++)    = ch;
                        v_chars     ++;
                        protector   = false;
                    }
                    else
                        protector   = true;
                    break;

                case ' ':
                case '\t':
                case '#':
                    if (quoted)
                    {
                        if (protector)
                        {
                            *(dst++)    = '\\';
                            protector   = false;
                        }
                        *(dst++)    = ch;
                        v_chars     ++;
                    }
                    else if (protector)
                    {
                        *(dst++)    = ch;
                        v_chars     ++;
                        protector   = false;
                    }
                    else if (ch == '#')
                    {
                        *(dst++)    = '\0';
                        return true;
                    }
                    else if (v_chars > 0)
                        goto parse_end;
                    break;

                case '\n':
                    if (quoted)
                        return false;
                    *(dst++)    = '\0';
                    return true;

                case 'n':
                    if (protector)
                        ch          = '\n';
                    *(dst++)    = ch;
                    protector   = false;
                    v_chars     ++;
                    break;

                default:
                    if (protector)
                    {
                        *(dst++)    = '\\';
                        protector   = false;
                    }
                    *(dst++)    = ch;
                    v_chars     ++;
                    break;
            }
        }
        parse_end:
        *(dst++) = '\0';

        // Ensure that line ends correctly
        while (*src != '\0')
        {
            switch (*(src++))
            {
                case '\n':
                case '#':
                    return true;
                case ' ':
                case '\t':
                    break;
                default:
                    return false;
            }
        }

        return true;
    }

    plugin_ui::plugin_ui(const char *name, const plugin_metadata_t *mdata, IWidgetFactory *factory)
    {
        sName           = name;
        pMetadata       = mdata;
        pFactory        = factory;
        pWrapper        = NULL;
    }

    plugin_ui::~plugin_ui()
    {
        destroy();
    }

    void plugin_ui::init(IUIWrapper *wrapper)
    {
        pWrapper        = wrapper;
    }

    void plugin_ui::build()
    {
        // Generate path to theme
        char path[PATH_MAX];
        #ifdef LSP_USE_EXPAT
            snprintf(path, PATH_MAX, "%s/ui/theme.xml", pFactory->path());
        #else
            strncpy(path, "theme.xml", PATH_MAX);
        #endif /* LSP_USE_EXPAT */
        lsp_trace("Loading theme from file %s", path);

        // Load theme
        if (sTheme.load(path))
        {
            // Generate path to UI schema
            #ifdef LSP_USE_EXPAT
                snprintf(path, PATH_MAX, "%s/ui/%s.xml", pFactory->path(), sName);
            #else
                snprintf(path, PATH_MAX, "%s.xml", sName);
            #endif /* LSP_USE_EXPAT */
            lsp_trace("Generating UI from file %s", path);

            // Build UI
            ui_builder builder(this);
            if (!builder.build(path))
                lsp_error("Could not build UI from file %s", path);
        }
    }

    void plugin_ui::destroy()
    {
        // Delete widgets
        for (size_t i=0; i<vWidgets.size(); ++i)
            delete vWidgets[i];
        vWidgets.clear();

        // Delete factory
        if (pFactory != NULL)
        {
            delete pFactory;
            pFactory = NULL;
        }

        // Destroy switched ports
        for (size_t i=0; i<vSwitched.size(); ++i)
        {
            lsp_trace("Destroy switched port id=%s", vSwitched[i]->id());
            if (vSwitched[i] != NULL)
                delete vSwitched[i];
        }

        // Clear ports
        vSortedPorts.clear();
        vPorts.clear();
        vSwitched.clear();
        vAliases.clear();
    }

    bool plugin_ui::add_port(IUIPort *port)
    {
        if (!vPorts.add(port))
            return false;

        lsp_trace("added port id=%s", port->metadata()->id);
        return true;
    }

    bool plugin_ui::export_settings(const char *filename)
    {
        // Open file for writing
        FILE *fd = fopen(filename, "w+");
        if (fd == NULL)
            return false;

        // Change locale
        char *saved_locale  = setlocale(1, "C");

        // Print the comment
        fputs("#-------------------------------------------------------------------------------\n", fd);
        fputs("#\n", fd);
        fputs("# This file contains configuration of the audio plugin.\n", fd);
        fprintf(fd, "#   Plugin name:         %s (%s)\n", pMetadata->name, pMetadata->description);
        fprintf(fd, "#   Plugin version:      %d.%d.%d\n",
                int(LSP_VERSION_MAJOR(pMetadata->version)),
                int(LSP_VERSION_MINOR(pMetadata->version)),
                int(LSP_VERSION_MICRO(pMetadata->version))
            );
        if (pMetadata->lv2_uid != NULL)
            fprintf(fd, "#   LV2 URI:             %s%s\n", LSP_URI(lv2), pMetadata->lv2_uid);
        if (pMetadata->vst_uid != NULL)
            fprintf(fd, "#   VST identifier:      %s\n", pMetadata->vst_uid);
        if (pMetadata->ladspa_id > 0)
            fprintf(fd, "#   LADSPA identifier:   %d\n", pMetadata->ladspa_id);
        fputs("#\n", fd);
        fputs("# (C) " LSP_FULL_NAME " \n", fd);
        fputs("#   " LSP_BASE_URI " \n#\n", fd);
        fputs("#-------------------------------------------------------------------------------\n", fd);
        fputs("\n", fd);

        for (size_t i=0; i<vPorts.size(); ++i)
        {
            // Get port
            IUIPort *up         = vPorts[i];
            if (up == NULL)
                continue;

            // Get metadata
            const port_t *p    = up->metadata();
            if (p == NULL)
                continue;

            // Skip output and proxy ports
            if (!IS_IN_PORT(p))
                continue;

            switch (p->role)
            {
                case R_PORT_SET:
                case R_CONTROL:
                {
                    // Serialize meta information
                    const char *unit = encode_unit(p->unit);
                    if (unit != NULL)
                        fprintf(fd, "# %s [%s]", p->name, unit);
                    else if (p->unit == U_BOOL)
                        fprintf(fd, "# %s", p->name);
                    else
                        fprintf(fd, "# %s", p->name);

                    if ((p->flags & (F_LOWER | F_UPPER)) || (p->unit == U_ENUM) || (p->unit == U_BOOL))
                    {
                        if (is_discrete_unit(p->unit) || (p->flags & F_INT))
                        {
                            if (p->unit != U_BOOL)
                            {
                                if (p->unit == U_ENUM)
                                {
                                    int value       = p->min + list_size(p->items) - 1;
                                    fprintf(fd, ": %d..%d", int(p->min), int(value));
                                }
                                else
                                    fprintf(fd, ": %d..%d", int(p->min), int(p->max));
                            }
                            else
                                fprintf(fd, ": true/false");
                        }
                        else
                            fprintf(fd, ": %.6f..%.6f", p->min, p->max);
                    }
                    fputs("\n", fd);

                    // Describe enum
                    if ((p->unit == U_ENUM) && (p->items != NULL))
                    {
                        int value   = p->min;
                        for (const char **item = p->items; *item != NULL; ++item)
                            fprintf(fd, "#   %d: %s\n", value++, *item);
                    }

                    // Serialize value
                    float value = up->getValue();
                    if (is_discrete_unit(p->unit) || (p->flags & F_INT))
                    {
                        if (p->unit == U_BOOL)
                            fprintf(fd, "%s = %s\n", p->id, (value >= 0.5f) ? "true" : "false");
                        else
                            fprintf(fd, "%s = %d\n", p->id, int(value));
                    }
                    else
                        fprintf(fd, "%s = %.6f\n", p->id, value);
                    fputs("\n", fd);
                    break;
                }
                case R_PATH:
                {
                    fprintf(fd, "# %s [pathname]\n", p->name);
                    const char *path    = up->getBuffer<const char>();
                    char *value         = escape_characters(path);
                    if (value != NULL)
                    {
                        fprintf(fd, "%s = \"%s\"\n", p->id, value);
                        lsp_free(value);
                    }
                    else
                        fprintf(fd, "%s = \"\"\n", p->id);
                    fputs("\n", fd);
                    break;
                }
                default:
                    break;
            }
        }

        fputs("#-------------------------------------------------------------------------------\n", fd);

        // Return locale
        setlocale(1, saved_locale);

        // Close file
        fclose(fd);

        return true;
    }

    bool plugin_ui::import_settings(const char *filename)
    {
        // Open file for writing
        FILE *fd = fopen(filename, "r");
        if (fd == NULL)
            return false;

        bool result = true;
        buffer_t line;
        init_buf(&line, 4096);

        while (true)
        {
            // Read line from file
            ssize_t read    = read_line(fd, &line);
            if (read < 0)
            {
                result      = false;
                break;
            }

            // Parse the line
            char *key = NULL, *value = NULL;
            if (parse_line(&line, &key, &value))
            {
                lsp_trace("Configuration: %s = %s", key, value);
                apply_changes(key, value);
            }

            // Finally, clear the buffer
            clear_buf(&line);
        }

        // Free resources
        destroy_buf(&line);
        fclose(fd);

        return result;
    }

    bool plugin_ui::apply_changes(const char *key, const char *value)
    {
        // Get UI port
        IUIPort *up     = port(key);
        if (up == NULL)
            return false;

        // Get metadata
        const port_t *p = up->metadata();
        if (p == NULL)
            return false;

        // Check that it's a control port
        if (!IS_IN_PORT(p))
            return false;

        // Apply changes
        switch (p->role)
        {
            case R_PORT_SET:
            case R_CONTROL:
            {
                if (is_discrete_unit(p->unit))
                {
                    if (p->unit == U_BOOL)
                    {
                        PARSE_BOOL(value,
                            up->setValue(__);
                            up->notifyAll();
                        );
                    }
                    else
                    {
                        PARSE_INT(value,
                            up->setValue(__);
                            up->notifyAll();
                        );
                    }
                }
                else
                {
                    PARSE_FLOAT(value,
                        up->setValue(__);
                        up->notifyAll();
                    );
                }
                break;
            }
            case R_PATH:
            {
                size_t len      = strlen(value);
                up->write(value, len);
                up->notifyAll();
                break;
            }
            default:
                return false;
        }
        return true;
    }

    IUIPort *plugin_ui::port(const char *name)
    {
        // Check aliases
        for (size_t i=0; i<vAliases.size(); ++i)
        {
            PortAlias *pa = vAliases[i];
            if ((pa->id() == NULL) || (pa->alias() == NULL))
                continue;

            if (!strcmp(name, pa->id()))
            {
                name    = pa->alias();
                break;
            }
        }

        // Check that port name contains index
        if (strchr(name, '[') != NULL)
        {
            // Try to find switched port
            size_t count = vSwitched.size();
            for (size_t i=0; i<count; ++i)
            {
                UISwitchedPort *p   = vSwitched[i];
                if (p == NULL)
                    continue;
                const char *p_id    = p->id();
                if (p_id == NULL)
                    continue;
                if (!strcmp(p_id, name))
                    return p;
            }

            // Create new switched port
            UISwitchedPort *s   = new UISwitchedPort(this);
            if (s == NULL)
                return NULL;

            if (s->compile(name))
            {
                if (vSwitched.add(s))
                    return s;
            }

            delete s;
            return NULL;
        }

        // Do usual stuff
        size_t count = vPorts.size();
        if (vSortedPorts.size() != count)
            count = rebuild_sorted_ports();

        // Try to find the corresponding port
        ssize_t first = 0, last = count - 1;
        while (first <= last)
        {
            size_t center       = (first + last) >> 1;
            IUIPort *p          = vSortedPorts[center];
            const port_t *ctl   = p->metadata();
            if (ctl == NULL)
                return NULL;

            int cmp     = strcmp(name, ctl->id);
            if (cmp == 0)
                return p;
            else if (cmp < 0)
                last    = center - 1;
            else
                first   = center + 1;
        }
        return NULL;

//        for (size_t i=0; i<count; ++i)
//        {
//            IUIPort *p  = vPorts[i];
//            const port_t *ctl = p->metadata();
//            if (ctl == NULL)
//                continue;
//
//            if (!strcmp(ctl->id, name))
//                return p;
//        }
//
//        return NULL;
    }

    size_t plugin_ui::rebuild_sorted_ports()
    {
        size_t count = vPorts.size();
        vSortedPorts.clear();

        for (size_t i=0; i<count; ++i)
            vSortedPorts.add(vPorts[i]);

        count   = vSortedPorts.size();

        // Sort by port ID
        if (count >= 2)
        {
            for (size_t i=0; i<(count-1); ++i)
                for (size_t j=i+1; j<count; ++j)
                {
                    IUIPort *a  = vSortedPorts[i];
                    IUIPort *b  = vSortedPorts[j];
                    if ((a == NULL) || (b == NULL))
                        continue;
                    const port_t *am    = a->metadata();
                    const port_t *bm    = b->metadata();
                    if ((am == NULL) || (bm == NULL))
                        continue;
                    if (strcmp(am->id, bm->id) > 0)
                        vSortedPorts.swap(i, j);
                }
        }

//        #ifdef LSP_TRACE
//            for (size_t i=0; i<count; ++i)
//                lsp_trace("sorted port idx=%d, id=%s", int(i), vSortedPorts[i]->metadata()->id);
//        #endif /* LSP_TRACE */

        return count;
    }

    IWidget *plugin_ui::createWidget(widget_t w_class)
    {
        IWidget *widget = pFactory->createWidget(this, w_class);
        return (widget != NULL) ? addWidget(widget) : widget;
    }

    IWidget *plugin_ui::createWidget(const char *w_class)
    {
        IWidget *widget = pFactory->createWidget(this, w_class);
        return (widget != NULL) ? addWidget(widget) : widget;
    }

    IWidget *plugin_ui::addWidget(IWidget *widget)
    {
        if (widget == NULL)
            return NULL;
        if (widget->getClass() == W_PORT)
            vAliases.add(static_cast<PortAlias *>(widget));
        return (vWidgets.add(widget)) ? widget : NULL;
    }
} /* namespace lsp */
