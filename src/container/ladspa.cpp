
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <core/types.h>
#include <core/lib.h>
#include <core/debug.h>
#include <core/ipc/NativeExecutor.h>
#include <core/resource.h>
#include <plugins/plugins.h>

#ifdef PLATFORM_WINDOWS
    #include <3rdparty/ladspa/ladspa.h>
#else
    #include <ladspa.h>
#endif /* PLATFORM_WINDOWS */

#include <container/ladspa/ports.h>
#include <container/ladspa/wrapper.h>

#ifndef LSP_IDE_DEBUG
    /* LADSPA format does not require any built-in resources */
    BUILTIN_RESOURCES_STUB
#endif /* LSP_IDE_DEBUG */

namespace lsp
{
    inline bool ladspa_port_supported(const port_t *port)
    {
        switch (port->role)
        {
            case R_UI_SYNC:
            case R_MESH:
            case R_FBUFFER:
                return false;
            default:
                return true;
        }
        return true;
    }

    LADSPA_Handle ladspa_instantiate(
        const struct _LADSPA_Descriptor * Descriptor,
        unsigned long                     SampleRate)
    {
        lsp_debug_init("ladspa");

        // Check sample rate
        if (SampleRate > MAX_SAMPLE_RATE)
        {
            lsp_error("Unsupported sample rate: %ld, maximum supportes sample rate is %ld", long(SampleRate), long(MAX_SAMPLE_RATE));
            return NULL;
        }

        // Initialize DSP
        dsp::init();

        // Instantiate plugin
        plugin_t *p         = NULL;

        #define MOD_PLUGIN(plugin, ui) \
            if ((!p) && (plugin::metadata.ladspa_id > 0) && (Descriptor->UniqueID == plugin::metadata.ladspa_id) && (!strcmp(Descriptor->Label, LSP_PLUGIN_URI(ladspa, plugin)))) \
                p = new plugin();
        #include <metadata/modules.h>

        if (!p)
            return NULL;

        LADSPAWrapper *w    = new LADSPAWrapper(p);
        w->init(SampleRate);

        return reinterpret_cast<LADSPA_Handle>(w);
    }

    void ladspa_connect_port(
        LADSPA_Handle Instance,
        unsigned long Port,
        LADSPA_Data * DataLocation)
    {
        LADSPAWrapper *w = reinterpret_cast<LADSPAWrapper *>(Instance);
        w->connect(Port, DataLocation);
    }

    void ladspa_activate(LADSPA_Handle Instance)
    {
        LADSPAWrapper *w = reinterpret_cast<LADSPAWrapper *>(Instance);
        w->activate();
    }

    void ladspa_run(LADSPA_Handle Instance, unsigned long SampleCount)
    {
        dsp::context_t ctx;
        LADSPAWrapper *w = reinterpret_cast<LADSPAWrapper *>(Instance);

        // Call the plugin for processing
        dsp::start(&ctx);
        w->run(SampleCount);
        dsp::finish(&ctx);
    }

    void ladspa_deactivate(LADSPA_Handle Instance)
    {
        LADSPAWrapper *w = reinterpret_cast<LADSPAWrapper *>(Instance);
        w->deactivate();
    }

    void ladspa_cleanup(LADSPA_Handle Instance)
    {
        LADSPAWrapper *w = reinterpret_cast<LADSPAWrapper *>(Instance);
        w->destroy();
        delete w;
    }

    LADSPA_Descriptor *ladspa_descriptors = NULL;
    size_t ladspa_descriptors_count    = 0;

    const char *ladspa_add_units(const char *s, size_t units)
    {
        char buf[256];
        const char *unit = encode_unit(units);
        if (unit == NULL)
            return strdup(s);

        snprintf(buf, sizeof(buf) - 1, "%s (%s)", s, unit);
        return strdup(buf);
    }

