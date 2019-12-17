/*
 * SamplePlayer.cpp
 *
 *  Created on: 13 марта 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/debug.h>
#include <core/sampling/SamplePlayer.h>

namespace lsp
{
    SamplePlayer::SamplePlayer()
    {
        vSamples        = NULL;
        nSamples        = 0;
        vPlayback       = NULL;
        nPlayback       = 0;
        sActive.pHead   = NULL;
        sActive.pTail   = NULL;
        sInactive.pHead = NULL;
        sInactive.pTail = NULL;
        fGain           = 1.0f;
    }
    
    SamplePlayer::~SamplePlayer()
    {
        destroy(true);
    }

    inline void SamplePlayer::list_remove(list_t *list, playback_t *pb)
    {
        if (pb->pPrev != NULL)
            pb->pPrev->pNext    = pb->pNext;
        else
            list->pHead         = pb->pNext;
        if (pb->pNext != NULL)
            pb->pNext->pPrev    = pb->pPrev;
        else
            list->pTail         = pb->pPrev;
    }

    inline SamplePlayer::playback_t *SamplePlayer::list_remove_first(list_t *list)
    {
        playback_t *pb          = list->pHead;
        if (pb == NULL)
            return NULL;
        list->pHead             = pb->pNext;
        if (pb->pNext != NULL)
            pb->pNext->pPrev    = pb->pPrev;
        else
            list->pTail         = pb->pPrev;
        return pb;
    }

    inline void SamplePlayer::list_add_first(list_t *list, playback_t *pb)
    {
        if (list->pHead == NULL)
        {
            list->pHead         = pb;
            list->pTail         = pb;
            pb->pPrev           = NULL;
            pb->pNext           = NULL;
        }
        else
        {
            pb->pNext           = list->pHead;
            pb->pPrev           = NULL;
            list->pHead->pPrev  = pb;
            list->pHead         = pb;
        }
    }

    inline void SamplePlayer::list_insert_from_tail(list_t *list, playback_t *pb)
    {
        // Find the position to insert data
        playback_t *prev    = list->pTail;
        while (prev != NULL)
        {
            if (pb->nOffset <= prev->nOffset)
                break;
            prev    = prev->pPrev;
        }

        // Position is before the first element?
        if (prev == NULL)
        {
            list_add_first(list, pb);
            return;
        }

        // Insert after the found element
        if (prev->pNext == NULL)
            list->pTail         = pb;
        else
            prev->pNext->pPrev  = pb;
        pb->pPrev           = prev;
        pb->pNext           = prev->pNext;
        prev->pNext         = pb;
    }

    inline void SamplePlayer::cleanup(playback_t *pb)
    {
        pb->pSample         = NULL;
        pb->nID             = -1;
        pb->nChannel        = 0;
        pb->nFadeout        = -1;
        pb->nFadeOffset     = 0;
        pb->nVolume         = 0.0f;
        pb->nOffset         = 0;
    }

    bool SamplePlayer::init(size_t max_samples, size_t max_playbacks)
    {
        // Check arguments
        if ((max_samples <= 0) || (max_playbacks <= 0))
            return false;

        // Allocate array of samples
        vSamples            = new Sample *[max_samples];
        if (vSamples == NULL)
            return false;

        // Allocate playback array
        vPlayback           = new playback_t[max_playbacks];
        if (vPlayback == NULL)
        {
            delete [] vSamples;
            vSamples            = NULL;
            return false;
        }

        // Update state
        nSamples            = max_samples;
        nPlayback           = max_playbacks;
        for (size_t i=0; i<max_samples; ++i)
            vSamples[i]         = NULL;

        // Init active list (empty)
        sActive.pHead       = NULL;
        sActive.pTail       = NULL;

        // Init inactive list (full)
        playback_t *last    = NULL;
        sInactive.pHead     = NULL;
        for (size_t i=0; i<max_playbacks; ++i)
        {
            playback_t *curr = &vPlayback[i];

            // Initialize fields
            cleanup(curr);

            // Link
            curr->pPrev     = last;
            if (last == NULL)
                sInactive.pHead = curr;
            else
                last->pNext     = curr;

            last            = curr;
        }
        last->pNext         = NULL;
        sInactive.pTail     = last;

        return true;
    }

    void SamplePlayer::destroy(bool cascade)
    {
        if (vSamples != NULL)
        {
            // Delete all bound samples
            if (cascade)
            {
                for (size_t i=0; i<nSamples; ++i)
                {
                    if (vSamples[i] != NULL)
                    {
                        vSamples[i]->destroy();
                        delete vSamples[i];
                        vSamples[i] = NULL;
                    }
                }
            }

            // Delete the array
            delete [] vSamples;
            vSamples        = NULL;
        }
        nSamples        = 0;

        if (vPlayback != NULL)
        {
            delete [] vPlayback;
            vPlayback       = NULL;
        }
        nPlayback       = 0;
        sActive.pHead   = NULL;
        sActive.pTail   = NULL;
        sInactive.pHead = NULL;
        sInactive.pTail = NULL;
    }

    bool SamplePlayer::bind(size_t id, Sample **sample)
    {
        if (id >= nSamples)
            return false;

        Sample     *old = vSamples[id];
        if (sample != NULL)
        {
            Sample     *ns  = *sample;
            if (old == ns)
            {
                *sample     = NULL;
                return true;
            }

            vSamples[id]    = ns;
            *sample         = old;
        }

        // Cleanup all active playbacks associated with this sample
        playback_t *pb = sActive.pHead;
        while (pb != NULL)
        {
            playback_t *next    = pb->pNext;
            if (pb->pSample == old)
            {
                pb->pSample     = NULL;
                list_remove(&sActive, pb);
                list_add_first(&sInactive, pb);
            }
            pb          = next;
        }

        return true;
    }

    bool SamplePlayer::bind(size_t id, Sample *sample, bool destroy)
    {
        if (!bind(id, &sample))
            return false;

        if ((destroy) && (sample != NULL))
        {
            sample->destroy();
            delete [] sample;
        }

        return true;
    }

    bool SamplePlayer::unbind(size_t id, Sample **sample)
    {
        *sample     = NULL;
        return bind(id, sample);
    }

    bool SamplePlayer::unbind(size_t id, bool destroy)
    {
        return bind(id, reinterpret_cast<Sample *>(NULL), destroy);
    }

    void SamplePlayer::process(float *dst, const float *src, size_t samples)
    {
        if (src == NULL)
            dsp::fill_zero(dst, samples);
        else
            dsp::copy(dst, src, samples);
        do_process(dst, samples);
    }

    void SamplePlayer::process(float *dst, size_t samples)
    {
        dsp::fill_zero(dst, samples);
        do_process(dst, samples);
    }

    void SamplePlayer::do_process(float *dst, size_t samples)
    {
        playback_t *pb      = sActive.pHead;

        // Iterate playbacks
        while (pb != NULL)
        {
            // Get next playback
            playback_t *next    = pb->pNext;

            // Check bounds
            ssize_t src_head    = pb->nOffset;
            pb->nOffset        += samples;
            Sample *s           = pb->pSample;
            ssize_t s_len       = s->length();

            // Handle sample if active
            if (pb->nOffset > 0)
            {
//                lsp_trace("pb->nOffset=%d, samples=%d, s_len=%d", int(pb->nOffset), int(samples), int(s_len));

                ssize_t dst_off     = 0;
                ssize_t count       = samples;
                if (pb->nOffset < count)
                {
                    src_head    = 0;
                    dst_off     = samples - pb->nOffset;
                    count       = pb->nOffset;
                }
                if (pb->nOffset > s_len)
                    count      += s_len - pb->nOffset;

                // Add sample data to the output buffer
                if (count > 0)
                {
//                    lsp_trace("add_multiplied dst_off=%d, src_head=%d, volume=%f, count=%d", int(dst_off), int(src_head), pb->nVolume, int(count));
                    if (pb->nFadeout < 0)
                        dsp::fmadd_k3(&dst[dst_off], s->getBuffer(pb->nChannel, src_head), pb->nVolume * fGain, count);
                    else
                    {
                        ssize_t fade_head   = pb->nFadeOffset;
                        float gain          = pb->nVolume * fGain;
                        float *sp           = s->getBuffer(pb->nChannel, src_head);
                        float *dp           = &dst[dst_off];

                        float fgain         = gain / (pb->nFadeout + 1);
                        for (ssize_t i=0; (i<count) && (fade_head < pb->nFadeout); ++i, ++fade_head)
                        {
                            if (fade_head < 0)
                                *(dp++)        += *(sp++) * gain;
                            else
                                *(dp++)        += *(sp++) * fgain * (pb->nFadeout - fade_head);
                        }

                        pb->nFadeOffset     = fade_head;
                    }
                }
            }

            // Check that there are no samples to process in the future
            if ((pb->nOffset >= s_len) ||
                ((pb->nFadeout >= 0) && (pb->nFadeOffset >= pb->nFadeout)))
            {
                // Cleanup playback
                cleanup(pb);

                // Move to inactive
                list_remove(&sActive, pb);
                list_add_first(&sInactive, pb);

//                lsp_trace("freed playback %p", pb);
            }

            // Iterate next playback
            pb                  = next;
        }
    }

    bool SamplePlayer::play(size_t id, size_t channel, float volume, ssize_t delay)
    {
        // Check that ID of the sample is correct
        if (id >= nSamples)
            return false;

        // Check that the sample is bound and valid
        Sample *s       = vSamples[id];
        if ((s == NULL) || (!s->valid()))
            return false;

        // Check that ID of channel matches
        if (channel >= s->channels())
            return false;

        // Try to acquire playback
        playback_t *pb  = list_remove_first(&sInactive);
        if (pb == NULL)
            pb              = list_remove_first(&sActive);
        if (pb == NULL)
            return false;

//        lsp_trace("acquired playback %p", pb);

        // Now we are ready to activate sample
        pb->pSample     = s;
        pb->nID         = id;
        pb->nChannel    = channel;
        pb->nVolume     = volume;
        pb->nOffset     = -delay;
        pb->nFadeout    = -1;  // No fadeout
        pb->nFadeOffset = -1; // No cancellation

        // Add the playback to the active list
        list_insert_from_tail(&sActive, pb);

        return true;
    }

    ssize_t SamplePlayer::cancel_all(size_t id, size_t channel, size_t fadeout, ssize_t delay)
    {
        // Check that ID of the sample is correct
        if (id >= nSamples)
            return -1;

        ssize_t result = 0;

        // Get first playback
        playback_t *pb      = sActive.pHead;
        if (pb == NULL)
            return result;

        // Stop all playbacks
        do
        {
            // Remember next playback
            playback_t *next        = pb->pNext;

            // Cancel playback if not already cancelled
            if ((pb->nID == ssize_t(id)) &&
                (pb->pSample != NULL) &&
                (pb->nFadeout < 0))
            {
                pb->nFadeout    = fadeout;
                pb->nFadeOffset = -delay;
                result          ++;

//                lsp_trace("marked playback %p for cancelling", pb);
            }

            // Iterate to next playback
            pb                      = next;
        } while (pb != NULL);

        return result;
    }

    void SamplePlayer::stop()
    {
        // Get first playback
        playback_t *pb      = sActive.pHead;
        if (pb == NULL)
            return;

        // Stop all playbacks
        do
        {
            // Cancel playback
            cleanup(pb);

            // Iterate next playback
            pb                      = pb->pNext;
        } while (pb != NULL);

        // Move all data from active list to inactive
        if (sInactive.pHead == NULL)
            sInactive.pTail         = sActive.pTail;
        else
        {
            sActive.pTail->pNext    = sInactive.pHead;
            sInactive.pHead->pPrev  = sActive.pTail;
        }

        sInactive.pHead     = sActive.pHead;
        sActive.pHead       = NULL;
        sActive.pTail       = NULL;
    }
} /* namespace lsp */
