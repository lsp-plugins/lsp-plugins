/*
 * metadata.h
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#ifndef METADATA_METADATA_H_
#define METADATA_METADATA_H_

#include <core/types.h>

#define LSP_BASE_URI                                    "http://lsp-plug.in/"
#define LSP_ACRONYM                                     "LSP"
#define LSP_PREFIX                                      "lsp"
#define LSP_ARTIFACT_ID                                 LSP_PREFIX "-plugins"
#define LSP_R3D_BACKEND_PREFIX                          LSP_ARTIFACT_ID "-r3d"
#define LSP_BINARY                                      LSP_ARTIFACT_ID
#define LSP_FULL_NAME                                   "Linux Studio Plugins Project"
#define LSP_COPYRIGHT                                   LSP_ACRONYM " (Linux Studio Plugins)"
#define LSP_PLUGIN_NAME(name, description)              LSP_ACRONYM " " name " - " description
#define LSP_DEVELOPERS_URI                              LSP_BASE_URI "developers/"
#define LSP_URI(format)                                 LSP_BASE_URI "plugins/" #format "/"
#define LSP_TYPE_URI(format)                            LSP_BASE_URI "types/" #format
#define LSP_UI_URI(format)                              LSP_BASE_URI "ui/" #format "/"
#define LSP_KVT_URI                                     LSP_BASE_URI "kvt"
#define LSP_PLUGIN_URI(format, plugin)                  LSP_BASE_URI "plugins/" #format "/" #plugin
#define LSP_PLUGIN_UI_URI(format, plugin)               LSP_UI_URI(format) #plugin
#define LSP_LADSPA_BASE                                 0x4C5350
#define LSP_DONATION_URI1                               "https://salt.bountysource.com/teams/" LSP_ARTIFACT_ID
#define LSP_DONATION_URI2                               "https://liberapay.com/sadko4u/donate"
#define LSP_DOWNLOAD_URI                                LSP_BASE_URI "?page=download"

// Different LV2 UI classes for different platforms
#if defined(PLATFORM_LINUX) || defined(PLATFORM_BSD)
    #define LSP_LV2UI_CLASS                                 "X11UI"
#elif defined(PLATFORM_WINDOWS)
    #define LSP_LV2UI_CLASS                                 "WindowsUI"
#elif defined(PLATFORM_MACOSX)
    #define LSP_LV2UI_CLASS                                 "CocoaUI"
#elif defined(PLATFORM_UNIX_COMPATIBLE)
    #define LSP_LV2UI_CLASS                                 "X11UI"
#else
    #error "Could not determine LV2 UI class for target platform"
#endif

#define LSP_VERSION(a, b, c)                            uint32_t(((uint32_t(a) & 0xff) << 16) | ((uint32_t(b) & 0xff) << 8) | (uint32_t(c) & 0xff))
#define LSP_VERSION_MAJOR(v)                            (uint32_t(((v) >> 16) & 0xff))
#define LSP_VERSION_MINOR(v)                            (uint32_t(((v) >> 8) & 0xff))
#define LSP_VERSION_MICRO(v)                            (uint32_t((v) & 0xff))
#define LSP_MAX_PARAM_ID_BYTES                          64

#if defined(ARCH_I386)
    #define LSP_ARCHITECTURE                                "i586"
#elif defined(ARCH_X86_64)
    #define LSP_ARCHITECTURE                                "x86_64"
#else
    #define LSP_ARCHITECTURE                                "unknown"
#endif /* ARCH */

#ifndef LSP_MAIN_VERSION
    #define LSP_MAIN_VERSION                                "0.0.0"
#endif /* LSP_MAIN_VERSION */

#define LSP_LV2_LATENCY_PORT                            "out_latency"
#define LSP_LV2_ATOM_PORT_IN                            "in_ui"
#define LSP_LV2_MIDI_PORT_IN                            "in_midi"
#define LSP_LV2_OSC_PORT_IN                             "in_osc"
#define LSP_LV2_ATOM_PORT_OUT                           "out_ui"
#define LSP_LV2_MIDI_PORT_OUT                           "out_midi"
#define LSP_LV2_OSC_PORT_OUT                            "out_osc"

