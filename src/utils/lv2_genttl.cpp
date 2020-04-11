#include <stdio.h>
#include <string.h>

#include <core/types.h>
#include <core/lib.h>

#include <metadata/metadata.h>
#include <plugins/plugins.h>
#include <utils/common.h>

#include <container/lv2/extensions.h>

#define LSP_LV2_EMIT_HEADER(count, text)    \
    if (count == 0) \
    { \
        fputs(text, out); \
        fputc(' ', out); \
    }

#define LSP_LV2_EMIT_OPTION(count, condition, text)    \
    if (condition) \
    { \
        if (count++)    \
            fputs(", ", out); \
        fputs(text, out); \
    }

#define LSP_LV2_EMIT_END(count) \
    if (count > 0) \
    { \
        fprintf(out, " ;\n"); \
        count = 0; \
    }

namespace lsp
{
    enum lv2_requirements
    {
        REQ_PATCH       = 1 << 0,
        REQ_STATE       = 1 << 1,
        REQ_LV2UI       = 1 << 2,
        REQ_PORT_GROUPS = 1 << 3,
        REQ_WORKER      = 1 << 4,
        REQ_MIDI_IN     = 1 << 5,
        REQ_MIDI_OUT    = 1 << 6,
        REQ_PATCH_WR    = 1 << 7,
        REQ_INSTANCE    = 1 << 8,
        REQ_TIME        = 1 << 9,
        REQ_IDISPLAY    = 1 << 10,
        REQ_OSC_IN      = 1 << 11,
        REQ_OSC_OUT     = 1 << 12,
        REQ_MAP_PATH    = 1 << 13,

        REQ_PATH_MASK   = REQ_PATCH | REQ_STATE | REQ_MAP_PATH | REQ_WORKER | REQ_PATCH_WR,
        REQ_MIDI        = REQ_MIDI_IN | REQ_MIDI_OUT
    };

    typedef struct lv2_plugin_group_t
    {
        int            id;
        const char    *name;
    } lv2_plugin_group_t;

    const lv2_plugin_group_t lv2_plugin_groups[] =
    {
        { C_DELAY, "DelayPlugin" },
        { C_REVERB, "ReverbPlugin" },
        { C_DISTORTION, "DistortionPlugin" },
        { C_WAVESHAPER, "WaveshaperPlugin" },
        { C_DYNAMICS, "DynamicsPlugin" },
        { C_AMPLIFIER, "AmplifierPlugin" },
        { C_COMPRESSOR, "CompressorPlugin" },
        { C_ENVELOPE, "EnvelopePlugin" },
        { C_EXPANDER, "ExpanderPlugin" },
        { C_GATE, "GatePlugin" },
        { C_LIMITER, "LimiterPlugin" },
        { C_FILTER, "FilterPlugin" },
        { C_ALLPASS, "AllpassPlugin" },
        { C_BANDPASS, "BandpassPlugin" },
        { C_COMB, "CombPlugin" },
        { C_EQ, "EQPlugin" },
        { C_MULTI_EQ, "MultiEQPlugin" },
        { C_PARA_EQ, "ParaEQPlugin" },
        { C_HIGHPASS, "HighpassPlugin" },
        { C_LOWPASS, "LowpassPlugin" },
        { C_GENERATOR, "GeneratorPlugin" },
        { C_CONSTANT, "ConstantPlugin" },
        { C_INSTRUMENT, "InstrumentPlugin" },
        { C_OSCILLATOR, "OscillatorPlugin" },
        { C_MODULATOR, "ModulatorPlugin" },
        { C_CHORUS, "ChorusPlugin" },
        { C_FLANGER, "FlangerPlugin" },
        { C_PHASER, "PhaserPlugin" },
        { C_SIMULATOR, "SimulatorPlugin" },
        { C_SPATIAL, "SpatialPlugin" },
        { C_SPECTRAL, "SpectralPlugin" },
        { C_PITCH, "PitchPlugin" },
        { C_UTILITY, "UtilityPlugin" },
        { C_ANALYSER, "AnalyserPlugin" },
        { C_CONVERTER, "ConverterPlugin" },
        { C_FUNCTION, "FunctionPlugin" },
        { C_MIXER, "MixerPlugin" },
        { -1, NULL }
    };

    typedef struct lv2_plugin_unit_t
    {
        int             id;
        const char     *name;
        const char     *label;
        const char     *render;
    } lv2_plugin_unit_t;

