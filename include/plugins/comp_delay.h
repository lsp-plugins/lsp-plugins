/*
 * comp_delay.h
 *
 *  Created on: 8 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_PLUGINS_COMP_DELAY_H_
#define CORE_PLUGINS_COMP_DELAY_H_

#include <core/plugin.h>
#include <core/util/Delay.h>
#include <core/util/Bypass.h>

#include <metadata/plugins.h>

namespace lsp
{
    class comp_delay_base
    {
        protected:
            Delay       vLine;
            Bypass      vBypass;

            size_t      nMode;
            float       fSamples;
            float       fDistance;
            float       fTemperature;
            float       fTime;
            float       fDry;
            float       fWet;
            float      *vBuffer;
            size_t      nBufSize;
            ssize_t     nDelay;
            ssize_t     nNewDelay;
            long        nSampleRate;
            bool        bRamping;
            IPort      *pIn;
            IPort      *pOut;

        public:
            explicit comp_delay_base();
            virtual ~comp_delay_base();

        public:
            inline void set_ports(IPort *in, IPort *out)
            {
                pIn     = in;
                pOut    = out;
            }

            inline void set_bypass(bool bypass)         { vBypass.set_bypass(bypass);   };

            inline void set_mode(size_t mode)           { nMode         = mode;         };
            inline void set_samples(float samples)      { fSamples      = samples;      };
            inline void set_distance(float distance)    { fDistance     = distance;     };
            inline void set_time(float time)            { fTime         = time;         };
            inline void set_temperature(float temp)     { fTemperature  = temp;         };
            inline void set_dry(float dry)              { fDry          = dry;          };
            inline void set_wet(float wet)              { fWet          = wet;          };
            inline void set_ramping(bool ramp)          { bRamping      = ramp;         };

            void configure();

            inline float get_samples() const            { return fSamples;      };
            inline float get_distance() const           { return fDistance;     };
            inline float get_time() const               { return fTime;         };

        public:
            void init(long sample_rate, float *buffer, size_t buf_size);

            void destroy();

            void process(size_t samples);
    };

    class comp_delay_impl: public plugin_t
    {
        protected:
            float       *vBuffer;

        protected:
            virtual void dropBuffers();
            virtual void createBuffers();

        public:
            explicit comp_delay_impl(const plugin_metadata_t &mdata);
            virtual ~comp_delay_impl();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void update_sample_rate(long sr);
            virtual void destroy();
    };

    class comp_delay_mono: public comp_delay_impl, public comp_delay_mono_metadata
    {
        private:
            comp_delay_base     vDelay;

        protected:
            virtual void dropBuffers();
            virtual void createBuffers();

        public:
            explicit comp_delay_mono();
            virtual ~comp_delay_mono();

        public:
            virtual void update_settings();
            virtual void process(size_t samples);
    };

    class comp_delay_stereo: public comp_delay_impl, public comp_delay_stereo_metadata
    {
        private:
            comp_delay_base     vDelay[2];

        protected:
            virtual void dropBuffers();
            virtual void createBuffers();

        public:
            explicit comp_delay_stereo();
            virtual ~comp_delay_stereo();

        public:
            virtual void update_settings();
            virtual void process(size_t samples);
    };

    class comp_delay_x2_stereo: public comp_delay_impl, public comp_delay_x2_stereo_metadata
    {
        private:
            comp_delay_base     vDelay[2];

        protected:
            virtual void dropBuffers();
            virtual void createBuffers();

        public:
            explicit comp_delay_x2_stereo();
            virtual ~comp_delay_x2_stereo();

        public:
            virtual void update_settings();
            virtual void process(size_t samples);
    };

} /* namespace ddb */

#endif /* CORE_PLUGINS_COMP_DELAY_H_ */
