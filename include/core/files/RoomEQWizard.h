/*
 * RoomEQWizard.h
 *
 *  Created on: 6 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_ROOMEQWIZARD_H_
#define CORE_FILES_ROOMEQWIZARD_H_

#include <common/types.h>
#include <core/status.h>
#include <core/io/Path.h>
#include <core/io/File.h>
#include <core/io/IInStream.h>

namespace lsp
{
    namespace room_ew
    {
        enum filter_type_t
        {
            NONE,
            PK,     // peaking (parametric) filter
            MODAL,  // Modal filter
            LP,     // 12dB/octave Low Pass filter (Q=0.7071)
            HP,     // 12dB/octave High Pass filter (Q=0.7071)
            LPQ,    // 12dB/octave Low Pass filter with adjustable Q
            HPQ,    // 12dB/octave High Pass filter with adjustable Q
            LS,     // Low Shelf filter
            HS,     // High Shelf filter
            LS6,    // 6dB/octave Low Shelf filter
            HS6,    // 6dB/octave High Shelf filter
            LS12,   // 12dB/octave Low Shelf filter
            HS12,   // 12dB/octave High Shelf filter
            NO,     // notch filter
            AP      // allpass filter
        };

        typedef struct filter_t
        {
            double          Q;
            double          fc;
            double          gain;
            filter_type_t   filterType;
            bool            enabled;

// Other parameters will be used some times... maybe
//            double          A;
//            double          a[3];
//            double          aC[3];
//            double          aSum;
//            double          alpha;
//            double          b[3];
//            double          bC[3];
//            double          bSum;
//            double          betasn;
//            int32_t         control;
//            double          cs;
//            double          freqNorm;
//            double          omega;
//            float           optMaxFreq;
//            float           optMinFreq;
//            double          p[5];
//            double          sTh[2];
//            double          sampleRate;
//            double          sn;
//            double          t60Target;
//            int32_t         type;
        } filter_t;

        typedef struct config_t
        {
            int32_t         nVerMaj;       // Major version
            int32_t         nVerMin;       // Minor version
            const char     *sEqType;    // Equalizer type
            const char     *sNotes;     // Notes
            size_t          nFilters;   // Number of filters
            filter_t       *vFilters;   // List of filters
        } config_t;

        /**
         * Load configuration file
         * @param path UTF-8 encoded location of the file
         * @param dst pointer to store configuration,
         *   configuration should be free()'d after use
         * @return status of operation
         */
        status_t load(const char *path, config_t **dst);

        /**
         * Load configuration file
         * @param path location of the file
         * @param dst pointer to store configuration,
         *   configuration should be free()'d after use
         * @return status of operation
         */
        status_t load(const LSPString *path, config_t **dst);

        /**
         * Load configuration file
         * @param path location of the file
         * @param dst pointer to store configuration,
         *   configuration should be free()'d after use
         * @return status of operation
         */
        status_t load(const io::Path *path, config_t **dst);

        /**
         * Load configuration from file
         * @param fd file handle
         * @param dst pointer to store configuration,
         *   configuration should be free()'d after use
         * @return status of operation
         */
        status_t load(FILE *fd, config_t **dst);

        /**
         * Load configuration from character input stream
         * @param input stream pointer
         * @param dst pointer to store configuration,
         *   configuration should be free()'d after use
         * @return status of operation
         */
        status_t load(io::IInStream *is, config_t **dst);

        /**
         * Load configuration from file
         * @param input stream pointer
         * @param dst pointer to store configuration,
         *   configuration should be free()'d after use
         * @return status of operation
         */
        status_t load(io::File *fd, config_t **dst);

        /**
         * Load configuration from data chunk
         * @param data buffer with data
         * @param size size of buffer
         * @param dst pointer to store configuration,
         *   configuration should be free()'d after use
         * @return status of operation
         */
        status_t load(const void *data, size_t size, config_t **dst);
    }
}

#endif /* CORE_FILES_ROOMEQWIZARD_H_ */