    const lv2_plugin_unit_t lv2_plugin_units[] =
    {
        { U_PERCENT,    "pc" },
        { U_MM,         "mm" },
        { U_CM,         "cm" },
        { U_M,          "m" },
        { U_INCH,       "inch" },
        { U_KM,         "km" },
        { U_HZ,         "hz" },
        { U_KHZ,        "khz" },
        { U_MHZ,        "mhz" },
        { U_BPM,        "bpm" },
        { U_CENT,       "cent" },
        { U_BAR,        "bar" },
        { U_BEAT,       "beat" },
        { U_SEC,        "s" },
        { U_MSEC,       "ms" },
        { U_DB,         "db" },
        { U_MIN,        "min" },
        { U_DEG,        "degree" },
        { U_OCTAVES,    "oct" },
        { U_SEMITONES,  "semitone12TET" },

        { U_SAMPLES,    NULL,       "samples",              "%.0f"      },
        { U_GAIN_AMP,   NULL,       "gain",                 "%.8f"      },
        { U_GAIN_POW,   NULL,       "gain",                 "%.8f"      },

        { U_DEG_CEL,    NULL,       "degrees Celsium",      "%.2f"      },
        { U_DEG_FAR,    NULL,       "degrees Fahrenheit",   "%.2f"      },
        { U_DEG_K,      NULL,       "degrees Kelvin",       "%.2f"      },
        { U_DEG_R,      NULL,       "degrees Rankine",      "%.2f"      },

        { -1, NULL }
    };

    static void print_additional_groups(FILE *out, const int *c)
    {
        while ((c != NULL) && ((*c) >= 0))
        {
            const lv2_plugin_group_t *grp = lv2_plugin_groups;

            while ((grp != NULL) && (grp->id >= 0))
            {
                if (grp->id == *c)
                {
                    fprintf(out, ", lv2:%s", grp->name);
                    break;
                }
                grp++;
            }

            c++;
        }
    }

    static void print_units(FILE *out, int unit)
    {
        const lv2_plugin_unit_t *u = lv2_plugin_units;

        while ((u != NULL) && (u->id >= 0))
        {
            if (u->id == unit)
            {
                // Check that lv2 contains name
                if (u->name != NULL)
                    fprintf(out, "\t\tunits:unit units:%s ;\n", u->name);
                else
                {
                    const char *symbol = encode_unit(unit);

                    // Build custom type
                    if (symbol != NULL)
                    {
                        fprintf(out, "\t\tunits:unit [\n");
                        fprintf(out, "\t\t\ta units:Unit ;\n");
                        fprintf(out, "\t\t\trdfs:label \"%s\" ;\n", u->label);
                        fprintf(out, "\t\t\tunits:symbol \"%s\" ;\n", symbol);
                        fprintf(out, "\t\t\tunits:render \"%s %s\" ;\n", u->render, symbol);
                        fprintf(out, "\t\t] ;\n");
                    }
                }

                return;
            }
            u++;
        }
    }

    void gen_plugin_ui_ttl (FILE *out, size_t requirements, const plugin_metadata_t &m, const char *name, const char *ui_uri, const char *uri)
    {
        fprintf(out, LSP_PREFIX "_ui:%s\n", name);
        fprintf(out, "\ta ui:" LSP_LV2UI_CLASS " ;\n");
        fprintf(out, "\tlv2:minorVersion %d ;\n", int(LSP_VERSION_MINOR(m.version)));
        fprintf(out, "\tlv2:microVersion %d ;\n", int(LSP_VERSION_MICRO(m.version)));
        fprintf(out, "\tlv2:requiredFeature urid:map, ui:idleInterface ;\n");
        {
            size_t count = 1;
//            fprintf(out, "\tlv2:optionalFeature ui:parent, ui:resize, ui:noUserResize");
            fprintf(out, "\tlv2:optionalFeature ui:parent, ui:resize");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_INSTANCE, "lv2ext:instance-access");
            fprintf(out, " ;\n");
        }
        fprintf(out, "\tlv2:extensionData ui:idleInterface ;\n");
        fprintf(out, "\tui:binary <" LSP_ARTIFACT_ID "-lv2.so> ;\n");
        fprintf(out, "\n");

        size_t ports        = 0;
        size_t port_id      = 0;

        for (const port_t *p = m.ports; (p->id != NULL) && (p->name != NULL); ++p)
        {
            // Skip virtual ports
            switch (p->role)
            {
                case R_UI_SYNC:
                case R_MESH:
                case R_FBUFFER:
                case R_PATH:
                case R_PORT_SET:
                case R_MIDI:
                case R_OSC:
                    continue;
                case R_AUDIO:
                    port_id++;
                    continue;
                default:
                    break;
            }

            fprintf(out, "%s [\n", (ports == 0) ? "\tui:portNotification" : ",");
            fprintf(out, "\t\tui:plugin " LSP_PREFIX ":%s ;\n", name);
            fprintf(out, "\t\tui:portIndex %d ;\n", int(port_id));

            switch (p->role)
            {
                case R_METER:
                    if (p->flags & F_PEAK)
                        fprintf(out, "\t\tui:protocol ui:peakProtocol ;\n");
                    else
                        fprintf(out, "\t\tui:protocol ui:floatProtocol ;\n");
                    break;
                default:
                    fprintf(out, "\t\tui:protocol ui:floatProtocol ;\n");
                    break;
            }

            fprintf(out, "\t] ");

            ports++;
            port_id++;
        }

