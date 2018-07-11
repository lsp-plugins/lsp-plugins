#include <plugins/plugins.h>
#include <stdio.h>
#include <ladspa.h>

#define BUFFER_SIZE 512
#define SAMPLE_RATE 48000

namespace ladspa_test
{
    using namespace lsp;

    void run_plugin(const LADSPA_Descriptor *d, LADSPA_Handle h, cvector<float> &ports)
    {
        // Issue bind
        for (size_t i=0; i<ports.size(); ++i)
            d->connect_port(h, i, ports[i]);

        // Run pluin
        d->run(h, BUFFER_SIZE);
    }

    int test(int argc, const char **argv)
    {
        using namespace lsp;

#define LADSPA_URI(plugin) LSP_PLUGIN_URI(ladspa, plugin)
#define LADSPA_PLUGIN spectrum_analyzer_x1

        int plugin_id = 0;
        const LADSPA_Descriptor *d = NULL;
        while (true)
        {
            d = ladspa_descriptor(plugin_id++);
            if (d == NULL)
            {
                lsp_error("Plugin not found");
                return -1;
            }

            if (!strcmp(d->Label, LADSPA_URI(LADSPA_PLUGIN)))
                break;
        }

        lsp_info("Instantiating plugin %s (%s)", d->Name, d->Label);

        // Create plugin
        LADSPA_Handle h = d->instantiate(d, SAMPLE_RATE);

        cvector<float> ports;
        cvector<float> audio;
        float *regular      = NULL;
        size_t regular_ports = 0;

        // Determine number of ports
        for (size_t i=0; i<d->PortCount; ++i)
        {
            if (LADSPA_IS_PORT_AUDIO(d->PortDescriptors[i]))
            {
                float *ptr     = new float[BUFFER_SIZE];
                memset(ptr, 0, BUFFER_SIZE * sizeof(float));
                if (LADSPA_IS_PORT_INPUT(d->PortDescriptors[i]))
                {
                    for (size_t i=0; i<BUFFER_SIZE; ++i)
                        ptr[i] = sinf((2 * M_PI * i * 8) / BUFFER_SIZE);
                }

                audio.add(ptr);
            }
            else
                regular_ports++;
        }

        // Create regular ports
        if (regular_ports > 0)
            regular         = new float[regular_ports];

        // Now create port map
        size_t a_id = 0, reg_id = 0;
        for (size_t i=0; i<d->PortCount; ++i)
        {
            if (LADSPA_IS_PORT_AUDIO(d->PortDescriptors[i]))
            {
                float *ptr      = audio[a_id++];
                ports.add(ptr);
            }
            else
            {
                float *ptr      = &regular[reg_id++];

                // Initialize with default value
                const LADSPA_PortRangeHint *h = &d->PortRangeHints[i];
                LADSPA_PortRangeHintDescriptor hd = h->HintDescriptor;

                if (LADSPA_IS_HINT_HAS_DEFAULT(hd))
                {
                    if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hd))
                        *ptr    = h->LowerBound;
                    else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hd))
                        *ptr    = h->UpperBound;
                    else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hd))
                        *ptr    = h->LowerBound + (h->UpperBound - h->LowerBound) * 0.5;
                    else if (LADSPA_IS_HINT_DEFAULT_LOW(hd))
                        *ptr    = h->LowerBound + (h->UpperBound - h->LowerBound) * 0.25;
                    else if (LADSPA_IS_HINT_DEFAULT_HIGH(hd))
                        *ptr    = h->LowerBound + (h->UpperBound - h->LowerBound) * 0.75;
                    else if (LADSPA_IS_HINT_DEFAULT_0(hd))
                        *ptr    = 0.0f;
                    else if (LADSPA_IS_HINT_DEFAULT_1(hd))
                        *ptr    = 1.0f;
                    else if (LADSPA_IS_HINT_DEFAULT_100(hd))
                        *ptr    = 100.0f;
                    else if (LADSPA_IS_HINT_DEFAULT_440(hd))
                        *ptr    = 440.0f;
                }
                ports.add(ptr);
            }
        }

        // Activate plugin
        d->activate(h);

        // Issue cycle
        for (size_t i=0; i < 1000; ++i)
        {
            for (size_t j=0; j < 123; ++j)
                run_plugin(d, h, ports);
        }

        // Deactivate plugin
        d->deactivate(h);

        // Kill plugin
        d->cleanup(h);

        // Destroy all port pointers
        for (size_t i=0; i< audio.size(); ++i)
            delete [] audio[i];
        audio.clear();
        ports.clear();

        return 0;
    }
}
