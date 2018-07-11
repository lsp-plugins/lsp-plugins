#include <core/plugins.h>
#include <stdio.h>
#include <ladspa.h>

#define BUFFER_SIZE 512
#define SAMPLE_RATE 48000

namespace ladspa_test
{
    using namespace lsp;

    void run_plugin(const LADSPA_Descriptor *d, LADSPA_Handle h)
    {
        cvector<float> ports;

        for (size_t i=0; i<d->PortCount; ++i)
        {
            float *ptr = NULL;

            if (LADSPA_IS_PORT_AUDIO(d->PortDescriptors[i]))
            {
                ptr     = new float[BUFFER_SIZE];
                memset(ptr, 0, BUFFER_SIZE * sizeof(float));
            }
            else
            {
                ptr     = new float[1];
                *ptr    = 0.0f;

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
            }

            if (ptr != NULL)
                ports.add(ptr);

            // Issue bind
            d->connect_port(h, i, ptr);
        }

        // Run pluin
        d->run(h, BUFFER_SIZE);

        // Destroy all port pointers
        for (size_t i=0; i< ports.size(); ++i)
            delete [] ports[i];
        ports.clear();
    }

    int test(int argc, const char **argv)
    {
        using namespace lsp;

        int plugin_id = 0;

        const LADSPA_Descriptor *d = ladspa_descriptor(plugin_id);
        if (d == NULL)
        {
            lsp_error("Plugin not found");
            return -1;
        }

        lsp_info("Instantiating plugin %s (%s)", d->Name, d->Label);

        // Create plugin
        LADSPA_Handle h = d->instantiate(d, SAMPLE_RATE);

        for (size_t i=0; i < 1000; ++i)
        {
            d->activate(h);
            for (size_t j=0; j < 123; ++j)
                run_plugin(d, h);
            d->deactivate(h);
        }

        // Kill plugin
        d->cleanup(h);

        return 0;
    }
}