        // Add atom ports for state serialization
        for (size_t i=0; i<2; ++i)
        {
            fprintf(out, "%s [\n", (ports == 0) ? "\tui:portNotification" : " ,");
            fprintf(out, "\t\tui:plugin " LSP_PREFIX ":%s ;\n", name);
            fprintf(out, "\t\tui:portIndex %d ;\n", int(port_id));
            fprintf(out, "\t\tui:protocol atom:eventTransfer ;\n");
            fprintf(out, "\t\tui:notifyType atom:Sequence ;\n");
            fprintf(out, "\t]");

            ports++;
            port_id++;
        }

        // Add latency report port
        {
            const port_t *p = &lv2_latency_port;
            if ((p->id != NULL) && (p->name != NULL))
            {
                fprintf(out, "%s [\n", (ports == 0) ? "\tui:portNotification" : " ,");
                fprintf(out, "\t\tui:plugin " LSP_PREFIX ":%s ;\n", name);
                fprintf(out, "\t\tui:portIndex %d ;\n", int(port_id));
                fprintf(out, "\t\tui:protocol ui:floatProtocol ;\n");
                fprintf(out, "\t]");

                ports++;
                port_id++;
            }
        }

        // Finish port list
        fprintf(out, "\n\t.\n\n");
    }

    static void print_port_groups(FILE *out, const port_t *port, const port_group_t *pg)
    {
        // For each group
        while ((pg != NULL) && (pg->id != NULL))
        {
            // Scan list of port
            for (const port_group_item_t *p = pg->items; p->id != NULL; ++p)
            {
                if (!strcmp(p->id, port->id))
                {
                    fprintf(out, "\t\tpg:group lsp_pg:%s ;\n", pg->id);
                    const char *role = NULL;
                    switch (p->role)
                    {
                        case PGR_CENTER:        role = "center"; break;
                        case PGR_CENTER_LEFT:   role = "centerLeft"; break;
                        case PGR_CENTER_RIGHT:  role = "centerRight"; break;
                        case PGR_LEFT:          role = "left"; break;
                        case PGR_LO_FREQ:       role = "lowFrequencyEffects"; break;
                        case PGR_REAR_CENTER:   role = "rearCenter"; break;
                        case PGR_REAR_LEFT:     role = "rearLeft"; break;
                        case PGR_REAR_RIGHT:    role = "rearRight"; break;
                        case PGR_RIGHT:         role = "right"; break;
                        case PGR_SIDE:          role = "side"; break;
                        case PGR_SIDE_LEFT:     role = "sideLeft"; break;
                        case PGR_SIDE_RIGHT:    role = "sideRight"; break;
                        default:
                            break;
                    }
                    if (role != NULL)
                        fprintf(out, "\t\tlv2:designation pg:%s ;\n", role);
                    break;
                }
            }

            pg++;
        }
    }

    static size_t scan_port_requirements(const port_t *meta)
    {
        size_t result = REQ_TIME;
        for (const port_t *p = meta; p->id != NULL; ++p)
        {
            switch (p->role)
            {
                case R_PATH:
                    result     |= REQ_PATH_MASK | REQ_INSTANCE;
                    break;
                case R_MESH:
                case R_FBUFFER:
                    result     |= REQ_INSTANCE;
                    break;
                case R_MIDI:
                    if (IS_OUT_PORT(p))
                        result     |= REQ_MIDI_OUT;
                    else
                        result     |= REQ_MIDI_IN;
                    break;
                case R_OSC:
                    if (IS_OUT_PORT(p))
                        result     |= REQ_OSC_OUT;
                    else
                        result     |= REQ_OSC_IN;
                    break;
                case R_PORT_SET:
                    if ((p->members != NULL) && (p->items != NULL))
                        result         |= scan_port_requirements(p->members);
                    result     |= REQ_INSTANCE;
                    break;
                default:
                    break;
            }
        }
        return result;
    }

    static size_t scan_requirements(const plugin_metadata_t &m)
    {
        size_t result   = 0;

#ifndef LSP_NO_LV2_UI
        if (m.lv2_uid != NULL)
        {
            if (m.ui_resource != NULL)
                result |= REQ_LV2UI;
            if (m.extensions & E_INLINE_DISPLAY)
                result |= REQ_IDISPLAY;
        }
#endif

        result |= scan_port_requirements(m.ports);

        if ((m.port_groups != NULL) && (m.port_groups->id != NULL))
            result |= REQ_PORT_GROUPS;

        return result;
    }

    void gen_plugin_ttl(const char *path, const plugin_metadata_t &m, const char *uri)
    {
        char fname[PATH_MAX];
        FILE *out = NULL;
        snprintf(fname, sizeof(fname)-1, "%s/%s.ttl", path, m.lv2_uid);
        size_t requirements     = scan_requirements(m);

        // Generate manifest.ttl
        if (!(out = fopen(fname, "w+")))
            return;
        printf("Writing file %s\n", fname);

        // Output header
        fprintf(out, "@prefix doap:      <http://usefulinc.com/ns/doap#> .\n");
        fprintf(out, "@prefix dc:        <http://purl.org/dc/terms/> .\n");
        fprintf(out, "@prefix foaf:      <http://xmlns.com/foaf/0.1/> .\n");
        fprintf(out, "@prefix rdf:       <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n");
        fprintf(out, "@prefix rdfs:      <http://www.w3.org/2000/01/rdf-schema#> .\n");
        fprintf(out, "@prefix lv2:       <" LV2_CORE_PREFIX "> .\n");
        if (requirements & REQ_INSTANCE)
            fprintf(out, "@prefix lv2ext:    <http://lv2plug.in/ns/ext/> .\n");
        fprintf(out, "@prefix pp:        <" LV2_PORT_PROPS_PREFIX "> .\n");
        if (requirements & REQ_PORT_GROUPS)
            fprintf(out, "@prefix pg:        <" LV2_PORT_GROUPS_PREFIX "> .\n");
        if (requirements & REQ_LV2UI)
            fprintf(out, "@prefix ui:        <" LV2_UI_PREFIX "> .\n");
        fprintf(out, "@prefix units:     <" LV2_UNITS_PREFIX "> .\n");
        fprintf(out, "@prefix atom:      <" LV2_ATOM_PREFIX "> .\n");
        fprintf(out, "@prefix urid:      <" LV2_URID_PREFIX "> .\n");
        fprintf(out, "@prefix opts:      <" LV2_OPTIONS_PREFIX "> .\n");
        if (requirements & REQ_WORKER)
            fprintf(out, "@prefix work:      <" LV2_WORKER_PREFIX "> .\n");
        fprintf(out, "@prefix rsz:       <" LV2_RESIZE_PORT_PREFIX "> .\n");
        if (requirements & REQ_PATCH)
            fprintf(out, "@prefix patch:     <" LV2_PATCH_PREFIX "> .\n");
        if (requirements & (REQ_STATE | REQ_MAP_PATH))
            fprintf(out, "@prefix state:     <" LV2_STATE_PREFIX "> .\n");
        if (requirements & REQ_MIDI)
            fprintf(out, "@prefix midi:      <" LV2_MIDI_PREFIX "> .\n");
        if (requirements & REQ_TIME)
            fprintf(out, "@prefix time:      <" LV2_TIME_URI "#> .\n");
        if (requirements & REQ_IDISPLAY)
            fprintf(out, "@prefix hcid:      <" LV2_INLINEDISPLAY_PREFIX "> .\n");

        fprintf(out, "@prefix " LSP_PREFIX ":       <" LSP_URI(lv2) "> .\n");
        if (requirements & REQ_PORT_GROUPS)
            fprintf(out, "@prefix lsp_pg:    <%s%s/port_groups#> .\n", LSP_URI(lv2), m.lv2_uid);
        if (requirements & REQ_LV2UI)
            fprintf(out, "@prefix " LSP_PREFIX "_ui:    <" LSP_UI_URI(lv2) "> .\n");
        fprintf(out, "@prefix " LSP_PREFIX "_dev:   <" LSP_DEVELOPERS_URI "> .\n");
        if (requirements & REQ_PATCH)
            fprintf(out, "@prefix lsp_p:     <%s%s/ports#> .\n", LSP_URI(lv2), m.lv2_uid);

        fprintf(out, "\n\n");

        // Output developer and maintainer objects
        const person_t *dev = m.developer;
        if ((dev != NULL) && (dev->uid != NULL))
        {
            fprintf(out, LSP_PREFIX "_dev:%s\n", dev->uid);
            fprintf(out, "\ta foaf:Person");
            if (dev->name != NULL)
                fprintf(out, " ;\n\tfoaf:name \"%s\"", dev->name);
            if (dev->nick != NULL)
                fprintf(out, " ;\n\tfoaf:nick \"%s\"", dev->nick);
            if (dev->mailbox != NULL)
                fprintf(out, " ;\n\tfoaf:mbox <mailto:%s>", dev->mailbox);
            if (dev->homepage != NULL)
                fprintf(out, " ;\n\tfoaf:homepage <%s#%s>", dev->homepage, dev->uid);
            fprintf(out, "\n\t.\n\n");
        }

        fprintf(out, LSP_PREFIX "_dev:lsp\n");
        fprintf(out, "\ta foaf:Person");
        fprintf(out, " ;\n\tfoaf:name \"" LSP_ACRONYM " LV2\"");
        fprintf(out, " ;\n\tfoaf:homepage <" LSP_BASE_URI "#lsp>");
        fprintf(out, "\n\t.\n\n");

        // Output port groups
        if (requirements & REQ_PORT_GROUPS)
        {
            for (const port_group_t *pg = m.port_groups; (pg != NULL) && (pg->id != NULL); pg++)
            {
                const char *grp_type = NULL, *grp_dir = (pg->flags & PGF_OUT) ? "OutputGroup" : "InputGroup";
                switch (pg->type)
                {
                    case GRP_1_0:   grp_type = "MonoGroup"; break;
                    case GRP_2_0:   grp_type = "StereoGroup"; break;
                    case GRP_MS:    grp_type = "MidSideGroup"; break;
                    case GRP_3_0:   grp_type = "ThreePointZeroGroup"; break;
                    case GRP_4_0:   grp_type = "FourPointZeroGroup"; break;
                    case GRP_5_0:   grp_type = "FivePointZeroGroup"; break;
                    case GRP_5_1:   grp_type = "FivePointOneGroup"; break;
                    case GRP_6_1:   grp_type = "SixPointOneGroup"; break;
                    case GRP_7_1:   grp_type = "SevenPointOneGroup"; break;
                    case GRP_7_1W:  grp_type = "SevenPointOneWideGroup"; break;
                    default:
                        break;
                }

                fprintf(out, "lsp_pg:%s\n", pg->id);
                if (grp_type != NULL)
                    fprintf(out, "\ta pg:%s, pg:%s ;\n", grp_type, grp_dir);
                else
                    fprintf(out, "\ta pg:%s ;\n", grp_dir);

                if (pg->flags & PGF_SIDECHAIN)
                    fprintf(out, "\tpg:sideChainOf lsp_pg:%s;\n", pg->parent_id);

                fprintf(out, "\tlv2:symbol \"%s\";\n", pg->id);
                fprintf(out, "\trdfs:label \"%s\"\n", pg->name);
                fprintf(out, "\t.\n\n");
            }
        }

        // Output special parameters
        for (const port_t *p = m.ports; p->id != NULL; ++p)
        {
            switch (p->role)
            {
                case R_PATH:
                {
                    if (requirements & REQ_PATCH)
                    {
                        fprintf(out, "lsp_p:%s\n", p->id);
                        fprintf(out, "\ta lv2:Parameter ;\n");
                        fprintf(out, "\trdfs:label \"%s\" ;\n", p->name);
                        fprintf(out, "\trdfs:range atom:Path\n");
                        fprintf(out, "\t.\n\n");
                    }
                    break;
                }
                default:
                    break;
            }
        }

        // Output plugin
        fprintf(out, LSP_PREFIX ":%s\n", m.lv2_uid);
        fprintf(out, "\ta lv2:Plugin, doap:Project");
        print_additional_groups(out, m.classes);
        fprintf(out, " ;\n");
        fprintf(out, "\tdoap:name \"" LSP_ACRONYM " %s\", \"" LSP_ACRONYM " %s\"@de ;\n", m.description, m.name);
        fprintf(out, "\tlv2:minorVersion %d ;\n", int(LSP_VERSION_MINOR(m.version)));
        fprintf(out, "\tlv2:microVersion %d ;\n", int(LSP_VERSION_MICRO(m.version)));
        if ((dev != NULL) && (dev->uid != NULL))
            fprintf(out, "\tdoap:developer " LSP_PREFIX "_dev:%s ;\n", m.developer->uid);
        fprintf(out, "\tdoap:maintainer " LSP_PREFIX "_dev:lsp ;\n");
        fprintf(out, "\tdoap:license \"" LSP_COPYRIGHT "\" ;\n");
        fprintf(out, "\tlv2:binary <" LSP_ARTIFACT_ID "-lv2.so> ;\n");
        if (requirements & REQ_LV2UI)
            fprintf(out, "\tui:ui " LSP_PREFIX "_ui:%s ;\n", m.lv2_uid);

        fprintf(out, "\n");

        fprintf(out, "\tlv2:requiredFeature urid:map ;\n");

        // Emit optional features
        {
            size_t count = 1;
            fprintf(out, "\tlv2:optionalFeature lv2:hardRTCapable");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_WORKER, "work:schedule");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_IDISPLAY, "hcid:queue_draw");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_MAP_PATH, "state:mapPath");
            fprintf(out, " ;\n");
        }

        // Emit extension data
        if (requirements & (REQ_STATE | REQ_WORKER | REQ_IDISPLAY))
        {
            size_t count = 0;
            fprintf(out, "\tlv2:extensionData ");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_STATE, "state:interface");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_WORKER, "work:interface");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_IDISPLAY, "hcid:interface");
            fprintf(out, " ;\n");
        }

        // Different supported options
        if (requirements & REQ_LV2UI)
            fprintf(out, "\topts:supportedOption ui:updateRate ;\n");

        // Replacement for LADSPA plugin
        if (m.ladspa_id > 0)
            fprintf(out, "\tdc:replaces <urn:ladspa:%ld> ;\n", long(m.ladspa_id));
        fprintf(out, "\n");

        size_t port_id = 0;

        // Output special parameters
        if (requirements & REQ_PATCH_WR)
        {
            size_t count = 0;
            const port_t *first = NULL;
            for (const port_t *p = m.ports; p->id != NULL; ++p)
            {
                switch (p->role)
                {
                    case R_PATH:
                        count++;
                        if (first == NULL)
                            first = p;
                        break;
                    default:
                        break;
                }
            }

            if (first != NULL)
            {
                fprintf(out, "\tpatch:writable");
                if (count > 1)
                {
                    fprintf(out, "\n");
                    for (const port_t *p = m.ports; (p->id != NULL) && (p->name != NULL); ++p)
                    {
                        switch (p->role)
                        {
                            case R_PATH:
                            {
                                fprintf(out, "\t\tlsp_p:%s", p->id);
                                if (--count)
                                    fprintf(out, " ,\n");
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    fprintf(out, " ;\n\n");
                }
                else
                    fprintf(out, " lsp_p:%s ;\n\n", first->id);
            }
        }

        for (const port_t *p = m.ports; (p->id != NULL) && (p->name != NULL); ++p)
        {
            // Skip virtual ports
            switch (p->role)
            {
                case R_UI_SYNC:
                case R_MESH:
                case R_FBUFFER:
                case R_PATH:
                case R_PORT_SET:
                case R_MIDI:
                case R_OSC:
                    continue;
                default:
                    break;
            }

            fprintf(out, "%s [\n", (port_id == 0) ? "\tlv2:port" : " ,");
            fprintf(out, "\t\ta lv2:%s, ", (p->flags & F_OUT) ? "OutputPort" : "InputPort");

            switch (p->role)
            {
                case R_AUDIO:
                    fprintf(out, "lv2:AudioPort ;\n");
                    break;
                case R_CONTROL:
                case R_BYPASS:
                case R_METER:
                    fprintf(out, "lv2:ControlPort ;\n");
                    break;
                default:
                    break;
            }

            fprintf(out, "\t\tlv2:index %d ;\n", (int)port_id);
            fprintf(out, "\t\tlv2:symbol \"%s\" ;\n", (p->role == R_BYPASS) ? "enabled" : p->id);
            fprintf(out, "\t\tlv2:name \"%s\" ;\n", (p->role == R_BYPASS) ? "Enabled" : p->name);
            if (p->role == R_BYPASS)
                fprintf(out, "\t\tlv2:designation lv2:enabled ;\n");

            print_units(out, p->unit);

            size_t p_prop = 0;

            if (p->flags & F_LOG)
            {
                LSP_LV2_EMIT_HEADER(p_prop, "\t\tlv2:portProperty");
                LSP_LV2_EMIT_OPTION(p_prop, true, "pp:logarithmic");
            }

            if (p->unit == U_BOOL)
            {
                LSP_LV2_EMIT_HEADER(p_prop, "\t\tlv2:portProperty");
                LSP_LV2_EMIT_OPTION(p_prop, true, "lv2:toggled");
                LSP_LV2_EMIT_END(p_prop);
//                if (p->flags & F_TRG)
//                    fprintf(out, "\t\tlv2:portProperty pp:trigger ;\n");
                fprintf(out, "\t\tlv2:minimum %d ;\n", 0);
                fprintf(out, "\t\tlv2:maximum %d ;\n", 1);
                fprintf(out, "\t\tlv2:default %d ;\n", (p->role == R_BYPASS) ? (1 - int(p->start)) : int(p->start));
            }
            else if (p->unit == U_ENUM)
            {
                LSP_LV2_EMIT_HEADER(p_prop, "\t\tlv2:portProperty");
                LSP_LV2_EMIT_OPTION(p_prop, true, "lv2:integer");
                LSP_LV2_EMIT_OPTION(p_prop, true, "lv2:enumeration");
                LSP_LV2_EMIT_OPTION(p_prop, true, "pp:hasStrictBounds");
                LSP_LV2_EMIT_END(p_prop);

                int min  = (p->flags & F_LOWER) ? p->min : 0;
                int curr = min;
                size_t count = list_size(p->items);
                int max  = min + list_size(p->items) - 1;

                const port_item_t *list = p->items;
                if (count > 1)
                {
                    fprintf(out, "\t\tlv2:scalePoint\n");
                    for ( ; list->text != NULL; ++list)
                    {
                        fprintf(out, "\t\t\t[ rdfs:label \"%s\"; rdf:value %d ]", list->text, curr);
                        if (--count)
                            fprintf(out, " ,\n");
                        else
                            fprintf(out, " ;\n");
                        curr ++;
                    }
                } else if (count > 0)
                    fprintf(out, "\t\tlv2:scalePoint [ rdfs:label \"%s\"; rdf:value %d ] ;\n", list->text, curr);

                fprintf(out, "\t\tlv2:minimum %d ;\n", min);
                fprintf(out, "\t\tlv2:maximum %d ;\n", max);
                fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
            }
            else if (p->unit == U_SAMPLES)
            {
                LSP_LV2_EMIT_HEADER(p_prop, "\t\tlv2:portProperty");
                LSP_LV2_EMIT_OPTION(p_prop, true, "lv2:integer");
                if ((p->flags & (F_LOWER | F_UPPER)) == (F_LOWER | F_UPPER))
                    LSP_LV2_EMIT_OPTION(p_prop, true, "pp:hasStrictBounds");
                LSP_LV2_EMIT_END(p_prop);

                if (p->flags & F_LOWER)
                    fprintf(out, "\t\tlv2:minimum %d ;\n", int(p->min));
                if (p->flags & F_UPPER)
                    fprintf(out, "\t\tlv2:maximum %d ;\n", int(p->max));
                fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
            }
            else if (p->flags & F_INT)
            {
                LSP_LV2_EMIT_HEADER(p_prop, "\t\tlv2:portProperty");
                LSP_LV2_EMIT_OPTION(p_prop, true, "lv2:integer");
                if ((p->flags & (F_LOWER | F_UPPER)) == (F_LOWER | F_UPPER))
                    LSP_LV2_EMIT_OPTION(p_prop, true, "pp:hasStrictBounds");
                LSP_LV2_EMIT_END(p_prop);

                if (p->flags & F_LOWER)
                    fprintf(out, "\t\tlv2:minimum %d ;\n", int(p->min));
                if (p->flags & F_UPPER)
                    fprintf(out, "\t\tlv2:maximum %d ;\n", int(p->max));
                if ((p->role == R_CONTROL) || (p->role == R_METER))
                    fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
            }
            else
            {
                if ((p->flags & (F_LOWER | F_UPPER)) == (F_LOWER | F_UPPER))
                {
                    LSP_LV2_EMIT_HEADER(p_prop, "\t\tlv2:portProperty");
                    LSP_LV2_EMIT_OPTION(p_prop, true, "pp:hasStrictBounds");
                    LSP_LV2_EMIT_END(p_prop);
                }

                if (p->flags & F_LOWER)
                    fprintf(out, "\t\tlv2:minimum %.6f ;\n", p->min);
                if (p->flags & F_UPPER)
                    fprintf(out, "\t\tlv2:maximum %.6f ;\n", p->max);
                if ((p->role == R_CONTROL) || (p->role == R_METER))
                    fprintf(out, "\t\tlv2:default %.6f ;\n", p->start);
            }

            LSP_LV2_EMIT_END(p_prop);

            // Output all port groups of the port
            if (requirements & REQ_PORT_GROUPS)
                print_port_groups(out, p, m.port_groups);

            fprintf(out, "\t]");
            port_id++;
        }

        // Add atom ports for state serialization
        for (size_t i=0; i<2; ++i)
        {
            const port_t *p = &lv2_atom_ports[i];

            fprintf(out, "%s [\n", (port_id == 0) ? "\tlv2:port" : " ,");
            fprintf(out, "\t\ta lv2:%s, atom:AtomPort ;\n", (i > 0) ? "OutputPort" : "InputPort");
            fprintf(out, "\t\tatom:bufferType atom:Sequence ;\n");

            fprintf(out, "\t\tatom:supports atom:Sequence");
            if (requirements & REQ_PATCH)
                fprintf(out, ", patch:Message");
            if (requirements & REQ_TIME)
                fprintf(out, ", time:Position");
            if ((i == 0) && (requirements & REQ_MIDI_IN))
                fprintf(out, ", midi:MidiEvent");
            else if ((i == 1) && (requirements & REQ_MIDI_OUT))
                fprintf(out, ", midi:MidiEvent");
            fprintf(out, " ;\n");

            const char *p_id    = p->id;
            const char *p_name  = p->name;
            const char *comm    = NULL;
            if (IS_IN_PORT(p))
            {
                comm            = "UI -> DSP communication";
                if (requirements & REQ_MIDI_IN)
                {
                    p_id            = LSP_LV2_MIDI_PORT_IN;
                    p_name          = "MIDI Input, UI Input";
                    comm            = "MIDI IN, UI -> DSP communication";
                }
            }
            else
            {
                comm            = "DSP -> UI communication";
                if (requirements & REQ_MIDI_IN)
                {
                    p_id            = LSP_LV2_MIDI_PORT_OUT;
                    p_name          = "MIDI Output, UI Output";
                    comm            = "MIDI OUT, DSP -> UI communication";
                }
            }

            long bufsize    = lv2_all_port_sizes(m.ports, IS_IN_PORT(p), IS_OUT_PORT(p));
            if (m.extensions & E_KVT_SYNC)
                bufsize        += OSC_BUFFER_MAX;

            fprintf(out, "\t\tlv2:designation lv2:control ;\n");
            fprintf(out, "\t\tlv2:index %d ;\n", int(port_id));
            fprintf(out, "\t\tlv2:symbol \"%s\" ;\n", p_id);
            fprintf(out, "\t\tlv2:name \"%s\" ;\n", p_name);
            fprintf(out, "\t\trdfs:comment \"%s\" ;\n", comm);
            fprintf(out, "\t\trsz:minimumSize %ld ;\n", bufsize * 2);
            fprintf(out, "\t]");

            port_id++;
        }

        // Add sample rate reporting port
        {
            const port_t *p = &lv2_latency_port;
            if ((p->id != NULL) && (p->name != NULL))
            {
                fprintf(out, "%s [\n", (port_id == 0) ? "\tlv2:port" : " ,");
                fprintf(out, "\t\ta lv2:%s, lv2:ControlPort ;\n", (p->flags & F_OUT) ? "OutputPort" : "InputPort");
                fprintf(out, "\t\tlv2:index %d ;\n", int(port_id));
                fprintf(out, "\t\tlv2:symbol \"%s\" ;\n", p->id);
                fprintf(out, "\t\tlv2:name \"%s\" ;\n", p->name);
                fprintf(out, "\t\trdfs:comment \"DSP -> Host latency report\" ;\n");

                if ((p->flags & (F_LOWER | F_UPPER)) == (F_LOWER | F_UPPER))
                    fprintf(out, "\t\tlv2:portProperty pp:hasStrictBounds ;\n");
                if (p->flags & F_INT)
                    fprintf(out, "\t\tlv2:portProperty lv2:integer ;\n");
                fprintf(out, "\t\tlv2:portProperty lv2:reportsLatency ;\n");

                if (p->flags & F_LOWER)
                    fprintf(out, "\t\tlv2:minimum %d ;\n", int(p->min));
                if (p->flags & F_UPPER)
                    fprintf(out, "\t\tlv2:maximum %d ;\n", int(p->max));
                fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
                fprintf(out, "\t]");

                port_id++;
            }
        }

        // Finish port list
        fprintf(out, "\n\t.\n\n");

        // Output plugin UIs
        if (requirements & REQ_LV2UI)
        {
            char *ui_uri = NULL, *plugin_uri = NULL;
            int n = asprintf(&ui_uri, LSP_PLUGIN_UI_URI(lv2, "%s"), m.lv2_uid);
            if (n >= 0)
                n = asprintf(&plugin_uri, LSP_PLUGIN_URI(lv2, "%s"), m.lv2_uid);

            if ((n >= 0) && (ui_uri != NULL) && (plugin_uri != NULL))
                gen_plugin_ui_ttl(out, requirements, m, m.lv2_uid, ui_uri, plugin_uri);

            if (ui_uri != NULL)
                free(ui_uri);
            if (plugin_uri != NULL)
                free(plugin_uri);
        }

        fclose(out);
    }

    void gen_manifest(const char *path)
    {
        char fname[2048];
        snprintf(fname, sizeof(fname)-1, "%s/manifest.ttl", path);
        FILE *out = NULL;

        // Generate manifest.ttl
        if (!(out = fopen(fname, "w+")))
            return;
        printf("Writing file %s\n", fname);

        fprintf(out, "@prefix lv2:   <http://lv2plug.in/ns/lv2core#> .\n");
        fprintf(out, "@prefix rdfs:  <http://www.w3.org/2000/01/rdf-schema#> .\n");
        fprintf(out, "@prefix " LSP_PREFIX ":   <" LSP_URI(lv2) "> .\n\n");

        #define MOD_PLUGIN(plugin, ui) \
            if (plugin::metadata.lv2_uid != NULL) \
            { \
                fprintf(out, LSP_PREFIX ":" #plugin "\n"); \
                fprintf(out, "\ta lv2:Plugin ;\n"); \
                fprintf(out, "\tlv2:binary <" LSP_ARTIFACT_ID "-lv2.so> ;\n"); \
                fprintf(out, "\trdfs:seeAlso <%s.ttl> .\n\n", #plugin); \
            }
        #include <metadata/modules.h>
        fclose(out);
    }

    void gen_ttl(const char *path)
    {
        gen_manifest(path);

        // Output plugins
        size_t id = 0;
        #define MOD_PLUGIN(plugin, ui) \
        if (plugin::metadata.lv2_uid != NULL) \
        { \
            gen_plugin_ttl(path, plugin::metadata, LSP_PLUGIN_URI(lv2, plugin)); \
            id++; \
        }
        #include <metadata/modules.h>
    }
}

#ifndef LSP_IDE_DEBUG
int main(int argc, const char **argv)
{
    if (argc <= 0)
        fprintf(stderr, "required destination path");
    lsp::gen_ttl(argv[1]);

    return 0;
}
#endif /* LSP_IDE_DEBUG */
