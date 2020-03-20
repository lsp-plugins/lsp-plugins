/*
  Copyright 2017 Linux Studio Plugins Project <lsp.plugin@gmail.com>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef _3RDPARTY_STEINBERG_VST2_H_
#define _3RDPARTY_STEINBERG_VST2_H_

#include <string.h>
#include <stdint.h>
#include <limits.h>

//-------------------------------------------------------------------------------------------------------
// Configure compilation options
//-------------------------------------------------------------------------------------------------------
#if ((defined(__GNUC__) && (defined(__APPLE_CPP__) || defined(__APPLE_CC__))) || (defined (__MWERKS__) && defined (__MACH__)))
    #ifndef TARGET_API_MAC_CARBON
        #define TARGET_API_MAC_CARBON 1
    #endif
    #if __ppc__
        #ifndef VST_FORCE_DEPRECATED
            #define VST_FORCE_DEPRECATED 0
        #endif
    #endif
#endif

// Define __cdecl modifier
#ifdef __GNUC__
    #ifndef __cdecl
        #if defined(__i386__) || defined(__i386)
            #define __cdecl __attribute__((__cdecl__))
        #elif defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_AMD64)
            #define VST_64BIT_PLATFORM      1
            #define __cdecl
        #elif defined(__aarch64__)
            #define VST_64BIT_PLATFORM      1
            #define __cdecl
        #elif defined(__arm__) || defined(__arm) || defined(_M_ARM) || defined(_ARM)
            #define __cdecl
        #elif defined(__PPC64__) || defined(__ppc64__) || defined(__ppc64) || defined(__powerpc64__) || defined(_ARCH_PPC64)
            #define VST_64BIT_PLATFORM      1
            #define __cdecl
        #elif defined(__PPC__) || defined(__ppc__) || defined(__powerpc__) || defined(__ppc) || defined(_M_PPC) || defined(_ARCH_PPC)
            #define __cdecl
        #elif defined(__s390x__) || defined(__s390__) || defined(__zarch__)
            #define VST_64BIT_PLATFORM      1
            #define __cdecl
        #elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
            #define __cdecl
        #endif /* __cdecl */
    #endif /* __cdecl */
#endif /* __GNUC__ */

#ifdef __cplusplus
    #define VST_C_EXTERN        extern "C"
#else
    #define VST_C_EXTERN
#endif /* __cplusplus */

/** Test whether system runs in 64-bit mode */
#ifdef __GNUC__
    #ifndef VST_64BIT_PLATFORM
        #if defined(__WORDSIZE) && (__WORDSIZE == 64)
            #define VST_64BIT_PLATFORM      1
        #elif defined(__SIZE_WIDTH__) && (__SIZE_WIDTH__ == 64)
            #define VST_64BIT_PLATFORM      1
        #endif /* __WORDSIZE, __SIZE_WIDTH__ */
    #endif

    #ifndef VST_64BIT_PLATFORM
        #define VST_64BIT_PLATFORM  (__x86_64__) || (__aarch64__) || (__ppc64__) || (__s390x__) || (__zarch__)
    #endif /* VST_64BIT_PLATFORM */
#else
    #ifndef VST_64BIT_PLATFORM
        #define VST_64BIT_PLATFORM _WIN64 || __LP64__
    #endif /* VST_64BIT_PLATFORM */
#endif /* __GNUC__ */

#if TARGET_API_MAC_CARBON
    #ifdef (__LP64__) || (__ppc64__)
        #pragma options align=power
    #else
        #pragma options align=mac68k
    #endif
    #define VSTCALLBACK
#elif defined __BORLANDC__
    #pragma -a8
#elif defined(__GNUC__)
    #pragma pack(push, 8)
    #define VSTCALLBACK __cdecl
#elif defined(WIN32) || defined(__FLAT__) || defined CBUILDER
    #pragma pack(push)
    #pragma pack(8)
    #define VSTCALLBACK __cdecl
#else
    #define VSTCALLBACK
#endif

#if defined (__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define VST_EXPORT  __attribute__ ((visibility ("default")))
#else
    #define VST_EXPORT
#endif

#define VST_2_1_EXTENSIONS 1
#define VST_2_2_EXTENSIONS 1
#define VST_2_3_EXTENSIONS 1
#define VST_2_4_EXTENSIONS 1
#define DECLARE_VST_DEPRECATED(x)       x

//-------------------------------------------------------------------------------------------------------
// Simple type definitions
//-------------------------------------------------------------------------------------------------------
/**
 * Integral data types
 */
typedef int16_t VstInt16;
typedef int32_t VstInt32;
typedef int64_t VstInt64;
#if VST_64BIT_PLATFORM
    typedef VstInt64 VstIntPtr;
#else
    typedef VstInt32 VstIntPtr;
#endif

/**
 *  Magic number encoding macro
 */
#ifdef CCONST
    #undef CCONST
#endif /* CCONST */
#define CCONST(a, b, c, d) \
     ((((VstInt32)a) << 24) | (((VstInt32)b) << 16) | (((VstInt32)c) << 8) | (((VstInt32)d) << 0))


//-------------------------------------------------------------------------------------------------------
// Constants definition
//-------------------------------------------------------------------------------------------------------
/**
 * The VST interface version, current is 2.4.0
 */
#define kVstVersion 2400

/**
 * Magic numbers
 */
#define kEffectMagic        CCONST('V', 's', 't', 'P')      /* AEffect magic number */
#define kEffectIdentify     CCONST('N', 'v', 'E', 'f')      /* Audio effect identification */
#define cMagic              CCONST('C', 'c', 'n', 'K')      /* Root chunk magic number for programs (FXP) and banks (FXB) */
#define fMagic              CCONST('F', 'x', 'C', 'k')      /* Regular program chunk magic number (FXP) */
#define bankMagic           CCONST('F', 'x', 'B', 'k')      /* Regular program bank magic number (FXB) */
#define chunkPresetMagic    CCONST('F', 'P', 'C', 'h')      /* Opaque chunk (preset) magic number for program chunk (FXP) */
#define chunkBankMagic      CCONST('F', 'B', 'C', 'h')      /* Opaque chunk (preset) magic number for bank chunk (FXB) */

/**
 * Host can do the following
 */
#define canDoSendVstEvents                  "sendVstEvents"                     /* Host supports send of Vst events to plug-in */
#define canDoSendVstMidiEvent               "sendVstMidiEvent"                  /* Host supports send of Vst events to plug-in */
#define canDoSendVstTimeInfo                "sendVstTimeInfo"                   /* Host supports send of VstTimeInfo to plug-in */
#define canDoReceiveVstEvents               "receiveVstEvents"                  /* Host can receive Vst events from plug-in */
#define canDoReceiveVstMidiEvent            "receiveVstMidiEvent"               /* Host can receive MIDI events from plug-in */
#define canDoReportConnectionChanges        "reportConnectionChanges"           /* Host will indicates the plug-in when something change in plug-in´s routing/connections with suspend/resume/setSpeakerArrangement */
#define canDoAcceptIOChanges                "acceptIOChanges"                   /* Host supports ioChanged () */
#define canDoSizeWindow                     "sizeWindow"                        /* used by VSTGUI */
#define canDoOffline                        "offline"                           /* Host supports offline feature */
#define canDoOpenFileSelector               "openFileSelector"                  /* Host supports function openFileSelector () */
#define canDoCloseFileSelector              "closeFileSelector"                 /* Host supports function closeFileSelector () */
#define canDoStartStopProcess               "startStopProcess"                  /* Host supports functions startProcess () and stopProcess () */
#define canDoShellCategory                  "shellCategory"                     /* 'shell' handling via uniqueID. If supported by the Host and the Plug-in has the category kPlugCategShell */
#define canDoSendVstMidiEventFlagIsRealtime "sendVstMidiEventFlagIsRealtime"    /* Host supports flags for VstMidiEvent */

/**
 * Plugin can do the following
 */
#define canDoSendVstEvents                  "sendVstEvents"                     /* plug-in will send Vst events to Host */
#define canDoSendVstMidiEvent               "sendVstMidiEvent"                  /* plug-in will send MIDI events to Host */
#define canDoReceiveVstEvents               "receiveVstEvents"                  /* plug-in can receive MIDI events from Host */
#define canDoReceiveVstMidiEvent            "receiveVstMidiEvent"               /* plug-in can receive MIDI events from Host  */
#define canDoReceiveVstTimeInfo             "receiveVstTimeInfo"                /* plug-in can receive Time info from Host  */
#define canDoOffline                        "offline"                           /* plug-in supports offline functions (offlineNotify, offlinePrepare, offlineRun) */
#define canDoMidiProgramNames               "midiProgramNames"                  /* plug-in supports function getMidiProgramName () */
#define canDoBypass                         "bypass"                            /* plug-in supports function setBypass () */


enum VstAEffectFlags
{
    /** Plugin provides custom user interface/editor
     *
     */
    effFlagsHasEditor           = 1 << 0, //!< effFlagsHasEditor

    /** This flag is deprecated
     * @deprecated since VST 2.4
     */
    effFlagsHasClip             = 1 << 1, //!< effFlagsHasClip

    /** This flag is deprecated
     * @deprecated since VST 2.4
     */
    effFlagsHasVu               = 1 << 2, //!< effFlagsHasVu

    /** This flag is deprecated
     * @deprecated since VST 2.4
     */
    effFlagsCanMono             = 1 << 3, //!< effFlagsCanMono

    /** Supports replacing process mode, default mode for VST 2.4
     *
     */
    effFlagsCanReplacing        = 1 << 4, //!< effFlagsCanReplacing

    /** Program data is handled in formatless chunks
     *
     */
    effFlagsProgramChunks       = 1 << 5, //!< effFlagsProgramChunks

    /** Plug-in is a synthesizer/instrument (VSTi),
     * host may assign mixer channels for its outputs
     *
     */
    effFlagsIsSynth             = 1 << 8, //!< effFlagsIsSynth

    /** Plug-in does not produce sound when there is zero data on all inputs
     *
     */
    effFlagsNoSoundInStop       = 1 << 9, //!< effFlagsNoSoundInStop

    /** This flag is deprecated
     * @deprecated since VST 2.4
     */
    effFlagsExtIsAsync          = 1 << 10,//!< effFlagsExtIsAsync

    /** This flag is deprecated
     * @deprecated since VST 2.4
     */
    effFlagsExtHasBuffer        = 1 << 11,//!< effFlagsExtHasBuffer

    /** Plug-in supports double precision processing
     * @since VST 2.4
     */
    effFlagsCanDoubleReplacing  = 1 << 12 //!< effFlagsCanDoubleReplacing
};

/** Host to plugin communication codes
 * @see AEffectDispatcherProc
 *
 */
enum AEffectOpcodes
{
    /** The plugin initialization call
     *
     */
    effOpen = 0,

    /** The plugin destruction call
     *
     */
    effClose,

    /** Set the current program
     * @param value - new program number
     */
    effSetProgram,

    /** Get the current program
     * @return current program number
     */
    effGetProgram,

