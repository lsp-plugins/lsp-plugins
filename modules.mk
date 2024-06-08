#
# Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of lsp-plugins
#
# lsp-plugins is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# lsp-plugins is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with lsp-plugins.  If not, see <https://www.gnu.org/licenses/>.
#

# Variables that describe dependencies
LSP_3RD_PARTY_VERSION      := 1.0.16
LSP_3RD_PARTY_NAME         := lsp-3rd-party
LSP_3RD_PARTY_TYPE         := hdr
LSP_3RD_PARTY_INC_OPT      := -idirafter
LSP_3RD_PARTY_URL_RO       := https://github.com/lsp-plugins/$(LSP_3RD_PARTY_NAME).git
LSP_3RD_PARTY_URL_RW       := git@github.com:lsp-plugins/$(LSP_3RD_PARTY_NAME).git

LSP_COMMON_LIB_VERSION     := 1.0.36
LSP_COMMON_LIB_NAME        := lsp-common-lib
LSP_COMMON_LIB_TYPE        := src
LSP_COMMON_LIB_URL_RO      := https://github.com/lsp-plugins/$(LSP_COMMON_LIB_NAME).git
LSP_COMMON_LIB_URL_RW      := git@github.com:lsp-plugins/$(LSP_COMMON_LIB_NAME).git

LSP_DSP_LIB_VERSION        := 1.0.24
LSP_DSP_LIB_NAME           := lsp-dsp-lib
LSP_DSP_LIB_TYPE           := src
LSP_DSP_LIB_URL_RO         := https://github.com/lsp-plugins/$(LSP_DSP_LIB_NAME).git
LSP_DSP_LIB_URL_RW         := git@github.com:lsp-plugins/$(LSP_DSP_LIB_NAME).git

LSP_DSP_UNITS_VERSION      := 1.0.23
LSP_DSP_UNITS_NAME         := lsp-dsp-units
LSP_DSP_UNITS_TYPE         := src
LSP_DSP_UNITS_URL_RO       := https://github.com/lsp-plugins/$(LSP_DSP_UNITS_NAME).git
LSP_DSP_UNITS_URL_RW       := git@github.com:lsp-plugins/$(LSP_DSP_UNITS_NAME).git

LSP_LLTL_LIB_VERSION       := 1.0.19
LSP_LLTL_LIB_NAME          := lsp-lltl-lib
LSP_LLTL_LIB_TYPE          := src
LSP_LLTL_LIB_URL_RO        := https://github.com/lsp-plugins/$(LSP_LLTL_LIB_NAME).git
LSP_LLTL_LIB_URL_RW        := git@github.com:lsp-plugins/$(LSP_LLTL_LIB_NAME).git

LSP_R3D_BASE_LIB_VERSION   := 1.0.18
LSP_R3D_BASE_LIB_NAME      := lsp-r3d-base-lib
LSP_R3D_BASE_LIB_TYPE      := src
LSP_R3D_BASE_LIB_URL_RO    := https://github.com/lsp-plugins/$(LSP_R3D_BASE_LIB_NAME).git
LSP_R3D_BASE_LIB_URL_RW    := git@github.com:lsp-plugins/$(LSP_R3D_BASE_LIB_NAME).git

LSP_R3D_IFACE_VERSION      := 1.0.18
LSP_R3D_IFACE_NAME         := lsp-r3d-iface
LSP_R3D_IFACE_TYPE         := src
LSP_R3D_IFACE_URL_RO       := https://github.com/lsp-plugins/$(LSP_R3D_IFACE_NAME).git
LSP_R3D_IFACE_URL_RW       := git@github.com:lsp-plugins/$(LSP_R3D_IFACE_NAME).git

LSP_R3D_GLX_LIB_VERSION    := 1.0.18
LSP_R3D_GLX_LIB_NAME       := lsp-r3d-glx-lib
LSP_R3D_GLX_LIB_TYPE       := bin
LSP_R3D_GLX_LIB_URL_RO     := https://github.com/lsp-plugins/$(LSP_R3D_GLX_LIB_NAME).git
LSP_R3D_GLX_LIB_URL_RW     := git@github.com:lsp-plugins/$(LSP_R3D_GLX_LIB_NAME).git

LSP_R3D_WGL_LIB_VERSION    := 1.0.13
LSP_R3D_WGL_LIB_NAME       := lsp-r3d-wgl-lib
LSP_R3D_WGL_LIB_TYPE       := bin
LSP_R3D_WGL_LIB_URL_RO     := https://github.com/lsp-plugins/$(LSP_R3D_WGL_LIB_NAME).git
LSP_R3D_WGL_LIB_URL_RW     := git@github.com:lsp-plugins/$(LSP_R3D_WGL_LIB_NAME).git

