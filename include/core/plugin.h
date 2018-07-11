#ifndef CORE_PLUGIN_H_
#define CORE_PLUGIN_H_

#include <core/dsp.h>
#include <core/types.h>
#include <core/metadata.h>
#include <core/IPort.h>
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

//    template <class T> inline T abs(T x)
//    {
//        return (x < 0) ? -x : x;
//    }

    const size_t FLAG_OPTIONAL          = (1 << 0);
    const size_t SOUND_SPEED_M_S        = 340.29f; // Sound speed [ Meters / second ]

    class plugin_t
    {
        protected:
            cvector<IPort>              vExtPorts;
            cvector<IPort>              vIntPorts;
            const plugin_metadata_t    *pMetadata;

//            dsp                        *pDSP;
            int                         fSampleRate;
            ssize_t                     nLatency;

        public:
            plugin_t(const plugin_metadata_t &mdata);

            virtual ~plugin_t();

        protected:
            inline float samples_to_seconds(float samples) const
            {
                return samples / fSampleRate;
            }

            inline float seconds_to_samples(float seconds) const
            {
                return seconds * fSampleRate;
            }

            inline float samples_to_millis(float samples) const
            {
                return (samples * 1000.0) / fSampleRate;
            }

            inline float samples_to_meters(float samples) const
            {
                return (samples * SOUND_SPEED_M_S) / fSampleRate;
            }

            inline float samples_to_centimeters(float samples) const
            {
                return samples_to_meters(samples) * 100.0f;
            }

            inline float millis_to_samples(float seconds) const
            {
                return (seconds * 0.001) * fSampleRate;
            }

            template <class A, class B, class C>
                inline static A limit(A value, B min, C max)
                {
                    return (value < min) ? min : (value > max) ? max : value;
                }

            inline static float limit(float value, float min, float max)
            {
                return (value < min) ? min : (value > max) ? max : value;
            }

        public:
            const plugin_metadata_t *get_metadata() const { return pMetadata;   };
            inline ssize_t get_latency() const          { return nLatency;      };
            inline void set_latency(ssize_t latency)    { nLatency = latency;   };

            bool add_port(IPort *port, bool external = true);
            IPort *port(size_t id, bool external = true);
            inline size_t port_count(bool external = true) { return (external) ? vExtPorts.size() : vIntPorts.size(); };

            void run(size_t samples);
            void set_sample_rate(int sr);

            inline int  get_sample_rate() const         { return fSampleRate;   };

        public:
            virtual void init();
            virtual void update_sample_rate(int sr);
            virtual void activate();
            virtual void update_settings();
            virtual void process(size_t samples);
            virtual void deactivate();
            virtual void destroy();

    };

}

#endif /* CORE_PLUGIN_H_ */