    /** Set current program name, maximum string length is kVstMaxProgNameLen
     * Please be aware that the string lengths supported by the default VST interface
     * are normally limited to kVstMaxProgNameLen characters. If you copy too much data
     * into the buffers provided, you will break the Host application.
     *
     * @param ptr pointer to string with new program name
     * @see kVstMaxProgNameLen
     */
    effSetProgramName,

    /** Get current program name, maximum string length is kVstMaxProgNameLen characters
     * Please be aware that the string lengths supported by the default VST interface
     * are normally limited to kVstMaxProgNameLen characters. If you copy too much data
     * into the buffers provided, you will break the Host application.
     *
     * @param ptr pointer to buffer to store string with current program name
     * @see kVstMaxProgNameLen
     */
    effGetProgramName,

    /** Get the label for the parameter displayed by host in the interface
     * The length of the label is limited to kVstMaxParamStrLen characters
     * @param ptr pointer to buffer to store label
     * @see kVstMaxParamStrLen
     */
    effGetParamLabel,

    /** Get the parameter display string
     * The length of the passed buffer is limited to kVstMaxParamStrLen characters
     * @param ptr pointer to buffer to store parameter string
     * @see kVstMaxParamStrLen
     */
    effGetParamDisplay,

    /** Get the parameter name used/displayed by host
     * The length of parameter name is limited to kVstMaxParamStrLen characters
     * @param ptr pointer to buffer to store parameter name string
     * @see kVstMaxParamStrLen
     */
    effGetParamName,

    /** This is deprecated command
     * @deprecated since VST 2.4
     *
     */
    effGetVu,

    /** Change the audio processing sample rate
     * @param opt new sample rate value
     *
     */
    effSetSampleRate,

    /** Set the maximum block size that host can pass to plugin while
     * performing audio processing.
     * @param value new maximum block size for audio processing
     */
    effSetBlockSize,

    /** The plugin is suspended/resumed
     * @param value suspend/resume flag: 0 means "turn off", 1 means "turn on"
     *
     */
    effMainsChanged,

    /** Get custom editor area size
     * @param ptr double pointer to ERect structure (ERect **) that contains editor area parameters
     */
    effEditGetRect,

    /** Activate (show) the custom editor provided by the plugin
     * @param ptr platform-specific Window handle (HWND for Windows, XID for X11, etc.)
     *
     */
    effEditOpen,

    /** Deactivate (hide) the custom editor provided by the plugin
     *
     */
    effEditClose,

    /** This is deprecated command
     * @deprecated since VST 2.4
     *
     */
    effEditDraw,

    /** This is deprecated command
     * @deprecated since VST 2.4
     *
     */
    effEditMouse,

    /** This is deprecated command
     * @deprecated since VST 2.4
     *
     */
    effEditKey,

    /** Called by the host for the custom UI editor
     * to process events from windowing subsystem
     */
    effEditIdle,

    /** This is deprecated command
     * @deprecated since VST 2.4
     *
     */
    effEditTop,

    /** This is deprecated command
     * @deprecated since VST 2.4
     *
     */
    effEditSleep,

    /** This is deprecated command, should return kEffectIdentify ('NvEf')
     * @see kEffectIdentify
     * @deprecated since VST 2.4
     *
     */
    effIdentify,

    /** Get program chunk
     *  If your plug-in is configured to use chunks (see AudioEffect::programsAreChunks),
     *  the Host will ask for a block of memory describing the current plug-in state for saving.
     *  To restore the state at a later stage, the same data is passed back by effSetChunk.
     *  Alternatively, when not using chunk, the Host will simply save all parameter values.
     *
     * @param ptr double pointer to void (void **) for obtaining chunk data address
     * @param index type of the chunk: 0 for bank, 1 for program
     */
    effGetChunk,

    /** Set program chunk
     * @param ptr pointer to chunk data
     * @param value the size in bytes
     * @param index type of the chunk: 0 for bank, 1 for program
     */
    effSetChunk,

    /** Process events passed to the host
     * @param ptr pointer to VstEvents structure
     * @see VstEvents
     */
    effProcessEvents,

    /** Check whether parameter can be automated
     * @param index parameter index
     * @return 1 if parameter can be automated, 0 otherwise
     */
    effCanBeAutomated,

    /** Convert a string representation to a parameter value
     * @param index parameter index
     * @param ptr parameter string
     * @return 1 if success, 0 otherwise
     */
    effString2Parameter,

    /**
     * @deprecated since VST 2.4
     */
    effGetNumProgramCategories,

    /** Fill text with name of program index (category deprecated in VST 2.4)
     *
     * @param index program index
     * @param ptr buffer for program name limited to kVstMaxProgNameLen
     * @return true for success
     * @see kVstMaxProgNameLen
     */
    effGetProgramNameIndexed,

    /**
     * @deprecated since VST 2.4
     */
    effCopyProgram,

    /**
     * @deprecated since VST 2.4
     */
    effConnectInput,

    /**
     * @deprecated since VST 2.4
     */
    effConnectOutput,

    /** Return the properties of input index
     * @param index input index
     * @param ptr pointer to VstPinProperties structure
     * @return 1 if supported
     */
    effGetInputProperties,

    /** Return the properties of output index
     * @param index input index
     * @param ptr pointer to VstPinProperties structure
     * @return 1 if supported
     */
    effGetOutputProperties,

    /** Get plugin category
     * @return plugin category (one of VstPlugCategory enum constants)
     * @see VstPlugCategory
     */
    effGetPlugCategory,

    /**
     * @deprecated since VST 2.4
     */
    effGetCurrentPosition,

    /**
     * @deprecated since VST 2.4
     */
    effGetDestinationBuffer,

    /**
     * @param ptr pointer to array of VstAudioFile structures
     * @param value count of elements in array
     * @param index start flag
     */
    effOfflineNotify,

    /**
     * @param ptr pointer to array of VstOfflineTask structures
     * @param value count of elements in array
     * @see VstOfflineTask
     */
    effOfflinePrepare,

    /**
     * @param ptr pointer to array of VstOfflineTask structures
     * @param value count of elements in array
     * @see VstOfflineTask
     */
    effOfflineRun,

    /** Used for variable I/O processing (offline processing like timestreching)
     * @param ptr pointer to VstVariableIo structure
     */
    effProcessVarIo,

    /** Set the plug-in's speaker arrangements
     * @param value pointer to VstSpeakerArrangements
     * @param ptr output pointer to VstSpeakerArrangement
     */
    effSetSpeakerArrangement,

    /**
     * @deprecated since VST 2.4
     */
    effSetBlockSizeAndSampleRate,

    /** For 'soft-bypass' (this could be automated (in Audio Thread)
     * that why you could NOT call iochanged (if needed) in this function,
     * do it in fxidle).
     *
     * @param value 1 = bypass, 0 = no bypass
     */
    effSetBypass,

    /** Get effect name
     * @param ptr buffer to store effect name, maximum length is kVstMaxEffectNameLen characters
     * @see kVstMaxEffectNameLen
     */
    effGetEffectName,

    /**
     * @deprecated since VST 2.4
     */
    effGetErrorText,

    /** Get plugin vendor string
     * @param ptr pointer to buffer to store vendor string, maximum length is kVstMaxVendorStrLen characters
     * @see kVstMaxVendorStrLen
     */
    effGetVendorString,

    /** Get plugin product string
     * @param ptr pointer to buffer to store product string, maximum length is kVstMaxProductStrLen characters
     * @see kVstMaxVendorStrLen
     */
    effGetProductString,

    /** Get plugin vendor-specific version
     * @return vendor-specific version
     */
    effGetVendorVersion,

    /** No special definition, vendor specific handling
     *
     */
    effVendorSpecific,

    /** Check whether plugin supports a feature
     * @param ptr the pointer to string with supported feature identifier
     * @return 0: "don't know" -1: "no" 1: "yes"
     */
    effCanDo,

    /** Get post-processing tail size of plugin (for example the reverb time of a reverb plug-in)
     * @return tail size, 0 is default, 1 if there is no tail
     */
    effGetTailSize,

    /**
     * @deprecated since VST 2.4
     */
    effIdle,

    /**
     * @deprecated since VST 2.4
     */
    effGetIcon,

    /**
     * @deprecated since VST 2.4
     */
    effSetViewPosition,

    /** Get plugin parameter properties
     * @param index parameter index
     * @param ptr pointer to VstParameterProperties structure
     * @return 1 if supported
     */
    effGetParameterProperties,

    /**
     * @deprecated since VST 2.4
     */
    effKeysRequired,

    /** Return the VST interface version used by plugin
     * @return VST interface version
     * @see kVstVersion
     */
    effGetVstVersion,

    /** Receive key down event. Return true only if key was really used!
     * @param index ASCII character
     * @param value virtual key
     * @param opt key modifiers
     * @return 1 if key was used, 0 otherwise
     * @since VST 2.1
     */
    effEditKeyDown,

    /** Receive key up event. Return true only if key was really used!
     * @param index ASCII character
     * @param value virtual key
     * @param opt key modifiers
     * @return 1 if key was used, 0 otherwise
     * @since VST 2.1
     */
    effEditKeyUp,

    /** Set knob mode (if supported by Host)
     * @param value knob mode: 0 = circular, 1 = circular relative, 2 = linear
     * @return 1 if key was used, 0 otherwise
     * @since VST 2.1
     */
    effSetEditKnobMode,

    /** Fill midiProgramName with information for 'thisProgramIndex'.
     * @param index MIDI channel
     * @param ptr pointer to MidiProgramName structure
     * @return number of used programs, 0 if unsupported
     * @since VST 2.1
     */
    effGetMidiProgramName,

    /** Fill currentProgram with information for the current MIDI program.
     * @param index MIDI channel
     * @param ptr pointer to MidiProgramName structure
     * @return index of current program
     * @since VST 2.1
     */
    effGetCurrentMidiProgram,

    /** Fill category with information for 'thisCategoryIndex'.
     * @param index MIDI channel
     * @param ptr pointer to MidiProgramCategory structure
     * @return number of used categories, 0 if unsupported
     * @since VST 2.1
     */
    effGetMidiProgramCategory,

    /** Return true if the MidiProgramNames, MidiKeyNames or MidiControllerNames had changed on this MIDI channel.
     * @param index MIDI channel
     * @return 1 if the at least one program name or key name has been changed
     * @see MidiProgramNames
     * @see MidiKeyNames
     * @see MidiControllerNames
     * @since VST 2.1
     */
    effHasMidiProgramsChanged,

    /** Fill keyName with information for 'thisProgramIndex' and 'thisKeyNumber'
     * @param index MIDI channel
     * @param ptr pointer to MidiKeyName structure
     * @return 1 if supported, 0 if not
     * @see MidiKeyName
     * @since VST 2.1
     */
    effGetMidiKeyName,

    /** Called before a program is loaded. No arguments
     * @since VST 2.1
     */
    effBeginSetProgram,

    /** Called after a program was loaded. No arguments
     * @since VST 2.1
     */
    effEndSetProgram,

    /** Set the plug-in's speaker arrangements
     * @param value pointer to VstSpeakerArrangement structure
     * @param ptr pointer to output VstSpeakerArrangement structure
     * @since VST 2.3
     */
    effGetSpeakerArrangement,

