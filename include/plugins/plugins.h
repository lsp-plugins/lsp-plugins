/*
 * plugins.h
 *
 *  Created on: 06 окт. 2015 г.
 *      Author: sadko
 */

#ifndef PLUGINS_PLUGINS_H_
#define PLUGINS_PLUGINS_H_

#include <core/debug.h>
#include <core/plugin.h>

// List of plugins
#include <plugins/phase_detector.h>
#include <plugins/comp_delay.h>
#include <plugins/spectrum_analyzer.h>
#include <plugins/sampler.h>
#include <plugins/trigger.h>
#include <plugins/para_equalizer.h>
#include <plugins/graph_equalizer.h>
#include <plugins/compressor.h>
#include <plugins/dyna_processor.h>
#include <plugins/expander.h>
#include <plugins/gate.h>
#include <plugins/limiter.h>
#include <plugins/impulse_responses.h>
#include <plugins/impulse_reverb.h>
#include <plugins/slap_delay.h>
#include <plugins/oscillator.h>
#include <plugins/latency_meter.h>
#include <plugins/mb_compressor.h>

#include <plugins/profiler.h>
#include <plugins/nonlinear_convolver.h>

#include <plugins/experimental.h>

#endif /* PLUGINS_PLUGINS_H_ */