#ifdef LSP_INSTALL_PREFIX
    #define LSP_LIB_PREFIX(x)       LSP_INSTALL_PREFIX x
#else
    #define LSP_LIB_PREFIX(x)       x
#endif /* PREFIX */

namespace lsp
{
    enum unit_t
    {
        U_NONE,                 // Simple value

        U_BOOL,                 // Boolean: true if > 0.5, false otherwise
        U_STRING,               // String
        U_PERCENT,              // Something in percents

        // Distance
        U_MM,                   // Millimeters
        U_CM,                   // Centimeters
        U_M,                    // Meters
        U_INCH,                 // Inches
        U_KM,                   // Kilometers

        // Speed
        U_MPS,                  // Meters per second
        U_KMPH,                 // Kilometers per hour

        // Samples
        U_SAMPLES,              // Something in samples

        // Frequency
        U_HZ,                   // Hertz
        U_KHZ,                  // Kilohertz
        U_MHZ,                  // Megahertz
        U_BPM,                  // Beats per minute
        U_CENT,                 // Cents
        U_OCTAVES,              // Octaves
        U_SEMITONES,            // Semitones

        // Time measurement
        U_BAR,                  // Bars
        U_BEAT,                 // Beats
        U_MIN,                  // Minute
        U_SEC,                  // Seconds
        U_MSEC,                 // Milliseconds

        // Level measurement
        U_DB,                   // Decibels
        U_GAIN_AMP,             // Gain (amplitude amplification)
        U_GAIN_POW,             // Gain (power amplification)

        // Degrees
        U_DEG,                  // Degrees
        U_DEG_CEL,              // Degrees (Celsium)
        U_DEG_FAR,              // Degrees (Fahrenheit)
        U_DEG_K,                // Degrees (Kelvin)
        U_DEG_R,                // Degrees (Rankine)

        U_ENUM                  // List index
    };

    enum role_t
    {
        R_UI_SYNC,              // Synchronization with UI
        R_AUDIO,                // Audio port
        R_CONTROL,              // Control port
        R_METER,                // Metering port
        R_MESH,                 // Mesh port
        R_FBUFFER,              // Frame buffer
        R_PATH,                 // Path to the local file
        R_MIDI,                 // MIDI events
        R_PORT_SET,             // Set of ports
        R_OSC,                  // OSC events
        R_BYPASS                // Bypass
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
        F_GROWING       = (1 << 7),     // Proportionally growing default value (for port sets)
        F_LOWERING      = (1 << 8),     // Proportionally lowering default value (for port sets)
        F_PEAK          = (1 << 9),     // Peak flag
        F_CYCLIC        = (1 << 10),    // Cyclic flag
        F_EXT           = (1 << 11),    // Extended range
    };

    #define IS_OUT_PORT(p)      ((p)->flags & F_OUT)
    #define IS_IN_PORT(p)       (!((p)->flags & F_OUT))
    #define IS_GROWING_PORT(p)  (((p)->flags & (F_GROWING | F_UPPER | F_LOWER)) == (F_GROWING | F_UPPER | F_LOWER))
    #define IS_LOWERING_PORT(p) (((p)->flags & (F_LOWERING | F_UPPER | F_LOWER)) == (F_LOWERING | F_UPPER | F_LOWER))
    #define IS_TRIGGER_PORT(p)  ((p)->flags & F_TRG)

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

    enum plugin_extension_t
    {
        E_NONE                  = 0,        // No extensions
        E_INLINE_DISPLAY        = 1 << 0,   // Supports InlineDisplay extension originally implemented in LV2 plugin format
        E_3D_BACKEND            = 1 << 1,   // Supports 3D rendering backend
        E_OSC                   = 1 << 2,   // Supports OSC protocol messaging
        E_KVT_SYNC              = 1 << 3    // KVT synchronization required
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
        PGF_OUT         = (1 << 0),     // Output group
        PGF_SIDECHAIN   = (1 << 1)      // Sidechain
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
        const char                 *parent_id;  // Reference to parent group
    } port_group_t;

