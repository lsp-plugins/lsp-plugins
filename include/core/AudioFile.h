/*
 * AudioFile.h
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_AUDIOFILE_H_
#define CORE_AUDIOFILE_H_

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
                size_t      nSize;
                size_t      nChannels;
                size_t      nCapacity;
                float      *vData;
                float      *vChannels[];
            } temporary_buffer_t;

            file_content_t *pData;

        protected:
            static file_content_t *create_file_content(size_t channels, size_t samples);
            static void destroy_file_content(file_content_t *content);

            static temporary_buffer_t *create_temporary_buffer(file_content_t *content);
            static void flush_temporary_buffer(temporary_buffer_t *buffer);
            static void destroy_temporary_buffer(temporary_buffer_t *buffer);

        public:
            AudioFile();
            ~AudioFile();

        public:
            /** Load file
             *
             * @param executor task executor
             * @param path path to the file
             * @param max_samples maximum samples to load, unlimited if less than zero
             * @return true if loading was successful
             */
            bool load(const char *path, ssize_t max_samples = -1);

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
            const float *channel(size_t track) const;

            /** Reverse track(s)
             *
             * @param track_id ID of track, track_id < 0 - all tracks
             * @param convolution build aligned convolution
             * @return true on success
             */
            bool reverse(ssize_t track_id = -1);

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

#endif /* CORE_AUDIOFILE_H_ */
