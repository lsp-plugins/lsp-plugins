/*
 * metadata.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef METADATA_PLUGINS_H_
#define METADATA_PLUGINS_H_

#include <core/types.h>
#include <core/windows.h>
#include <core/envelope.h>
#include <core/status.h>
#include <metadata/metadata.h>

// LADSPA ID allocation
#define LSP_PHASE_DETECTOR_BASE         LSP_LADSPA_BASE + 0
#define LSP_COMP_DELAY_BASE             LSP_LADSPA_BASE + 1
#define LSP_SPECTRUM_ANALYZER_BASE      LSP_LADSPA_BASE + 4
#define LSP_PARA_EQUALIZER_BASE         LSP_LADSPA_BASE + 10
#define LSP_GRAPH_EQUALIZER_BASE        LSP_LADSPA_BASE + 18
#define LSP_COMPRESSOR_BASE             LSP_LADSPA_BASE + 26
#define LSP_DYNAMIC_PROCESSOR_BASE      LSP_LADSPA_BASE + 34
#define LSP_EXPANDER_BASE               LSP_LADSPA_BASE + 42
#define LSP_GATE_BASE                   LSP_LADSPA_BASE + 50
#define LSP_LIMITER_BASE                LSP_LADSPA_BASE + 58
#define LSP_IMPULSE_RESPONSES_BASE      LSP_LADSPA_BASE + 62
#define LSP_IMPULSE_REVERB_BASE         LSP_LADSPA_BASE + 64
#define LSP_SLAP_DELAY_BASE             LSP_LADSPA_BASE + 66
#define LSP_OSCILLATOR_BASE             LSP_LADSPA_BASE + 68
#define LSP_LATENCY_METER_BASE          LSP_LADSPA_BASE + 69
#define LSP_MB_COMPRESSOR_BASE          LSP_LADSPA_BASE + 70
#define LSP_PROFILER_BASE               LSP_LADSPA_BASE + 78
#define LSP_NONLINEAR_CONVOLVER_BASE    LSP_LADSPA_BASE + 79


// Plugin metadata definitions
#include <metadata/phase_detector.h>
#include <metadata/comp_delay.h>
#include <metadata/spectrum_analyzer.h>
#include <metadata/sampler.h>
#include <metadata/trigger.h>
#include <metadata/para_equalizer.h>
#include <metadata/graph_equalizer.h>
#include <metadata/compressor.h>
#include <metadata/dyna_processor.h>
#include <metadata/expander.h>
#include <metadata/gate.h>
#include <metadata/limiter.h>
#include <metadata/impulse_responses.h>
#include <metadata/impulse_reverb.h>
#include <metadata/slap_delay.h>
#include <metadata/experimental.h>
#include <metadata/oscillator.h>
#include <metadata/latency_meter.h>
#include <metadata/mb_compressor.h>

#include <metadata/profiler.h>
#include <metadata/nonlinear_convolver.h>

#endif /* METADATA_PLUGINS_H_ */
