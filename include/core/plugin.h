#ifndef CORE_PLUGIN_H_
#define CORE_PLUGIN_H_

#include <dsp/dsp.h>
#include <core/types.h>
#include <core/IPort.h>
#include <core/IWrapper.h>
#include <core/ICanvas.h>
#include <core/debug.h>

#include <metadata/metadata.h>

#include <data/cvector.h>

namespace lsp
{
    class plugin_t
    {
        protected:
            cvector<IPort>              vPorts;
            const plugin_metadata_t    *pMetadata;
            IWrapper                   *pWrapper;

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
                    lsp_trace("UI has been activated");
                    ui_activated();
                }
            }

            inline void deactivate_ui()
            {
                if (bUIActive)
                {
                    bUIActive       = false;
                    lsp_trace("UI has been deactivated");
                    ui_deactivated();
                }
            }

            inline void activate()
            {
                if (!bActivated)
                {
                    bActivated      = true;
                    activated();
                    pWrapper->query_display_draw();
                }
            }

            inline void deactivate()
            {
                if (bActivated)
                {
                    bActivated      = false;
                    deactivated();
                    pWrapper->query_display_draw();
                }
            }

        public:
            /** Initialize plugin
             *
             * @param wrapper plugin wrapper interface
             */
            virtual void init(IWrapper *wrapper);

            /** Update sample rate of data processing
             *
             * @param sr new sample rate
             */
            virtual void update_sample_rate(long sr);

            /** Destroy plugin state
             *
             */
            virtual void destroy();

            /** Triggered plugin activation
             *
             */
            virtual void activated();

            /** Triggered UI activation
             *
             */
            virtual void ui_activated();

            /** Triggered input port change, need to update configuration
             *
             */
            virtual void update_settings();

            /** Report current time position for plugin
             *
             * @param pos current time position
             * @return true if need to call for plugin setting update
             */
            virtual bool set_position(const position_t *pos);

            /** Process data
             *
             * @param samples number of samples to process
             */
            virtual void process(size_t samples);

            /** Draw inline display on canvas
             *
             * @param cv canvas
             * @param width maximum canvas width
             * @param height maximum canvas height
             * @return status of operation
             */
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);

            /** Triggered UI deactivation
             *
             */
            virtual void ui_deactivated();

            /** Triggered plugin deactivation
             *
             */
            virtual void deactivated();
    };

}

#endif /* CORE_PLUGIN_H_ */