    typedef struct port_item_t {
        const char             *text;           // Text to display, required
        const char             *lc_key;         // Localized key  (optional)
    } port_item_t;

    typedef struct port_t
    {
        const char             *id;             // Control ID
        const char             *name;           // Control name
        unit_t                  unit;           // Units
        role_t                  role;           // Role
        int                     flags;          // Flags
        float                   min;            // Minimum value
        float                   max;            // Maximum value
        float                   start;          // Initial value
        float                   step;           // Change step
        const port_item_t      *items;          // Items for enum / port set
        const port_t           *members;        // Port members for group
    } port_t;

    typedef struct person_t
    {
        const char             *uid;            // UID of person
        const char             *nick;           // Nickname
        const char             *name;           // Name
        const char             *mailbox;        // E-mail
        const char             *homepage;       // Homepage
    } person_t;

    typedef struct plugin_metadata_t
    {
        const char             *name;           // Plugin name
        const char             *description;    // Plugin description
        const char             *acronym;        // Plugin acronym
        const person_t         *developer;      // Developer
        const char             *lv2_uid;        // LV2 unique identifier
        const char             *vst_uid;        // Steinberg VST ID of the plugin
        const uint32_t          ladspa_id;      // LADSPA ID of the plugin
        const uint32_t          version;        // Version of the plugin
        const int              *classes;        // List of plugin classes terminated by negative value
        const int               extensions;     // Additional extensions
        const port_t           *ports;          // List of all ports
        const char             *ui_resource;    // Location of the UI file resource
        const char             *ui_presets;     // Prefix of the preset location
        const port_group_t     *port_groups;    // List of all port groups
    } plugin_metadata_t;

    // Atom ports' metadata for LV2 plugins
    extern const port_t         lv2_atom_ports[];
    extern const port_t         lv2_latency_port;

    const char     *encode_unit(size_t unit);
    const char     *unit_lc_key(size_t code);
    unit_t          decode_unit(const char *name);
    bool            is_discrete_unit(size_t unit);
    bool            is_decibel_unit(size_t unit);
    bool            is_degree_unit(size_t unit);
    bool            is_log_rule(const port_t *port);

    size_t          list_size(const port_item_t *list);
    float           limit_value(const port_t *port, float value);

    void            format_float(char *buf, size_t len, const port_t *meta, float value, ssize_t precision = -1);
    void            format_int(char *buf, size_t len, const port_t *meta, float value);
    void            format_enum(char *buf, size_t len, const port_t *meta, float value);
    void            format_decibels(char *buf, size_t len, const port_t *meta, float value, ssize_t precision = -1);
    void            format_bool(char *buf, size_t len, const port_t *meta, float value);

    void            format_value(char *buf, size_t len, const port_t *meta, float value, ssize_t precision = -1);

    status_t        parse_bool(float *dst, const char *text);
    status_t        parse_enum(float *dst, const char *text, const port_t *meta);
    status_t        parse_decibels(float *dst, const char *text, const port_t *meta);
    status_t        parse_int(float *dst, const char *text, const port_t *meta);
    status_t        parse_float(float *dst, const char *text, const port_t *meta);
    status_t        parse_value(float *dst, const char *text, const port_t *meta);

    void            get_port_parameters(const port_t *p, float *min, float *max, float *step);

    /** Clone port metadata
     *
     * @param metadata port list
     * @param postfix potfix to be added to the port list, can be NULL
     * @return cloned port metadata
     */
    port_t         *clone_port_metadata(const port_t *metadata, const char *postfix);

    /** Drop port metadata
     *
     * @param metadata port metadata to drop
     */
    void            drop_port_metadata(port_t *metadata);

    /** Size of port list
     *
     * @param metadata port list metadata
     * @return number of elements excluding PORTS_END
     */
    size_t          port_list_size(const port_t *metadata);
}

#endif /* METADATA_METADATA_H_ */