LSP_RUNTIME_LIB_VERSION    := 1.0.22
LSP_RUNTIME_LIB_NAME       := lsp-runtime-lib
LSP_RUNTIME_LIB_TYPE       := src
LSP_RUNTIME_LIB_URL_RO     := https://github.com/lsp-plugins/$(LSP_RUNTIME_LIB_NAME).git
LSP_RUNTIME_LIB_URL_RW     := git@github.com:lsp-plugins/$(LSP_RUNTIME_LIB_NAME).git

LSP_TEST_FW_VERSION        := 1.0.25
LSP_TEST_FW_NAME           := lsp-test-fw
LSP_TEST_FW_TYPE           := src
LSP_TEST_FW_URL_RO         := https://github.com/lsp-plugins/$(LSP_TEST_FW_NAME).git
LSP_TEST_FW_URL_RW         := git@github.com:lsp-plugins/$(LSP_TEST_FW_NAME).git

LSP_TK_LIB_VERSION         := 1.0.22
LSP_TK_LIB_NAME            := lsp-tk-lib
LSP_TK_LIB_TYPE            := src
LSP_TK_LIB_URL_RO          := https://github.com/lsp-plugins/$(LSP_TK_LIB_NAME).git
LSP_TK_LIB_URL_RW          := git@github.com:lsp-plugins/$(LSP_TK_LIB_NAME).git

LSP_WS_LIB_VERSION         := 1.0.22
LSP_WS_LIB_NAME            := lsp-ws-lib
LSP_WS_LIB_TYPE            := src
LSP_WS_LIB_URL_RO          := https://github.com/lsp-plugins/$(LSP_WS_LIB_NAME).git
LSP_WS_LIB_URL_RW          := git@github.com:lsp-plugins/$(LSP_WS_LIB_NAME).git

# Plugin-related module dependencies
LSP_PLUGIN_FW_VERSION      := 1.0.24
LSP_PLUGIN_FW_NAME         := lsp-plugin-fw
LSP_PLUGIN_FW_TYPE         := src
LSP_PLUGIN_FW_URL_RO       := https://github.com/lsp-plugins/$(LSP_PLUGIN_FW_NAME).git
LSP_PLUGIN_FW_URL_RW       := git@github.com:lsp-plugins/$(LSP_PLUGIN_FW_NAME).git

LSP_PLUGINS_SHARED_VERSION := 1.0.23
LSP_PLUGINS_SHARED_NAME    := lsp-plugins-shared
LSP_PLUGINS_SHARED_TYPE    := src
LSP_PLUGINS_SHARED_URL_RO  := https://github.com/lsp-plugins/$(LSP_PLUGINS_SHARED_NAME).git
LSP_PLUGINS_SHARED_URL_RW  := git@github.com:lsp-plugins/$(LSP_PLUGINS_SHARED_NAME).git

# Specify description of plugin dependencies
LSP_PLUGINS_AB_TESTER_VERSION           := 1.0.11
LSP_PLUGINS_AB_TESTER_NAME              := lsp-plugins-ab-tester
LSP_PLUGINS_AB_TESTER_TYPE              := plug
LSP_PLUGINS_AB_TESTER_URL_RO            := https://github.com/lsp-plugins/$(LSP_PLUGINS_AB_TESTER_NAME).git
LSP_PLUGINS_AB_TESTER_URL_RW            := git@github.com:lsp-plugins/$(LSP_PLUGINS_AB_TESTER_NAME).git

LSP_PLUGINS_ART_DELAY_VERSION           := 1.0.20
LSP_PLUGINS_ART_DELAY_NAME              := lsp-plugins-art-delay
LSP_PLUGINS_ART_DELAY_TYPE              := plug
LSP_PLUGINS_ART_DELAY_URL_RO            := https://github.com/lsp-plugins/$(LSP_PLUGINS_ART_DELAY_NAME).git
LSP_PLUGINS_ART_DELAY_URL_RW            := git@github.com:lsp-plugins/$(LSP_PLUGINS_ART_DELAY_NAME).git

