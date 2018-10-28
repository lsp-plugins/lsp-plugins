#include <stdio.h>
#include <string.h>

#include <core/types.h>
#include <metadata/metadata.h>
#include <metadata/plugins.h>

#include <plugins/plugins.h>

namespace lsp
{
    typedef struct php_plugin_group_t
    {
        int            id;
        const char    *name;
    } php_plugin_group_t;

    const php_plugin_group_t php_plugin_groups[] =
    {
        { C_DELAY, "Delay" },
        { C_REVERB, "Reverb" },
        { C_DISTORTION, "Distortion" },
        { C_WAVESHAPER, "Waveshaper" },
        { C_DYNAMICS, "Dynamics" },
        { C_AMPLIFIER, "Amplifier" },
        { C_COMPRESSOR, "Compressor" },
        { C_ENVELOPE, "Envelope" },
        { C_EXPANDER, "Expander" },
        { C_GATE, "Gate" },
        { C_LIMITER, "Limiter" },
        { C_FILTER, "Filter" },
        { C_ALLPASS, "Allpass" },
        { C_BANDPASS, "Bandpass" },
        { C_COMB, "Comb" },
        { C_EQ, "Equalizer" },
        { C_MULTI_EQ, "Multiband Equalizer" },
        { C_PARA_EQ, "Parametric Equalizer" },
        { C_HIGHPASS, "Highpass" },
        { C_LOWPASS, "Lowpass" },
        { C_GENERATOR, "Generator" },
        { C_CONSTANT, "Constant" },
        { C_INSTRUMENT, "Instrument" },
        { C_OSCILLATOR, "Oscillator" },
        { C_MODULATOR, "Modulator" },
        { C_CHORUS, "Chorus" },
        { C_FLANGER, "Flanger" },
        { C_PHASER, "Phaser" },
        { C_SIMULATOR, "Simulator" },
        { C_SPATIAL, "Spatial" },
        { C_SPECTRAL, "Spectral" },
        { C_PITCH, "Pitch" },
        { C_UTILITY, "Utility" },
        { C_ANALYSER, "Analyser" },
        { C_CONVERTER, "Converter" },
        { C_FUNCTION, "Function" },
        { C_MIXER, "Mixer" },
        { -1, NULL }
    };

    void php_print_plugin_groups(FILE *out, const int *c)
    {
        fprintf(out, "array(");
        size_t items = 0;

        while ((c != NULL) && ((*c) >= 0))
        {
            const php_plugin_group_t *grp = php_plugin_groups;

            while ((grp != NULL) && (grp->id >= 0))
            {
                if (grp->id == *c)
                {
                    if ((items++) > 0)
                        fprintf(out, ", ");
                    fprintf(out, "'%s'", grp->name);
                    break;
                }
                grp++;
            }

            c++;
        }

        fprintf(out, ")");
    }

    void gen_plugin_php_descriptor(FILE *out, const plugin_metadata_t *m, const char *plugin_id)
    {
        fprintf(out, "\t\tarray(\n");
        fprintf(out, "\t\t\t'id' => '%s',\n", m->lv2_uid);
        fprintf(out, "\t\t\t'name' => '%s',\n", m->name);
        fprintf(out, "\t\t\t'description' => '%s',\n", m->description);
        fprintf(out, "\t\t\t'acronym' => '%s',\n", m->acronym);

        #define MOD_PLUGIN(x) \
            if (&x::metadata == m) \
            { \
                if (m->ladspa_id > 0) \
                    fprintf(out, "\t\t\t'fmt_ladspa' => '%d',\n", m->ladspa_id); \
                if (m->lv2_uid != NULL) \
                    fprintf(out, "\t\t\t'fmt_lv2' => '%s',\n", LSP_PLUGIN_URI(lv2, x)); \
                if (m->vst_uid != NULL) \
                    fprintf(out, "\t\t\t'fmt_vst' => '%s',\n", m->vst_uid); \
                fprintf(out, "\t\t\t'fmt_jack' => true,\n"); \
            }
        #include <metadata/modules.h>

        fprintf(out, "\t\t\t'author' => '%s',\n", m->developer->name);
        fprintf(out, "\t\t\t'version' => '%d.%d.%d',\n",
                LSP_VERSION_MAJOR(m->version),
                LSP_VERSION_MINOR(m->version),
                LSP_VERSION_MICRO(m->version)
            );
        fprintf(out, "\t\t\t'groups' => ");
        php_print_plugin_groups(out, m->classes);
        fprintf(out, "\n");
        fprintf(out, "\t\t)");
    }

    void gen_php(const char *version, const char *fname)
    {
        // Generate PHP file
        FILE *out;
        if (!(out = fopen(fname, "w+")))
            return;
        printf("Writing file %s\n", fname);

        // Write PHP header
        fprintf(out, "<?php\n");
        fprintf(out, "\n");
        fprintf(out, "\t$LATEST_RELEASE = '%s';\n", version);
        fprintf(out, "\n");
        fprintf(out, "\t$PLUGINS = array(\n");

        // Output plugins
        size_t id = 0;
        #define MOD_PLUGIN(plugin) \
            if ((id++) > 0) \
                fprintf(out, ",\n"); \
            gen_plugin_php_descriptor(out, &plugin::metadata, #plugin);
        #include <metadata/modules.h>

        fprintf(out, "\n\t);\n");
        fprintf(out, "?>\n");

        fclose(out);
    }
}

#ifndef LSP_IDE_DEBUG
int main(int argc, const char **argv)
{
    if (argc < 2)
        fprintf(stderr, "required destination file name");
    lsp::gen_php(LSP_MAIN_VERSION, argv[1]);

    return 0;
}
#endif /* LSP_IDE_DEBUG */
