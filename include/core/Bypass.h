/*
 * Bypass.h
 *
 *  Created on: 07 дек. 2015 г.
 *      Author: sadko
 */

#ifndef BYPASS_H_
#define BYPASS_H_

#include <core/types.h>
#include <core/dsp.h>

namespace lsp
{
    class Bypass
    {
        private:
            enum state_t
            {
                S_ON,
                S_ACTIVE,
                S_OFF
            };

            state_t nState;
            float   fDelta;
            float   fGain;

        public:
            Bypass();
            ~Bypass();

        public:
            void init(int sample_rate, float time = 0.005 ); // By default 5msec bypass

            void process(float *dst, const float *dry, const float *wet, size_t count);

            bool set_bypass(bool bypass);
            inline void set_bypass(float bypass) { set_bypass(bypass >= 0.5f); };

            inline bool on() const      { return nState == S_ON; };
            inline bool off() const     { return nState == S_OFF; };
            inline bool active() const  { return nState == S_ACTIVE; };
            bool bypassing() const;
    };

} /* namespace lsp */

#endif /* BYPASS_H_ */
