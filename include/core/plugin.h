#ifndef CORE_PLUGIN_H_
#define CORE_PLUGIN_H_

#include <core/dsp.h>
#include <core/types.h>
#include <core/IPort.h>
#include <core/IWrapper.h>

#include <metadata/metadata.h>

#include <data/cvector.h>

namespace lsp
{
    template <class T> T align(T x)
    {
        T v = 1;
        while (v < x)
            v <<= 1;
        return v;
    };

//    const size_t FLAG_OPTIONAL          = (1 << 0);

    class plugin_t
    {
        protected:
            cvector<IPort>              vPorts;
            const plugin_metadata_t    *pMetadata;

            long                        fSampleRate;
            ssize_t                     nLatency;
            bool                        bActivated;
            bool                        bUIActive;

        public:
            plugin_t(const plugin_metadata_t &mdata);
            virtual ~plugin_t();

        public:
            const plugin_metadata_t *get_metadata() const { return pMetadata;       };
            inline ssize_t get_latency() const          { return nLatency;          };
            inline void set_latency(ssize_t latency)    { nLatency = latency;       };

            inline bool add_port(IPort *port)           { return vPorts.add(port);  };
            inline IPort *port(size_t id)               { return vPorts[id];        };
            inline size_t ports_count() const           { return vPorts.size();     };

            void set_sample_rate(long sr);

            inline long get_sample_rate() const         { return fSampleRate;       };
            inline bool active() const                  { return bActivated;        };
            inline bool ui_active() const               { return bUIActive;         };

            inline void activate_ui()
            {
                if (!bUIActive)
                {
                    bUIActive       = true;
                    ui_activated();
                }
            }

            inline void deactivate_ui()
            {
                if (bUIActive)
                {
                    bUIActive       = false;
                    ui_deactivated();
                }
            }

            inline void activate()
            {
                if (!bActivated)
                {
                    bActivated      = true;
                    activated();
                }
            }

            inline void deactivate()
            {
                if (bActivated)
                {
                    bActivated      = false;
                    deactivated();
                }
            }

        public:
            virtual void init(IWrapper *wrapper);
            virtual void update_sample_rate(long sr);
            virtual void activated();
            virtual void ui_activated();
            virtual void update_settings();
            virtual void process(size_t samples);
            virtual void ui_deactivated();
            virtual void deactivated();
            virtual void destroy();
    };

}

#endif /* CORE_PLUGIN_H_ */
