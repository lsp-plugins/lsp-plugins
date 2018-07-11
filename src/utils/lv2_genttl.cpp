#include <stdio.h>
#include <string.h>

#include <core/metadata.h>
#include <plugins/plugins.h>
#include <core/lib.h>

#include <container/lv2ext.h>

#define LSP_LV2_EMIT_OPTION(count, condition, text)    \
    if (condition) \
    { \
        if (count++)    \
            fputs(", ", out); \
        fputs(text, out); \
    }

namespace lsp
{
    enum lv2_requirements
    {
        REQ_PATCH       = 1 << 0,
        REQ_STATE       = 1 << 1,
        REQ_GTK2_UI     = 1 << 2,
        REQ_GTK3_UI     = 1 << 3,
        REQ_QT4_UI      = 1 << 4,
        REQ_QT5_UI      = 1 << 5,
        REQ_PORT_GROUPS = 1 << 6,
        REQ_WORKER      = 1 << 7,


        REQ_UI_MASK     = REQ_GTK2_UI | REQ_GTK3_UI | REQ_QT4_UI | REQ_QT5_UI,
        REQ_PATH_MASK   = REQ_PATCH | REQ_STATE | REQ_WORKER
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

    // TODO

    const lv2_plugin_unit_t lv2_plugin_units[] =
    {
        { U_PERCENT, "pc" },
        { U_MM, "mm" },
        { U_CM, "cm" },
        { U_M,  "m" },
        { U_INCH,  "inch" },
        { U_KM,  "km" },
        { U_HZ,  "hz" },
        { U_KHZ,  "khz" },
        { U_MHZ,  "mhz" },
        { U_BPM,  "bpm" },
        { U_CENT,  "cent" },
        { U_BAR, "bar" },
        { U_BEAT, "beat" },
        { U_SEC, "s" },
        { U_MSEC, "ms" },
        { U_DB, "db" },
        { U_DEG, "degree" },

        { U_SAMPLES, NULL, "samples", "%.0f" },
        { U_GAIN_AMP, NULL, "gain", "%f"     },
        { U_GAIN_POW, NULL, "gain", "%f"     },

        { U_DEG_CEL, NULL, "degrees Celsium", "%.2f"         },
        { U_DEG_CEL, NULL, "degrees Fahrenheit", "%.2f"      },
        { U_DEG_CEL, NULL, "degrees Kelvin", "%.2f"          },
        { U_DEG_CEL, NULL, "degrees Rankine", "%.2f"         },

        { -1, NULL }
    };

    enum ui_type_t
    {
        gtk,
        gtk3,
        qt4,
        qt5,
        win
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

