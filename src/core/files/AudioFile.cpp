/*
 * AudioFile.cpp
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/endian.h>
#include <core/types.h>
#include <core/debug.h>
#include <core/files/LSPCFile.h>
#include <core/files/AudioFile.h>
#include <core/files/lspc/LSPCAudioReader.h>
#include <core/alloc.h>

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <guiddef.h>
    #include <propidl.h>
    #include <propvarutil.h>
    #include <mfapi.h>
    #include <mfidl.h>
    #include <mferror.h>
    #include <mfreadwrite.h>
#else
    #include <sndfile.h>
#endif /* PLATFORM_WINDOWS */

#define TMP_BUFFER_SIZE         1024
#define RESAMPLING_PERIODS      8

namespace lsp
{
    static size_t gcd_euclid(size_t a, size_t b)
    {
        while (b)
        {
            size_t c = a % b;
            a = b;
            b = c;
        }
        return a;
    }

    AudioFile::AudioFile()
    {
        pData       = NULL;
    }

    AudioFile::~AudioFile()
    {
        destroy();
    }

    void AudioFile::destroy()
    {
        lsp_trace("Destroy this=%p, pData=%p", this, pData);
        if (pData != NULL)
        {
            destroy_file_content(pData);
            pData       = NULL;
        }
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
        uint8_t *ptr        = lsp_tmalloc(uint8_t, total_size);
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
        if (content != NULL)
            lsp_free(content);
    }

    AudioFile::temporary_buffer_t *AudioFile::create_temporary_buffer(file_content_t *content, size_t from)
    {
        // Make number of samples multiple of 0x20 bytes
        size_t buffer_bytes     = content->nChannels * TMP_BUFFER_SIZE * sizeof(float);
        size_t buffer_size      = ALIGN_SIZE(buffer_bytes, 0x20);
        // Make header size multiple of 0x20 bytes
        size_t header_size      = ALIGN_SIZE(sizeof(temporary_buffer_t) + sizeof(float *) * content->nChannels, 0x20);
        // Calculate total size
        size_t total_size       = header_size + buffer_size;

        // Allocate structure
        uint8_t *ptr            = lsp_tmalloc(uint8_t, total_size);
        if (ptr == NULL)
            return NULL;

        // Initialize structure
        temporary_buffer_t *tb  = reinterpret_cast<temporary_buffer_t *>(ptr);
        ptr                    += header_size;

        tb->nSize               = 0;
        tb->nCapacity           = buffer_bytes;
        tb->nFrameSize          = content->nChannels * sizeof(float);
        tb->nChannels           = content->nChannels;
        tb->bData               = ptr;
        for (size_t i=0; i<content->nChannels; ++i)
        {
            float *chPtr            = content->vChannels[i];
            tb->vChannels[i]        = &chPtr[from];
        }

        return tb;
    }

    void AudioFile::flush_temporary_buffer(temporary_buffer_t *tb)
    {
        // Estimate number of bytes in buffer
        size_t avail    = tb->nSize;
        float *src      = tb->fData;

        // Process all fully-read frames
        while (avail >= tb->nFrameSize)
        {
            // Decode frame
            for (size_t i=0; i<tb->nChannels; ++i)
                *(tb->vChannels[i]++)   = *(src++);
            avail  -= tb->nFrameSize;
        }

        // Update buffer contents
        if (avail > 0)
            ::memmove(tb->fData, src, avail);
        tb->nSize           = avail;
    }

    size_t AudioFile::fill_temporary_buffer(temporary_buffer_t *tb, size_t max_samples)
    {
        size_t avail    = tb->nCapacity - tb->nSize;
        size_t count    = 0;
        float *dst      = tb->fData;

        while ((avail >= tb->nFrameSize) && (count < max_samples))
        {
            // Encode frame
            for (size_t i=0; i<tb->nChannels; ++i)
                *(dst++)    = *(tb->vChannels[i]++);
            avail      -= tb->nFrameSize;
            ++count;
        }

        tb->nSize  += count * tb->nFrameSize;
        return count;
    }

    void AudioFile::destroy_temporary_buffer(temporary_buffer_t *buffer)
    {
        if (buffer != NULL)
            lsp_free(buffer);
    }

    status_t AudioFile::create_samples(size_t channels, size_t sample_rate, size_t count)
    {
        // Allocate content
        file_content_t *fc  = create_file_content(channels, count);
        if (fc == NULL)
            return STATUS_NO_MEM;

        // Cleanup content
        fc->nSampleRate     = sample_rate;
        for (size_t i=0; i<channels; ++i)
            dsp::fill_zero(fc->vChannels[i], count);

        // Destroy previously used content and store new
        if (pData != NULL)
            destroy_file_content(pData);
        pData               = fc;
        return STATUS_OK;
    }