    /** This opcode is only called, if the plug-in is of type
     * kPlugCategShell, in order to extract all included sub-plugin's names.
     * @param ptr buffer for plug-in name, limited to kVstMaxProductStrLen
     * @return next plugin's uniqueID
     * @see kPlugCategShell
     * @see kVstMaxProductStrLen
     * @since VST 2.3
     */
    effShellGetNextPlugin,

    /** Called one time before the start of process call. This indicates that
     * the process call will be interrupted (due to Host reconfiguration or
     * bypass state when the plug-in doesn't support softBypass)
     * No arguments
     * @since VST 2.3
     */
    effStartProcess,

    /** Called after the stop of process call
     * No arguments
     * @since VST 2.3
     */
    effStopProcess,

    /** Called in offline mode before process() or processVariableIo ()
     * @param value number of samples to process, offline mode only!
     * @since VST 2.3
     */
    effSetTotalSampleToProcess,

    /** Set the Panning Law used by the Host.
     * @param value pan law (one of the VstPanLawType enumeration constants)
     * @param opt gain
     * @see VstPanLawType
     * @since VST 2.3
     */
    effSetPanLaw,

    /** Called before a Bank is loaded.
     * @param ptr pointer to VstPatchChunkInfo structure
     * @return -1 if ban can not be loaded, 1 if bank can be loaded, 0 if function is not supported
     * @see VstPatchChunkInfo
     * @since VST 2.3
     */
    effBeginLoadBank,

    /** Called before a Program is loaded, called before beginSetProgram.
     * @param ptr pointer to VstPatchChunkInfo structure
     * @return -1 if program can not be loaded, 1 if program can be loaded, 0 if not supported
     * @see effBeginSetProgram
     * @see VstPatchChunkInfo
     * @since VST 2.3
     */
    effBeginLoadProgram,

    /** Set floating-point precision used for processing (32 or 64 bit)
     * @value constant defined in VstProcessPrecision enumeration
     * @see VstProcessPrecision
     * @since VST 2.4
     */
    effSetProcessPrecision,

    /** Returns number of MIDI input channels used [0, 16]
     * @return number of used MIDI input channels
     * @since VST 2.4
     */
    effGetNumMidiInputChannels,

    /** Returns number of MIDI output channels used [0, 16]
     * @return number of used MIDI input channels
     * @since VST 2.4
     */
    effGetNumMidiOutputChannels
};

/** Plugin to host communication codes
 *
 */
enum AudioMasterOpcodes
{
    /**
     * An important thing to notice is that if the user changes a parameter in your editor, which is
     * out of the Host's control if you are not using the default string based interface, you should
     * issue audioMasterAutomate callback. This ensures that the Host is notified of the parameter
     * change, which allows it to record these changes for automation.
     * @param index parameter index
     * @param opt parameter value
     *
     */
    audioMasterAutomate = 0,

    /** Get host VST version
     * @return Host VST version (for example 2400 for VST 2.4)
     */
    audioMasterVersion,

    /** Get unique identifier for shell plugin
     * @return current uinque identifier on shell plugin
     */
    audioMasterCurrentId,

    /**
     *
     */
    audioMasterIdle,

    /** Pin connection flag
     * @deprecated since VST 2.4 r2
     *
     */
    audioMasterPinConnected,

    /** Just padding
     *
     */
    __audioMasterPad,

    /** Want MIDI flag
     * @deprecated since VST 2.4
     */
    audioMasterWantMidi,

    /** Get time
     * @param value request mask
     * @return the pointer to VstTimeInfo or NULL if not supported
     * @see VstTimeInfo
     * @see VstTimeInfoFlags
     *
     */
    audioMasterGetTime,

    /** Process events delivered to plugin
     * @param ptr pointer to events structure
     * @see VstEvents
     *
     */
    audioMasterProcessEvents,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterSetTime,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterTempoAt,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterGetNumAutomatableParameters,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterGetParameterQuantization,

    /** Called when the IO of plugin has changed or a latency value has been updated
     * @return 1 if supported
     *
     */
    audioMasterIOChanged,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterNeedIdle,

    /** Called by plugin when it's custom UI editor window should be resized
     *
     * @param index new window width
     * @param value new window height
     * @return 1 if supported
     */
    audioMasterSizeWindow,

    /** Get current sample rate
     * @return current sample rate
     */
    audioMasterGetSampleRate,

    /** Get current processing block size passed by host
     * @return current block size
     */
    audioMasterGetBlockSize,

    /** Get input latency in audio samples
     * @return input latency value
     */
    audioMasterGetInputLatency,

    /** Get output latency in audio samples
     * @return output latency value
     */
    audioMasterGetOutputLatency,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterGetPreviousPlug,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterGetNextPlug,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterWillReplaceOrAccumulate,

    /** Get current process level
     * @return current process level
     * @see VstProcessLevels
     */
    audioMasterGetCurrentProcessLevel,

    /** Get current automation state
     * @return current automation state
     * @see VstAutomationStates
     */
    audioMasterGetAutomationState,

    /** Start offline processing
     * @param index numBewAudioFiles
     * @param value numAduioFiles
     * @param ptr pointer to VstAudioFile
     * @see VstAudioFile
     */
    audioMasterOfflineStart,

    /** Read offline
     * @param index readSource (boolean)
     * @param value pointer to VstOfflineOption
     * @param ptr pointer to VstOfflineTask
     * @see VstOfflineOption
     * @see VstOfflineTask
     */
    audioMasterOfflineRead,

    /** Write offline
     * @param index readSource (boolean)
     * @param value pointer to VstOfflineOption
     * @param ptr pointer to VstOfflineTask
     * @see VstOfflineOption
     * @see VstOfflineTask
     */
    audioMasterOfflineWrite,

    /** Get current pass
     *
     */
    audioMasterOfflineGetCurrentPass,

    /** Get current meta pass
     *
     */
    audioMasterOfflineGetCurrentMetaPass,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterSetOutputSampleRate,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterGetOutputSpeakerArrangement,

    /** Get host vendor string
     * @param ptr buffer to store vendor string, maximum kVstMaxVendorStrLen characters
     * @see kVstMaxVendorStrLen
     */
    audioMasterGetVendorString,

    /** Get host product string
     * @param ptr buffer to store product string, maximum kVstMaxProductStrLen characters
     * @see kVstMaxProductStrLen
     */
    audioMasterGetProductString,

    /** Get vendor-specific version
     * @return vendor-specific version
     *
     */
    audioMasterGetVendorVersion,

    /** No special definition, vendor-specific handling
     *
     */
    audioMasterVendorSpecific,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterSetIcon,

    /** Check whether host supports some features
     * @param ptr the pointer to string containing the feature name
     * @return 1 if feature is supported, 0 otherwise
     */
    audioMasterCanDo,

    /** Get language code
     * @see VstHostLanguage
     */
    audioMasterGetLanguage,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterOpenWindow,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterCloseWindow,

    /** Get directory
     * @return pointer to FSSpec on MAC or pointer to character string otherwise
     *
     */
    audioMasterGetDirectory,

    /** No arguments
     *
     */
    audioMasterUpdateDisplay,

    /** Start editing
     * @param index parameter index
     */
    audioMasterBeginEdit,

    /** Finish editing
     * @param index parameter index
     */
    audioMasterEndEdit,

    /** Open the file selector
     * @param ptr pointer to VstFileSelect
     * @return 1 if feature is supported
     */
    audioMasterOpenFileSelector,

    /** Close the file selector
     * @param ptr pointer to VstFileSelect
     */
    audioMasterCloseFileSelector,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterEditFile,

    /**
     * @param ptr: char[2048] or sizeof (FSSpec)
     * @return 1 if supported
     * @deprecated since VST 2.4
     */
    audioMasterGetChunkFile,

    /**
     * @deprecated since VST 2.4
     */
    audioMasterGetInputSpeakerArrangement
};

/** VST event types
 *
 */
enum VstEventTypes
{
    /** MIDI event
     * @see VstMidiEvent
     */
    kVstMidiType = 1,

    /** Audio event (unused)
     * @deprecated
     */
    kVstAudioType,

    /** Video event (unused)
     * @deprecated
     */
    kVstVideoType,

    /** Parameter event (unused)
     * @deprecated
     */
    kVstParameterType,

    /** Trigger event (unused)
     * @deprecated
     */
    kVstTriggerType,

    /** System exclusive MIDI event
     * @see VstMidiSysexEvent
     */
    kVstSysExType
};

/** MIDI event types
 *
 */
enum VstMidiEventFlags
{
    /** This event is played at life performance, not from sequencer's MIDI track
     * his allows the Plug-In to handle these flagged events with higher priority,
     * especially when the Plug-In has a big latency
     */
    kVstMidiEventIsRealtime = 1 << 0
};

/** Different limitations of string constants
 *
 */
enum VstLimits
{
    /** Maximum program name length
     * @see effGetProgramName
     * @see effSetProgramName
     * @see effGetProgramNameIndexed
     *
     */
    kVstMaxProgNameLen   = 24,

    /** Maximum parameter name length
     * @see effGetParamLabel
     * @see effGetParamDisplay
     * @see effGetParamName
     *
     */
    kVstMaxParamStrLen   = 8,

    /** Maximum vendor string length
     * @see effGetVendorString
     * @see audioMasterGetVendorString
     *
     */
    kVstMaxVendorStrLen  = 64,

    /** Maximum product string length
     * @see effGetProductString
     * @see audioMasterGetProductString
     */
    kVstMaxProductStrLen = 64,

    /** Maximum plugin name length
     * @see effGetEffectName
     *
     */
    kVstMaxEffectNameLen = 32,

    /** Maximum name length
     * @see MidiProgramName
     * @see MidiProgramCategory
     * @see MidiKeyName
     * @see VstSpeakerProperties
     * @see VstPinProperties
     *
     */
    kVstMaxNameLen       = 64,

    /** Maximum label length
     * @see VstParameterProperties.label
     * @see VstPinProperties.label
     *
     */
    kVstMaxLabelLen      = 64,

    /** Maximum length of short label
     * @see VstParameterProperties.shortLabel
     * @see VstPinProperties.shortLabel
     *
     */
    kVstMaxShortLabelLen = 8,

    /** Maximum category label length
     * @see VstParameterProperties.label
     */
    kVstMaxCategLabelLen = 24,

    /** Maximum file name length
     * @see VstAudioFile.name
     */
    kVstMaxFileNameLen   = 100
};

/** Available host languages
 *
 */
enum VstHostLanguage
{
    /** English
     *
     */
    kVstLangEnglish = 1,

    /** German
     *
     */
    kVstLangGerman,

    /** French
     *
     */
    kVstLangFrench,

    /** Italian
     *
     */
    kVstLangItalian,

    /** Spanish
     *
     */
    kVstLangSpanish,

    /** Japanese
     *
     */
    kVstLangJapanese
};

/** VST processing precision constants
 *
 */
enum VstProcessPrecision
{
    /** Single-precision floating point (32 bits per sample)
     *
     */
    kVstProcessPrecision32 = 0,

    /** Double-precision floating point (64 bits per sample)
     *
     */
    kVstProcessPrecision64
};

/** Flags for VstTimeInfo structure
 * @see VstTimeInfo
 */
enum VstTimeInfoFlags
{
    /** Play, cycle or record state has been changed
     *
     */
    kVstTransportChanged     = 1 << 0,