LSP_PLUGINS_AUTOGAIN_VERSION            := 1.0.5
LSP_PLUGINS_AUTOGAIN_NAME               := lsp-plugins-autogain
LSP_PLUGINS_AUTOGAIN_TYPE               := plug
LSP_PLUGINS_AUTOGAIN_URL_RO             := https://github.com/lsp-plugins/$(LSP_PLUGINS_AUTOGAIN_NAME).git
LSP_PLUGINS_AUTOGAIN_URL_RW             := git@github.com:lsp-plugins/$(LSP_PLUGINS_AUTOGAIN_NAME).git

LSP_PLUGINS_BEAT_BREATHER_VERSION       := 1.0.8
LSP_PLUGINS_BEAT_BREATHER_NAME          := lsp-plugins-beat-breather
LSP_PLUGINS_BEAT_BREATHER_TYPE          := plug
LSP_PLUGINS_BEAT_BREATHER_URL_RO        := https://github.com/lsp-plugins/$(LSP_PLUGINS_BEAT_BREATHER_NAME).git
LSP_PLUGINS_BEAT_BREATHER_URL_RW        := git@github.com:lsp-plugins/$(LSP_PLUGINS_BEAT_BREATHER_NAME).git

LSP_PLUGINS_CHORUS_VERSION              := 1.0.1
LSP_PLUGINS_CHORUS_NAME                 := lsp-plugins-chorus
LSP_PLUGINS_CHORUS_TYPE                 := plug
LSP_PLUGINS_CHORUS_URL_RO               := https://github.com/lsp-plugins/$(LSP_PLUGINS_CHORUS_NAME).git
LSP_PLUGINS_CHORUS_URL_RW               := git@github.com:lsp-plugins/$(LSP_PLUGINS_CHORUS_NAME).git

LSP_PLUGINS_CLIPPER_VERSION             := 1.0.3
LSP_PLUGINS_CLIPPER_NAME                := lsp-plugins-clipper
LSP_PLUGINS_CLIPPER_TYPE                := plug
LSP_PLUGINS_CLIPPER_URL_RO              := https://github.com/lsp-plugins/$(LSP_PLUGINS_CLIPPER_NAME).git
LSP_PLUGINS_CLIPPER_URL_RW              := git@github.com:lsp-plugins/$(LSP_PLUGINS_CLIPPER_NAME).git

LSP_PLUGINS_COMP_DELAY_VERSION          := 1.0.23
LSP_PLUGINS_COMP_DELAY_NAME             := lsp-plugins-comp-delay
LSP_PLUGINS_COMP_DELAY_TYPE             := plug
LSP_PLUGINS_COMP_DELAY_URL_RO           := https://github.com/lsp-plugins/$(LSP_PLUGINS_COMP_DELAY_NAME).git
LSP_PLUGINS_COMP_DELAY_URL_RW           := git@github.com:lsp-plugins/$(LSP_PLUGINS_COMP_DELAY_NAME).git

LSP_PLUGINS_COMPRESSOR_VERSION          := 1.0.24
LSP_PLUGINS_COMPRESSOR_NAME             := lsp-plugins-compressor
LSP_PLUGINS_COMPRESSOR_TYPE             := plug
LSP_PLUGINS_COMPRESSOR_URL_RO           := https://github.com/lsp-plugins/$(LSP_PLUGINS_COMPRESSOR_NAME).git
LSP_PLUGINS_COMPRESSOR_URL_RW           := git@github.com:lsp-plugins/$(LSP_PLUGINS_COMPRESSOR_NAME).git

LSP_PLUGINS_CROSSOVER_VERSION           := 1.0.20
LSP_PLUGINS_CROSSOVER_NAME              := lsp-plugins-crossover
LSP_PLUGINS_CROSSOVER_TYPE              := plug
LSP_PLUGINS_CROSSOVER_URL_RO            := https://github.com/lsp-plugins/$(LSP_PLUGINS_CROSSOVER_NAME).git
LSP_PLUGINS_CROSSOVER_URL_RW            := git@github.com:lsp-plugins/$(LSP_PLUGINS_CROSSOVER_NAME).git

LSP_PLUGINS_DYNA_PROCESSOR_VERSION      := 1.0.23
LSP_PLUGINS_DYNA_PROCESSOR_NAME         := lsp-plugins-dyna-processor
LSP_PLUGINS_DYNA_PROCESSOR_TYPE         := plug
LSP_PLUGINS_DYNA_PROCESSOR_URL_RO       := https://github.com/lsp-plugins/$(LSP_PLUGINS_DYNA_PROCESSOR_NAME).git
LSP_PLUGINS_DYNA_PROCESSOR_URL_RW       := git@github.com:lsp-plugins/$(LSP_PLUGINS_DYNA_PROCESSOR_NAME).git