    status_t AudioFile::create(const Sample *sample, size_t sample_rate)
    {
        if (sample == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Allocate content
        size_t channels     = sample->channels();
        size_t length       = sample->length();
        file_content_t *fc  = create_file_content(channels, length);
        if (fc == NULL)
            return STATUS_NO_MEM;

        // Cleanup content
        fc->nSampleRate     = sample_rate;
        for (size_t i=0; i<channels; ++i)
            dsp::copy(fc->vChannels[i], sample->getBuffer(i), length);

        // Destroy previously used content and store new
        if (pData != NULL)
            destroy_file_content(pData);
        pData               = fc;
        return STATUS_OK;
    }

    status_t AudioFile::create(size_t channels, size_t sample_rate, float duration)
    {
        // Calculate the file length (in samples) and call the previous method
        size_t count        = sample_rate * duration;
        return create_samples(channels, sample_rate, count);
    }

    status_t AudioFile::load_lspc(const char *path, float max_duration)
    {
        LSPCFile fd;
        status_t res = fd.open(path);
        if (res != STATUS_OK)
        {
            fd.close();
            return res;
        }

        uint32_t chunk_id = 0;

        // Read profile (if present)
        size_t skip         = 0;
        size_t profVersion  = 1;
        LSPCChunkReader *prof = fd.find_chunk(LSPC_CHUNK_PROFILE);
        if (prof != NULL)
        {
            // Read profile header and check version
            lspc_chunk_audio_profile_t p;
            ssize_t n = prof->read_header(&p, sizeof(lspc_chunk_audio_profile_t));
            if (n < 0)
                res     = status_t(-n);
            else if ((p.common.version < 1) || (p.common.size < sizeof(lspc_chunk_audio_profile_t)))
                res     = STATUS_CORRUPTED_FILE;

            // Get related chunk identifier
            chunk_id = BE_TO_CPU(p.chunk_id);
            if ((res == STATUS_OK) && (chunk_id == 0))
                res = STATUS_CORRUPTED_FILE;

            // Get skip value:
            profVersion = p.common.version;
            if (profVersion >= 2)
                skip = BE_TO_CPU(p.skip);

            // Analyze final status
            status_t res2 = prof->close();
            if (res == STATUS_OK)
                res = res2;
            delete prof;

            // Analyze status
            if (res != STATUS_OK)
            {
                fd.close();
                return res;
            }
        }

        // Try to open audio file chunk
        LSPCAudioReader ar;
        res = (chunk_id > 0) ? ar.open(&fd, chunk_id) : ar.open(&fd);
        if (res != STATUS_OK)
        {
            ar.close();
            fd.close();
            return STATUS_BAD_FORMAT;
        }

        // Read audio chunk header and check its size
        lspc_audio_parameters_t aparams;
        res = ar.get_parameters(&aparams);
        if (res != STATUS_OK)
        {
            ar.close();
            fd.close();
            return res;
        }

        // Setting up skip value for version 1 headers
        if (profVersion < 2)
        {
            LSPCChunkReader *rd     = fd.read_chunk(ar.unique_id()); // Read the chunk with same ID as audio stream reader found
            lspc_chunk_audio_header_t hdr;

            ssize_t res = rd->read_header(&hdr, sizeof(lspc_chunk_audio_header_t));
            if ((res >= 0) && (hdr.common.version < 2)) // Field 'offset' is deprecated in header since version 2
            {
                ssize_t offset      = BE_TO_CPU(hdr.offset);

                size_t middle       = aparams.frames / 2 - 1;
                size_t skipNoOffset = middle - 1;
                size_t maxAhead     = aparams.frames - skipNoOffset;

                if (offset >= 0)
                {
                    size_t nOffset  = offset;
                    nOffset         = (nOffset > maxAhead)? maxAhead : nOffset;
                    skip            = skipNoOffset + nOffset;
                }
                else
                {
                    size_t nOffset  = -offset;
                    nOffset         = (nOffset > skipNoOffset)? skipNoOffset : nOffset;
                    skip            = skipNoOffset - nOffset;
                }
            }

            // Close reader and free resource
            res = rd->close();
            if (res != STATUS_OK)
            {
                rd->close();
                delete rd;
                ar.close();
                fd.close();
                return res;
            }
            delete rd;
            rd = NULL;
        }

        skip                = (skip > aparams.frames)? aparams.frames : skip;
        size_t max_samples  = (max_duration >= 0.0f) ? seconds_to_samples(aparams.sample_rate, max_duration) : -1;
        lsp_trace("file parameters: frames=%d, channels=%d, sample_rate=%d max_duration=%.3f, max_samples=%d",
                    int(aparams.frames), int(aparams.channels), int(aparams.sample_rate), max_duration, int(max_samples));

        aparams.frames     -= skip; // Remove number of frames to skip from audio parameters

        // Patch audio header
        if ((max_samples >= 0) && (aparams.frames > max_samples))
            aparams.frames     = max_samples;

        // Skip set of frames
        if (skip > 0)
        {
            ssize_t skipped = ar.skip_frames(skip);
            if (skipped != ssize_t(skip))
            {
                ar.close();
                fd.close();
                return (skipped >= 0) ? STATUS_CORRUPTED_FILE : -skipped;
            }
        }

        // Create file content
        file_content_t *fc      = NULL;
        if (res == STATUS_OK)
        {
            fc = create_file_content(aparams.channels, aparams.frames);
            if (fc == NULL)
            {
                ar.close();
                fd.close();
                return STATUS_NO_MEM;
            }

            fc->nSampleRate         = aparams.sample_rate;
        }

        // Allocate temporary buffer
        temporary_buffer_t *tb  = create_temporary_buffer(fc);
        if (tb == NULL)
        {
            destroy_file_content(fc);
            ar.close();
            fd.close();
            return STATUS_NO_MEM;
        }

        // Read frames
        skip = aparams.frames;
        while (skip > 0)
        {
            // Determine how many data is available to read
            size_t can_read     = tb->nCapacity - tb->nSize;
            if (can_read <= 0)
            {
                flush_temporary_buffer(tb);
                can_read            = tb->nCapacity - tb->nSize;
            }

            // Calculate amount of samples to read
            size_t to_read      = (skip > can_read) ? can_read : skip;

            ssize_t n           = ar.read_frames(reinterpret_cast<float *>(&tb->bData[tb->nSize]), to_read);
            if (n < 0)
            {
                destroy_temporary_buffer(tb);
                destroy_file_content(fc);
                ar.close();
                fd.close();
                return -n;
            }

            // Update counters
            tb->nSize          += n * tb->nFrameSize;
            skip               -= n;
        }

        // Flush last read data (if present)
        flush_temporary_buffer(tb);

        // Destroy temporary buffer
        if (tb != NULL)
        {
            destroy_temporary_buffer(tb);
            tb = NULL;
        }

        // Close chunk reader
        res = ar.close();
        if (res != STATUS_OK)
        {
            destroy_file_content(fc);
            ar.close();
            fd.close();
            return res;
        }

        // Close LSPC file
        res = fd.close();
        if (res != STATUS_OK)
        {
            destroy_file_content(fc);
            fd.close();
            return res;
        }

        // Destroy previously used content and store new
        if (pData != NULL)
            destroy_file_content(pData);
        pData               = fc;

        return STATUS_OK;
    }

    status_t AudioFile::load(const char *path, float max_duration)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;

        LSPString spath;
        if (!spath.set_utf8(path))
            return STATUS_NO_MEM;

        return load(&spath, max_duration);
    }

