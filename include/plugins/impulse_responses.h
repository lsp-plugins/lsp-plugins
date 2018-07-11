/*
 * impulse_responses.h
 *
 *  Created on: 21 янв. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_PLUGINS_IMPULSE_RESPONSES_H_
#define CORE_PLUGINS_IMPULSE_RESPONSES_H_

#ifndef LSP_NO_EXPERMIENTAL

#include <core/plugin.h>
#include <core/plugin_metadata.h>

#include <core/IExecutor.h>
#include <core/AudioFile.h>
#include <core/Convolver.h>
#include <core/Bypass.h>

namespace lsp
{

    class impulse_responses_base: public plugin_t
    {
        protected:
            struct af_descriptor_t;

            class IRLoader: public ITask
            {
                private:
                    impulse_responses_base     *pCore;
                    af_descriptor_t            *pDescr;

                public:
                    IRLoader(impulse_responses_base *base, af_descriptor_t *descr);
                    virtual ~IRLoader();

                public:
                    int run();
            };

            struct af_descriptor_t
            {
                AudioFile          *pCurr;          // Currently used audio file
                AudioFile          *pNew;           // Audio file pending for change
                AudioFile          *pOld;           // Old audio file for deletion
                IRLoader           *pLoader;        // Audio file loader task
                IPort              *pPort;          // Audio file port
            };

            struct ac_descriptor_t
            {
                ShiftBuffer         vBuffer;        // Shift buffer for convolution history
                Bypass              vBypass;        // Bypass control
                IPort              *pInput;         // Input audio port
                IPort              *pOutput;        // Output audio port
                const float        *pConv;          // Pointer to start of convolution
                size_t              nConvLen;       // Length of convolution in samples
                size_t              nFile;          // Input file
                size_t              nChannel;       // Input channel
                float               fLength;        // Length of convolution in percent
                float               fDry;           // Amount of dry signal
                float               fWet;           // Amount of wet signal
            };

            size_t              nMaxSamples;        // Them maximum number of samples per file
            float              *vBuffer;            // Temporary buffer for post-processing

        protected:
            int                     load(af_descriptor_t *descr);
            af_descriptor_t        *create_files(size_t count);
            void                    destroy_files(af_descriptor_t *files, size_t count);

            ac_descriptor_t        *create_channels(size_t count);
            void                    destroy_channels(ac_descriptor_t *channels, size_t count);

        protected:
            IExecutor              *pExecutor;
            size_t                  nChannels;
            size_t                  nFiles;
            af_descriptor_t        *vFiles;
            ac_descriptor_t        *vChannels;

        public:
            impulse_responses_base(const plugin_metadata_t &metadata);
            virtual ~impulse_responses_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);

            virtual void reconfigure();
    };

    class impulse_responses_mono: public impulse_responses_base, public impulse_responses_metadata
    {
        private:
            AudioFile       sFile;
            Convolver       sConvolver;

        public:
            impulse_responses_mono();
            virtual ~impulse_responses_mono();

        public:
            virtual void init(IWrapper *wrapper);

            virtual void reconfigure();
    };

} /* namespace ddb */

#endif /* LSP_NO_EXPERMIENTAL */

#endif /* CORE_PLUGINS_IMPULSE_RESPONSES_H_ */
