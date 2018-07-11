/*
 * metadata.h
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_METADATA_H_
#define CORE_METADATA_H_

#include <sys/types.h>

#define LSP_BASE_URI                                    "http://lsp-plug.in/"
#define LSP_ACRONYM                                     "LSP"
#define LSP_PREFIX                                      "lsp"
#define LSP_BINARY                                      "lsp-plugins"
#define LSP_VST_BINARY                                  "lsp-vst-core"
#define LSP_COPYRIGHT                                   LSP_ACRONYM " (Linux Studio Plugins)"
#define LSP_PLUGIN_NAME(name, description)              LSP_ACRONYM " " name " - " description
#define LSP_URI(format)                                 LSP_BASE_URI "plugins/" #format "/"
#define LSP_TYPE_URI(format)                            LSP_BASE_URI "types/" #format
#define LSP_UI_URI(format, package)                     LSP_BASE_URI "ui/" #format "/" #package
#define LSP_PLUGIN_URI(format, plugin)                  LSP_BASE_URI "plugins/" #format "/" #plugin
#define LSP_PLUGIN_UI_URI(format, plugin, package)      LSP_UI_URI(format, package) "/" #plugin
#define LSP_LADSPA_BASE                                 0x4C5350

#define LSP_LV2_LATENCY_PORT                            "_latency_out_"
#define LSP_LV2_ATOM_PORT_IN                            "_control_in_"
#define LSP_LV2_ATOM_PORT_OUT                           "_control_out_"


namespace lsp
{
    enum unit_t
    {
        U_NONE,                 // Simple value

        U_BOOL,                 // Boolean: true if > 0.5, false otherwise
        U_PERCENT,              // Something in percents

        // Distance
        U_MM,                   // Millimeters
        U_CM,                   // Centimeters
        U_M,                    // Meters
        U_INCH,                 // Inches
        U_KM,                   // Kilometers

        U_SAMPLES,              // Something in samples

        // Frequency
        U_HZ,                   // Hertz
        U_KHZ,                  // Kilohertz
        U_MHZ,                  // Megahertz
        U_BPM,                  // Beats per minute
        U_CENT,                 // Cents

        // Time measurement
        U_BAR,                  // Bars
        U_BEAT,                 // Beats
        U_SEC,                  // Seconds
        U_MSEC,                 // Millis

        // Level measurement
        U_DB,                   // Decibels
        U_GAIN_AMP,             // Gain (amplitude amplification)
        U_GAIN_POW,             // Gain (power amplification)

        // Degrees
        U_DEG_CEL,              // Degrees (Celsium)
        U_DEG_FAR,              // Degrees (Farengheit)
        U_DEG_K,                // Degrees (Kelvin)

        U_ENUM                 // List index

        /*
        coef
        db
        degree
        frame
        midiNote
        mile
        min
        oct
        semitone12TET
         */
    };

    enum role_t
    {
        R_UI_SYNC,
        R_AUDIO,
        R_CONTROL,
        R_METER,
        R_MESH
    };

    enum flags_t
    {
        F_IN            = (0 << 0),     // Input port
        F_OUT           = (1 << 0),     // Output port
        F_UPPER         = (1 << 1),     // Upper-limit defined
        F_LOWER         = (1 << 2),     // Lower-llmit defined
        F_STEP          = (1 << 3),     // Step defined
        F_LOG           = (1 << 4),     // Logarithmic scale
        F_INT           = (1 << 5),     // Integer value
        F_TRG           = (1 << 6),     // Trigger
    };

    enum plugin_class_t
    {
        C_DELAY,
            C_REVERB,
        C_DISTORTION,
            C_WAVESHAPER,
        C_DYNAMICS,
            C_AMPLIFIER,
            C_COMPRESSOR,
            C_ENVELOPE,
            C_EXPANDER,
            C_GATE,
            C_LIMITER,
        C_FILTER,
            C_ALLPASS,
            C_BANDPASS,
            C_COMB,
            C_EQ,
                C_MULTI_EQ,
                C_PARA_EQ,
            C_HIGHPASS,
            C_LOWPASS,
        C_GENERATOR,
            C_CONSTANT,
            C_INSTRUMENT,
            C_OSCILLATOR,
        C_MODULATOR,
            C_CHORUS,
            C_FLANGER,
            C_PHASER,
        C_SIMULATOR,
        C_SPATIAL,
        C_SPECTRAL,
            C_PITCH,
        C_UTILITY,
            C_ANALYSER,
            C_CONVERTER,
            C_FUNCTION,
            C_MIXER
    };

    enum port_group_type_t
    {
        GRP_MONO,                       // Mono
        GRP_1_0     = GRP_MONO,         // Mono
        GRP_STEREO,                     // Stereo
        GRP_2_0     = GRP_STEREO,       // Stereo
        GRP_2_1     = GRP_STEREO,       // Stereo
        GRP_MS,                         // Mid-side
        GRP_3_0,                        // 3.0
        GRP_4_0,                        // 4.0
        GRP_5_0,                        // 5.0
        GRP_5_1,                        // 5.1
        GRP_6_1,                        // 6.1
        GRP_7_1,                        // 7.1
        GRP_7_1W,                       // 7.1 Wide
    };

    enum port_group_role_t
    {
        PGR_CENTER,
        PGR_CENTER_LEFT,
        PGR_CENTER_RIGHT,
        PGR_LEFT,
        PGR_LO_FREQ,
        PGR_REAR_CENTER,
        PGR_REAR_LEFT,
        PGR_REAR_RIGHT,
        PGR_RIGHT,
        PGR_SIDE,
        PGR_SIDE_LEFT,
        PGR_SIDE_RIGHT
    };

    enum port_group_flags_t
    {
        PGF_IN          = (0 << 0),     // Input group
        PGF_OUT         = (1 << 0)      // Output group
    };

    typedef struct port_group_item_t
    {
        const char         *id;
        port_group_role_t   role;
    } port_group_item_t;

    typedef struct port_group_t
    {
        const char                 *id;         // Group ID
        const char                 *name;       // Group name
        port_group_type_t           type;       // Group type
        int                         flags;
        const port_group_item_t    *items;
    } port_group_t;

    typedef struct port_t
    {
        const char     *id;         // Control ID
        const char     *name;       // Control name
        unit_t          unit;       // Units
        role_t          role;       // Role
        int             flags;      // Flags
        float           min;        // Minimum value
        float           max;        // Maximum value
        float           start;      // Initial value
        float           step;       // Change step
        const char    **items;      // Items for list
    } port_t;

    typedef struct plugin_metadata_t
    {
        const char             *name;           // Plugin name
        const char             *description;    // Plugin description
        const char             *acronym;        // Plugin acronym
        const char             *author;         // Author
        const char             *vst_uid;         // Steinberg VST ID of the plugin
        const int              *classes;        // List of plugin classes terminated by negative value
        const port_t           *ports;          // List of all ports
        const port_group_t     *port_groups;    // List of all port groups
    } plugin_metadata_t;

    // Atom ports' metadata for LV2 plugins
    extern const port_t         lv2_atom_ports[];
    extern const port_t         lv2_latency_port;

    const char     *encode_unit(size_t unit);
    unit_t          decode_unit(const char *name);
    bool            is_discrete_unit(unit_t unit);
    bool            is_decibel_unit(unit_t unit);

    size_t          list_size(const char **list);
}

#endif /* CORE_METADATA_H_ */