    status_t AudioFile::load(const LSPString *path, float max_duration)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;

        const char *npath = path->get_native();
        status_t res = load_lspc(npath, max_duration);
        if (res != STATUS_OK)
        {
            #ifdef PLATFORM_WINDOWS
                res = load_mfapi(path, max_duration);
            #else
                res = load_sndfile(npath, max_duration);
            #endif /* PLATFORM_WINDOWS */
        }
        return res;
    }

    status_t AudioFile::load(const io::Path *path, float max_duration)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        return load(path->as_string(), max_duration);
    }

    status_t AudioFile::store_samples(const io::Path *path, size_t from, size_t max_count) {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        return store_samples(path->as_string(), from, max_count);
    }

    status_t AudioFile::store_samples(const char *path, size_t from, size_t max_count)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        LSPString spath;
        if (!spath.set_utf8(path))
            return STATUS_NO_MEM;
        return store_samples(path, from, max_count);
    }

    status_t AudioFile::store_samples(const char *path, size_t max_count)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        LSPString spath;
        if (!spath.set_utf8(path))
            return STATUS_NO_MEM;
        return store_samples(&spath, 0, max_count);
    }

    status_t AudioFile::store_samples(const LSPString *path, size_t max_count)
    {
        return store_samples(path, 0, max_count);
    }

    status_t AudioFile::store_samples(const io::Path *path, size_t max_count)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        return store_samples(path->as_string(), 0, max_count);
    }

    status_t AudioFile::store(const char *path, float max_duration)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Calculate the file length (in samples) and call the previous method
        size_t max_count = (max_duration < 0) ? pData->nSamples : max_duration * pData->nSampleRate;
        LSPString spath;
        if (!spath.set_utf8(path))
            return STATUS_NO_MEM;

        return store_samples(&spath, 0, max_count);
    }

    status_t AudioFile::store(const LSPString *path, float max_duration)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Calculate the file length (in samples) and call the previous method
        size_t max_count = (max_duration < 0) ? pData->nSamples : max_duration * pData->nSampleRate;
        return store_samples(path, 0, max_count);
    }

    status_t AudioFile::store(const io::Path *path, float max_duration)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Calculate the file length (in samples) and call the previous method
        size_t max_count = (max_duration < 0) ? pData->nSamples : max_duration * pData->nSampleRate;
        return store_samples(path->as_string(), 0, max_count);
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
            dsp::reverse1(pData->vChannels[track_id], pData->nSamples);
        }
        else
        {
            size_t count = pData->nChannels;
            if (count <= 0)
                return false;
            for (size_t i=0; i<count; ++i)
            {
                lsp_trace("reverse %p, %d", pData->vChannels[i], int(pData->nSamples));
                dsp::reverse1(pData->vChannels[i], pData->nSamples);
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

    status_t AudioFile::resample(size_t new_sample_rate)
    {
        // Check that resampling is actually needed
        if (new_sample_rate > pData->nSampleRate)
        {
            // Need to up-sample data
            if ((new_sample_rate % pData->nSampleRate) == 0)
                return fast_upsample(new_sample_rate);
            else
                return complex_upsample(new_sample_rate);
        }
        else if (new_sample_rate < pData->nSampleRate)
        {
            // Need to down-sample data
            if ((pData->nSampleRate % new_sample_rate) == 0)
                return fast_downsample(new_sample_rate);
            else
                return complex_downsample(new_sample_rate);
        }

        // Return OK status
        return STATUS_OK;
    }

    status_t AudioFile::fast_downsample(size_t new_sample_rate)
    {
        size_t rkf          = pData->nSampleRate / new_sample_rate;
        size_t new_samples  = pData->nSamples / rkf;

        // Prepare new data structure to store resampled data
        file_content_t *fc  = create_file_content(pData->nChannels, new_samples);
        if (fc == NULL)
            return STATUS_NO_MEM;
        fc->nSampleRate     = new_sample_rate;

        // Iterate each channel
        for (size_t c=0; c<fc->nChannels; ++c)
        {
            const float *src    = pData->vChannels[c];
            float *dst          = fc->vChannels[c];

            for (size_t i=0, p=0; i < pData->nSamples; i += rkf, p++)
                dst[p]              = src[i];
        }

        // Destroy old data content
        destroy_file_content(pData);

        // Store new file content
        pData       = fc;

        return STATUS_OK;
    }

    status_t AudioFile::fast_upsample(size_t new_sample_rate)
    {
        // Calculate parameters of transformation
        ssize_t kf          = new_sample_rate / pData->nSampleRate;
        float rkf           = 1.0f / kf;

        // Prepare kernel for resampling
        ssize_t k_periods   = RESAMPLING_PERIODS; // * (kf >> 1);
        ssize_t k_base      = k_periods * kf;
        ssize_t k_center    = k_base + 1;
        ssize_t k_len       = (k_center << 1) + 1;
        ssize_t k_size      = ALIGN_SIZE(k_len + 1, 4); // Additional sample for time offset
        float *k            = lsp_tmalloc(float, k_size);
        if (k == NULL)
            return STATUS_NO_MEM;

        // Prepare temporary buffer for resampling
        size_t new_samples  = kf * pData->nSamples;
        size_t b_len        = new_samples + k_size;
        size_t b_size       = ALIGN_SIZE(b_len, 4);
        float *b            = lsp_tmalloc(float, b_size);
        if (b == NULL)
        {
            lsp_free(k);
            return STATUS_NO_MEM;
        }

        // Prepare new data structure to store resampled data
        file_content_t *fc  = create_file_content(pData->nChannels, new_samples);
        if (fc == NULL)
        {
            lsp_free(b);
            lsp_free(k);
            return STATUS_NO_MEM;
        }
        fc->nSampleRate     = new_sample_rate;

        // Generate Lanczos kernel
        for (ssize_t j=0; j<k_size; ++j)
        {
            float t         = (j - k_center) * rkf;

            if ((t > -k_periods) && (t < k_periods))
            {
                if (t != 0)
                {
                    float t2    = M_PI * t;
                    k[j]        = k_periods * sinf(t2) * sinf(t2 / k_periods) / (t2 * t2);
                }
                else
                    k[j]        = 1.0f;
            }
            else
                k[j]        = 0.0f;
        }

        // Output dump
//        printf("----------------------\n");
//        printf("j;t;k(j);\n");
//        for (ssize_t j=0; j<k_len; ++j)
//        {
//            float t         = (j - k_center) * rkf;
//            printf("%d;%f;%f;\n", int(j), t, k[j]);
//        }
//        printf("----------------------\n");

        // Iterate each channel
        for (size_t c=0; c<fc->nChannels; ++c)
        {
            const float *src    = pData->vChannels[c];
            dsp::fill_zero(b, b_size);  // Clear the temporary buffer

            // Perform convolutions
            for (size_t i=0, p=0; i<pData->nSamples; i++, p += kf)
                dsp::scale_add3(&b[p], k, src[i], k_size);

            // Copy the data to the file content
            dsp::copy(fc->vChannels[c], &b[k_center], fc->nSamples);
        }

        // Delete  temporary buffers
        destroy_file_content(pData);
        lsp_free(b);
        lsp_free(k);

        // Store new file content
        pData       = fc;

        return STATUS_OK;
    }

    status_t AudioFile::complex_upsample(size_t new_sample_rate)
    {
        // Calculate parameters of transformation
        ssize_t gcd         = gcd_euclid(new_sample_rate, pData->nSampleRate);
        ssize_t src_step    = pData->nSampleRate / gcd;
        ssize_t dst_step    = new_sample_rate / gcd;
        float kf            = float(dst_step) / float(src_step);
        float rkf           = float(src_step) / float(dst_step);

        // Prepare kernel for resampling
        ssize_t k_periods   = RESAMPLING_PERIODS; // Number of periods
        ssize_t k_base      = k_periods * kf;
        ssize_t k_center    = k_base + 1;
        ssize_t k_len       = (k_center << 1) + 1; // Centered impulse response
        ssize_t k_size      = ALIGN_SIZE(k_len + 1, 4); // Additional sample for time offset
        float *k            = lsp_tmalloc(float, k_size);
        if (k == NULL)
            return STATUS_NO_MEM;

        // Prepare temporary buffer for resampling
        size_t new_samples  = kf * pData->nSamples;
        size_t b_len        = new_samples + k_size;
        size_t b_size       = ALIGN_SIZE(b_len, 4);
        float *b            = lsp_tmalloc(float, b_size);
        if (b == NULL)
        {
            lsp_free(k);
            return STATUS_NO_MEM;
        }

        // Prepare new data structure to store resampled data
        file_content_t *fc  = create_file_content(pData->nChannels, new_samples);
        if (fc == NULL)
        {
            lsp_free(b);
            lsp_free(k);
            return STATUS_NO_MEM;
        }
        fc->nSampleRate     = new_sample_rate;

        // Iterate each channel
        for (size_t c=0; c<fc->nChannels; ++c)
        {
            const float *src    = pData->vChannels[c];
            dsp::fill_zero(b, b_size);  // Clear the temporary buffer

            for (ssize_t i=0; i<src_step; ++i)
            {
                // calculate the offset between nearest samples
                ssize_t p       = kf * i;
                float dt        = i*kf - p;

                // Generate Lanczos kernel
                for (ssize_t j=0; j<k_size; ++j)
                {
                    float t         = (j - k_center - dt) * rkf;

                    if ((t > -k_periods) && (t < k_periods))
                    {
                        if (t != 0.0f)
                        {
                            float t2    = M_PI * t;
                            k[j]        = k_periods * sinf(t2) * sinf(t2 / k_periods) / (t2 * t2);
                        }
                        else
                            k[j]        = 1.0f;
                    }
                    else
                        k[j]        = 0.0f;
                }

                // Output dump
//                printf("----------------------\n");
//                printf("j;t;k(j);\n");
//                for (ssize_t j=0; j<k_len; ++j)
//                {
//                    float t         = (j - k_center - dt) * kt;
//                    printf("%d;%f;%f;\n", int(j), t, k[j]);
//                }
//                printf("----------------------\n");

                // Perform convolutions
                for (size_t j=i; j<pData->nSamples; j += src_step)
                {
                    dsp::scale_add3(&b[p], k, src[j], k_size);
                    p   += dst_step;
                }
            }

            // Copy the data to the file content
            dsp::copy(fc->vChannels[c], &b[k_center], fc->nSamples);
        }

        // Delete  temporary buffers
        destroy_file_content(pData);
        lsp_free(b);
        lsp_free(k);

        // Store new file content
        pData       = fc;

        return STATUS_OK;
    }

    status_t AudioFile::complex_downsample(size_t new_sample_rate)
    {
        // Calculate parameters of transformation
        ssize_t gcd         = gcd_euclid(new_sample_rate, pData->nSampleRate);
        ssize_t src_step    = pData->nSampleRate / gcd;
        ssize_t dst_step    = new_sample_rate / gcd;
        float kf            = float(dst_step) / float(src_step);
        float rkf           = float(src_step) / float(dst_step);

        // Prepare kernel for resampling
        ssize_t k_base      = RESAMPLING_PERIODS;
        ssize_t k_periods   = k_base * rkf; // Number of periods
        ssize_t k_center    = k_base + 1;
        ssize_t k_len       = (k_center << 1) + rkf + 1; // Centered impulse response
        ssize_t k_size      = ALIGN_SIZE(k_len + 1, 4); // Additional sample for time offset
        float *k            = lsp_tmalloc(float, k_size);
        if (k == NULL)
            return STATUS_NO_MEM;

        // Prepare temporary buffer for resampling
        size_t new_samples  = kf * pData->nSamples;
        size_t b_len        = new_samples + k_size;
        size_t b_size       = ALIGN_SIZE(b_len, 4);
        float *b            = lsp_tmalloc(float, b_size);
        if (b == NULL)
        {
            lsp_free(k);
            return STATUS_NO_MEM;
        }

        // Prepare new data structure to store resampled data
        file_content_t *fc  = create_file_content(pData->nChannels, new_samples);
        if (fc == NULL)
        {
            lsp_free(b);
            lsp_free(k);
            return STATUS_NO_MEM;
        }
        fc->nSampleRate     = new_sample_rate;

        // Iterate each channel
        for (size_t c=0; c<fc->nChannels; ++c)
        {
            const float *src    = pData->vChannels[c];
            dsp::fill_zero(b, b_size);  // Clear the temporary buffer

            for (ssize_t i=0; i<src_step; ++i)
            {
                // calculate the offset between nearest samples
                ssize_t p       = kf * i;
                float dt        = i*kf - p; // Always positive, in range of [0..1]

                // Generate Lanczos kernel
                for (ssize_t j=0; j<k_size; ++j)
                {
                    float t         = (j - k_center - dt) * rkf;

                    if ((t > -k_periods) && (t < k_periods))
                    {
                        if (t != 0.0f)
                        {
                            float t2    = M_PI * t;
                            k[j]        = k_periods * sinf(t2) * sinf(t2 / k_periods) / (t2 * t2);
                        }
                        else
                            k[j]        = 1.0f;
                    }
                    else
                        k[j]        = 0.0f;
                }

                // Output dump
//                printf("----------------------\n");
//                printf("j;t;k(j);\n");
//                for (ssize_t j=0; j<k_size; ++j)
//                {
//                    float t         = (j - k_center - dt) * kt;
//                    printf("%d;%f;%f;\n", int(j), t, k[j]);
//                }
//                printf("----------------------\n");

                // Perform convolutions
                for (size_t j=i; j<pData->nSamples; j += src_step)
                {
                    dsp::scale_add3(&b[p], k, src[j], k_size);
                    p   += dst_step;
                }
            }

            // Copy the data to the file content
            dsp::copy(fc->vChannels[c], &b[k_center], fc->nSamples);
        }

        // Delete  temporary buffers
        destroy_file_content(pData);
        lsp_free(b);
        lsp_free(k);

        // Store new file content
        pData       = fc;

        return STATUS_OK;
    }

    float *AudioFile::channel(size_t track)
    {
        if (pData == NULL)
            return NULL;
        if (track >= pData->nChannels)
            return NULL;
        return pData->vChannels[track];
    }

    status_t AudioFile::convert_to_sample(Sample *dst)
    {
        if (dst == NULL)
            return STATUS_BAD_ARGUMENTS;
        if (pData == NULL)
            return STATUS_BAD_STATE;

        // Create and initialize temorary sample instance
        Sample tmp;
        if (!tmp.init(pData->nChannels, pData->nSamples, pData->nSamples))
            return STATUS_NO_MEM;

        // Copy file contents to sample
        for (size_t i=0; i<pData->nChannels; ++i)
            dsp::copy(tmp.getBuffer(i), pData->vChannels[i], pData->nSamples);

        tmp.swap(dst);
        tmp.destroy();
        return STATUS_OK;
    }

#ifdef PLATFORM_WINDOWS
    typedef struct stream_info_t
    {
        UINT64  frames;     // Number of frames
        UINT32  srate;      // Sample rate
        UINT32  channels;   // Number of channels
        DWORD   stream;     // Identifier of the stream
    } stream_info_t;

    static IMFMediaType *copy_media_type(IMFMediaType *src, stream_info_t *cfg)
    {
        // Obtain media type
        HRESULT hr = src->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &cfg->srate);
        if (SUCCEEDED(hr))
            hr = src->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &cfg->channels);
        if (!SUCCEEDED(hr))
            return NULL;

        // Create target media type
        IMFMediaType *dst = NULL;
        hr = ::MFCreateMediaType(&dst);
        if (!SUCCEEDED(hr))
            return NULL;

        // Configure and set media type for reader
        hr = dst->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        if (SUCCEEDED(hr))
            hr = dst->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
        if (SUCCEEDED(hr))
            hr = dst->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, sizeof(float) * 8);
        if (SUCCEEDED(hr))
            hr = dst->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, cfg->srate);
        if (SUCCEEDED(hr))
            hr = dst->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, cfg->channels);

        // Check the final result
        if (SUCCEEDED(hr))
            return dst;

        // Return error
        dst->Release();
        return NULL;
    }

    status_t open_mfapi_reader(const WCHAR *path, stream_info_t *info, IMFSourceReader **reader)
    {
        // Create source reader
        IMFSourceReader *pReader = NULL;
        HRESULT hr = ::MFCreateSourceReaderFromURL(path, NULL, &pReader);
        if ((!SUCCEEDED(hr)) || (pReader == NULL))
            return STATUS_UNKNOWN_ERR;

        // Obtain the length of stream in nanoseconds
        INT64 nsDuration;
        PROPVARIANT var;
        HRESULT hr = pReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE,
            MF_PD_DURATION, &var);
        if (SUCCEEDED(hr))
        {
            hr = ::PropVariantToInt64(var, &nsDuration);
            ::PropVariantClear(&var);
        }

        if (!SUCCEEDED(hr))
        {
            pReader->Release();
            return STATUS_UNKNOWN_ERR;
        }

        // Find the audio stream
        IMFMediaType *pNativeType = NULL;
        DWORD dwStreamIndex = 0;

        while (true)
        {
            hr = pReader->GetNativeMediaType(dwStreamIndex, 0, &pNativeType);
            if (hr == MF_E_NO_MORE_TYPES)
            {
                pReader->Release();
                return STATUS_BAD_FORMAT;
            }
            else if (SUCCEEDED(hr))
            {
                GUID guid;
                hr = pNativeType->GetGUID(MF_MT_MAJOR_TYPE, &guid);
                if ((SUCCEEDED(hr)) && (::IsEqualGUID(guid, MFMediaType_Audio)))
                    break;
                pNativeType->Release();
            }
            ++dwStreamIndex;
        }

        // Create media type for decoding
        IMFMediaType *pMediaType = copy_media_type(pNativeType, info);
        pNativeType->Release();
        if (pMediaType == NULL)
        {
            pReader->Release();
            return STATUS_UNKNOWN_ERR;
        }

        // Set current media type for decoding
        hr = pReader->SetCurrentMediaType(dwStreamIndex, NULL, pMediaType);
        pMediaType->Release();

        if (!SUCCEEDED(hr))
        {
            pReader->Release();
            return STATUS_UNKNOWN_ERR;
        }

        // Return success status
        info->frames    = (nsDuration * info->srate) / 10000000;
        info->stream    = dwStreamIndex;
        *reader         = pReader;
        return STATUS_OK;
    }

    status_t AudioFile::load_mfapi(const LSPString *path, float max_duration)
    {
        stream_info_t sf_info;
        IMFSourceReader *pReader = NULL;

        // Open sound file
        lsp_trace("loading file: %s\n", path->get_native());
        status_t res = open_mfapi_reader(path->get_utf16(), &sf_info, &pReader);
        if (res != STATUS_OK)
            return res;

        // Read sample file
        ssize_t max_samples     = (max_duration >= 0.0f) ? seconds_to_samples(sf_info.srate, max_duration) : -1;
        lsp_trace("file parameters: frames=%d, channels=%d, sample_rate=%d max_duration=%.3f\n, max_samples=%d",
            int(sf_info.frames), int(sf_info.channels), int(sf_info.srate), max_duration, int(max_samples));

        // Patch sf_info
        if ((max_samples >= 0) && (sf_info.frames > UINT64(max_samples)))
            sf_info.frames  = max_samples;

        // Create file content
        file_content_t *fc      = create_file_content(sf_info.channels, sf_info.frames);
        if (fc == NULL)
        {
            pReader->Release();
            return STATUS_NO_MEM;
        }
        fc->nSampleRate         = sf_info.srate;

        // Allocate temporary buffer
        temporary_buffer_t *tb  = create_temporary_buffer(fc);
        if (tb == NULL)
        {
            destroy_file_content(fc);
            pReader->Release();
            return STATUS_NO_MEM;
        }

        size_t count = sf_info.frames;
        while (count > 0)
        {
            // Read sample
            IMFSample *pSample = NULL;
            DWORD streamIndex, flags;
            LONGLONG llTimeStamp;

            HRESULT hr = pReader->ReadSample(
                sf_info.stream,                 // Stream index.
                0,                              // Flags.
                &streamIndex,                   // Receives the actual stream index.
                &flags,                         // Receives status flags.
                &llTimeStamp,                   // Receives the time stamp.
                &pSample                        // Receives the sample or NULL.
                );

            if (!SUCCEEDED(hr))
            {
                destroy_temporary_buffer(tb);
                destroy_file_content(fc);
                pReader->Release();
                return STATUS_CORRUPTED_FILE;
            }

            // Obtain number of buffers
            DWORD cBuffers = 0;
            hr = pSample->GetBufferCount(&cBuffers);
            if (!SUCCEEDED(hr))
            {
                destroy_temporary_buffer(tb);
                destroy_file_content(fc);
                pSample->Release();
                pReader->Release();
                return STATUS_CORRUPTED_FILE;
            }

            // Iterate each buffer
            for (DWORD i = 0; (i < cBuffers) && (count > 0); i++)
            {
                IMFMediaBuffer *pBuffer = NULL;
                hr = pSample->GetBufferByIndex(i, &pBuffer);
                if (!SUCCEEDED(hr))
                {
                    destroy_temporary_buffer(tb);
                    destroy_file_content(fc);
                    pSample->Release();
                    pReader->Release();
                    return STATUS_CORRUPTED_FILE;
                }

                // Use buffer
                BYTE *pData = NULL;
                DWORD nCurrLength = 0;
                hr = pBuffer->Lock(&pData, NULL, &nCurrLength);
                if (SUCCEEDED(hr))
                {
                    while ((count > 0) && (nCurrLength > 0))
                    {

                    }
                }
                if (SUCCEEDED(hr))
                    hr = pBuffer->Unlock();

                // Release buffer
                pBuffer->Release();
            }

            // Release sample
            pSample->Release();
        }

        // Flush last read data (if present)
        flush_temporary_buffer(tb);

        // Free allocated resources
        destroy_temporary_buffer(tb);
        pReader->Release();

        // Destroy previously used content and store new
        if (pData != NULL)
            destroy_file_content(pData);
        pData               = fc;

        return STATUS_OK;
    }

    status_t AudioFile::store_samples(const LSPString *path, size_t from, size_t max_count)
    {

    }

