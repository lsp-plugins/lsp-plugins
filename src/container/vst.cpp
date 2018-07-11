#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <alloca.h>
#include <dlfcn.h>
#include <time.h>

// Core include
#include <core/metadata.h>
#include <core/lib.h>
#include <plugins/plugins.h>

#include <data/cvector.h>

// UI includes
#include <ui/ui.h>

#if defined(LSP_UI_GTK2)
    #include <ui/gtk2/ui.h>
    #define LSP_PACKAGE gtk2
    #define LSP_WIDGET_FACTORY Gtk2WidgetFactory
#elif defined(LSP_UI_GTK3)
    #include <ui/gtk3/ui.h>
    #define LSP_PACKAGE gtk3
    #define LSP_WIDGET_FACTORY Gtk3WidgetFactory
#endif /* LSP_UI_GTK3 */

// VST SDK includes
#include <container/vst/defs.h>
#include <container/vst/helpers.h>
#include <container/vst/wrapper.h>
#include <container/vst/ports.h>

namespace lsp
{
    IWidgetFactory *vst_create_widget_factory(const char *bundle_path, VstInt32 uid)
    {
        // Instantiate widget factory (if possible)
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/" LSP_ARTIFACT_ID ".vst", bundle_path);
        lsp_trace("path=%s", path);

        #define UI_MODULE(plugin)   \
            if (uid == vst_cconst(plugin::metadata.vst_uid)) \
            { \
                IWidgetFactory *wf  = new LSP_WIDGET_FACTORY(path); \
                if (wf != NULL) \
                    return wf; \
            }

        // Define module macro and include modules
        #if defined(LSP_UI_GTK2)
            #define MOD_GTK2(plugin)    UI_MODULE(plugin)
        #elif defined(LSP_UI_GTK3)
            #define MOD_GTK3(plugin)    UI_MODULE(plugin)
        #endif /* LSP_UI_GTK3 */

        #include <core/modules.h>

        #undef UI_MODULE

        return NULL;
    }

    bool vst_has_widget_factory(VstInt32 uid)
    {
        #define UI_MODULE(plugin)   \
            if (uid == vst_cconst(plugin::metadata.vst_uid)) \
                return true;

        // Define module macro and include modules
        #if defined(LSP_UI_GTK2)
            #define MOD_GTK2(plugin)    UI_MODULE(plugin)
        #elif defined(LSP_UI_GTK3)
            #define MOD_GTK3(plugin)    UI_MODULE(plugin)
        #endif /* LSP_UI_GTK3 */

        #include <core/modules.h>

        #undef UI_MODULE

        return false;
    }

    void vst_finalize(AEffect *e)
    {
        lsp_trace("vst_finalize effect=%p", e);
        if (e == NULL)
            return;

        // Get VST object
        VSTWrapper *w     = reinterpret_cast<VSTWrapper *>(e->object);
        if (w != NULL)
        {
            w->destroy();
            delete w;

            e->object           = NULL;
        }

        // Delete audio effect
        delete e;
    }