    /** Host sequencer is currently playing
     *
     */
    kVstTransportPlaying     = 1 << 1,

    /** Host sequencer plays in cycle/loop mode
     *
     */
    kVstTransportCycleActive = 1 << 2,

    /** Host sequencer is recording/is in record mode
     *
     */
    kVstTransportRecording   = 1 << 3,

    /** Automation write mode is active (record parameter changes)
     *
     */
    kVstAutomationWriting    = 1 << 6,

    /** Automation read mode is active (play parameter changes)
     *
     */
    kVstAutomationReading    = 1 << 7,

    /** nanoSeconds field value is valid
     *
     */
    kVstNanosValid           = 1 << 8,

    /** ppqPos field value is valid
     *
     */
    kVstPpqPosValid          = 1 << 9,

    /** tempo field value is valid
     *
     */
    kVstTempoValid           = 1 << 10,

    /** barStartPos field value is valid
     *
     */
    kVstBarsValid            = 1 << 11,

    /** cycleStartPos field value is valid
     *
     */
    kVstCyclePosValid        = 1 << 12,

    /** timeSigNumerator field value is valid
     *
     */
    kVstTimeSigValid         = 1 << 13,

    /** smpteOffset field value is valid
     *
     */
    kVstSmpteValid           = 1 << 14,

    /** samplesToNextClock field value is valid
     *
     */
    kVstClockValid           = 1 << 15
};

/**
 * SMPTE (Society of Motion Picture and Television Engineers) frame rates
 * SMPTE timecode is a set of cooperating standards to label individual
 * frames of video or film with a time code
 */
enum VstSmpteFrameRate
{
    /** 24 FPS frame rate
     *
     */
    kVstSmpte24fps    = 0,

    /** 25 FPS frame rate
     *
     */
    kVstSmpte25fps    = 1,

    /** 29.97 FPS frame rate
     *
     */
    kVstSmpte2997fps  = 2,

    /** 30 FPS frame rate
     *
     */
    kVstSmpte30fps    = 3,

    /** 29.97 FPS drop frame rate
     *
     */
    kVstSmpte2997dfps = 4,

    /** 30 FPS drop frame rate
     *
     */
    kVstSmpte30dfps   = 5,

    /** 16mm film
     *
     */
    kVstSmpteFilm16mm = 6,

    /** 35 mm film
     *
     */
    kVstSmpteFilm35mm = 7,

    /** 23.976 FPS HDTV frame rate
     *
     */
    kVstSmpte239fps   = 10,

    /** 24.976 FPS HDTV frame rate
     *
     */
    kVstSmpte249fps   = 11,

    /** 59.94 FPS HDTV frame rate
     *
     */
    kVstSmpte599fps   = 12,

    /** 60 FPS HDTV frame rate
     *
     */
    kVstSmpte60fps    = 13
};

/** Constants used in VstFileSelect structure
 * @see VstFileSelect
 */
enum VstFileSelectCommand
{
    /** For loading a file
     *
     */
    kVstFileLoad = 0,

    /** For saving a file
     *
     */
    kVstFileSave,

    /** For loading multiple files
     *
     */
    kVstMultipleFilesLoad,

    /** For selecting a directory/folder
     *
     */
    kVstDirectorySelect
};

/** Types used in VstFileSelect structure
 * @see VstFileSelect
 */
enum VstFileSelectType
{
    /** Regular file selector
     *
     */
    kVstFileType = 0
};

/** Pan law rule constants
 *
 */
enum VstPanLawType
{
    /** Linear pan law:
     * L = pan * M; R = (1 - pan) * M;
     */
    kLinearPanLaw = 0,

    /** Equal power pan law:
     * L = sqrt(pan) * M; R = sqrt(1 - pan) * M;
     */
    kEqualPowerPanLaw
};

/** Process levels returned by audioMasterGetCurrentProcessLevel
 * @see audioMasterGetCurrentProcessLevel
 */
enum VstProcessLevels
{
    /** Unknown process level, unsupported by host
     *
     */
    kVstProcessLevelUnknown = 0,

    /** Currently in user thread (GUI)
     *
     */
    kVstProcessLevelUser,

    /** Currently in audio thread (where process method is called)
     *
     */
    kVstProcessLevelRealtime,

    /** Currently in 'sequencer' thread (MIDI, timer etc.)
     *
     */
    kVstProcessLevelPrefetch,

    /** Currently offline processing and thus in the user thread
     *
     */
    kVstProcessLevelOffline
};

/** Automation states returned by audioMasterGetAutomationState
 * @see audioMasterGetAutomationState
 */
enum VstAutomationStates
{
    /** Not supported by Host
     *
     */
    kVstAutomationUnsupported = 0,

    /** Automation is turned off
     *
     */
    kVstAutomationOff,

    /** Automation is in read state
     *
     */
    kVstAutomationRead,

    /** Automation is in write state
     *
     */
    kVstAutomationWrite,

    /** Automation is in read and write state
     *
     */
    kVstAutomationReadWrite
};

/**
 * Flags used in VstParameterProperties structure
 * @see VstParameterProperties
 */
enum VstParameterFlags
{
    /** Parameter is a binary-state switch
     * that can be toggled into on/off state
     *
     */
    kVstParameterIsSwitch                = 1 << 0,

    /** The minimum and maximum integer values (minInteger, maxInteger) are valid
     *
     */
    kVstParameterUsesIntegerMinMax       = 1 << 1,

    /** Parameters stepFloat, smallStepFloat, largeStepFloat are valid
     *
     */
    kVstParameterUsesFloatStep           = 1 << 2,

    /** Fields stepInteger and largeStepInteger are valid
     *
     */
    kVstParameterUsesIntStep             = 1 << 3,

    /** The displayIndex field is valid
     *
     */
    kVstParameterSupportsDisplayIndex    = 1 << 4,

    /** Parameter supports category feature and fields
     * related to category are valid
     *
     */
    kVstParameterSupportsDisplayCategory = 1 << 5,

    /** This flag indicates that parameter value can ramp up/down
     *
     */
    kVstParameterCanRamp                 = 1 << 6
};

/**
 * Flags used in VstPinProperties structure
 * @see VstPinProperties
 */
enum VstPinPropertiesFlags
{
    /** Pin is active, ignored by host
     *
     */
    kVstPinIsActive   = 1 << 0,

    /** Pin is first of a stereo pair
     *
     */
    kVstPinIsStereo   = 1 << 1,

    /** arrangementType field is valid and can be used
     * to get wanted arrangement
     */
    kVstPinUseSpeaker = 1 << 2
};

/**
 * Available plugin categories
 */
enum VstPlugCategory
{
    /** Unknown category or category not implemented
     *
     */
    kPlugCategUnknown = 0,

    /** Simple effect
     *
     */
    kPlugCategEffect,

    /** VST instrument: synthesizer, sampler, etc.
     *
     */
    kPlugCategSynth,

    /** Analysis: scope, tuner, etc.
     *
     */
    kPlugCategAnalysis,

    /** Maternig category: dynamics, etc.
     *
     */
    kPlugCategMastering,

    /** Spatial processing: panners, etc.
     *
     */
    kPlugCategSpacializer,

    /** Spacial effects: delays, reverberations, etc.
     *
     */
    kPlugCategRoomFx,

    /** Dedicated surround processor
     *
     */
    kPlugSurroundFx,

    /** Restoration effect: denoiser, declipper, etc.
     *
     */
    kPlugCategRestoration,

    /** Offline processor
     *
     */
    kPlugCategOfflineProcess,

    /** Shell plugin, it's a container for other plugins
     * @see effShellGetNextPlugin
     */
    kPlugCategShell,

    /** Different generators: tone generator, etc.
     *
     */
    kPlugCategGenerator,

    /** The maximum category number marker
     *
     */
    kPlugCategMaxCount
};


/**
 * Flags used in MidiProgramName
 * @see MidiProgramName
 */
enum VstMidiProgramNameFlags
{
    /** Omni flag (default is Multi)
     * For omni mode, channel 0 is used for inquiries and program changes
     */
    kMidiIsOmni = 1
};

/**
 * Speaker Types
 */
enum VstSpeakerType
{
    /**
     * Undefined
     */
    kSpeakerUndefined = 0x7fffffff,

    /**
     * Mono (M)
     */
    kSpeakerM = 0,

    /**
     * Left (L)
     */
    kSpeakerL,

    /**
     * Right (R)
     */
    kSpeakerR,

    /**
     * Center (C)
     */
    kSpeakerC,

    /**
     * Subbass (Lfe)
     */
    kSpeakerLfe,

    /**
     * Left Surround (Ls)
     */
    kSpeakerLs,

    /**
     * Right Surround (Rs)
     */
    kSpeakerRs,

    /**
     * Left of Center (Lc)
     */
    kSpeakerLc,

    /**
     * Right of Center (Rc)
     */
    kSpeakerRc,

    /**
     * Surround (S)
     */
    kSpeakerS,

    /**
     * Center of Surround (Cs) = Surround (S)
     */
    kSpeakerCs = kSpeakerS,

    /**
     * Side Left (Sl)
     */
    kSpeakerSl,

    /**
     * Side Right (Sr)
     */
    kSpeakerSr,

    /**
     * Top Middle (Tm)
     */
    kSpeakerTm,

    /**
     * Top Front Left (Tfl)
     */
    kSpeakerTfl,

    /**
     * Top Front Center (Tfc)
     */
    kSpeakerTfc,

    /**
     * Top Front Right (Tfr)
     */
    kSpeakerTfr,

    /**
     * Top Rear Left (Trl)
     */
    kSpeakerTrl,

    /**
     * Top Rear Center (Trc)
     */
    kSpeakerTrc,

    /**
     * Top Rear Right (Trr)
     */
    kSpeakerTrr,

    /**
     * Subbass 2 (Lfe2)
     */
    kSpeakerLfe2
};

/**
 * User-defined speaker types, to be extended in the negative range.
 * Will be handled as their corresponding speaker types with abs values:
 *    e.g abs(kSpeakerU1) == kSpeakerL, abs(kSpeakerU2) == kSpeakerR)
 */
enum VstUserSpeakerType
{
    kSpeakerU32 = -32,
    kSpeakerU31,
    kSpeakerU30,
    kSpeakerU29,
    kSpeakerU28,
    kSpeakerU27,
    kSpeakerU26,
    kSpeakerU25,
    kSpeakerU24,
    kSpeakerU23,
    kSpeakerU22,
    kSpeakerU21,
    kSpeakerU20     = -kSpeakerLfe2,
    kSpeakerU19     = -kSpeakerTrr,
    kSpeakerU18     = -kSpeakerTrc,
    kSpeakerU17     = -kSpeakerTrl,
    kSpeakerU16     = -kSpeakerTfr,
    kSpeakerU15     = -kSpeakerTfc,
    kSpeakerU14     = -kSpeakerTfl,
    kSpeakerU13     = -kSpeakerTm,
    kSpeakerU12     = -kSpeakerSr,
    kSpeakerU11     = -kSpeakerSl,
    kSpeakerU10     = -kSpeakerCs,
    kSpeakerU9      = -kSpeakerS,
    kSpeakerU8      = -kSpeakerRc,
    kSpeakerU7      = -kSpeakerLc,
    kSpeakerU6      = -kSpeakerRs,
    kSpeakerU5      = -kSpeakerLs,
    kSpeakerU4      = -kSpeakerLfe,
    kSpeakerU3      = -kSpeakerC,
    kSpeakerU2      = -kSpeakerR,
    kSpeakerU1      = -kSpeakerL
};

