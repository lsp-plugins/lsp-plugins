/*
 * AudioFile.cpp
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/dsp.h>
#include <core/AudioFile.h>

#include <sndfile.h>

#define TMP_BUFFER_SIZE         1024

namespace lsp
{
    AudioFile::AudioFile()
    {
        pData       = NULL;
    }

    AudioFile::~AudioFile()
    {
        pData       = NULL;
    }

    AudioFile::file_content_t *AudioFile::create_file_content(size_t channels, size_t samples)
    {
        // Make number of samples multiple of 0x20 bytes
        size_t buffer_len   = (samples + 0x03) & (~size_t(0x03));
        size_t buffer_size  = (buffer_len * sizeof(float) + 0x1f) & (~size_t(0x1f)); // +3 - for convolution alignment
        // Make header size multiple of 0x20 bytes
        size_t header_size  = (sizeof(file_content_t) + sizeof(float *) * channels + 0x1f) & (~size_t(0x1f));
        // Calculate total size
        size_t total_size   = header_size + buffer_size * channels;

        // Allocate structure
        uint8_t *ptr = new uint8_t[total_size];
        if (ptr == NULL)
            return NULL;

        // Initialize content
        file_content_t *ct  = reinterpret_cast<file_content_t *>(ptr);
        ct->nChannels       = channels;
        ct->nSamples        = buffer_len;
        ct->nSampleRate     = 0;
        ptr                += header_size;

        for (size_t i=0; i < channels; ++i)
        {
            ct->vChannels[i]    = reinterpret_cast<float *>(ptr);
            dsp::fill_zero(ct->vChannels[i], buffer_len);

            ptr                += buffer_size;
        }

        return ct;
    }

    void AudioFile::destroy_file_content(file_content_t *content)
    {
        if (content == NULL)
            return;
        delete [] reinterpret_cast<uint8_t *>(content);
    }

    AudioFile::temporary_buffer_t *AudioFile::create_temporary_buffer(file_content_t *content)
    {
        // Make number of samples multiple of 0x20 bytes
        size_t buffer_samples   = content->nChannels * TMP_BUFFER_SIZE;
        size_t buffer_size      = (buffer_samples * sizeof(float) + 0x1f) & (~size_t(0x1f));
        // Make header size multiple of 0x20 bytes
        size_t header_size      = (sizeof(temporary_buffer_t) + sizeof(float *) * content->nChannels + 0x1f) & (~size_t(0x1f));
        // Calculate total size
        size_t total_size       = header_size + buffer_size;

        // Allocate structure
        uint8_t *ptr = new uint8_t[total_size];
        if (ptr == NULL)
            return NULL;

        // Initialize structure
        temporary_buffer_t *tb  = reinterpret_cast<temporary_buffer_t *>(ptr);
        tb->nSize               = 0;
        tb->nChannels           = content->nChannels;
        tb->nCapacity           = TMP_BUFFER_SIZE;
        tb->vData               = reinterpret_cast<float *>(&ptr[header_size]);
        for (size_t i=0; i<content->nChannels; ++i)
            tb->vChannels[i]        = content->vChannels[i];

        return tb;
    }

    void AudioFile::flush_temporary_buffer(temporary_buffer_t *tb)
    {
        // Decode frames, buffer->nSize = number of frames
        for (size_t j=0; j<tb->nChannels; ++j)
        {
            const float *src    = &tb->vData[j];
            float *dst          = tb->vChannels[j];

            // Copy frame data
            for (size_t i=0; i<tb->nSize; ++i)
            {
                *dst            = *src;

                // Move pointers
                src             += tb->nChannels;
                dst             ++;
            }

            // Update data pointer
            tb->vChannels[j]    = dst;
        }

        // Clear buffer size
        tb->nSize           = 0;
    }

    void AudioFile::destroy_temporary_buffer(temporary_buffer_t *buffer)
    {
        if (buffer == NULL)
            return;
        delete [] reinterpret_cast<uint8_t *>(buffer);
    }

    bool AudioFile::load(const char *path, ssize_t max_samples)
    {
        // Destroy previously used content
        if (pData != NULL)
        {
            destroy_file_content(pData);
            pData       = NULL;
        }

        // Load sound file
        SNDFILE *sf_obj;
        SF_INFO sf_info;

        // Open sound file
        lsp_trace("loading file: %s\n", path);
        if ((sf_obj = sf_open(path, SFM_READ, &sf_info)) == NULL)
            return false;

        lsp_trace("file parameters: frames=%d, channels=%d, sample_rate=%d max_samples=%d\n",
            int(sf_info.frames), int(sf_info.channels), int(sf_info.samplerate), int(max_samples));

        // Patch sf_info
        if ((max_samples >= 0) && (sf_info.frames > sf_count_t(max_samples)))
            sf_info.frames  = max_samples;

        // Create file content
        file_content_t *fc      = create_file_content(sf_info.channels, sf_info.frames);
        if (fc == NULL)
            return false;

        // Allocate temporary buffer
        temporary_buffer_t *tb  = create_temporary_buffer(fc);
        if (tb == NULL)
        {
            destroy_file_content(fc);
            return false;
        }

        size_t count = sf_info.frames;
        while (count > 0)
        {
            // Determine how many data is available to read
            size_t can_read     = tb->nCapacity - tb->nSize;
            if (can_read <= 0)
            {
                flush_temporary_buffer(tb);
                can_read            = tb->nCapacity - tb->nSize;
            }

            // Calculate amount of samples to read
            size_t to_read      = (count > can_read) ? can_read : count;
            sf_count_t amount   = sf_readf_float(sf_obj, &tb->vData[tb->nSize], to_read);
            if (amount <= 0)
            {
                sf_close(sf_obj);
                destroy_temporary_buffer(tb);
                destroy_file_content(fc);
                return false;
            }

            // Update counters
            tb->nSize          += amount;
            count              -= amount;
        }

        // Flush last read data (if present)
        flush_temporary_buffer(tb);

        // Free allocated resources
        sf_close(sf_obj);
        destroy_temporary_buffer(tb);

        // Store new file content
        fc->nSampleRate     = sf_info.samplerate;
        pData               = fc;

        return true;
    }

    bool AudioFile::reverse(ssize_t track_id)
    {
        if (pData == NULL)
            return false;

        if (track_id >= 0)
        {
            if (size_t(track_id) >= pData->nChannels)
                return false;
            lsp_trace("reverse %p, %d", pData->vChannels[track_id], int(pData->nSamples));
            dsp::reverse(pData->vChannels[track_id], pData->nSamples);
        }
        else
        {
            size_t count = pData->nChannels;
            if (count <= 0)
                return false;
            for (size_t i=0; i<count; ++i)
            {
                lsp_trace("reverse %p, %d", pData->vChannels[i], int(pData->nSamples));
                dsp::reverse(pData->vChannels[i], pData->nSamples);
            }
        }

        return true;
    }

    size_t AudioFile::channels() const
    {
        return (pData != NULL) ? pData->nChannels : 0;
    }

    size_t AudioFile::samples() const
    {
        return (pData != NULL) ? pData->nSamples : 0;
    }

    size_t AudioFile::sample_rate() const
    {
        return (pData != NULL) ? pData->nSampleRate : 0;
    }

    const float *AudioFile::channel(size_t track) const
    {
        if (pData == NULL)
            return NULL;
        if (track >= pData->nChannels)
            return NULL;
        return pData->vChannels[track];
    }

    void AudioFile::destroy()
    {
        if (pData != NULL)
        {
            destroy_file_content(pData);
            pData       = NULL;
        }
    }

    LoadAudioFileTask::LoadAudioFileTask()
    {
        sPath[0]        = '\0';
        pAF             = NULL;
        bTaken          = false;
    }

    LoadAudioFileTask::~LoadAudioFileTask()
    {
        sPath[0]        = '\0';
        pAF             = NULL;
        bTaken          = false;
    }

    void LoadAudioFileTask::configure(const char *filename)
    {
        strncpy(sPath, filename, PATH_MAX);
        sPath[PATH_MAX-1] ='\0';
    }

    AudioFile *LoadAudioFileTask::file()
    {
        if (!completed())
            return NULL;

        bTaken      = true;
        return pAF;
    }

    int LoadAudioFileTask::run()
    {
        lsp_trace("path=%s, af=%p, taken=%s", sPath, pAF, (bTaken) ? "true" : "false");

        // Drop previous data
        if (pAF != NULL)
        {
            if (!bTaken)
            {
                pAF->destroy();
                delete pAF;
            }
            pAF     = NULL;
        }

        // Load new file
        AudioFile *af = new AudioFile();
        if (!af->load(sPath))
        {
            af->destroy();
            delete af;
            return -1;
        }

        // Store file
        pAF     = af;
        return 0;
    }

    void LoadAudioFileTask::destroy()
    {
        // Drop stored data
        if (pAF != NULL)
        {
            if (!bTaken)
            {
                pAF->destroy();
                delete pAF;
            }
            pAF     = NULL;
        }
    }
} /* namespace lsp */
