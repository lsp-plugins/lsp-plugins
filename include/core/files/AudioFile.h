/*
 * AudioFile.h
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_AUDIOFILE_H_
#define CORE_FILES_AUDIOFILE_H_

#include <core/types.h>
#include <core/IExecutor.h>
#include <core/ITask.h>

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
                size_t      nSize;          // Size in frames
                size_t      nChannels;      // Total number of channels
                size_t      nCapacity;      // Capacity in frames
                float      *vData;
                float      *vChannels[];
            } temporary_buffer_t;

            file_content_t *pData;

        protected:
            static file_content_t *create_file_content(size_t channels, size_t samples);
            static void destroy_file_content(file_content_t *content);

            static temporary_buffer_t *create_temporary_buffer(file_content_t *content, size_t from = 0);
//            static temporary_buffer_t *create_temporary_buffer(file_content_t *content);
            static void flush_temporary_buffer(temporary_buffer_t *buffer);
            static size_t fill_temporary_buffer(temporary_buffer_t *buffer, size_t max_samples);
            static void destroy_temporary_buffer(temporary_buffer_t *buffer);

            status_t fast_downsample(size_t new_sample_rate);
            status_t fast_upsample(size_t new_sample_rate);
            status_t complex_upsample(size_t new_sample_rate);
            status_t complex_downsample(size_t new_sample_rate);

            status_t load_lspc(const char *path, float max_duration);
            status_t load_sndfile(const char *path, float max_duration);

        public:
            AudioFile();
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
             * @param max_count maximum duration of the file to store (in samples)
             * @return status of operation
             */
            status_t store_samples(const char *path, size_t max_count);

            /** Save file
             *
             * @param path path to the file
             * @param max_duration maximum duration of the file to store (in seconds)
             * @return status of operation
             */
            status_t store(const char *path, float max_duration = -1);

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

    class LoadAudioFileTask: public ITask
    {
        protected:
            char            sPath[PATH_MAX];
            AudioFile      *pAF;
            bool            bTaken;

        public:
            LoadAudioFileTask();
            virtual ~LoadAudioFileTask();

        public:
            /** Configure task
             *
             * @param filename file name
             */
            void    configure(const char *filename);

            /** Get audio file object
             *
             * @return audio file object
             */
            AudioFile   *file();

            /** Get file name
             *
             * @return file name
             */
            inline const char *path() const { return sPath; };

            /** Execute task
             *
             * @return status of operation
             */
            virtual int run();

            /**
             *
             */
            virtual void destroy();
    };

} /* namespace lsp */

#endif /* CORE_FILES_AUDIOFILE_H_ */