    void ladspa_make_descriptor(LADSPA_Descriptor *d, unsigned long id, const char *label, const plugin_metadata_t &m)
    {
        char *plugin_name = NULL;
        int n = asprintf(&plugin_name, "%s - %s", m.description, m.name);

        d->UniqueID             = id;
        d->Label                = label;
        d->Properties           = LADSPA_PROPERTY_HARD_RT_CAPABLE;
        d->Name                 = (n >= 0) ? plugin_name : NULL;
        d->Maker                = LSP_ACRONYM " LADSPA";
        d->ImplementationData   = const_cast<char *>(m.developer->name);
        d->Copyright            = LSP_COPYRIGHT;
        d->PortCount            = 1; // 1 port used for latency output

        // Calculate number of ports
        for (const port_t *port = m.ports; (port->id != NULL) && (port->name != NULL); ++port)
        {
            if (ladspa_port_supported(port))
                d->PortCount ++;
        }

        LADSPA_PortDescriptor *p_descr      = new LADSPA_PortDescriptor[d->PortCount];
        const char **p_name                 = new const char *[d->PortCount];
        LADSPA_PortRangeHint *p_hint        = new LADSPA_PortRangeHint[d->PortCount];

        d->PortDescriptors                  = p_descr;
        d->PortNames                        = p_name;
        d->PortRangeHints                   = p_hint;

        for (const port_t *p = m.ports ; (p->id != NULL) && (p->name != NULL); ++p)
        {
            // Skip ports invisible for LADSPA
            if (!ladspa_port_supported(p))
                continue;

            // Generate port descriptor
            switch (p->role)
            {
                case R_AUDIO:
                    *p_descr = (IS_OUT_PORT(p)) ? LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO : LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
                    break;
                case R_CONTROL:
                case R_METER:
                default:
                    *p_descr = (IS_OUT_PORT(p)) ? LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL : LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
                    break;
            }

            *p_name                 = ladspa_add_units(p->name, p->unit);
            p_hint->HintDescriptor  = 0;

            if (p->unit == U_BOOL)
            {
                p_hint->HintDescriptor |= LADSPA_HINT_TOGGLED | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_BELOW;
                p_hint->HintDescriptor |= (p->start > 0) ? LADSPA_HINT_DEFAULT_1 : LADSPA_HINT_DEFAULT_0;
                p_hint->LowerBound      = 0.0f;
                p_hint->UpperBound      = 1.0f;
            }
            else if (p->unit == U_ENUM)
            {
                p_hint->HintDescriptor  |= LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_BOUNDED_BELOW;
                p_hint->LowerBound      = (p->flags & F_LOWER) ? p->min : 0;
                p_hint->UpperBound      = p_hint->LowerBound + list_size(p->items) - 1;

                if (p->start == p_hint->LowerBound)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_MINIMUM;
                else if (p->start == p_hint->UpperBound)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_MAXIMUM;
                else if (p->start == 1.0f)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_1;
                else if (p->start == 0.0f)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_0;
            }
            else if (p->unit == U_SAMPLES)
            {
//                p_hint->HintDescriptor  |= LADSPA_HINT_INTEGER;
                if (p->flags & F_LOWER)
                {
                    p_hint->HintDescriptor |= LADSPA_HINT_BOUNDED_BELOW;
                    p_hint->LowerBound      = p->min;
                }
                if (p->flags & F_UPPER)
                {
                    p_hint->HintDescriptor |= LADSPA_HINT_BOUNDED_ABOVE;
                    p_hint->UpperBound      = p->max;
                }
            }
            else
            {
                if (p->flags & F_LOWER)
                {
                    p_hint->HintDescriptor |= LADSPA_HINT_BOUNDED_BELOW;
                    p_hint->LowerBound      = p->min;
                }
                if (p->flags & F_UPPER)
                {
                    p_hint->HintDescriptor |= LADSPA_HINT_BOUNDED_ABOVE;
                    p_hint->UpperBound      = p->max;
                }
                if (p->flags & F_LOG)
                    p_hint->HintDescriptor |= LADSPA_HINT_LOGARITHMIC;
            }

            // Solve default value
            if ((p->role != R_AUDIO) && ((p_hint->HintDescriptor & LADSPA_HINT_DEFAULT_MASK) == LADSPA_HINT_DEFAULT_NONE))
            {
                if (p->start == 1.0f)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_1;
                else if (p->start == 0.0f)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_0;
                else if (p->start == 100.0f)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_100;
                else if (p->start == 440.0f)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_440;
                else if ((p->flags & (F_LOWER | F_UPPER))  == (F_LOWER | F_UPPER))
                {
                    if (p->start <= p->min)
                        p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_MINIMUM;
                    else if (p->start >= p->max)
                        p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_MAXIMUM;
                    else
                    {
                        float factor = (p->flags & F_LOG) ?
                            (logf(p->start) - logf(p->min)) / (logf(p->max) - logf(p->min)) :
                            (p->start - p->min) / (p->max - p->min);

                        if (factor <= 0.33)
                            p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_LOW;
                        else if (factor >= 0.66)
                            p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_HIGH;
                        else
                            p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_MIDDLE;
                    }
                }
                else if (p->flags & F_LOWER)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_MINIMUM;
                else if (p->flags & F_UPPER)
                    p_hint->HintDescriptor |= LADSPA_HINT_DEFAULT_MAXIMUM;
            }

            p_descr++;
            p_name++;
            p_hint++;
        }

        // Describe latency port
        *p_descr                = LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL;
        *p_name                 = strdup("latency");
        p_hint->HintDescriptor  = LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW;
        p_hint->LowerBound      = 0;
        p_hint->UpperBound      = 0;

        // Complete the LADSPA descriptor
        d->instantiate          = ladspa_instantiate;
        d->connect_port         = ladspa_connect_port;
        d->activate             = ladspa_activate;
        d->run                  = ladspa_run;
        d->run_adding           = NULL;
        d->set_run_adding_gain  = NULL;
        d->deactivate           = ladspa_deactivate;
        d->cleanup              = ladspa_cleanup;
    }