LSP_PLUGINS_EXPANDER_VERSION            := 1.0.23
LSP_PLUGINS_EXPANDER_NAME               := lsp-plugins-expander
LSP_PLUGINS_EXPANDER_TYPE               := plug
LSP_PLUGINS_EXPANDER_URL_RO             := https://github.com/lsp-plugins/$(LSP_PLUGINS_EXPANDER_NAME).git
LSP_PLUGINS_EXPANDER_URL_RW             := git@github.com:lsp-plugins/$(LSP_PLUGINS_EXPANDER_NAME).git

LSP_PLUGINS_FILTER_VERSION             	:= 1.0.8
LSP_PLUGINS_FILTER_NAME                	:= lsp-plugins-filter
LSP_PLUGINS_FILTER_TYPE                	:= plug
LSP_PLUGINS_FILTER_URL_RO              	:= https://github.com/lsp-plugins/$(LSP_PLUGINS_FILTER_NAME).git
LSP_PLUGINS_FILTER_URL_RW              	:= git@github.com:lsp-plugins/$(LSP_PLUGINS_FILTER_NAME).git

LSP_PLUGINS_FLANGER_VERSION             := 1.0.8
LSP_PLUGINS_FLANGER_NAME                := lsp-plugins-flanger
LSP_PLUGINS_FLANGER_TYPE                := plug
LSP_PLUGINS_FLANGER_URL_RO              := https://github.com/lsp-plugins/$(LSP_PLUGINS_FLANGER_NAME).git
LSP_PLUGINS_FLANGER_URL_RW              := git@github.com:lsp-plugins/$(LSP_PLUGINS_FLANGER_NAME).git

LSP_PLUGINS_GATE_VERSION                := 1.0.23
LSP_PLUGINS_GATE_NAME                   := lsp-plugins-gate
LSP_PLUGINS_GATE_TYPE                   := plug
LSP_PLUGINS_GATE_URL_RO                 := https://github.com/lsp-plugins/$(LSP_PLUGINS_GATE_NAME).git
LSP_PLUGINS_GATE_URL_RW                 := git@github.com:lsp-plugins/$(LSP_PLUGINS_GATE_NAME).git

LSP_PLUGINS_GOTT_COMPRESSOR_VERSION     := 1.0.8
LSP_PLUGINS_GOTT_COMPRESSOR_NAME        := lsp-plugins-gott-compressor
LSP_PLUGINS_GOTT_COMPRESSOR_TYPE        := plug
LSP_PLUGINS_GOTT_COMPRESSOR_URL_RO      := https://github.com/lsp-plugins/$(LSP_PLUGINS_GOTT_COMPRESSOR_NAME).git
LSP_PLUGINS_GOTT_COMPRESSOR_URL_RW      := git@github.com:lsp-plugins/$(LSP_PLUGINS_GOTT_COMPRESSOR_NAME).git

LSP_PLUGINS_GRAPH_EQUALIZER_VERSION     := 1.0.23
LSP_PLUGINS_GRAPH_EQUALIZER_NAME        := lsp-plugins-graph-equalizer
LSP_PLUGINS_GRAPH_EQUALIZER_TYPE        := plug
LSP_PLUGINS_GRAPH_EQUALIZER_URL_RO      := https://github.com/lsp-plugins/$(LSP_PLUGINS_GRAPH_EQUALIZER_NAME).git
LSP_PLUGINS_GRAPH_EQUALIZER_URL_RW      := git@github.com:lsp-plugins/$(LSP_PLUGINS_GRAPH_EQUALIZER_NAME).git

LSP_PLUGINS_IMPULSE_RESPONSES_VERSION   := 1.0.23
LSP_PLUGINS_IMPULSE_RESPONSES_NAME      := lsp-plugins-impulse-responses
LSP_PLUGINS_IMPULSE_RESPONSES_TYPE      := plug
LSP_PLUGINS_IMPULSE_RESPONSES_URL_RO    := https://github.com/lsp-plugins/$(LSP_PLUGINS_IMPULSE_RESPONSES_NAME).git
LSP_PLUGINS_IMPULSE_RESPONSES_URL_RW    := git@github.com:lsp-plugins/$(LSP_PLUGINS_IMPULSE_RESPONSES_NAME).git