/**
 * Speaker Arrangement Types
 */
enum VstSpeakerArrangementType
{
    /** User-defined
     *
     */
    kSpeakerArrUserDefined = -2,///< user defined

    /** Empty arrangement (no arrangement)
     *
     */
    kSpeakerArrEmpty = -1,

    /** M
     *
     */
    kSpeakerArrMono  =  0,

    /** L R
     *
     */
    kSpeakerArrStereo,

    /** Ls Rs
     *
     */
    kSpeakerArrStereoSurround,

    /** Lc Rc
     *
     */
    kSpeakerArrStereoCenter,

    /** Sl Sr
     *
     */
    kSpeakerArrStereoSide,

    /** C Lfe
     *
     */
    kSpeakerArrStereoCLfe,

    /** L R C
     *
     */
    kSpeakerArr30Cine,

    /** L R S
     *
     */
    kSpeakerArr30Music,

    /** L R C Lfe
     *
     */
    kSpeakerArr31Cine,

    /** L R Lfe S
     *
     */
    kSpeakerArr31Music,

    /** L R C   S (LCRS)
     *
     */
    kSpeakerArr40Cine,

    /** L R Ls  Rs (Quadro)
     *
     */
    kSpeakerArr40Music,

    /** L R C   Lfe S (LCRS+Lfe)
     *
     */
    kSpeakerArr41Cine,

    /** L R Lfe Ls Rs (Quadro+Lfe)
     *
     */
    kSpeakerArr41Music,

    /** L R C Ls  Rs
     *
     */
    kSpeakerArr50,

    /** L R C Lfe Ls Rs
     *
     */
    kSpeakerArr51,

    /** L R C   Ls  Rs Cs
     *
     */
    kSpeakerArr60Cine,

    /** L R Ls  Rs  Sl Sr
     *
     */
    kSpeakerArr60Music,

    /** L R C   Lfe Ls Rs Cs
     *
     */
    kSpeakerArr61Cine,

    /** L R Lfe Ls  Rs Sl Sr
     *
     */
    kSpeakerArr61Music,

    /** L R C Ls  Rs Lc Rc
     *
     */
    kSpeakerArr70Cine,

    /** L R C Ls  Rs Sl Sr
     *
     */
    kSpeakerArr70Music,

    /** L R C Lfe Ls Rs Lc Rc
     *
     */
    kSpeakerArr71Cine,

    /** L R C Lfe Ls Rs Sl Sr
     *
     */
    kSpeakerArr71Music,

    /** L R C Ls  Rs Lc Rc Cs
     *
     */
    kSpeakerArr80Cine,

    /** L R C Ls  Rs Cs Sl Sr
     *
     */
    kSpeakerArr80Music,

    /** L R C Lfe Ls Rs Lc Rc Cs
     *
     */
    kSpeakerArr81Cine,

    /** L R C Lfe Ls Rs Cs Sl Sr
     *
     */
    kSpeakerArr81Music,

    /** L R C Lfe Ls Rs Tfl Tfc Tfr Trl Trr Lfe2
     *
     */
    kSpeakerArr102,

    /** The overall number of arrangements
     *
     */
    kNumSpeakerArr
};


/**
 * Flags used in VstOfflineTask structure
 * @see VstOfflineTask
 */
enum VstOfflineTaskFlags
{
    /** Unalid parameter, set by host
     *
     */
    kVstOfflineUnvalidParameter = 1 << 0,

    /** New file, set by host
     *
     */
    kVstOfflineNewFile          = 1 << 1,

    /** Plug error, set by plugin
     *
     */
    kVstOfflinePlugError        = 1 << 10,

    /** Interleaved Audio, set by plugin
     *
     */
    kVstOfflineInterleavedAudio = 1 << 11,

    /** Temporary output file, set by plugin
     *
     */
    kVstOfflineTempOutputFile   = 1 << 12,

    /** Float output file, set by plugin
     *
     */
    kVstOfflineFloatOutputFile  = 1 << 13,

    /** Random write, set by plugin
     *
     */
    kVstOfflineRandomWrite      = 1 << 14,

    /** Offline stretch, set by plugin
     *
     */
    kVstOfflineStretch          = 1 << 15,

    /** Offline no thread, set by plugin
     *
     */
    kVstOfflineNoThread         = 1 << 16
};

/**
 * Option passed to audioMasterOfflineRead/audioMasterOfflineWrite
 * @see audioMasterOfflineRead
 * @see audioMasterOfflineWrite
 */
enum VstOfflineOption
{
    /** Reading/writing audio samples
     *
     */
    kVstOfflineAudio,

    /** Reading graphic representation
     *
     */
    kVstOfflinePeaks,

    /** Reading/writing parameters
     *
     */
    kVstOfflineParameter,

    /** Reading/writing marker
     *
     */
    kVstOfflineMarker,

    /** Reading/moving edit cursor
     *
     */
    kVstOfflineCursor,

    /** Reading/changing selection
     *
     */
    kVstOfflineSelection,

    /** To request the host to call asynchronously effOfflineNotify
     * @see effOfflineNotify
     */
    kVstOfflineQueryFiles
};

/**
 * Flags used in VstAudioFile
 * @see VstAudioFile
 */
enum VstAudioFileFlags
{
    /** Offline read-only, set by host in call effOfflineNotify
     * @see effOfflineNotify
     */
    kVstOfflineReadOnly             = 1 << 0,

    /** Offline no rate conversion, set by host in call effOfflineNotify
     * @see effOfflineNotify
     */
    kVstOfflineNoRateConversion     = 1 << 1,

    /** Offline no channel change, set by host in call effOfflineNotify
     * @see effOfflineNotify
     */
    kVstOfflineNoChannelChange      = 1 << 2,

    /** Offline can process selection, set by plugin in call audioMasterOfflineStart
     * @see audioMasterOfflineStart
     */
    kVstOfflineCanProcessSelection  = 1 << 10,

    /** Offline no crossfade, set by plugin in call audioMasterOfflineStart
     * @see audioMasterOfflineStart
     */
    kVstOfflineNoCrossfade          = 1 << 11,

    /** Offline want read, set by plugin in call audioMasterOfflineStart
     * @see audioMasterOfflineStart
     */
    kVstOfflineWantRead             = 1 << 12,

    /** Offline want write, set by plugin in call audioMasterOfflineStart
     * @see audioMasterOfflineStart
     */
    kVstOfflineWantWrite            = 1 << 13,

    /** Offline want write marker, set by plugin in call audioMasterOfflineStart
     * @see audioMasterOfflineStart
     */
    kVstOfflineWantWriteMarker      = 1 << 14,

    /** Offline want move cursor, set by plugin in call audioMasterOfflineStart
     * @see audioMasterOfflineStart
     */
    kVstOfflineWantMoveCursor       = 1 << 15,

    /** Offline want select, set by plugin in call audioMasterOfflineStart
     * @see audioMasterOfflineStart
     */
    kVstOfflineWantSelect           = 1 << 16
};

/**
 * Platform-independent definition of Virtual Keys used in VstKeyCode.
 * @see VstKeyCode
 */
enum VstVirtualKey
{
    VKEY_BACK = 1,
    VKEY_TAB,
    VKEY_CLEAR,
    VKEY_RETURN,
    VKEY_PAUSE,
    VKEY_ESCAPE,
    VKEY_SPACE,
    VKEY_NEXT,
    VKEY_END,
    VKEY_HOME,
    VKEY_LEFT,
    VKEY_UP,
    VKEY_RIGHT,
    VKEY_DOWN,
    VKEY_PAGEUP,
    VKEY_PAGEDOWN,
    VKEY_SELECT,
    VKEY_PRINT,
    VKEY_ENTER,
    VKEY_SNAPSHOT,
    VKEY_INSERT,
    VKEY_DELETE,
    VKEY_HELP,
    VKEY_NUMPAD0,
    VKEY_NUMPAD1,
    VKEY_NUMPAD2,
    VKEY_NUMPAD3,
    VKEY_NUMPAD4,
    VKEY_NUMPAD5,
    VKEY_NUMPAD6,
    VKEY_NUMPAD7,
    VKEY_NUMPAD8,
    VKEY_NUMPAD9,
    VKEY_MULTIPLY,
    VKEY_ADD,
    VKEY_SEPARATOR,
    VKEY_SUBTRACT,
    VKEY_DECIMAL,
    VKEY_DIVIDE,
    VKEY_F1,
    VKEY_F2,
    VKEY_F3,
    VKEY_F4,
    VKEY_F5,
    VKEY_F6,
    VKEY_F7,
    VKEY_F8,
    VKEY_F9,
    VKEY_F10,
    VKEY_F11,
    VKEY_F12,
    VKEY_NUMLOCK,
    VKEY_SCROLL,
    VKEY_SHIFT,
    VKEY_CONTROL,
    VKEY_ALT,
    VKEY_EQUALS
};

/**
 * Modifier flags used in VstKeyCode.
 * @see VstKeyCode
 */
enum VstModifierKey
{
    /** Shift modifier
     *
     */
    MODIFIER_SHIFT     = 1<<0,

    /** Alt modifier
     *
     */
    MODIFIER_ALTERNATE = 1<<1,

    /** Ctrl on Mac modifier
     *
     */
    MODIFIER_COMMAND   = 1<<2,

    /** Ctrl on PC, Apple on Mac modifier
     *
     */
    MODIFIER_CONTROL   = 1<<3
};

//-------------------------------------------------------------------------------------------------------
// Data structures definition
//-------------------------------------------------------------------------------------------------------
struct AEffect;

/**
 * VST processor routine prototypes
 */
typedef VstIntPtr   (VSTCALLBACK *audioMasterCallback)      (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);
typedef VstIntPtr   (VSTCALLBACK *AEffectDispatcherProc)    (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);
typedef void        (VSTCALLBACK *AEffectProcessProc)       (AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames);
typedef void        (VSTCALLBACK *AEffectProcessDoubleProc) (AEffect* effect, double** inputs, double** outputs, VstInt32 sampleFrames);
typedef void        (VSTCALLBACK *AEffectSetParameterProc)  (AEffect* effect, VstInt32 index, float parameter);
typedef float       (VSTCALLBACK *AEffectGetParameterProc)  (AEffect* effect, VstInt32 index);

/** Basic VST Effect interface structure
 *
 */