    static void print_plugin_ui(FILE *out, const char *name, const char *uri)
    {
        #define MOD_UI(plugin, package)             \
            if (!strcmp(name, #plugin))             \
                fprintf(out, "\tui:ui " LSP_PREFIX "_%s:%s ;\n", #package, #plugin);

        #define MOD_GTK2(plugin)            MOD_UI(plugin, gtk2)
        #define MOD_GTK3(plugin)            MOD_UI(plugin, gtk3)
        #define MOD_QT4(plugin)             MOD_UI(plugin, qt4)
        #define MOD_QT5(plugin)             MOD_UI(plugin, qt5)

        #include <core/modules.h>
    }

    static void print_ladspa_replacement(FILE *out, const char *name)
    {
        long ladspa_id = LSP_LADSPA_BASE;

        #define MOD_LADSPA(plugin) \
            if (!strcmp(name, #plugin)) \
                fprintf(out, "\tdc:replaces <urn:ladspa:%d> ;\n", (int)ladspa_id); \
            ladspa_id++;

        #include <core/modules.h>
    }

    void gen_plugin_ui_ttl (FILE *out, const plugin_metadata_t &m, const char *name, const char *ui_uri, const char *ui_class, const char *package, const char *uri)
    {
        fprintf(out, LSP_PREFIX "_%s:%s\n", package, name);
        fprintf(out, "\ta ui:%s ;\n", ui_class);
        fprintf(out, "\tlv2:minorVersion %d ;\n", int(LSP_VERSION_MINOR(m.version)));
        fprintf(out, "\tlv2:microVersion %d ;\n", int(LSP_VERSION_MICRO(m.version)));
        fprintf(out, "\tui:binary <" LSP_ARTIFACT_ID "-lv2-%s.so> ;\n", package);
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
                    continue;
                case R_AUDIO:
                    port_id++;
                    continue;
                default:
                    break;
            }

            fprintf(out, "%s [\n", (ports == 0) ? "\tui:portNotification" : ",");
            fprintf(out, "\t\tui:plugin " LSP_PREFIX ":%s ;\n", name);
            fprintf(out, "\t\tui:portIndex %d ;\n", (int)port_id);

            switch (p->role)
            {
                case R_METER:
                    fprintf(out, "\t\tui:protocol ui:peakProtocol ;\n");
                    break;
                default:
                    fprintf(out, "\t\tui:protocol ui:floatProtocol ;\n");
                    break;
            }

            fprintf(out, "\t] ");

            ports++;
            port_id++;
        }

        // Add atom communication
        for (const port_t *p = lv2_atom_ports; (p->id != NULL) && (p->name != NULL); ++p)
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

    static size_t scan_requirements(const plugin_metadata_t &m, const char *name, const char *uri)
    {
        size_t result   = 0;

        #define SET_REQUIREMENTS(plugin, flag)   \
            if (!strcmp(name, #plugin))    result |= flag;

        #define MOD_GTK2(plugin)        SET_REQUIREMENTS(plugin, REQ_GTK2_UI)
        #define MOD_GTK3(plugin)        SET_REQUIREMENTS(plugin, REQ_GTK3_UI)
        #define MOD_QT4(plugin)         SET_REQUIREMENTS(plugin, REQ_QT4_UI)
        #define MOD_QT5(plugin)         SET_REQUIREMENTS(plugin, REQ_QT5_UI)
        #include <core/modules.h>
        #undef SET_REQUIREMENTS

        for (const port_t *p = m.ports; (p->id != NULL) && (p->name != NULL); ++p)
        {
            switch (p->role)
            {
                case R_PATH:
                    result    |= REQ_PATH_MASK;
                    break;
                default:
                    break;
            }
        }

        if ((m.port_groups != NULL) && (m.port_groups->id != NULL))
            result |= REQ_PORT_GROUPS;

        return result;
    }

    void gen_plugin_ttl(const char *path, const plugin_metadata_t &m, const char *name, const char *uri)
    {
        char fname[2048];
        FILE *out = NULL;
        snprintf(fname, sizeof(fname)-1, "%s/%s.ttl", path, name);
        size_t requirements     = scan_requirements(m, name, uri);
//        bool patch_support = false;

        // Generate manifest.ttl
        if (!(out = fopen(fname, "w+")))
            return;
        printf("Writing file %s\n", fname);

        // Output header
        fprintf(out, "@prefix lv2:       <" LV2_CORE_PREFIX "> .\n");
        fprintf(out, "@prefix pp:        <" LV2_PORT_PROPS_PREFIX "> .\n");
        if (requirements & REQ_PORT_GROUPS)
            fprintf(out, "@prefix pg:        <" LV2_PORT_GROUPS_PREFIX "> .\n");
        if (requirements & REQ_UI_MASK)
            fprintf(out, "@prefix ui:        <" LV2_UI_PREFIX "> .\n");
        fprintf(out, "@prefix units:     <" LV2_UNITS_PREFIX "> .\n");
        fprintf(out, "@prefix atom:      <" LV2_ATOM_PREFIX "> .\n");
        fprintf(out, "@prefix urid:      <" LV2_URID_PREFIX "> .\n");
        if (requirements & REQ_WORKER)
            fprintf(out, "@prefix work:      <" LV2_WORKER_PREFIX "> .\n");
        fprintf(out, "@prefix rsz:       <" LV2_RESIZE_PORT_PREFIX "> .\n");
        if (requirements & REQ_PATCH)
            fprintf(out, "@prefix patch:     <" LV2_PATCH_PREFIX "> .\n");
        if (requirements & REQ_STATE)
            fprintf(out, "@prefix state:     <" LV2_STATE_PREFIX "> .\n");
        fprintf(out, "@prefix doap:      <http://usefulinc.com/ns/doap#> .\n");
        fprintf(out, "@prefix foaf:      <http://xmlns.com/foaf/0.1/> .\n");
        fprintf(out, "@prefix dc:        <http://purl.org/dc/terms/> .\n");
        fprintf(out, "@prefix rdf:       <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n");
        fprintf(out, "@prefix rdfs:      <http://www.w3.org/2000/01/rdf-schema#> .\n");
        fprintf(out, "@prefix " LSP_PREFIX ":       <" LSP_URI(lv2) "> .\n");
        if (requirements & REQ_PATCH)
            fprintf(out, "@prefix lsp_p:     <%s/ports#> .\n", uri);
        if (requirements & REQ_PORT_GROUPS)
            fprintf(out, "@prefix lsp_pg:    <%s/port_groups#> .\n", uri);
        if (requirements & REQ_GTK2_UI)
            fprintf(out, "@prefix " LSP_PREFIX "_gtk2:  <" LSP_UI_URI(lv2, gtk2) "/> .\n");
        if (requirements & REQ_GTK3_UI)
            fprintf(out, "@prefix " LSP_PREFIX "_gtk3:  <" LSP_UI_URI(lv2, gtk3) "/> .\n");
        if (requirements & REQ_QT4_UI)
            fprintf(out, "@prefix " LSP_PREFIX "_qt4:   <" LSP_UI_URI(lv2, qt4)  "/> .\n");
        if (requirements & REQ_QT5_UI)
            fprintf(out, "@prefix " LSP_PREFIX "_qt5:   <" LSP_UI_URI(lv2, qt5)  "/> .\n");
        fprintf(out, "\n\n");

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

                fprintf(out, "\tlv2:symbol \"%s\";\n", pg->id);
                fprintf(out, "\trdfs:label \"%s\"\n", pg->name);
                fprintf(out, "\t.\n\n");
            }
        }

        // Output special parameters
        for (const port_t *p = m.ports; (p->id != NULL) && (p->name != NULL); ++p)
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
        fprintf(out, LSP_PREFIX ":%s\n", name);
        fprintf(out, "\ta lv2:Plugin");
        print_additional_groups(out, m.classes);
        fprintf(out, " ;\n");
        fprintf(out, "\tdoap:name \"" LSP_ACRONYM " %s - %s [LV2]\" ;\n", m.name, m.description);
        fprintf(out, "\tlv2:minorVersion %d ;\n", int(LSP_VERSION_MINOR(m.version)));
        fprintf(out, "\tlv2:microVersion %d ;\n", int(LSP_VERSION_MICRO(m.version)));
        fprintf(out, "\tdoap:developer [\n");
        fprintf(out, "\t\tfoaf:name \"%s\" ;\n", m.author);
        fprintf(out, "\t\tfoaf:homepage <" LSP_BASE_URI "> ;\n");
        fprintf(out, "\t] ;\n");
        fprintf(out, "\tdoap:maintainer [\n");
        fprintf(out, "\t\tfoaf:name \"" LSP_ACRONYM " [LV2]\" ;\n");
        fprintf(out, "\t\tfoaf:homepage <" LSP_BASE_URI "> ;\n");
        fprintf(out, "\t] ;\n");
        fprintf(out, "\tdoap:license \"" LSP_COPYRIGHT "\" ;\n");
        fprintf(out, "\tlv2:binary <" LSP_ARTIFACT_ID "-lv2.so> ;\n");
        if (requirements & REQ_UI_MASK)
            print_plugin_ui(out, name, uri);
        fprintf(out, "\n");

        {
            size_t count = 0;
            fprintf(out, "\tlv2:requiredFeature ");
            LSP_LV2_EMIT_OPTION(count, true, "urid:map");
            fprintf(out, " ;\n");
        }

        {
            size_t count = 0;
            fprintf(out, "\tlv2:optionalFeature ");
            LSP_LV2_EMIT_OPTION(count, true, "lv2:hardRTCapable");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_WORKER, "work:schedule");
            fprintf(out, " ;\n");
        }

        if (requirements & (REQ_STATE))
        {
            size_t count = 0;
            fprintf(out, "\tlv2:extensionData ");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_STATE, "state:interface");
            LSP_LV2_EMIT_OPTION(count, requirements & REQ_WORKER, "work:interface");
            fprintf(out, " ;\n");
        }

        print_ladspa_replacement(out, name);
        fprintf(out, "\n");

        size_t port_id = 0;

        // Output special parameters
        for (const port_t *p = m.ports; (p->id != NULL) && (p->name != NULL); ++p)
        {
            switch (p->role)
            {
                case R_PATH:
                {
                    if (requirements & REQ_PATCH)
                        fprintf(out, "\tpatch:writable lsp_p:%s;\n", p->id);
                    break;
                }
                default:
                    break;
            }
        }

        for (const port_t *p = m.ports; (p->id != NULL) && (p->name != NULL); ++p)
        {
            // Skip virtual ports
            switch (p->role)
            {
                case R_UI_SYNC:
                case R_MESH:
                case R_PATH:
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
                case R_METER:
                    fprintf(out, "lv2:ControlPort ;\n");
                    break;
                default:
                    break;
            }

            fprintf(out, "\t\tlv2:index %d ;\n", (int)port_id);
            fprintf(out, "\t\tlv2:symbol \"%s\" ;\n", p->id);
//            const char *units = encode_unit(p->unit);
//            if (units != NULL)
//                fprintf(out, "\t\tlv2:name \"%s (%s)\" ;\n", p->name, units);
//            else
            fprintf(out, "\t\tlv2:name \"%s\" ;\n", p->name);
            print_units(out, p->unit);

            if (p->flags & F_LOG)
                fprintf(out, "\t\tlv2:portProperty pp:logarithmic ;\n");

            if (p->unit == U_BOOL)
            {
                fprintf(out, "\t\tlv2:portProperty lv2:toggled ;\n");
//                if (p->flags & F_TRG)
//                    fprintf(out, "\t\tlv2:portProperty pp:trigger ;\n");
                fprintf(out, "\t\tlv2:minimum %d ;\n", 0);
                fprintf(out, "\t\tlv2:maximum %d ;\n", 1);
                fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
            }
            else if (p->unit == U_ENUM)
            {
                fprintf(out, "\t\tlv2:portProperty pp:hasStrictBounds ;\n");
                fprintf(out, "\t\tlv2:portProperty lv2:integer ;\n");
                fprintf(out, "\t\tlv2:portProperty lv2:enumeration ;\n");

                int min  = (p->flags & F_LOWER) ? p->min : 0;
                int curr = min;
                int max  = min + list_size(p->items) - 1;

                for (const char **list = p->items; *list != NULL; ++list, ++curr)
                    fprintf(out, "\t\tlv2:scalePoint [ rdfs:label \"%s\"; rdf:value %d ] ;\n", *list, curr);

                fprintf(out, "\t\tlv2:minimum %d ;\n", min);
                fprintf(out, "\t\tlv2:maximum %d ;\n", max);
                fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
            }
            else if (p->unit == U_SAMPLES)
            {
                if ((p->flags & (F_LOWER | F_UPPER)) == (F_LOWER | F_UPPER))
                    fprintf(out, "\t\tlv2:portProperty pp:hasStrictBounds ;\n");
                fprintf(out, "\t\tlv2:portProperty lv2:integer ;\n");
                if (p->flags & F_LOWER)
                    fprintf(out, "\t\tlv2:minimum %d ;\n", int(p->min));
                if (p->flags & F_UPPER)
                    fprintf(out, "\t\tlv2:maximum %d ;\n", int(p->max));
                fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
            }
            else
            {
                if ((p->flags & (F_LOWER | F_UPPER)) == (F_LOWER | F_UPPER))
                    fprintf(out, "\t\tlv2:portProperty pp:hasStrictBounds ;\n");

                if (p->flags & F_INT)
                {
                    fprintf(out, "\t\tlv2:portProperty lv2:integer ;\n");
                    if (p->flags & F_LOWER)
                        fprintf(out, "\t\tlv2:minimum %d ;\n", int(p->min));
                    if (p->flags & F_UPPER)
                        fprintf(out, "\t\tlv2:maximum %d ;\n", int(p->max));
                    if ((p->role == R_CONTROL) || (p->role == R_METER))
                        fprintf(out, "\t\tlv2:default %d ;\n", int(p->start));
                }
                else
                {
                    if (p->flags & F_LOWER)
                        fprintf(out, "\t\tlv2:minimum %.6f ;\n", p->min);
                    if (p->flags & F_UPPER)
                        fprintf(out, "\t\tlv2:maximum %.6f ;\n", p->max);
                    if ((p->role == R_CONTROL) || (p->role == R_METER))
                        fprintf(out, "\t\tlv2:default %.6f ;\n", p->start);
                }
            }

            // Output all port groups of the port
            if (requirements & REQ_PORT_GROUPS)
                print_port_groups(out, p, m.port_groups);

            fprintf(out, "\t]");
            port_id++;
        }

        // Add atom ports for state serialization
        for (const port_t *p = lv2_atom_ports; (p->id != NULL) && (p->name != NULL); ++p)
        {
            fprintf(out, "%s [\n", (port_id == 0) ? "\tlv2:port" : " ,");
            fprintf(out, "\t\ta lv2:%s, atom:AtomPort ;\n", (p->flags & F_OUT) ? "OutputPort" : "InputPort");
            fprintf(out, "\t\tatom:bufferType atom:Sequence ;\n");
            if (requirements & REQ_PATCH)
                fprintf(out, "\t\tatom:supports patch:Message ;\n");
            fprintf(out, "\t\tlv2:designation lv2:control ;\n");
            fprintf(out, "\t\tlv2:index %d ;\n", int(port_id));
            fprintf(out, "\t\tlv2:symbol \"%s\" ;\n", p->id);
            fprintf(out, "\t\tlv2:name \"%s\" ;\n", p->name);
            fprintf(out, "\t\trdfs:comment \"UI <-> DSP communication\" ;\n");
            fprintf(out, "\t\trsz:minimumSize %ld ;\n", lv2_all_port_sizes(&m, p->flags & F_OUT));
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
        if (requirements & REQ_UI_MASK)
        {
            #define MOD_LV2UI(plugin, package, ext) \
                if (!strcmp(name, #plugin)) \
                    gen_plugin_ui_ttl(out, plugin::metadata, #plugin, LSP_PLUGIN_UI_URI(lv2, plugin, package), ext, #package, LSP_PLUGIN_URI(lv2, plugin));
            #define MOD_GTK2(plugin)    MOD_LV2UI(plugin, gtk2, "GtkUI"     )
            #define MOD_GTK3(plugin)    MOD_LV2UI(plugin, gtk3, "Gtk3UI"    )
            #define MOD_QT4(plugin)     MOD_LV2UI(plugin, qt4,  "Qt4UI"     )
            #define MOD_QT5(plugin)     MOD_LV2UI(plugin, qt5,  "Qt5UI"     )
            #include <core/modules.h>
            #undef MOD_LV2UI
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

        #define MOD_LV2(plugin) \
            fprintf(out, LSP_PREFIX ":" #plugin "\n"); \
            fprintf(out, "\ta lv2:Plugin ;\n"); \
            fprintf(out, "\tlv2:binary <" LSP_ARTIFACT_ID "-lv2.so> ;\n"); \
            fprintf(out, "\trdfs:seeAlso <%s.ttl> .\n\n", #plugin);
        #include <core/modules.h>
        fclose(out);
    }

    void gen_ttl(const char *path)
    {
        gen_manifest(path);

        // Output plugins
        size_t id = 0;
        #define MOD_LV2(plugin) \
            gen_plugin_ttl(path, plugin::metadata, #plugin, LSP_PLUGIN_URI(lv2, plugin)); id++;
        #include <core/modules.h>
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
