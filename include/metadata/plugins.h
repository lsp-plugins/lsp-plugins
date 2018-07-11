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
#define LSP_EQUALIZER_BASE              LSP_LADSPA_BASE + 10

// Plugin metadata definitions
#include <metadata/phase_detector.h>
#include <metadata/comp_delay.h>
#include <metadata/spectrum_analyzer.h>
#include <metadata/sampler.h>
#include <metadata/trigger.h>
#include <metadata/equalizer.h>
#include <metadata/experimental.h>

#endif /* METADATA_PLUGINS_H_ */