typedef struct AEffect
{
    /** The magic number that allows to identify VST 2.x effect, should be kEffectMagic ('VstP')
     *
     */
    VstInt32 magic;

    /** Host to Plug-in event dispatcher
     *
     */
    AEffectDispatcherProc dispatcher;

    /** Accumulating process mode method
     * @deprecated since VST 2.4
     *
     */
    AEffectProcessProc process;

    /** Assign new value to automatable parameter
     * Parameters are the individual parameter settings the user can adjust.
     * A VST Host can automate these parameters.
     *
     * Parameter values, like all VST parameters, are declared as floats with an inclusive range of
     * 0.0 to 1.0. How data is presented to the user is merely in the user-interface handling. This is a
     * convention, but still worth regarding. Maybe the VST-Host's automation system depends on this range.
     *
     * @param index parameter index
     * @param parameter parameter value between 0.0 and 1.0 inclusive
     *
     */
    AEffectSetParameterProc setParameter;

    /** Return current value of automatable parameter
     * @param index parameter index
     * @return parameter value in range between 0.0 and 1.0 inclusive
     *
     */
    AEffectGetParameterProc getParameter;

    /** Number of programs
     *
     */
    VstInt32 numPrograms;

    /** The number of parameters for program.
     * All programs are assumed to have equal number of parameters
     *
     */
    VstInt32 numParams;

    /** Number of audio inputs
     *
     */
    VstInt32 numInputs;

    /** Number of audio outputs
     *
     */
    VstInt32 numOutputs;

    /** Additional processor flags
     * @see VstAEffectFlags
     */
    VstInt32 flags;

    /** Parameters reserved for a host, should be zeroed
     *
     */
    VstIntPtr __pad1;
    VstIntPtr __pad2;

    /** The latency value applied to the signal by plugin.
     * This value should be initialized in a resume state.
     * If this value is changed, the plugin needs to issue
     * audioMasterIOChanged signal to the host
     *
     */
    VstInt32 initialDelay;

    /** Unused member
     * @deprecated
     */
    VstInt32 realQualities;

    /** Unused member
     * @deprecated
     */
    VstInt32 offQualities;

    /** Unused member
     * @deprecated
     */
    float    ioRatio;

    /** Pointer to the wrapper object
     *
     */
    void *object;

    /** The user-defined pointer
     *
     */
    void *user;

    /** Unique plugin identifier, should be registered on this form:
     * http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
     * This is used to identify a plug-in during save+load of preset and project.
     *
     * The Host uses this to identify the plug-in, for instance when it is loading
     * effect programs and banks. On Steinberg Web Page you can find an UniqueID Database
     * where you can record your UniqueID, it will check if the ID is already used by an
     * another vendor. You can use CCONST('a','b','c','d') (defined in VST 2.0) to be
     * platform independent to initialize an UniqueID.
     */
    VstInt32 uniqueID;

    /** Plugin version: 1.1.0.0 should be encoded as 1100
     *
     */
    VstInt32 version;

    /** Process audio samples in replacing mode, the main processing method
     *
     * @warning Never call any Mac OS 9 functions (or other functions which call into the OS) inside your
     * audio process function! This will crash the system when your plug-in is run in MP (multiprocessor) mode.
     * If you must call into the OS, you must use MPRemoteCall () (see Apples' documentation), or
     * explicitly use functions which are documented by Apple to be MP safe. On Mac OS X read the system
     * header files to be sure that you only call thread safe functions.
     */
    AEffectProcessProc processReplacing;

    /** The same as processReplacing but operates with double-precision floating point samples
     * @since VST 2.4
     */
    AEffectProcessDoubleProc processDoubleReplacing;

    /** Reserved for future use, should be zeroed
     *
     */
    char reserved[56];
} AEffect;

/** Plugin parameter properties used in effGetParameterProperties
 * @see effGetParameterProperties
 *
 */
typedef struct VstParameterProperties
{
    /** Float step
     *
     */
    float stepFloat;

    /** Small float step
     *
     */
    float smallStepFloat;

    /** Large float step
     *
     */
    float largeStepFloat;

    /** Parameter label
     *
     */
    char label[kVstMaxLabelLen];

    /** Parameter flags
     * @see VstParameterFlags
     */
    VstInt32 flags;

    /** minimum integer value
     *
     */
    VstInt32 minInteger;

    /** Maximum integer value
     *
     */
    VstInt32 maxInteger;

    /** Step of the integer value
     *
     */
    VstInt32 stepInteger;

    /** Large step of the integer value
     *
     */
    VstInt32 largeStepInteger;

    /** Short label of the parameter,
     * SDK recommends 6 characters + delimiter
     */
    char shortLabel[kVstMaxShortLabelLen];

    /** The following are for remote controller display purposes.
     * Index where this parameter should be displayed (starting with 0)
     * @note the kVstParameterSupportsDisplayIndex flag must be set.
     * Host can scan all parameters, and find out in what order to display them.
     */
    int16_t displayIndex;

    /** Parameter category: 0 means no category, else it should be group index + 1
     * Host can also display the parameter group (category) if supports this feature
     * For example:
     *   +-------------------------+
     *   | Osc1:                   |
     *   | Wave Detune Octave Mod  |
     *   +-------------------------+
     * The support of this feature is indicated by kVstParameterSupportsDisplayCategory flag
     * @see kVstParameterSupportsDisplayCategory
     */
    int16_t category;

    /** Number of parameters in category
     *
     */
    int16_t numParametersInCategory;

    /** Reserved, should be zero
     *
     */
    int16_t reserved;

    /** Category label, for example "Osc 1"
     *
     */
    char categoryLabel[kVstMaxCategLabelLen];

    /** Reserved for future use
     *
     */
    char future[16];
} VstParameterProperties;

/**
 * Program (fxp) structure.
 * Stored in big-endian format. You have to swap integer
 * and floating-point values on Little Endian platforms (Windows, MacIntel)!
 */
typedef struct fxProgram
{
    /** The root chunk identifier for program magic number cMagic ('CcnK')
     * @see cMagic
     */
    VstInt32 chunkMagic;

    /** Size of the chunk excluding chunkMagic and byteSize fields
     *
     */
    VstInt32 byteSize;

    /** Type of chunk:
     *   fMagic ('FxCk') - regular chunk
     *   chunkPresetMagic ('FPCh') - opaque chunk
     * @see fMagic
     * @see chunkPresetMagic
     */
    VstInt32 fxMagic;

    /** Format version, currently 1
     *
     */
    VstInt32 version;

    /** fx unique identifier
     *
     */
    VstInt32 fxID;

    /** fx version
     *
     */
    VstInt32 fxVersion;

    /** Number of parameters
     *
     */
    VstInt32 numParams;

    /** Program name (null-terminated ASCII string)
     *
     */
    char prgName[28];

    /** Program content depending on the fxMagic field
     *
     */
    union
    {
        /** Variable sized array with parameter values
         *
         */
        float params[1];

        /** Program chunk data
         *
         */
        struct
        {
            /** Size of program data
             *
             */
            VstInt32 size;

            /** Variable sized array with opaque program data
             *
             */
            char chunk[1];
        } data;
    } content;
} fxProgram;

/**
 * Bank (fxb) structure.
 * Stored in big-endian format. You have to swap integer
 * and floating-point values on Little Endian platforms (Windows, MacIntel)!
 */
typedef struct fxBank
{
    /** Chunk magic number cMagic ('CcnK')
     * @see cMagic
     */
    VstInt32 chunkMagic;

    /** Size of the chunk excluding chunkMagic and byteSize fields
     *
     */
    VstInt32 byteSize;

    /** Magic number of the chunk:
     * bankMagic ('FxBk') - regular chunk
     * chunkBankMagic ('FBCh') - opaque chunk
     * @see bankMagic
     * @see chunkBankMagic
     */
    VstInt32 fxMagic;

    /** Format version (1 or 2)
     *
     */
    VstInt32 version;

    /** FX unique identifier
     *
     */
    VstInt32 fxID;

    /** FX version
     *
     */
    VstInt32 fxVersion;

    /** Number of programs
     *
     */
    VstInt32 numPrograms;

    /** Current program for fxVersion = 2
     *
     */
    VstInt32 currentProgram;

    /** Reserved, should be zero
     *
     */
    char reserved[124];

    /** Bank content depending on fxMagic field
     *
     */
    union
    {
        /** Variable number of programs
         *
         */
        fxProgram programs[1];

        /** Bank chunk data
         *
         */
        struct
        {
            /** Size of bank data
             *
             */
            VstInt32 size;

            /** Variable sized array with opaque bank data
             *
             */
            char chunk[1];
        } data;
    } content;
} fxBank;

/** Generic timestamped VST event
 *
 */
typedef struct VstEvent
{
    /** Type of event
     * @see VstEventTypes
     *
     */
    VstInt32 type;

    /** Size of event excluding type and byteSize
     *
     */
    VstInt32 byteSize;

    /** The offset of the event from the start of the current block
     * in samples
     */
    VstInt32 deltaFrames;

    /** Generic flags, none defined yet
     *
     */
    VstInt32 flags;

    /** Padding bytes, used by various event types
     *
     */
    char data[16];
} VstEvent;

/** MIDI event
 *
 */
typedef struct VstMidiEvent
{
    /** Type of event, should be kVstMidiType
     * @see kVstMidiType
     */
    VstInt32 type;

    /** Size of this structure - sizeof (VstMidiEvent)
     *
     */
    VstInt32 byteSize;

    /** The offset of the event from the start of the current block
     * in samples
     */
    VstInt32 deltaFrames;

    /** Flags of the MIDI event
     * @see VstMidiEventFlags
     */
    VstInt32 flags;

    /** The length of the entire note in samples, if available, 0 otherwise.
     *
     */
    VstInt32 noteLength;

    /** Offset (in samples) into note from note start if available, else 0
     *
     */
    VstInt32 noteOffset;

    /** 1-3 MIDI packet bytes.
     * midiData[3] is reserved and should be zero
     *
     */
    char midiData[4];

    /** Detune
     * -64 to +63 cents; for scales other than 'well-tempered' ('microtuning')
     */
    char detune;

    /** The velocity of the Note Off event [0, 127]
     *
     */
    char noteOffVelocity;

    /** Reserved for future use
     * should be zero
     */
    char reserved1;

    /** Reserved for future use
     * should be zero
     */
    char reserved2;
} VstMidiEvent;

/** MIDI Sysex Event
 *
 */
typedef struct VstMidiSysexEvent
{
    /** Type of event, should be kVstSysexType
     * @see kVstSysexType
     */
    VstInt32 type;

    /** Size of this structure - sizeof (VstMidiSysexEvent)
     *
     */
    VstInt32 byteSize;

    /** sample frames related to the current block start sample position
     *
     */
    VstInt32 deltaFrames;

    /** No flags defined, shluld be zero
     *
     */
    VstInt32 flags;

    /** The size of sysexDump data in bytes
     *
     */
    VstInt32 dumpBytes;

    /** Reserved for future use
     * should be zero
     */
    VstIntPtr resvd1;

    /** The dump of sysex
     *
     */
    char* sysexDump;

    /** Reserved for future use
     * should be zero
     */
    VstIntPtr resvd2;
} VstMidiSysexEvent;

/** The array of VST events
 *
 */
typedef struct VstEvents
{
    /** Number of events in array
     *
     */
    VstInt32 numEvents;

    /** Reserved, should be zero
     *
     */
    VstIntPtr reserved;

    /** The variable-size array of pointers to VstEvent
     * @see VstEvent
     */
    VstEvent* events[2];
} VstEvents;

/** MIDI program description
 *
 */