LSP_PLUGINS_IMPULSE_REVERB_VERSION      := 1.0.20
LSP_PLUGINS_IMPULSE_REVERB_NAME         := lsp-plugins-impulse-reverb
LSP_PLUGINS_IMPULSE_REVERB_TYPE         := plug
LSP_PLUGINS_IMPULSE_REVERB_URL_RO       := https://github.com/lsp-plugins/$(LSP_PLUGINS_IMPULSE_REVERB_NAME).git
LSP_PLUGINS_IMPULSE_REVERB_URL_RW       := git@github.com:lsp-plugins/$(LSP_PLUGINS_IMPULSE_REVERB_NAME).git

LSP_PLUGINS_LATENCY_METER_VERSION       := 1.0.19
LSP_PLUGINS_LATENCY_METER_NAME          := lsp-plugins-latency-meter
LSP_PLUGINS_LATENCY_METER_TYPE          := plug
LSP_PLUGINS_LATENCY_METER_URL_RO        := https://github.com/lsp-plugins/$(LSP_PLUGINS_LATENCY_METER_NAME).git
LSP_PLUGINS_LATENCY_METER_URL_RW        := git@github.com:lsp-plugins/$(LSP_PLUGINS_LATENCY_METER_NAME).git

LSP_PLUGINS_LIMITER_VERSION             := 1.0.21
LSP_PLUGINS_LIMITER_NAME                := lsp-plugins-limiter
LSP_PLUGINS_LIMITER_TYPE                := plug
LSP_PLUGINS_LIMITER_URL_RO              := https://github.com/lsp-plugins/$(LSP_PLUGINS_LIMITER_NAME).git
LSP_PLUGINS_LIMITER_URL_RW              := git@github.com:lsp-plugins/$(LSP_PLUGINS_LIMITER_NAME).git

LSP_PLUGINS_LOUD_COMP_VERSION           := 1.0.19
LSP_PLUGINS_LOUD_COMP_NAME              := lsp-plugins-loud-comp
LSP_PLUGINS_LOUD_COMP_TYPE              := plug
LSP_PLUGINS_LOUD_COMP_URL_RO            := https://github.com/lsp-plugins/$(LSP_PLUGINS_LOUD_COMP_NAME).git
LSP_PLUGINS_LOUD_COMP_URL_RW            := git@github.com:lsp-plugins/$(LSP_PLUGINS_LOUD_COMP_NAME).git

LSP_PLUGINS_MB_CLIPPER_VERSION       	:= 1.0.3
LSP_PLUGINS_MB_CLIPPER_NAME          	:= lsp-plugins-mb-clipper
LSP_PLUGINS_MB_CLIPPER_TYPE          	:= plug
LSP_PLUGINS_MB_CLIPPER_URL_RO        	:= https://github.com/lsp-plugins/$(LSP_PLUGINS_MB_CLIPPER_NAME).git
LSP_PLUGINS_MB_CLIPPER_URL_RW        	:= git@github.com:lsp-plugins/$(LSP_PLUGINS_MB_CLIPPER_NAME).git

LSP_PLUGINS_MB_COMPRESSOR_VERSION       := 1.0.21
LSP_PLUGINS_MB_COMPRESSOR_NAME          := lsp-plugins-mb-compressor
LSP_PLUGINS_MB_COMPRESSOR_TYPE          := plug
LSP_PLUGINS_MB_COMPRESSOR_URL_RO        := https://github.com/lsp-plugins/$(LSP_PLUGINS_MB_COMPRESSOR_NAME).git
LSP_PLUGINS_MB_COMPRESSOR_URL_RW        := git@github.com:lsp-plugins/$(LSP_PLUGINS_MB_COMPRESSOR_NAME).git

LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION   := 1.0.16
LSP_PLUGINS_MB_DYNA_PROCESSOR_NAME      := lsp-plugins-mb-dyna-processor
LSP_PLUGINS_MB_DYNA_PROCESSOR_TYPE      := plug
LSP_PLUGINS_MB_DYNA_PROCESSOR_URL_RO    := https://github.com/lsp-plugins/$(LSP_PLUGINS_MB_DYNA_PROCESSOR_NAME).git
LSP_PLUGINS_MB_DYNA_PROCESSOR_URL_RW    := git@github.com:lsp-plugins/$(LSP_PLUGINS_MB_DYNA_PROCESSOR_NAME).git

LSP_PLUGINS_MB_EXPANDER_VERSION         := 1.0.19
LSP_PLUGINS_MB_EXPANDER_NAME            := lsp-plugins-mb-expander
LSP_PLUGINS_MB_EXPANDER_TYPE            := plug
LSP_PLUGINS_MB_EXPANDER_URL_RO          := https://github.com/lsp-plugins/$(LSP_PLUGINS_MB_EXPANDER_NAME).git
LSP_PLUGINS_MB_EXPANDER_URL_RW          := git@github.com:lsp-plugins/$(LSP_PLUGINS_MB_EXPANDER_NAME).git