    void ladspa_gen_descriptors()
    {
        if (ladspa_descriptors != NULL)
            return;

        // Calculate number of plugins
        ladspa_descriptors_count    = 0;
        #define MOD_PLUGIN(plugin, ui) \
            if (plugin::metadata.ladspa_id > 0) \
                ladspa_descriptors_count++;
        #include <metadata/modules.h>

        // Now allocate descriptors
        ladspa_descriptors          = new LADSPA_Descriptor[ladspa_descriptors_count];
        LADSPA_Descriptor *d        = ladspa_descriptors;
        size_t id                   = 0;

        #define MOD_PLUGIN(plugin, ui) \
            if (plugin::metadata.ladspa_id > 0) \
            { \
                ladspa_make_descriptor(&d[id], plugin::metadata.ladspa_id, LSP_PLUGIN_URI(ladspa, plugin), plugin::metadata); \
                id++; \
            }
        #include <metadata/modules.h>
    };

    void ladspa_drop_descriptors()
    {
        if (ladspa_descriptors == NULL)
            return;

        LADSPA_Descriptor *d = ladspa_descriptors;
        while (ladspa_descriptors_count--)
        {
            for (size_t i=0; i < d->PortCount; ++i)
            {
                if (d->PortNames[i])
                    free(const_cast<char *>(d->PortNames[i]));
            }
            delete [] d->PortNames;
            delete [] d->PortDescriptors;
            delete [] d->PortRangeHints;

            free(const_cast<char *>(d->Name));
            d   ++;
        }

        delete [] ladspa_descriptors;
        ladspa_descriptors = NULL;
    };

    static StaticFinalizer ladspa_finalizer(ladspa_drop_descriptors);
}

#ifdef __cplusplus
extern "C"
{
#endif
    LSP_LIBRARY_EXPORT
    const LADSPA_Descriptor * ladspa_descriptor(unsigned long index)
    {
        using namespace lsp;

        ladspa_gen_descriptors();
        return (index < ladspa_descriptors_count) ? &ladspa_descriptors[index] : NULL;
    }
#ifdef __cplusplus
}
#endif