typedef struct MidiProgramName
{
    /** 0 or greater: fill struct for this program index
     *
     */
    VstInt32 thisProgramIndex;

    /** Program name
     *
     */
    char name[kVstMaxNameLen];

    /** MIDI program: 0-127, -1 = off
     *
     */
    char midiProgram;

    /** Most-significant byte of MIDI bank: 0-127, -1 = off
     *
     */
    char midiBankMsb;

    /** Least-significant byte of MIDI bank: 0-127, -1 = off
     *
     */
    char midiBankLsb;

    /** Reserved, should be zero
     *
     */
    char reserved;

    /** Index of the parent category, -1 means that there is no parent category
     *
     */
    VstInt32 parentCategoryIndex;

    /** Different flags
     * @see VstMidiProgramNameFlags
     */
    VstInt32 flags;
} MidiProgramName;

/**
 * MIDI Program Category.
 */
typedef struct MidiProgramCategory
{
    /** 0 or greater: fill struct for this category index
     *
     */
    VstInt32 thisCategoryIndex;

    /** Name of program category
     *
     */
    char name[kVstMaxNameLen];

    /** Index of parent category, -1 means that there is no parent category
     *
     */
    VstInt32 parentCategoryIndex;

    /** Different flags, currently there are none, should be zero
     *
     */
    VstInt32 flags;
} MidiProgramCategory;

/**
 * MIDI Key Description.
 */
typedef struct MidiKeyName
{
    /** 0 or greater: fill struct for this program index.
     *
     */
    VstInt32 thisProgramIndex;

    /** Key number: 0-127. Fill struct for this key number
     *
     */
    VstInt32 thisKeyNumber;

    /** Key name, empty value means regular key names
     *
     */
    char keyName[kVstMaxNameLen];

    /** Reserved, should be zero
     *
     */
    VstInt32 reserved;

    /** Different flags, currently there are none, should be zero
     *
     */
    VstInt32 flags;
} MidiKeyName;

/** Speaker Properties.
 * The origin for azimuth is right (as by math conventions dealing with radians).
 * The elevation origin is also right, visualizing a rotation of a circle across the
 * -PI..PI axis of the horizontal circle. Thus, an elevation of -PI/2 corresponds
 * to bottom, and a speaker standing on the left, and 'beaming' upwards would have
 * an azimuth of -PI, and an elevation of PI/2.
 * For user interface representation, grads are more likely to be used, and the
 * origins will obviously 'shift' accordingly.
 */
typedef struct VstSpeakerProperties
{
    /** Azimuth in radians, range: -PI...PI, exception: 10.f for LFE channel
     *
     */
    float azimuth;

    /** Elevation in radians, range: -PI/2...PI/2, exception: 10.f for LFE channel
     *
     */
    float elevation;

    /** Radius in meters, exception: 0.0f for LFE channel
     *
     */
    float radius;

    /** Reserved for future use, should be zero
     *
     */
    float reserved;

    /** For new setups new names should be given (L/R/C... won't do)
     *
     */
    char name[kVstMaxNameLen];

    /** Speaker type
     * @see VstSpeakerType
     */
    VstInt32 type;

    /** Reserved for future use
     *
     */
    char future[28];
} VstSpeakerProperties;

/**
 * Speaker Arrangement
 */
typedef struct VstSpeakerArrangement
{
    /** Speaker arrangement type
     * @see VstSpeakerArrangementType
     */
    VstInt32 type;

    /** Number of channels in this speaker arrangement
     *
     */
    VstInt32 numChannels;

    /** Variable sized speaker array
     *
     */
    VstSpeakerProperties speakers[8];
} VstSpeakerArrangement;

/**
 * Pin Properties used in effGetInputProperties and effGetOutputProperties.
 * @see effGetInputProperties
 * @see effGetOutputProperties
 */
typedef struct VstPinProperties
{
    /** Pin name
     *
     */
    char label[kVstMaxLabelLen];

    /** Pin flags
     * @see VstPinPropertiesFlags
     */
    VstInt32 flags;

    /** Pin arrangement type
     * @see VstSpeakerArrangementType
     */
    VstInt32 arrangementType;

    /** Short name for pin properties
     * SDK recommended: 6 characters + delimiter
     */
    char shortLabel[kVstMaxShortLabelLen];

    /** Reserved data for future use, should be zero
     *
     */
    char future[48];
} VstPinProperties;


/**
 * Variable IO for Offline Processing.
 */
typedef struct VstVariableIo
{
    /** Array of audio buffers for input data
     *
     */
    float** inputs;

    /** Array of audio buffers for output data
     *
     */
    float** outputs;

    /** Number of input samples for processing
     *
     */
    VstInt32 numSamplesInput;

    /** Number of output samples for processing
     *
     */
    VstInt32 numSamplesOutput;

    /** Pointer to store number of samples actually processed on input
     *
     */
    VstInt32 *numSamplesInputProcessed;

    /** Pointer to store number of samples actually processed on output
     *
     */
    VstInt32 *numSamplesOutputProcessed;
} VstVariableIo;

/**
 * Offline Task Description.
 */
typedef struct VstOfflineTask
{
    /** Process name, set by plugin
     *
     */
    char processName[96];

    /** Audio access: read position, set by plugin or host
     *
     */
    double readPosition;

    /** Audio access: write position, set by plugin or host
     *
     */
    double writePosition;

    /** Audio access: read count, set by plugin or host
     *
     */
    VstInt32 readCount;

    /** Audio access: write count, set by plugin or host
     *
     */
    VstInt32 writeCount;

    /** Audio access: input buffer size, set by host
     *
     */
    VstInt32 sizeInputBuffer;

    /** Audio access: output buffer size, set by host
     *
     */
    VstInt32 sizeOutputBuffer;

    /** Audio access: pointer to input buffer, set by host
     *
     */
    void* inputBuffer;

    /** Audio access: pointer to output buffer, set by host
     *
     */
    void* outputBuffer;

    /** Audio access: position to start processing from, set by host
     *
     */
    double positionToProcessFrom;

    /** Audio access: number of frames to process, set by host
     *
     */
    double numFramesToProcess;

    /** Audio access: maximum number of frames to write, set by plugin
     *
     */
    double maxFramesToWrite;

    /** Other data access: extra buffer, set by plugin
     *
     */
    void* extraBuffer;

    /** Audio access: some value, set by plugin or host
     *
     */
    VstInt32 value;

    /** Audio access: some index, set by plugin or host
     *
     */
    VstInt32 index;

    /** File attributes: number of frames in source file, set by host
     *
     */
    double numFramesInSourceFile;

    /** File attributes: sample rate of source file, set by plugin or host
     *
     */
    double sourceSampleRate;

    /** File attributes: sample rate of destination file, set by plugin or host
     *
     */
    double destinationSampleRate;

    /** File attributes: number of channels in source file, set by plugin or host
     *
     */
    VstInt32 numSourceChannels;

    /** File attributes: number of channels in destinatio file, set by plugin or host
     *
     */
    VstInt32 numDestinationChannels;

    /** File attributes: source file format, set by host
     *
     */
    VstInt32 sourceFormat;

    /** File attributes: destination file format, set by plugin
     *
     */
    VstInt32 destinationFormat;

    /** File attributes: output text, set by plugin or host
     *
     */
    char outputText[512];

    /** Progress notification: progress value, set by plugin
     *
     */
    double progress;

    /** File attributes: progress mode, reserved for future use
     *
     */
    VstInt32 progressMode;

    /** File attributes: the corresponding progress text, set by plugin
     *
     */
    char progressText[100];

    /** Offline task flags, set by host and plugin
     * @see VstOfflineTaskFlags
     */
    VstInt32 flags;

    /** Return value, reserved for future use
     *
     */
    VstInt32 returnValue;

    /** Some custom data set and used by host
     *
     */
    void* hostOwned;

    /** Some custom data set and used by plugin
     *
     */
    void* plugOwned;

    /** Reserved for future use, should be zero
     *
     */
    char future[1024];
} VstOfflineTask;

/**
 * Structure passed to effOfflineNotify and effOfflineStart
 * @see effOfflineNotify
 * @see effOfflineStart
 */
typedef struct VstAudioFile
{
    /** Audio file flags
     * @see VstAudioFileFlags
     */
    VstInt32 flags;

    /** Some custom data set and used by host
     *
     */
    void* hostOwned;

    /** Some custom data set and used by plugin
     *
     */
    void* plugOwned;

    /** File title
     * @see kVstMaxFileNameLen
     */
    char name[kVstMaxFileNameLen];

    /** Unique file identifier during a session
     *
     */
    VstInt32 uniqueId;

    /** Sample rate of the file
     *
     */
    double sampleRate;

    /** Number of audio channels in the file (1 = mono, 2 = stereo, etc.)
     *
     */
    VstInt32 numChannels;

    /** Number of frames in the audio file
     *
     */
    double numFrames;

    /** Audio file format, reserved for future use
     *
     */
    VstInt32 format;

    /** Editor curstor position, -1 if there is no such cursor
     *
     */
    double editCursorPosition;

    /** The frame index of first selected frame, -1 if no selection
     *
     */
    double selectionStart;

    /** Number of frame in selection, 0 if no selection
     *
     */
    double selectionSize;

    /** Mask of selected channels, 1 bit per each channel
     *
     */
    VstInt32 selectedChannelsMask;

    /** Number of markers in the file
     *
     */
    VstInt32 numMarkers;

    /** Time ruler unit
     *
     */
    VstInt32 timeRulerUnit;

    /** Offset in time ruler
     *
     */
    double timeRulerOffset;

    /** Tempo (BPM, Beats Per Minute)
     *
     */
    double tempo;

    /** Time signature numerator, for 3/4 it's 3
     *
     */
    VstInt32 timeSigNumerator;

    /** Time signature denominator, for 3/4 it's 4
     *
     */
    VstInt32 timeSigDenominator;

    /** Resolution
     *
     */
    VstInt32 ticksPerBlackNote;

    /** SMPTE frame rate
     * @see VstTimeInfo
     */
    VstInt32 smpteFrameRate;

    /** Reserved for future use, should be zero
     *
     */
    char future[64];
} VstAudioFile;

/**
 * File filter used in VstFileSelect
 * @see VstFileSelect
 */
struct VstFileType
{
    /** Display name
     *
     */
    char name[128];

    /** Type for MacOS
     *
     */
    char macType[8];

    /** Windows file extension
     *
     */
    char dosType[8];

    /** Unix file extension
     *
     */
    char unixType[8];

    /** MIME type
     *
     */
    char mimeType1[128];

    /** Additional MIME type
     *
     */
    char mimeType2[128];

#ifdef __cplusplus
    inline VstFileType (const char* _name = 0, const char* _macType = 0, const char* _dosType = 0,
                 const char* _unixType = 0, const char* _mimeType1 = 0, const char* _mimeType2 = 0);
#endif /* __cplusplus */
};

/**
 * File Selector Description used in audioMasterOpenFileSelector.
 * @see audioMasterOpenFileSelector
 */
struct VstFileSelect
{

    /** File selection command
     * @see VstFileSelectCommand
     */
    VstInt32 command;

    /** File selection type
     * @see VstFileSelectType
     */
    VstInt32 type;