#else

    static status_t decode_sf_error(SNDFILE *fd)
    {
        switch (sf_error(NULL))
        {
            case SF_ERR_NO_ERROR:
                return STATUS_OK;
            case SF_ERR_UNRECOGNISED_FORMAT:
                return STATUS_BAD_FORMAT;
            case SF_ERR_MALFORMED_FILE:
                return STATUS_CORRUPTED_FILE;
            case SF_ERR_UNSUPPORTED_ENCODING:
                return STATUS_BAD_FORMAT;
            default:
                return STATUS_UNKNOWN_ERR;
        }
    }

    status_t AudioFile::load_sndfile(const char *path, float max_duration)
    {
        // Load sound file
        SNDFILE *sf_obj;
        SF_INFO sf_info;

        // Open sound file
        lsp_trace("loading file: %s\n", path);
        if ((sf_obj = sf_open(path, SFM_READ, &sf_info)) == NULL)
            return decode_sf_error(sf_obj);

        // Read sample file
        ssize_t max_samples     = (max_duration >= 0.0f) ? seconds_to_samples(sf_info.samplerate, max_duration) : -1;
        lsp_trace("file parameters: frames=%d, channels=%d, sample_rate=%d max_duration=%.3f\n, max_samples=%d",
            int(sf_info.frames), int(sf_info.channels), int(sf_info.samplerate), max_duration, int(max_samples));

        // Patch sf_info
        if ((max_samples >= 0) && (sf_info.frames > sf_count_t(max_samples)))
            sf_info.frames  = max_samples;

        // Create file content
        file_content_t *fc      = create_file_content(sf_info.channels, sf_info.frames);
        if (fc == NULL)
        {
            sf_close(sf_obj);
            return STATUS_NO_MEM;
        }
        fc->nSampleRate         = sf_info.samplerate;

        // Allocate temporary buffer
        temporary_buffer_t *tb  = create_temporary_buffer(fc);
        if (tb == NULL)
        {
            destroy_file_content(fc);
            sf_close(sf_obj);
            return STATUS_NO_MEM;
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
            sf_count_t amount   = sf_readf_float(sf_obj, reinterpret_cast<float *>(&tb->bData[tb->nSize]), to_read);
            if (amount <= 0)
            {
                status_t status     = decode_sf_error(sf_obj);

                destroy_temporary_buffer(tb);
                destroy_file_content(fc);
                sf_close(sf_obj);

                return status;
            }

            // Update counters
            tb->nSize          += amount * tb->nFrameSize;
            count              -= amount;
        }

        // Flush last read data (if present)
        flush_temporary_buffer(tb);

        // Free allocated resources
        destroy_temporary_buffer(tb);
        sf_close(sf_obj);

        // Destroy previously used content and store new
        if (pData != NULL)
            destroy_file_content(pData);
        pData               = fc;

        return STATUS_OK;
    }

    status_t AudioFile::store_samples(const LSPString *path, size_t from, size_t max_count)
    {
        if (pData == NULL)
            return STATUS_NO_DATA;

        // Load sound file
        SNDFILE *sf_obj;
        SF_INFO sf_info;

        sf_info.frames      = max_count;
        sf_info.samplerate  = pData->nSampleRate;
        sf_info.channels    = pData->nChannels;
        sf_info.format      = SF_FORMAT_WAV | SF_FORMAT_FLOAT | SF_ENDIAN_LITTLE;
        sf_info.sections    = 0;
        sf_info.seekable    = 0;

        if (sf_info.frames > sf_count_t(pData->nSamples - from))
            sf_info.frames      = pData->nSamples - from;

        // Open sound file
        lsp_trace("storing file: %s\n", path->get_native());
        if ((sf_obj = sf_open(path->get_native(), SFM_WRITE, &sf_info)) == NULL)
        {
            lsp_trace("Error: %s", sf_strerror(sf_obj));
            return decode_sf_error(NULL);
        }

        // Allocate temporary buffer
        temporary_buffer_t *tb  = create_temporary_buffer(pData, from);
        if (tb == NULL)
            return STATUS_NO_MEM;

        while ((max_count > 0) || (tb->nSize > 0))
        {
            // Fill buffer
            max_count   -=  fill_temporary_buffer(tb, max_count);

            // Flush buffer
            if (tb->nSize > 0)
            {
                // Write buffer to file
                size_t offset = 0;
                size_t frames = tb->nSize / tb->nFrameSize;
                while (offset < tb->nSize)
                {
                    sf_count_t written  = sf_writef_float(sf_obj, reinterpret_cast<float *>(&tb->bData[offset]), frames);
                    if (written < 0)
                    {
                        status_t status     = decode_sf_error(sf_obj);
                        sf_close(sf_obj);
                        destroy_temporary_buffer(tb);
                        return status;
                    }
                    offset +=  written * tb->nFrameSize;
                    frames -=  written;
                }

                // Update buffer contents
                frames  = tb->nSize - offset;
                if (frames > 0)
                    ::memmove(tb->bdata, &tb->bData[offset], frames);
                tb->nSize   = frames;
            }
        }

        // Free allocated resources
        sf_close(sf_obj);
        destroy_temporary_buffer(tb);

        return STATUS_OK;
    }
#endif /* PLATFORM_WINDOWS */

} /* namespace lsp */
