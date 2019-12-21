/*
 * corellator.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <core/debug.h>
#include <plugins/comp_delay.h>
#include <math.h>

#include <string.h>

#define BUFFER_SIZE         (0x10000 / sizeof(float))

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
        nDelay          = 0;
        nNewDelay       = 0;
        bRamping        = false;
        pIn             = NULL;
        pOut            = NULL;
    }

    comp_delay_base::~comp_delay_base()
    {
        destroy();
    }

    void comp_delay_base::init(long sample_rate, float *buffer, size_t buf_size)
    {
        nSampleRate             = sample_rate;
        size_t samples          = comp_delay_base_metadata::SAMPLES_MAX;
        size_t time_samples     = comp_delay_base_metadata::TIME_MAX * 0.001 * nSampleRate;
        size_t dist_samples     = (comp_delay_base_metadata::METERS_MAX + comp_delay_base_metadata::CENTIMETERS_MAX * 0.01) /
                                    sound_speed(comp_delay_base_metadata::TEMPERATURE_MAX);

        lsp_trace("samples=%d, time_samples=%d, dist_samples=%d", int(samples), int(time_samples), int(dist_samples));
        if (samples < time_samples)
            samples     = time_samples;
        if (samples < dist_samples)
            samples     = dist_samples;

        vBuffer                 = buffer;
        nBufSize                = buf_size;

        vLine.init(samples);
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
        float snd_speed     = sound_speed(fTemperature);

        lsp_trace("mode=%d, distance=%.3f, time=%.3f, samples=%.3f, snd_speed=%.3f", int(nMode), fDistance, fTime, fSamples, snd_speed);

        if (nMode == comp_delay_base_metadata::M_DISTANCE)
            nNewDelay   = (nSampleRate * fDistance) / snd_speed;
        else if (nMode == comp_delay_base_metadata::M_TIME)
            nNewDelay   = fTime * 0.001f * nSampleRate;
        else
            nNewDelay   = fSamples;

        // Update delay parameter for delay line
        if (nNewDelay < 0)
            nNewDelay       = 0;
        if (!bRamping)
            nDelay          = nNewDelay;
        lsp_trace("final delay=%d, new_delay=%d", int(nDelay), int(nNewDelay));
        vLine.set_delay(nDelay);

        // Re-calculate parameters
        fSamples        = nNewDelay;
        fDistance       = (nNewDelay * snd_speed * 100.0) / float(nSampleRate);
        fTime           = (nNewDelay * 1000.0f) / float(nSampleRate);
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
            vLine.process_ramping(vBuffer, in, fWet, nNewDelay, samples);
            nDelay      = nNewDelay;

            // Apply 'dry' control
            if (fDry > 0.0)
                dsp::fmadd_k3(vBuffer, in, fDry, count);

            vBypass.process(out, in, vBuffer, count);

            // Increment pointers
            in          +=  count;
            out         +=  count;
            samples     -=  count;
        }
    }

    comp_delay_impl::comp_delay_impl(const plugin_metadata_t &mdata): plugin_t(mdata)
    {
        vBuffer     = NULL;
    }

    comp_delay_impl::~comp_delay_impl()
    {
    }

    void comp_delay_impl::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        if (vBuffer == NULL)
            vBuffer     = new float[BUFFER_SIZE];
    }

    void comp_delay_impl::update_sample_rate(long sr)
    {
        dropBuffers();
        createBuffers();
    }

    void comp_delay_impl::dropBuffers()
    {
    }

    void comp_delay_impl::createBuffers()
    {
    }

    void comp_delay_impl::destroy()
    {
        dropBuffers();

        if (vBuffer != NULL)
        {
            delete [] vBuffer;
            vBuffer = NULL;
        }
        plugin_t::destroy();
    }

    comp_delay_mono::comp_delay_mono(): comp_delay_impl(metadata)
    {

    }

    comp_delay_mono::~comp_delay_mono()
    {
    }

    void comp_delay_mono::createBuffers()
    {
        vDelay.init(fSampleRate, vBuffer, BUFFER_SIZE);
        vDelay.set_ports(vPorts[A_IN], vPorts[A_OUT]);
    }

    void comp_delay_mono::dropBuffers()
    {
        vDelay.destroy();
    }

    void comp_delay_mono::update_settings()
    {
        float out_gain      = vPorts[OUT_GAIN]->getValue();
        bool bypass         = vPorts[BYPASS]->getValue() >= 0.5;

        vDelay.set_bypass(bypass);

        vDelay.set_mode(vPorts[MODE]->getValue());
        vDelay.set_ramping(vPorts[RAMPING]->getValue() >= 0.5f);
        vDelay.set_samples(vPorts[SAMPLES]->getValue());
        vDelay.set_time(vPorts[TIME]->getValue());
        vDelay.set_distance(vPorts[METERS]->getValue() + (vPorts[CENTIMETERS]->getValue() * 0.01));
        vDelay.set_temperature(vPorts[TEMPERATURE]->getValue());
        vDelay.set_dry(vPorts[DRY]->getValue() * out_gain);
        vDelay.set_wet(vPorts[WET]->getValue() * out_gain);

        vDelay.configure();

        vPorts[DEL_TIME]        -> setValue(vDelay.get_time());
        vPorts[DEL_SAMPLES]     -> setValue(vDelay.get_samples());
        vPorts[DEL_DISTANCE]    -> setValue(vDelay.get_distance());

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

    void comp_delay_stereo::createBuffers()
    {
        vDelay[0].init(fSampleRate, vBuffer, BUFFER_SIZE);
        vDelay[0].set_ports(vPorts[IN_L], vPorts[OUT_L]);

        vDelay[1].init(fSampleRate, vBuffer, BUFFER_SIZE);
        vDelay[1].set_ports(vPorts[IN_R], vPorts[OUT_R]);
    }

    void comp_delay_stereo::dropBuffers()
    {
        vDelay[0].destroy();
        vDelay[1].destroy();
    }

    void comp_delay_stereo::update_settings()
    {
        float out_gain      = vPorts[OUT_GAIN]->getValue();
        bool bypass         = vPorts[BYPASS]->getValue() >= 0.5;

        for (size_t i=0; i<2; ++i)
        {
            comp_delay_base *d    = &vDelay[i];

            d   -> set_bypass(bypass);

            d   -> set_mode(vPorts[MODE]->getValue());
            d   -> set_ramping(vPorts[RAMPING]->getValue() >= 0.5f);
            d   -> set_samples(vPorts[SAMPLES]->getValue());
            d   -> set_time(vPorts[TIME]->getValue());
            d   -> set_distance(vPorts[METERS]->getValue() + (vPorts[CENTIMETERS]->getValue() * 0.01));
            d   -> set_temperature(vPorts[TEMPERATURE]->getValue());
            d   -> set_dry(vPorts[DRY]->getValue() * out_gain);
            d   -> set_wet(vPorts[WET]->getValue() * out_gain);

            d   -> configure();
        }

        vPorts[DEL_TIME]        -> setValue(vDelay[0].get_time());
        vPorts[DEL_SAMPLES]     -> setValue(vDelay[0].get_samples());
        vPorts[DEL_DISTANCE]    -> setValue(vDelay[0].get_distance());
    }

    void comp_delay_stereo::process(size_t samples)
    {
        vDelay[0].process(samples);
        vDelay[1].process(samples);
    }

    comp_delay_x2_stereo::comp_delay_x2_stereo(): comp_delay_impl(metadata)
    {
    }

    comp_delay_x2_stereo::~comp_delay_x2_stereo()
    {
    }

    void comp_delay_x2_stereo::createBuffers()
    {
        vDelay[0].init(fSampleRate, vBuffer, BUFFER_SIZE);
        vDelay[0].set_ports(vPorts[IN_L], vPorts[OUT_L]);

        vDelay[1].init(fSampleRate, vBuffer, BUFFER_SIZE);
        vDelay[1].set_ports(vPorts[IN_R], vPorts[OUT_R]);
    }

    void comp_delay_x2_stereo::dropBuffers()
    {
        vDelay[0].destroy();
        vDelay[1].destroy();
    }

    void comp_delay_x2_stereo::update_settings()
    {
        float out_gain      = vPorts[OUT_GAIN]->getValue();
        bool bypass         = vPorts[BYPASS]->getValue() >= 0.5;

        vDelay[0].set_bypass( bypass );
        vDelay[1].set_bypass( bypass );

        vDelay[0].set_mode(vPorts[MODE_L]->getValue());
        vDelay[0].set_ramping(vPorts[RAMPING_L]->getValue());
        vDelay[0].set_samples(vPorts[SAMPLES_L]->getValue());
        vDelay[0].set_time(vPorts[TIME_L]->getValue());
        vDelay[0].set_distance(vPorts[METERS_L]->getValue() + (vPorts[CENTIMETERS_L]->getValue() * 0.01));
        vDelay[0].set_temperature(vPorts[TEMPERATURE_L]->getValue());
        vDelay[0].set_dry(vPorts[DRY_L]->getValue() * out_gain);
        vDelay[0].set_wet(vPorts[WET_L]->getValue() * out_gain);

        vDelay[1].set_mode(vPorts[MODE_R]->getValue());
        vDelay[1].set_ramping(vPorts[RAMPING_R]->getValue());
        vDelay[1].set_samples(vPorts[SAMPLES_R]->getValue());
        vDelay[1].set_time(vPorts[TIME_R]->getValue());
        vDelay[1].set_distance(vPorts[METERS_R]->getValue() + (vPorts[CENTIMETERS_R]->getValue() * 0.01));
        vDelay[1].set_temperature(vPorts[TEMPERATURE_R]->getValue());
        vDelay[1].set_dry(vPorts[DRY_R]->getValue() * out_gain);
        vDelay[1].set_wet(vPorts[WET_R]->getValue() * out_gain);

        vDelay[0].configure();
        vDelay[1].configure();

        vPorts[DEL_TIME_L]      -> setValue(vDelay[0].get_time());
        vPorts[DEL_SAMPLES_L]   -> setValue(vDelay[0].get_samples());
        vPorts[DEL_DISTANCE_L]  -> setValue(vDelay[0].get_distance());

        vPorts[DEL_TIME_R]      -> setValue(vDelay[1].get_time());
        vPorts[DEL_SAMPLES_R]   -> setValue(vDelay[1].get_samples());
        vPorts[DEL_DISTANCE_R]  -> setValue(vDelay[1].get_distance());
    }

    void comp_delay_x2_stereo::process(size_t samples)
    {
        vDelay[0].process(samples);
        vDelay[1].process(samples);
    }
} /* namespace ddb */