    /** Optional: 0 means no creator
     *
     */
    VstInt32 macCreator;

    /** Number of file types
     *
     */
    VstInt32 nbFileTypes;

    /** List of file types
     * @see VstFileType
     */
    VstFileType* fileTypes;

    /** Text to display in file selector's title
     *
     */
    char title[1024];

    /** Initial path
     *
     */
    char* initialPath;

    /** Used with kVstFileLoad and kVstDirectorySelect
     * Should be NULL. Host allocates memory, plugin should call closeOpenFileSelector
     * @see kVstFileLoad
     * @see kVstDirectorySelect
     * @see closeOpenFileSelector
     */
    char* returnPath;

    /** Size of allocated memory for return paths
     *
     */
    VstInt32 sizeReturnPath;

    /** Used with kVstMultipleFilesLoad
     * Should be NULL. Host allocates memory, plugin should call closeOpenFileSelector
     * @see kVstMultipleFilesLoad
     */
    char** returnMultiplePaths;

    /** Number of selected paths
     *
     */
    VstInt32 nbReturnPath;

    /** Reserved for usage by the host
     *
     */
    VstIntPtr reserved;

    /** Reserved for future use
     *
     */
    char future[116];
};

/** Structure used to pass for audioMasterOpenWindow and audioMasterCloseWindow
 * @see audioMasterOpenWindow
 * @see audioMasterCloseWindow
 * @deprecated since VST 2.4
 */
typedef struct VstWindow
{
    /** Window title
     *
     */
    char title[128];

    /** The leftmost coordinate of the window
     *
     */
    int16_t xPos;

    /** The topmost coordinate of the window
     *
     */
    int16_t yPos;

    /** The width of the window
     *
     */
    int16_t width;

    /** The height of the window
     *
     */
    int16_t height;

    /** The style of the window
     *
     */
    VstInt32 style;

    /** Parent window handle
     *
     */
    void* parent;

    /** User window handle
     *
     */
    void* userHandle;

    /** Window handle
     *
     */
    void* winHandle;

    /** Reserved for future use
     *
     */
    char future[104];
} VstWindow;

/** Structure used for effEditKeyUp/effEditKeyDown
 * @see effEditKeyUp
 * @see effEditKeyDown
 */
typedef struct VstKeyCode
{
    /** ASCII character code
     *
     */
    VstInt32 character;

    /** Virtual key
     * @see VstVirtualKey
     */
    unsigned char virt;

    /** Key modifier
     * @see VstModifierKey
     */
    unsigned char modifier;
} VstKeyCode;

/**
 * Audio file marker structure
 */
typedef struct VstAudioFileMarker
{
    /** Maker position
     *
     */
    double position;

    /** Maker name
     *
     */
    char name[32];

    /** Maker type
     *
     */
    VstInt32 type;

    /** Maker identifier
     *
     */
    VstInt32 id;

    /** Reserved for future use
     *
     */
    VstInt32 reserved;
} VstAudioFileMarker;


/** This structure describes bounds of the custom plugin UI editor
 *
 * @see effEditGetRect
 */
typedef struct ERect
{
    /** The top coordinate
     *
     */
    int16_t top;

    /** The left coordinate
     *
     */
    int16_t left;

    /** The bottom coordinate
     *
     */
    int16_t bottom;

    /** The right coordinate
     *
     */
    int16_t right;
} ERect;

/** Time info structure
 *
 * @note samplePos: must always be valid, and should not cost a lot to ask for.
 *   The sample position is ahead of the time displayed to the user. In sequencer
 *   stop mode, its value does not change. A 32 bit integer is too small for sample
 *   positions, and it's a double to make it easier to convert between ppq and samples.
 *
 * @note ppqPos: at tempo 120, 1 quarter makes 1/2 second, so 2.0 ppq translates to 48000
 *   samples at 48kHz sample rate. 0.25 ppq is one sixteenth note then. if you need something
 *   like 480ppq, you simply multiply ppq by that scaler.
 *
 * @note barStartPos: Say we're at bars/beats readout 3.3.3. That's 2 bars + 2 q + 2 sixteenth,
 *   makes 2 * 4 + 2 + 0.25 = 10.25 ppq. at tempo 120, that's 10.25 * 0.5 = 5.125 seconds,
 *   times 48000 = 246000 samples.
 *
 * @note samplesToNextClock: MIDI Clock Resolution (24 per Quarter Note), can be negative the
 *   distance to the next midi clock (24 ppq, pulses per quarter) in samples. unless samplePos
 *   falls precicely on a midi clock, this will either be negative such that the previous MIDI
 *   clock is addressed, or positive when referencing the following (future) MIDI clock.
 *
 * @see audioMasterGetTime
 */
typedef struct VstTimeInfo
{
//-------------------------------------------------------------------------------------------------------
    /** Current position in audio samples (always valid)
     *
     */
    double samplePos;

    /** Current sample rate in Hz (always valid)
     *
     */
    double sampleRate;

    /** System time in nanoseconds (10^-9 seconds)
     *
     */
    double nanoSeconds;

    /** Musical Position, in Quarter Note (1.0 equals 1 Quarter Note)
     *
     */
    double ppqPos;

    /** current Tempo in BPM (Beats Per Minute)
     *
     */
    double tempo;

    /** last Bar Start Position, in Quarter Note
     *
     */
    double barStartPos;

    /** Cycle Start (left locator), in Quarter Note
     *
     */
    double cycleStartPos;

    /** Cycle End (right locator), in Quarter Note
     *
     */
    double cycleEndPos;

    /** Time Signature Numerator (e.g. 3 for 3/4)
     *
     */
    VstInt32 timeSigNumerator;

    /** Time Signature Denominator (e.g. 4 for 3/4)
     *
     */
    VstInt32 timeSigDenominator;

    /** SMPTE offset (in SMPTE subframes (bits; 1/80 of a frame)).
     * The current SMPTE position can be calculated using
     * samplePos, sampleRate, and smpteFrameRate.
     */
    VstInt32 smpteOffset;

    /** Frame rate
     * @see VstSmpteFrameRate
     */
    VstInt32 smpteFrameRate;

    /** MIDI Clock Resolution (24 Per Quarter Note),
     * can be negative (nearest clock)
     *
     */
    VstInt32 samplesToNextClock;

    /** Time information flags
     * @see VstTimeInfoFlags
     */
    VstInt32 flags;
} VstTimeInfo;

/** Structure for effBeginLoadBand/effBeginLoadProgram
 * @see effBeginLoadBand
 * @see effBeginLoadProgram
 */
typedef struct VstPatchChunkInfo
{
    /** Format Version, should be 1
     *
     */
    VstInt32 version;

    /** Unique identifier of the plugin
     *
     */
    VstInt32 pluginUniqueID;

    /** Version of the plugin
     *
     */
    VstInt32 pluginVersion;

    /** Number of programs (Bank) or parameters (program)
     *
     */
    VstInt32 numElements;

    /** Reserved for future use
     *
     */
    char reserved[48];
} VstPatchChunkInfo;

//-------------------------------------------------------------------------------------------------------
// Miscellaneous functions' definition
//-------------------------------------------------------------------------------------------------------
/** Copy character sting to destination location and complete with null-terminating byte
 *
 * @param dst destination string
 * @param src source string
 * @param max_len maximum destination string length
 * @return pointer to destination string
 */
inline char* vst_strncpy (char* dst, const char* src, size_t max_len)
{
    if (max_len <= 0)
        return dst;
    char* result = strncpy (dst, src, max_len-1);
    dst[max_len-1] = '\0';
    return result;
}

/** Append character sting to destination location and complete with null-terminating byte
 *
 * @param dst destination string
 * @param src source string
 * @param max_len maximum destination string length
 * @return pointer to destination string
 */
inline char* vst_strncat (char* dst, const char* src, size_t max_len)
{
    if (max_len <= 0)
        return dst;
    char* result = strncat (dst, src, max_len-1);
    dst[max_len-1] = '\0';
    return result;
}

/** Initialize file type structure
 *
 * @param dst structure to initialize
 * @param _name file name
 * @param _macType extension for MacOS
 * @param _dosType extension for Windows
 * @param _unixType extension for Unix
 * @param _mimeType1 MIME type
 * @param _mimeType2 Additional MIME type
 */
inline void vst_init_file_type(
    struct VstFileType *dst,
    const char* _name, const char* _macType, const char* _dosType,
    const char* _unixType, const char* _mimeType1, const char* _mimeType2
)
{
    if (_name)
        vst_strncpy (dst->name, _name, 127);
    else
        dst->name[0] = '\0';

    if (_macType)
        vst_strncpy (dst->macType, _macType, 7);
    else
        dst->macType[0] = '\0';

    if (_dosType)
        vst_strncpy (dst->dosType, _dosType, 7);
    else
        dst->dosType[0] = '\0';

    if (_unixType)
        vst_strncpy (dst->unixType, _unixType, 7);
    else
        dst->unixType[0] = '\0';

    if (_mimeType1)
        vst_strncpy (dst->mimeType1, _mimeType1, 127);
    else
        dst->mimeType1[0] = '\0';

    if (_mimeType2)
        vst_strncpy (dst->mimeType2, _mimeType2, 127);
    else
        dst->mimeType2[0] = '\0';
}

// Only C++ specific functions
#ifdef __cplusplus
    /** Cast the integer argument to pointer type
     *
     * @param arg the integer argument
     * @return the casted address
     */
    template <class T> inline T* FromVstPtr (const VstIntPtr& arg)
    {
        union
        {
            VstIntPtr src;
            T *dst;
        } conv;

        conv.src = arg;
        return conv.dst;
    }

    /** Cast pointer type to the integer argument type
     *
     * @param ptr pointer to cast
     * @return
     */
    template <class T> inline VstIntPtr ToVstPtr (const T* ptr)
    {
        union
        {
            const T *src;
            VstIntPtr dst;
        } conv;

        conv.src = ptr;
        return conv.dst;
    }

    inline VstFileType::VstFileType(
        const char* _name, const char* _macType, const char* _dosType,
        const char* _unixType, const char* _mimeType1, const char* _mimeType2
    )
    {
        vst_init_file_type(this, _name, _macType, _dosType, _unixType, _mimeType1, _mimeType2);
    }
#endif /** __cplusplus **/

//-------------------------------------------------------------------------------------------------------
// Main function definition
//-------------------------------------------------------------------------------------------------------
/** Main routine prototype
 *
 * @param audioMaster audio master callback interface
 * @return plugin instance
 */
VST_C_EXTERN typedef AEffect*  (*VstPluginMainProc)(audioMasterCallback audioMaster);

/** Main routine macro alias
 *
 * @param audioMaster audio master callback parameter
 * @return
 */
#define VST_MAIN(audioMaster) VST_C_EXTERN VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster)

//-------------------------------------------------------------------------------------------------------
// Restore compilation options
//-------------------------------------------------------------------------------------------------------
#if TARGET_API_MAC_CARBON
    #pragma options align=reset
#elif defined(WIN32) || defined(__FLAT__) || defined(__GNUC__)
    #pragma pack(pop)
#elif defined __BORLANDC__
    #pragma -a-
#endif

#endif /* _3RDPARTY_STEINBERG_VST2_H_ */