    void vst_get_parameter_properties(const port_t *m, VstParameterProperties *p)
    {
        lsp_trace("parameter id=%s, name=%s", m->id, m->name);

        float min = 0.0f, max = 1.0f, step = 0.001f;
        get_port_parameters(m, &min, &max, &step);

        p->stepFloat                = (max != min) ? step / (max - min) : 0.0f;
        p->smallStepFloat           = step;
        p->largeStepFloat           = step;
        vst_strncpy(p->label, m->name, kVstMaxLabelLen);
        p->flags                    = 0; // TODO
        p->minInteger               = min;
        p->maxInteger               = max;
        p->stepInteger              = step;
        p->largeStepInteger         = step;

        vst_strncpy(p->shortLabel, m->id, kVstMaxShortLabelLen);

//        // This code may crash the hosts that use VesTige, so for capability issues it is commented
//        p->displayIndex             = 0;
//        p->category                 = 0;
//        p->numParametersInCategory  = 0;
//        p->reserved                 = 0;
//        p->categoryLabel[0]         = '\0';

        if (m->unit == U_BOOL)
            p->flags                    = kVstParameterIsSwitch | kVstParameterUsesIntegerMinMax | kVstParameterUsesIntStep;
        else if (m->unit == U_ENUM)
            p->flags                    = kVstParameterUsesIntegerMinMax | kVstParameterUsesIntStep;
        else if (m->unit == U_SAMPLES)
            p->flags                    = kVstParameterUsesIntegerMinMax | kVstParameterUsesIntStep;
        else
        {
            if (m->flags & F_INT)
                p->flags                    = kVstParameterUsesIntegerMinMax | kVstParameterUsesIntStep;
            else
                p->flags                    = kVstParameterUsesFloatStep;
        }
    }

#ifdef LSP_DEBUG
    const char *vst_decode_opcode(VstInt32 opcode)
    {
        const char *r = NULL;

    #define C(code) case code: r = #code; break;
    #define D(code) case DECLARE_VST_DEPRECATED(code): r = #code "(deprecated)"; break;

        switch (opcode)
        {
            C(effGetVstVersion)
            C(effClose)
            C(effGetVendorString)
            C(effGetProductString)
            C(effGetParamName)
            C(effGetParamLabel)
            C(effGetParamDisplay)
            C(effCanBeAutomated)
            C(effGetParameterProperties)
            C(effSetSampleRate)
            C(effOpen)
            C(effMainsChanged)
            C(effSetProgram)
            C(effGetProgram)
            C(effSetProgramName)
            C(effGetProgramName)
            C(effSetBlockSize)
            C(effEditGetRect)
            C(effEditOpen)
            C(effEditClose)
            C(effEditIdle)
            C(effGetChunk)
            C(effSetChunk)
            C(effProcessEvents)
            C(effString2Parameter)
            C(effGetProgramNameIndexed)
            C(effGetInputProperties)
            C(effGetOutputProperties)
            C(effGetPlugCategory)
            C(effOfflineNotify)
            C(effOfflinePrepare)
            C(effOfflineRun)
            C(effGetVendorVersion)
            C(effVendorSpecific)
            C(effProcessVarIo)
            C(effSetSpeakerArrangement)
            C(effSetBypass)
            C(effGetEffectName)
            C(effGetTailSize)
            C(effCanDo)

    #if VST_2_1_EXTENSIONS
            C(effEditKeyDown)
            C(effEditKeyUp)
            C(effSetEditKnobMode)
            C(effGetMidiProgramName)
            C(effGetCurrentMidiProgram)
            C(effGetMidiProgramCategory)
            C(effHasMidiProgramsChanged)
            C(effGetMidiKeyName)
            C(effBeginSetProgram)
            C(effEndSetProgram)
    #endif // VST_2_1_EXTENSIONS

    #if VST_2_3_EXTENSIONS
            C(effGetSpeakerArrangement)
            C(effShellGetNextPlugin)
            C(effStartProcess)
            C(effStopProcess)
            C(effSetTotalSampleToProcess)
            C(effSetPanLaw)
            C(effBeginLoadBank)
            C(effBeginLoadProgram)
    #endif // VST_2_3_EXTENSIONS

    #if VST_2_4_EXTENSIONS
            C(effSetProcessPrecision)
            C(effGetNumMidiInputChannels)
            C(effGetNumMidiOutputChannels)
    #endif // VST_2_4_EXTENSIONS

            // DEPRECATED STUFF
            D(effGetVu)
            D(effEditDraw)
            D(effEditMouse)
            D(effEditKey)
            D(effEditTop)
            D(effEditSleep)
            D(effIdentify)
            D(effGetNumProgramCategories)
            D(effCopyProgram)
            D(effConnectInput)
            D(effConnectOutput)
            D(effGetCurrentPosition)
            D(effGetDestinationBuffer)
            D(effSetBlockSizeAndSampleRate)
            D(effGetErrorText)
            D(effIdle)
            D(effGetIcon)
            D(effSetViewPosition)
            D(effKeysRequired)

            default:
                r = "unknown";
                break;
        }
    #undef C
    #undef D
        return r;
    }
#endif /* LSP_DEBUG */

