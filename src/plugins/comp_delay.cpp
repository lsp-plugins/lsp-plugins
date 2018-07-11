/*
 * corellator.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#include <core/dsp.h>

#include <core/debug.h>
#include <plugins/comp_delay.h>
#include <math.h>

#include <string.h>

#define BUFFER_SIZE         1024

namespace lsp
{
    comp_delay_base::comp_delay_base()
    {
        lsp_trace("constructor");
        nMode           = comp_delay_base_metadata::M_SAMPLES;
        fSamples        = 0;
        fDistance       = 0;
        fTemperature    = 0;
        fTime           = 0;
        nSampleRate     = 0;
        fDry            = 0.0;
        fWet            = 1.0;
        vBuffer         = NULL;
        nBufSize        = 0;
        pIn             = NULL;
        pOut            = NULL;
        pDSP            = NULL;
    }

    comp_delay_base::~comp_delay_base()
    {
        destroy();
    }

    inline float comp_delay_base::sound_speed(float temp)
    {
        return sqrtf(AIR_ADIABATIC_INDEX * GAS_CONSTANT * (temp - TEMP_ABS_ZERO) * 1000 /* g/kg */ / AIR_MOLAR_MASS);
    }

    void comp_delay_base::init(dsp *p_dsp, int sample_rate, float *buffer, size_t buf_size)
    {
        pDSP                    = p_dsp;
        nSampleRate             = sample_rate;
        size_t samples          = comp_delay_base_metadata::SAMPLES_MAX;
        size_t time_samples     = comp_delay_base_metadata::TIME_MAX * 0.001 * nSampleRate;
        size_t dist_samples     = (comp_delay_base_metadata::METERS_MAX + comp_delay_base_metadata::CENTIMETERS_MAX * 0.01) /
                                    sound_speed(comp_delay_base_metadata::TEMPERATURE_MAX);

        lsp_trace("dsp=%p, samples=%d, time_samples=%d, dist_samples=%d", pDSP, int(samples), int(time_samples), int(dist_samples));
        if (samples < time_samples)
            samples     = time_samples;
        if (samples < dist_samples)
            samples     = dist_samples;

        vBuffer                 = buffer;
        nBufSize                = buf_size;

        vLine.init(pDSP, samples);
        vBypass.init(sample_rate);
    }

    void comp_delay_base::destroy()
    {
        lsp_trace("destroy");
        vLine.destroy();

        vBuffer         = NULL;
        nBufSize        = 0;
    }

    void comp_delay_base::configure()
    {
        // Calculate delay in samples
        size_t samples      = 0;
        float snd_speed     = sound_speed(fTemperature);

        lsp_trace("mode=%d, distance=%.3f, time=%.3f, samples=%.3f, snd_speed=%.3f", int(nMode), fDistance, fTime, fSamples, snd_speed);

        if (nMode == comp_delay_base_metadata::M_DISTANCE)
            samples     = (nSampleRate * fDistance) / snd_speed;
        else if (nMode == comp_delay_base_metadata::M_TIME)
            samples     = fTime * 0.001 * nSampleRate;
        else
            samples     = fSamples;

        // Update delay parameter for delay line
        lsp_trace("final samples=%d", int(samples));
        vLine.set_delay(samples);

        // Re-calculate parameters
        fSamples        = samples;
        fDistance       = (samples * snd_speed * 100.0) / nSampleRate;
        fTime           = (samples * 1000.0) / nSampleRate;
    }

    void comp_delay_base::process(size_t samples)
    {
//        lsp_trace("samples = %d", int(samples));

        // Get input and output buffers
        const float *in     = reinterpret_cast<const float *>(pIn->getBuffer());
        float *out          = reinterpret_cast<float *>(pOut->getBuffer());

        if ((in == NULL) || (out == NULL))
            return;

        while (samples > 0)
        {
            size_t count = (samples > nBufSize) ? nBufSize : samples;

            // Pre-process signal (fill buffer)
            vLine.process(pDSP, vBuffer, in, fWet, count);
            // Apply 'dry' control
            if (fDry > 0.0)
                pDSP->add_multiplied(vBuffer, in, fDry, count);

            vBypass.process(pDSP, out, in, vBuffer, count);

            // Increment pointers
            in          +=  count;
            out         +=  count;
            samples     -=  count;
        }
    }

    comp_delay_impl::comp_delay_impl(const plugin_metadata_t &mdata): plugin(mdata)
    {
        vBuffer     = NULL;
    }

    comp_delay_impl::~comp_delay_impl()
    {
        if (vBuffer != NULL)
        {
            delete [] vBuffer;
            vBuffer = NULL;
        }
    }

    void comp_delay_impl::init(int sample_rate)
    {
        plugin::init(sample_rate);
        vBuffer     = new float[BUFFER_SIZE];
    }


    comp_delay_mono::comp_delay_mono(): comp_delay_impl(metadata)
    {

    }

    comp_delay_mono::~comp_delay_mono()
    {
    }

    void comp_delay_mono::init(int sample_rate)
    {
        comp_delay_impl::init(sample_rate);

        vDelay.init(pDSP, sample_rate, vBuffer, BUFFER_SIZE);
        vDelay.set_ports(vIntPorts[IN], vIntPorts[OUT]);
    }

    void comp_delay_mono::update_settings()
    {
        float out_gain      = vIntPorts[OUT_GAIN]->getValue();
        bool bypass         = vIntPorts[BYPASS]->getValue() >= 0.5;

        vDelay.set_bypass(bypass);

        vDelay.set_mode(vIntPorts[MODE]->getValue());
        vDelay.set_samples(vIntPorts[SAMPLES]->getValue());
        vDelay.set_time(vIntPorts[TIME]->getValue());
        vDelay.set_distance(vIntPorts[METERS]->getValue() +( vIntPorts[CENTIMETERS]->getValue() * 0.01));
        vDelay.set_temperature(vIntPorts[TEMPERATURE]->getValue());
        vDelay.set_dry(vIntPorts[DRY]->getValue() * out_gain);
        vDelay.set_wet(vIntPorts[WET]->getValue() * out_gain);

        vDelay.configure();

        vIntPorts[DEL_TIME]     -> setValue(vDelay.get_time());
        vIntPorts[DEL_SAMPLES]  -> setValue(vDelay.get_samples());
        vIntPorts[DEL_DISTANCE] -> setValue(vDelay.get_distance());

        // TEST for latency compensation
//        set_latency(vDelay.get_samples());
    }

    void comp_delay_mono::process(size_t samples)
    {
        vDelay.process(samples);
    }


    comp_delay_stereo::comp_delay_stereo(): comp_delay_impl(metadata)
    {
    }

    comp_delay_stereo::~comp_delay_stereo()
    {
    }

    void comp_delay_stereo::init(int sample_rate)
    {
        comp_delay_impl::init(sample_rate);

        vDelay[0].init(pDSP, sample_rate, vBuffer, BUFFER_SIZE);
        vDelay[0].set_ports(vIntPorts[IN_L], vIntPorts[OUT_L]);

        vDelay[1].init(pDSP, sample_rate, vBuffer, BUFFER_SIZE);
        vDelay[1].set_ports(vIntPorts[IN_R], vIntPorts[OUT_R]);
    }

    void comp_delay_stereo::update_settings()
    {
        float out_gain      = vIntPorts[OUT_GAIN]->getValue();
        bool bypass         = vIntPorts[BYPASS]->getValue() >= 0.5;

        for (size_t i=0; i<2; ++i)
        {
            comp_delay_base *d    = &vDelay[i];

            d   -> set_bypass(bypass);

            d   -> set_mode(vIntPorts[MODE]->getValue());
            d   -> set_samples(vIntPorts[SAMPLES]->getValue());
            d   -> set_time(vIntPorts[TIME]->getValue());
            d   -> set_distance(vIntPorts[METERS]->getValue() + (vIntPorts[CENTIMETERS]->getValue() * 0.01));
            d   -> set_temperature(vIntPorts[TEMPERATURE]->getValue());
            d   -> set_dry(vIntPorts[DRY]->getValue() * out_gain);
            d   -> set_wet(vIntPorts[WET]->getValue() * out_gain);

            d   -> configure();
        }

        vIntPorts[DEL_TIME]     -> setValue(vDelay[0].get_time());
        vIntPorts[DEL_SAMPLES]  -> setValue(vDelay[0].get_samples());
        vIntPorts[DEL_DISTANCE] -> setValue(vDelay[0].get_distance());
    }

    void comp_delay_stereo::process(size_t samples)
    {
        for (size_t i=0; i<2; ++i)
            vDelay[i].process(samples);
    }


    comp_delay_x2_stereo::comp_delay_x2_stereo(): comp_delay_impl(metadata)
    {
    }

    comp_delay_x2_stereo::~comp_delay_x2_stereo()
    {
    }

    void comp_delay_x2_stereo::init(int sample_rate)
    {
        comp_delay_impl::init(sample_rate);

        vDelay[0].init(pDSP, sample_rate, vBuffer, BUFFER_SIZE);
        vDelay[0].set_ports(vIntPorts[IN_L], vIntPorts[OUT_L]);

        vDelay[1].init(pDSP, sample_rate, vBuffer, BUFFER_SIZE);
        vDelay[1].set_ports(vIntPorts[IN_R], vIntPorts[OUT_R]);
    }

    void comp_delay_x2_stereo::update_settings()
    {
        float out_gain      = vIntPorts[OUT_GAIN]->getValue();
        bool bypass         = vIntPorts[BYPASS]->getValue() >= 0.5;

        vDelay[0].set_bypass( bypass );
        vDelay[1].set_bypass( bypass );

        vDelay[0].set_mode(vIntPorts[MODE_L]->getValue());
        vDelay[0].set_samples(vIntPorts[SAMPLES_L]->getValue());
        vDelay[0].set_time(vIntPorts[TIME_L]->getValue());
        vDelay[0].set_distance(vIntPorts[METERS_L]->getValue() + (vIntPorts[CENTIMETERS_L]->getValue() * 0.01));
        vDelay[0].set_temperature(vIntPorts[TEMPERATURE_L]->getValue());
        vDelay[0].set_dry(vIntPorts[DRY_L]->getValue() * out_gain);
        vDelay[0].set_wet(vIntPorts[WET_L]->getValue() * out_gain);

        vDelay[1].set_mode(vIntPorts[MODE_R]->getValue());
        vDelay[1].set_samples(vIntPorts[SAMPLES_R]->getValue());
        vDelay[1].set_time(vIntPorts[TIME_R]->getValue());
        vDelay[1].set_distance(vIntPorts[METERS_R]->getValue() + (vIntPorts[CENTIMETERS_R]->getValue() * 0.01));
        vDelay[1].set_temperature(vIntPorts[TEMPERATURE_R]->getValue());
        vDelay[1].set_dry(vIntPorts[DRY_R]->getValue() * out_gain);
        vDelay[1].set_wet(vIntPorts[WET_R]->getValue() * out_gain);

        vDelay[0].configure();
        vDelay[1].configure();

        vIntPorts[DEL_TIME_L]       -> setValue(vDelay[0].get_time());
        vIntPorts[DEL_SAMPLES_L]    -> setValue(vDelay[0].get_samples());
        vIntPorts[DEL_DISTANCE_L]   -> setValue(vDelay[0].get_distance());

        vIntPorts[DEL_TIME_R]       -> setValue(vDelay[1].get_time());
        vIntPorts[DEL_SAMPLES_R]    -> setValue(vDelay[1].get_samples());
        vIntPorts[DEL_DISTANCE_R]   -> setValue(vDelay[1].get_distance());
    }

    void comp_delay_x2_stereo::process(size_t samples)
    {
        for (size_t i=0; i<2; ++i)
            vDelay[i].process(samples);
    }
} /* namespace ddb */


