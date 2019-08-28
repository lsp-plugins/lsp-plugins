/*
 * AudioFile.h
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_AUDIOFILE_H_
#define CORE_FILES_AUDIOFILE_H_

#include <core/types.h>
#include <core/sampling/Sample.h>
#include <core/LSPString.h>
#include <core/io/Path.h>

namespace lsp
{
    class AudioFile
    {
        private:
            typedef struct file_content_t
            {
                size_t      nChannels;
                size_t      nSamples;       // Actual number of samples
                size_t      nSampleRate;
                float      *vChannels[];
            } file_content_t;

            typedef struct temporary_buffer_t
            {
                size_t      nSize;          // Size in bytes
                size_t      nCapacity;      // Capacity in bytes
                size_t      nFrameSize;     // Frame size in bytes
                size_t      nChannels;      // Total number of channels
                uint8_t    *bData;          // Data for reading/writing (byte pointer)
                float      *vChannels[];    // Pointer to deploy samples to channels
            } temporary_buffer_t;

            file_content_t *pData;

        private:
            AudioFile & operator = (const AudioFile &);     // Deny copying

        protected:
            static file_content_t *create_file_content(size_t channels, size_t samples);
            static file_content_t *grow_file_content(file_content_t *src, size_t samples);
            static void destroy_file_content(file_content_t *content);

            static temporary_buffer_t *create_temporary_buffer(file_content_t *content, size_t from = 0);
            static void flush_temporary_buffer(temporary_buffer_t *buffer);
            static size_t fill_temporary_buffer(temporary_buffer_t *buffer, size_t max_samples);
            static void destroy_temporary_buffer(temporary_buffer_t *buffer);

            status_t fast_downsample(size_t new_sample_rate);
            status_t fast_upsample(size_t new_sample_rate);
            status_t complex_upsample(size_t new_sample_rate);
            status_t complex_downsample(size_t new_sample_rate);

            status_t load_lspc(const LSPString *path, float max_duration);

        #ifdef PLATFORM_WINDOWS
            status_t load_mfapi(const LSPString *path, float max_duration);
            status_t save_mfapi(const LSPString *path, size_t from, size_t max_count);
            status_t load_mmio(const LSPString *path, float max_duration);
            status_t save_mmio(const LSPString *path, size_t from, size_t max_count);
        #else
            status_t load_sndfile(const LSPString *path, float max_duration);
            status_t save_sndfile(const LSPString *path, size_t from, size_t max_count);
        #endif /* PLATFORM_WINDOWS */

        public:
            explicit AudioFile();
            ~AudioFile();

        public:
            /** Create file
             *
             * @param channels number of channels
             * @param sample_rate sample rate of the file
             * @param count the duration of the file in samples
             * @return status of operation
             */
            status_t create_samples(size_t channels, size_t sample_rate, size_t count);

            /** Create file from sample
             *
             * @param sample audio sample to create file
             * @param sample_rate sample rate
             * @return status of operation
             */
            status_t create(const Sample *sample, size_t sample_rate);

            /** Create file
             *
             * @param channels number of channels
             * @param sample_rate sample rate of the file
             * @param duration the duration of the file in seconds
             * @return status of operation
             */
            status_t create(size_t channels, size_t sample_rate, float duration);

            /** Load file
             *
             * @param path path to the file
             * @param max_duration maximum duration of the file to load (in seconds)
             * @return status of operation
             */
            status_t load(const char *path, float max_duration = -1);

            /** Load file
             *
             * @param path path to the file
             * @param max_duration maximum duration of the file to load (in seconds)
             * @return status of operation
             */
            status_t load(const LSPString *path, float max_duration = -1);

            /** Load file
             *
             * @param path path to the file
             * @param max_duration maximum duration of the file to load (in seconds)
             * @return status of operation
             */
            status_t load(const io::Path *path, float max_duration = -1);

            /** Save file
             *
             * @param path path to the file
             * @param from offset sample from which store the data
             * @param max_count maximum duration of the file to store (in samples)
             * @return status of operation
             */
            status_t store_samples(const char *path, size_t from, size_t max_count);

            /** Save file
             *
             * @param path path to the file
             * @param from offset sample from which store the data
             * @param max_count maximum duration of the file to store (in samples)
             * @return status of operation
             */
            status_t store_samples(const io::Path *path, size_t from, size_t max_count);

            /** Save file
             *
             * @param path path to the file
             * @param from offset sample from which store the data
             * @param max_count maximum duration of the file to store (in samples)
             * @return status of operation
             */
            status_t store_samples(const LSPString *path, size_t from, size_t max_count);

            /** Save file
             *
             * @param path path to the file
             * @param max_count maximum duration of the file to store (in samples)
             * @return status of operation
             */
            status_t store_samples(const char *path, size_t max_count);

            /** Save file
             *
             * @param path path to the file
             * @param max_count maximum duration of the file to store (in samples)
             * @return status of operation
             */
            status_t store_samples(const io::Path *path, size_t max_count);

            /** Save file
             *
             * @param path path to the file
             * @param max_count maximum duration of the file to store (in samples)
             * @return status of operation
             */
            status_t store_samples(const LSPString *path, size_t max_count);

            /** Save file
             *
             * @param path path to the file
             * @param max_duration maximum duration of the file to store (in seconds)
             * @return status of operation
             */
            status_t store(const char *path, float max_duration = -1);

            /** Save file
             *
             * @param path path to the file
             * @param max_duration maximum duration of the file to store (in seconds)
             * @return status of operation
             */
            status_t store(const LSPString *path, float max_duration = -1);

            /** Save file
             *
             * @param path path to the file
             * @param max_duration maximum duration of the file to store (in seconds)
             * @return status of operation
             */
            status_t store(const io::Path *path, float max_duration = -1);

            /** Return number of channels
             *
             * @return number of channels
             */
            size_t channels() const;

            /** Return number of samples per track
             *
             * @return number of samples per track
             */
            size_t samples() const;

            /** Return sample rate of the file
             *
             * @return sample rate
             */
            size_t sample_rate() const;

            /** Get track
             *
             * @param track number of track
             * @return pointer to the track data or NULL
             */
            float *channel(size_t track);

            /**
             * Convert data to sample data
             * @param dst destination sample to store data
             * @return status of operation
             */
            status_t convert_to_sample(Sample *dst);

            /** Reverse track(s)
             *
             * @param track_id ID of track, track_id < 0 - all tracks
             * @param convolution build aligned convolution
             * @return true on success
             */
            bool reverse(ssize_t track_id = -1);

            /** Resample file
             *
             * @param new_sample_rate new sample rate
             * @return status of operation
             */
            status_t resample(size_t new_sample_rate);

            /** Destroy all previously allocated file data
             *
             */
            void destroy();
    };

} /* namespace lsp */

#endif /* CORE_FILES_AUDIOFILE_H_ */