    VstIntPtr vst_get_category(const int *classes)
    {
        VstIntPtr result    = kPlugCategUnknown;

        while ((classes != NULL) && ((*classes) >= 0))
        {
            switch (*classes)
            {
                case C_DELAY:
                case C_REVERB:
                    result = kPlugCategRoomFx;
                    break;

                case C_GENERATOR:
                case C_CONSTANT:
                case C_OSCILLATOR:
                case C_ENVELOPE:
                    result = kPlugCategGenerator;
                    break;

                case C_INSTRUMENT:
                    result = kPlugCategSynth;
                    break;

                case C_DISTORTION:
                case C_WAVESHAPER:
                case C_AMPLIFIER:
                case C_FILTER:
                case C_ALLPASS:
                case C_BANDPASS:
                case C_COMB:
                case C_EQ:
                case C_MULTI_EQ:
                case C_PARA_EQ:
                case C_HIGHPASS:
                case C_LOWPASS:
                case C_MODULATOR:
                case C_CHORUS:
                case C_FLANGER:
                case C_PHASER:
                case C_SPECTRAL:
                case C_PITCH:
                case C_MIXER:
                    result = kPlugCategEffect;
                    break;

                case C_UTILITY:
                case C_ANALYSER:
                    result = kPlugCategAnalysis;
                    break;

                case C_DYNAMICS:
                case C_COMPRESSOR:
                case C_EXPANDER:
                case C_GATE:
                case C_LIMITER:
                    result = kPlugCategMastering;
                    break;

                case C_SPATIAL:
                    result = kPlugCategSpacializer;
                    break;

                case C_FUNCTION:
                case C_SIMULATOR:
                case C_CONVERTER:
                    result = kPlugCategRestoration;
                    break;

                // NOT SUPPORTED
//                    result = kPlugSurroundFx;            ///< Dedicated surround processor
//                    break;
//
//                    result = kPlugCategOfflineProcess;   ///< Offline Process
//                    break;
//
//                    result = kPlugCategShell;            ///< Plug-in is container of other plug-ins  @see effShellGetNextPlugin
//                    break;
                default:
                    break;
            }


            if (result != kPlugCategUnknown)
                break;

            classes++;
        }

        return result;
    }