LSP_PLUGINS_MB_GATE_VERSION             := 1.0.19
LSP_PLUGINS_MB_GATE_NAME                := lsp-plugins-mb-gate
LSP_PLUGINS_MB_GATE_TYPE                := plug
LSP_PLUGINS_MB_GATE_URL_RO              := https://github.com/lsp-plugins/$(LSP_PLUGINS_MB_GATE_NAME).git
LSP_PLUGINS_MB_GATE_URL_RW              := git@github.com:lsp-plugins/$(LSP_PLUGINS_MB_GATE_NAME).git

LSP_PLUGINS_MB_LIMITER_VERSION          := 1.0.8
LSP_PLUGINS_MB_LIMITER_NAME             := lsp-plugins-mb-limiter
LSP_PLUGINS_MB_LIMITER_TYPE             := plug
LSP_PLUGINS_MB_LIMITER_URL_RO           := https://github.com/lsp-plugins/$(LSP_PLUGINS_MB_LIMITER_NAME).git
LSP_PLUGINS_MB_LIMITER_URL_RW           := git@github.com:lsp-plugins/$(LSP_PLUGINS_MB_LIMITER_NAME).git

LSP_PLUGINS_MIXER_VERSION             	:= 1.0.11
LSP_PLUGINS_MIXER_NAME                	:= lsp-plugins-mixer
LSP_PLUGINS_MIXER_TYPE                	:= plug
LSP_PLUGINS_MIXER_URL_RO              	:= https://github.com/lsp-plugins/$(LSP_PLUGINS_MIXER_NAME).git
LSP_PLUGINS_MIXER_URL_RW              	:= git@github.com:lsp-plugins/$(LSP_PLUGINS_MIXER_NAME).git

LSP_PLUGINS_NOISE_GENERATOR_VERSION     := 1.0.14
LSP_PLUGINS_NOISE_GENERATOR_NAME        := lsp-plugins-noise-generator
LSP_PLUGINS_NOISE_GENERATOR_TYPE        := plug
LSP_PLUGINS_NOISE_GENERATOR_URL_RO      := https://github.com/lsp-plugins/$(LSP_PLUGINS_NOISE_GENERATOR_NAME).git
LSP_PLUGINS_NOISE_GENERATOR_URL_RW      := git@github.com:lsp-plugins/$(LSP_PLUGINS_NOISE_GENERATOR_NAME).git

LSP_PLUGINS_OSCILLATOR_VERSION          := 1.0.19
LSP_PLUGINS_OSCILLATOR_NAME             := lsp-plugins-oscillator
LSP_PLUGINS_OSCILLATOR_TYPE             := plug
LSP_PLUGINS_OSCILLATOR_URL_RO           := https://github.com/lsp-plugins/$(LSP_PLUGINS_OSCILLATOR_NAME).git
LSP_PLUGINS_OSCILLATOR_URL_RW           := git@github.com:lsp-plugins/$(LSP_PLUGINS_OSCILLATOR_NAME).git

LSP_PLUGINS_OSCILLOSCOPE_VERSION        := 1.0.19
LSP_PLUGINS_OSCILLOSCOPE_NAME           := lsp-plugins-oscilloscope
LSP_PLUGINS_OSCILLOSCOPE_TYPE           := plug
LSP_PLUGINS_OSCILLOSCOPE_URL_RO         := https://github.com/lsp-plugins/$(LSP_PLUGINS_OSCILLOSCOPE_NAME).git
LSP_PLUGINS_OSCILLOSCOPE_URL_RW         := git@github.com:lsp-plugins/$(LSP_PLUGINS_OSCILLOSCOPE_NAME).git

LSP_PLUGINS_PARA_EQUALIZER_VERSION      := 1.0.24
LSP_PLUGINS_PARA_EQUALIZER_NAME         := lsp-plugins-para-equalizer
LSP_PLUGINS_PARA_EQUALIZER_TYPE         := plug
LSP_PLUGINS_PARA_EQUALIZER_URL_RO       := https://github.com/lsp-plugins/$(LSP_PLUGINS_PARA_EQUALIZER_NAME).git
LSP_PLUGINS_PARA_EQUALIZER_URL_RW       := git@github.com:lsp-plugins/$(LSP_PLUGINS_PARA_EQUALIZER_NAME).git

LSP_PLUGINS_PHASE_DETECTOR_VERSION      := 1.0.19
LSP_PLUGINS_PHASE_DETECTOR_NAME         := lsp-plugins-phase-detector
LSP_PLUGINS_PHASE_DETECTOR_TYPE         := plug
LSP_PLUGINS_PHASE_DETECTOR_URL_RO       := https://github.com/lsp-plugins/$(LSP_PLUGINS_PHASE_DETECTOR_NAME).git
LSP_PLUGINS_PHASE_DETECTOR_URL_RW       := git@github.com:lsp-plugins/$(LSP_PLUGINS_PHASE_DETECTOR_NAME).git

LSP_PLUGINS_PROFILER_VERSION            := 1.0.19
LSP_PLUGINS_PROFILER_NAME               := lsp-plugins-profiler
LSP_PLUGINS_PROFILER_TYPE               := plug
LSP_PLUGINS_PROFILER_URL_RO             := https://github.com/lsp-plugins/$(LSP_PLUGINS_PROFILER_NAME).git
LSP_PLUGINS_PROFILER_URL_RW             := git@github.com:lsp-plugins/$(LSP_PLUGINS_PROFILER_NAME).git

LSP_PLUGINS_ROOM_BUILDER_VERSION        := 1.0.20
LSP_PLUGINS_ROOM_BUILDER_NAME           := lsp-plugins-room-builder
LSP_PLUGINS_ROOM_BUILDER_TYPE           := plug
LSP_PLUGINS_ROOM_BUILDER_URL_RO         := https://github.com/lsp-plugins/$(LSP_PLUGINS_ROOM_BUILDER_NAME).git
LSP_PLUGINS_ROOM_BUILDER_URL_RW         := git@github.com:lsp-plugins/$(LSP_PLUGINS_ROOM_BUILDER_NAME).git

LSP_PLUGINS_SAMPLER_VERSION             := 1.0.23
LSP_PLUGINS_SAMPLER_NAME                := lsp-plugins-sampler
LSP_PLUGINS_SAMPLER_TYPE                := plug
LSP_PLUGINS_SAMPLER_URL_RO              := https://github.com/lsp-plugins/$(LSP_PLUGINS_SAMPLER_NAME).git
LSP_PLUGINS_SAMPLER_URL_RW              := git@github.com:lsp-plugins/$(LSP_PLUGINS_SAMPLER_NAME).git

LSP_PLUGINS_SLAP_DELAY_VERSION          := 1.0.20
LSP_PLUGINS_SLAP_DELAY_NAME             := lsp-plugins-slap-delay
LSP_PLUGINS_SLAP_DELAY_TYPE             := plug
LSP_PLUGINS_SLAP_DELAY_URL_RO           := https://github.com/lsp-plugins/$(LSP_PLUGINS_SLAP_DELAY_NAME).git
LSP_PLUGINS_SLAP_DELAY_URL_RW           := git@github.com:lsp-plugins/$(LSP_PLUGINS_SLAP_DELAY_NAME).git

LSP_PLUGINS_SPECTRUM_ANALYZER_VERSION   := 1.0.25
LSP_PLUGINS_SPECTRUM_ANALYZER_NAME      := lsp-plugins-spectrum-analyzer
LSP_PLUGINS_SPECTRUM_ANALYZER_TYPE      := plug
LSP_PLUGINS_SPECTRUM_ANALYZER_URL_RO    := https://github.com/lsp-plugins/$(LSP_PLUGINS_SPECTRUM_ANALYZER_NAME).git
LSP_PLUGINS_SPECTRUM_ANALYZER_URL_RW    := git@github.com:lsp-plugins/$(LSP_PLUGINS_SPECTRUM_ANALYZER_NAME).git

LSP_PLUGINS_SURGE_FILTER_VERSION        := 1.0.19
LSP_PLUGINS_SURGE_FILTER_NAME           := lsp-plugins-surge-filter
LSP_PLUGINS_SURGE_FILTER_TYPE           := plug
LSP_PLUGINS_SURGE_FILTER_URL_RO         := https://github.com/lsp-plugins/$(LSP_PLUGINS_SURGE_FILTER_NAME).git
LSP_PLUGINS_SURGE_FILTER_URL_RW         := git@github.com:lsp-plugins/$(LSP_PLUGINS_SURGE_FILTER_NAME).git