    VstIntPtr VSTCALLBACK vst_dispatcher(AEffect* e, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
    {
        VstIntPtr v = 0;

        if (opcode != effEditIdle)
        lsp_trace("vst_dispatcher effect=%p, opcode=%d (%s), index=%d, value=%llx, ptr=%p, opt = %.3f",
                e, opcode, vst_decode_opcode(opcode), index, (long long)(value), ptr, opt);

        // Get VST object
        VSTWrapper *w   = reinterpret_cast<VSTWrapper *>(e->object);

        switch (opcode)
        {
            case effGetVstVersion: // Get VST version of plugin
                lsp_trace("vst_version = %d", int(kVstVersion));
                v   = kVstVersion;
                break;

            case effClose: // Finalize the plugin
                if (e != NULL)
                    vst_finalize(e);
                v = 1;
                break;

            case effGetVendorString: // Get vendor string
                vst_strncpy(reinterpret_cast<char *>(ptr), LSP_ACRONYM " [VST]" , kVstMaxVendorStrLen);
                lsp_trace("vendor_string = %s", reinterpret_cast<char *>(ptr));
                v = 1;
                break;

            case effGetEffectName: // Get effect name
            {
                const plugin_metadata_t *m = w->get_metadata();
                if (m != NULL)
                {
                    char *dst = reinterpret_cast<char *>(ptr);
                    vst_strncpy(dst, m->name, kVstMaxEffectNameLen);
                    lsp_trace("product_string = %s", dst);
                    v = 1;
                }
                break;
            }

            case effGetProductString: // Get product string
            {
                const plugin_metadata_t *m = w->get_metadata();
                if (m != NULL)
                {
                    char *dst = reinterpret_cast<char *>(ptr);
                    snprintf(dst, kVstMaxProductStrLen, LSP_ACRONYM " %s [VST]", m->name);
                    dst[kVstMaxProductStrLen - 1] = '\0';
                    lsp_trace("product_string = %s", dst);
                    v = 1;
                }
                break;
            }

            case effGetParamName: // Get parameter name
            case effGetParamLabel: // Get units of the parameter
            case effGetParamDisplay: // Get value of the parameter
            {
                VSTParameterPort *p = w->get_parameter(index);
                if (p == NULL)
                    break;

                const port_t *m = p->metadata();
                if (m == NULL)
                    break;

                char *dst = reinterpret_cast<char *>(ptr);

                if (opcode == effGetParamName)
                {
                    vst_strncpy(dst, m->id, kVstMaxParamStrLen);
                    lsp_trace("param_name = %s", dst);
                    if (strcmp(dst, m->id) != 0)
                        lsp_warn("parameter name was trimmed from %s to %s !!!", m->id, dst);
                }
                else if (opcode == effGetParamLabel)
                {
                    const char *label = encode_unit((is_decibel_unit(m->unit)) ? U_DB : m->unit);
                    if (label != NULL)
                        vst_strncpy(dst, label, kVstMaxParamStrLen);
                    else
                        dst[0] = '\0';
                    lsp_trace("param_label = %s", dst);
                }
                else
                {
                    format_value(dst, kVstMaxParamStrLen, m, p->getValue());
                    lsp_trace("param_display = %s", dst);
                }
                v = 1;
                break;
            }

            case effCanBeAutomated:
            case effGetParameterProperties: // Parameter properties
            {
                VSTParameterPort *p = w->get_parameter(index);
                if (p == NULL)
                    break;

                const port_t *m = p->metadata();
                if (m == NULL)
                    break;

                if (opcode == effGetParameterProperties)
                {
                    vst_get_parameter_properties(m, reinterpret_cast<VstParameterProperties *>(ptr));
                    v = 1;
                }
                else if ((opcode == effCanBeAutomated) && (!(m->flags & F_OUT)))
                    v = 1;

                break;
            }

            case DECLARE_VST_DEPRECATED (effSetBlockSizeAndSampleRate): // Set block size and sample rate
            case effSetSampleRate: // Set sample rate, always in suspended mode
                if (opt > MAX_SAMPLE_RATE)
                {
                    lsp_error("Unsupported sample rate: %f, maximum supported sample rate is %ld", float(opt), long(MAX_SAMPLE_RATE));
                    opt = MAX_SAMPLE_RATE;
                }
                w->set_sample_rate(opt);
                break;

            case effOpen: // Plugin initialization
                w->open();
                break;

            case effMainsChanged: // Plugin activation/deactivation
                w->mains_changed(value);
                break;

            case effGetPlugCategory:
            {
                const plugin_metadata_t *m = w->get_metadata();
                if (m == NULL)
                    break;

                v = vst_get_category(m->classes);
                lsp_trace("plugin_category = %d", int(v));
                break;
            }

            case effEditOpen: // Run editor
            {
                IWidgetFactory *wf = vst_create_widget_factory(w->get_bundle_path(),  e->uniqueID);
                if (wf == NULL)
                    break;
                if (w->show_ui(ptr, wf))
                    v = 1;
                break;
            }

            case effEditClose: // Close editor
                w->hide_ui();
                v = 1;
                break;

            case effEditIdle: // Run editor's iteration
                w->iterate_ui();
                v = 1;
                break;

            case effEditGetRect: // Return UI dimensions
            {
                ERect **er = reinterpret_cast<ERect **>(ptr);
                *er = w->get_ui_rect();
                v = 1;
                break;
            }

            case effSetProgram:
            case effGetProgram:
            case effSetProgramName:
            case effGetProgramName:
            case effSetBlockSize:
            case effGetChunk:
            case effSetChunk:
                break;

            case effProcessEvents:
            case effString2Parameter:
            case effGetProgramNameIndexed:
            case effGetInputProperties:
            case effGetOutputProperties:
            case effOfflineNotify:
            case effOfflinePrepare:
            case effOfflineRun:
            case effGetVendorVersion:
            {
                const plugin_metadata_t *m = w->get_metadata();
                if (m != NULL)
                    v = vst_version(m->version);
                break;
            }
            case effVendorSpecific:
            case effProcessVarIo:
            case effSetSpeakerArrangement:
            case effSetBypass:
            case effGetTailSize:
                break;
            case effCanDo:
                lsp_trace("can_do request: %s\n", reinterpret_cast<const char *>(ptr));
                break;

#if VST_2_1_EXTENSIONS
            case effEditKeyDown:
            case effEditKeyUp:
            case effSetEditKnobMode:

            case effGetMidiProgramName:
            case effGetCurrentMidiProgram:
            case effGetMidiProgramCategory:
            case effHasMidiProgramsChanged:
            case effGetMidiKeyName:

            case effBeginSetProgram:
            case effEndSetProgram:
                break;
#endif // VST_2_1_EXTENSIONS

#if VST_2_3_EXTENSIONS
            case effGetSpeakerArrangement:
            case effShellGetNextPlugin:

            case effStartProcess:
            case effStopProcess:
            case effSetTotalSampleToProcess:
            case effSetPanLaw:

            case effBeginLoadBank:
            case effBeginLoadProgram:
                break;
#endif // VST_2_3_EXTENSIONS

#if VST_2_4_EXTENSIONS
            case effSetProcessPrecision:    // Currently no double-precision processing supported
                v   = 0;
                break;
            case effGetNumMidiInputChannels:
            case effGetNumMidiOutputChannels:
                break;
#endif // VST_2_4_EXTENSIONS

            // DEPRECATED STUFF
            case DECLARE_VST_DEPRECATED (effIdentify):  v = VST_IDENTIFY_MAGIC;    break;

            case DECLARE_VST_DEPRECATED (effGetVu):
            case DECLARE_VST_DEPRECATED (effEditDraw):
            case DECLARE_VST_DEPRECATED (effEditMouse):
            case DECLARE_VST_DEPRECATED (effEditKey):
            case DECLARE_VST_DEPRECATED (effEditTop):
            case DECLARE_VST_DEPRECATED (effEditSleep):
            case DECLARE_VST_DEPRECATED (effGetNumProgramCategories):
            case DECLARE_VST_DEPRECATED (effCopyProgram):
            case DECLARE_VST_DEPRECATED (effConnectInput):
            case DECLARE_VST_DEPRECATED (effConnectOutput):
            case DECLARE_VST_DEPRECATED (effGetCurrentPosition):
            case DECLARE_VST_DEPRECATED (effGetDestinationBuffer):
            case DECLARE_VST_DEPRECATED (effGetErrorText):
            case DECLARE_VST_DEPRECATED (effIdle):
            case DECLARE_VST_DEPRECATED (effGetIcon):
            case DECLARE_VST_DEPRECATED (effSetViewPosition):
            case DECLARE_VST_DEPRECATED (effKeysRequired):
                break;

            default:
                break;
        }
        return v;
    }

    void VSTCALLBACK vst_process(AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames)
    {
//        lsp_trace("vst_process effect=%p, inputs=%p, outputs=%p, frames=%d", effect, inputs, outputs, int(sampleFrames));
        VSTWrapper *w     = reinterpret_cast<VSTWrapper *>(effect->object);
        w->run_legacy(inputs, outputs, sampleFrames);
    }

    void VSTCALLBACK vst_process_replacing(AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames)
    {
//        lsp_trace("vst_process effect=%p, inputs=%p, outputs=%p, frames=%d", effect, inputs, outputs, int(sampleFrames));
        VSTWrapper *w     = reinterpret_cast<VSTWrapper *>(effect->object);
        w->run(inputs, outputs, sampleFrames);
    }

    void VSTCALLBACK vst_set_parameter(AEffect* effect, VstInt32 index, float value)
    {
        lsp_trace("vst_set_parameter effect=%p, index=%d, value=%.3f", effect, int(index), value);

        // Get VST object
        VSTWrapper *w     = reinterpret_cast<VSTWrapper *>(effect->object);
        if (w == NULL)
            return;

        // Get VST parameter port
        VSTParameterPort *vp = w->get_parameter(index);
        if (vp == NULL)
            return;

        // Set value of plugin port
        vp->setVstValue(value);
    }

    float VSTCALLBACK vst_get_parameter(AEffect* effect, VstInt32 index)
    {
//        lsp_trace("vst_get_parameter effect=%p, index=%d", effect, int(index));

        // Get VST object
        VSTWrapper *w     = reinterpret_cast<VSTWrapper *>(effect->object);
        if (w == NULL)
            return 0.0f;

        // Get port and apply parameter
        VSTParameterPort *vp = w->get_parameter(index);

        if (vp != NULL)
            return vp->getVstValue();

        return 0.0f;
    }

    AEffect *vst_instantiate(const char *bundle_path, VstInt32 uid, audioMasterCallback callback)
    {
        // Initialize DSP
        dsp::init();

        // Instantiate plugin
        const plugin_metadata_t *m  = NULL;
        const char *plugin_name     = NULL;
        plugin_t *p                 = NULL;

        #define MOD_VST(plugin) \
            if ((!p) && (uid == vst_cconst(plugin::metadata.vst_uid))) \
            { \
                p   = new plugin(); \
                if (p == NULL) \
                    return NULL; \
                m   = &plugin::metadata; \
                plugin_name = #plugin; \
            }
        #include <core/modules.h>

        // Check that plugin instance is available
        if (p == NULL)
            return NULL;

        lsp_trace("Instantiated plugin %s - %s", m->name, m->description);
        lsp_trace("bundle_path = %s", bundle_path);

        // Create effect descriptor
        AEffect *e                  = new AEffect;
        if (e == NULL)
        {
            delete p;
            return NULL;
        }

        // Create wrapper
        VSTWrapper *w               = new VSTWrapper(e, p, bundle_path, plugin_name, callback);
        if (w == NULL)
        {
            vst_finalize(e);
            delete p;
            return NULL;
        }

        // Initialize effect structure
        ::memset(e, 0, sizeof(AEffect));

        // Fill effect with values depending on metadata
        e->magic                            = kEffectMagic;
        e->dispatcher                       = vst_dispatcher;
        e->DECLARE_VST_DEPRECATED(process)  = vst_process;
        e->setParameter                     = vst_set_parameter;
        e->getParameter                     = vst_get_parameter;
        e->numPrograms                      = 0;
        e->numParams                        = 0;
        e->numInputs                        = 0;
        e->numOutputs                       = 0;
        e->flags                            = effFlagsCanReplacing;
        e->initialDelay                     = 0;
        e->object                           = w;
        e->user                             = NULL;
        e->uniqueID                         = vst_cconst(m->vst_uid);
        e->version                          = vst_version(m->version);
        e->processReplacing                 = vst_process_replacing;

        // Currently no double-replacing
        #ifdef VST_2_4_EXTENSIONS
            e->processDoubleReplacing       = NULL;
        #endif /* VST_2_4_EXTENSIONS */

        // Additional flags
        if (vst_has_widget_factory(uid))
            e->flags                        |= effFlagsHasEditor; // Has custom UI

        // Initialize plugin and wrapper
        w->init();

        return e;
    }
}

extern "C"
{
    AEffect *VST_CREATE_INSTANCE_NAME(const char *bundle_path, VstInt32 uid, audioMasterCallback callback)
    {
        return lsp::vst_instantiate(bundle_path, uid, callback);
    }
}