LSP_PLUGINS_TRIGGER_VERSION             := 1.0.21
LSP_PLUGINS_TRIGGER_NAME                := lsp-plugins-trigger
LSP_PLUGINS_TRIGGER_TYPE                := plug
LSP_PLUGINS_TRIGGER_URL_RO              := https://github.com/lsp-plugins/$(LSP_PLUGINS_TRIGGER_NAME).git
LSP_PLUGINS_TRIGGER_URL_RW              := git@github.com:lsp-plugins/$(LSP_PLUGINS_TRIGGER_NAME).git

# System libraries
LIBADVAPI_VERSION          := system
LIBADVAPI_NAME             := libadvapi32
LIBADVAPI_TYPE             := opt
LIBADVAPI_LDFLAGS          := -ladvapi32

LIBCAIRO_VERSION           := system
LIBCAIRO_NAME              := cairo
LIBCAIRO_TYPE              := pkg

LIBDL_VERSION              := system
LIBDL_NAME                 := libdl
LIBDL_TYPE                 := opt
LIBDL_LDFLAGS              := -ldl

LIBD2D1_VERSION            := system
LIBD2D1_NAME               := libd2d1
LIBD2D1_TYPE               := opt
LIBD2D1_LDFLAGS            := -ld2d1

LIBDWRITE_VERSION          := system
LIBDWRITE_NAME             := libdwrite
LIBDWRITE_TYPE             := opt
LIBDWRITE_LDFLAGS          := -ldwrite

LIBFREETYPE_VERSION        := system
LIBFREETYPE_NAME           := freetype2
LIBFREETYPE_TYPE           := pkg

LIBGDI32_VERSION           := system
LIBGDI32_NAME              := libgid32
LIBGDI32_TYPE              := opt
LIBGDI32_LDFLAGS           := -lgdi32

LIBGL_VERSION              := system
LIBGL_NAME                 := gl
LIBGL_TYPE                 := pkg

LIBGSTREAMER_AUDIO_VERSION := system
LIBGSTREAMER_AUDIO_NAME    := gstreamer-audio-1.0
LIBGSTREAMER_AUDIO_TYPE    := pkg

LIBICONV_VERSION           := system
LIBICONV_NAME              := libiconv
LIBICONV_TYPE              := opt
LIBICONV_LDFLAGS           := -liconv

LIBMPR_VERSION             := system
LIBMPR_NAME                := libmpr
LIBMPR_TYPE                := opt
LIBMPR_LDFLAGS             := -lmpr

LIBJACK_VERSION            := system
LIBJACK_NAME               := jack
LIBJACK_TYPE               := pkg

LIBMSACM_VERSION           := system
LIBMSACM_NAME              := libmsacm
LIBMSACM_TYPE              := opt
LIBMSACM_LDFLAGS           := -lmsacm32

LIBOLE_VERSION             := system
LIBOLE_NAME                := libole
LIBOLE_TYPE                := opt
LIBOLE_LDFLAGS             := -lole32

LIBOPENGL32_VERSION        := system
LIBOPENGL32_NAME           := libole
LIBOPENGL32_TYPE           := opt
LIBOPENGL32_LDFLAGS        := -lopengl32

LIBPTHREAD_VERSION         := system
LIBPTHREAD_NAME            := libpthread
LIBPTHREAD_TYPE            := opt
LIBPTHREAD_LDFLAGS         := -lpthread

LIBRT_VERSION              := system
LIBRT_NAME                 := librt
LIBRT_TYPE                 := opt
LIBRT_LDFLAGS              := -lrt

LIBSNDFILE_VERSION         := system
LIBSNDFILE_NAME            := sndfile
LIBSNDFILE_TYPE            := pkg

LIBSHLWAPI_VERSION         := system
LIBSHLWAPI_NAME            := libshlwapi
LIBSHLWAPI_TYPE            := opt
LIBSHLWAPI_LDFLAGS         := -lshlwapi

LIBUUID_VERSION            := system
LIBUUID_NAME               := libuuid
LIBUUID_TYPE               := opt
LIBUUID_LDFLAGS            := -luuid

LIBWINCODEC_VERSION        := system
LIBWINCODEC_NAME           := libwincodec
LIBWINCODEC_TYPE           := opt
LIBWINCODEC_LDFLAGS        := -lwindowscodecs

LIBWINMM_VERSION           := system
LIBWINMM_NAME              := libwinmm
LIBWINMM_TYPE              := opt
LIBWINMM_LDFLAGS           := -lwinmm

LIBX11_VERSION             := system
LIBX11_NAME                := x11
LIBX11_TYPE                := pkg

LIBXRANDR_VERSION          := system
LIBXRANDR_NAME             := xrandr
LIBXRANDR_TYPE             := pkg


