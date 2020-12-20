/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <core/debug.h>
#include <core/files/Hydrogen.h>
#include <core/files/xml/PullParser.h>
#include <core/io/InStringSequence.h>
#include <core/calc/Tokenizer.h>

namespace lsp
{
    namespace hydrogen
    {
        //---------------------------------------------------------------------
        layer_t::layer_t()
        {
            min     = 0.0f;
            max     = 1.0f;
            gain    = 1.0f;
            pitch   = 0.0f;
        }

        layer_t::~layer_t()
        {
        }

        //---------------------------------------------------------------------
        instrument_t::instrument_t()
        {
            id                  = -1;
            volume              = 1.0f;
            muted               = false;
            locked              = false;
            pan_left            = 1.0f;
            pan_right           = 1.0f;
            random_pitch_factor = 0.0f;
            gain                = 1.0f;
            filter_active       = false;
            filter_cutoff       = 1.0f;
            filter_resonance    = 0.0f;
            attack              = 0.0f;
            decay               = 0.0f;
            sustain             = 0.0f;
            release             = 0.0f;
            mute_group          = -1;
            stop_note           = false;
            midi_out_channel    = -1;
            midi_out_note       = -1;
            midi_in_channel     = -1;
            midi_in_note        = -1;
            fx1_level           = 0.0f;
            fx2_level           = 0.0f;
            fx3_level           = 0.0f;
            fx4_level           = 0.0f;
        }

        instrument_t::~instrument_t()
        {
            for (size_t i=0, n=layers.size(); i<n; ++i)
            {
                layer_t *layer = layers.at(i);
                if (layer != NULL)
                    delete layer;
            }
            layers.flush();
        }

        //---------------------------------------------------------------------
        drumkit_t::drumkit_t()
        {
        }

        drumkit_t::~drumkit_t()
        {
            for (size_t i=0, n=instruments.size(); i<n; ++i)
            {
                instrument_t *instr = instruments.at(i);
                if (instr != NULL)
                    delete instr;
            }
            instruments.flush();
        }

        void drumkit_t::swap(drumkit_t *dst)
        {
            name.swap(&dst->name);
            author.swap(&dst->author);
            info.swap(&dst->info);
            license.swap(&dst->license);
            instruments.swap_data(&dst->instruments);
        }

        //---------------------------------------------------------------------
        status_t read_string(xml::PullParser *p, LSPString *dst)
        {
            LSPString tmp;
            status_t item, res = STATUS_OK;

            do
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                        res = (tmp.append(p->value())) ? STATUS_OK : STATUS_NO_MEM;
                        break;

                    case xml::XT_END_ELEMENT:
                    {
                        tmp.swap(dst);
                        return STATUS_OK;
                    }

                    default:
                        return STATUS_BAD_FORMAT;
                }
            } while (res == STATUS_OK);

            return res;
        }

        status_t read_int(xml::PullParser *p, ssize_t *value)
        {
            LSPString tmp;
            status_t res = read_string(p, &tmp);
            if (res != STATUS_OK)
                return res;

            io::InStringSequence sq(&tmp);
            calc::Tokenizer tok(&sq);
            calc::token_t t = tok.get_token(calc::TF_GET);
            if (t != calc::TT_IVALUE)
                return STATUS_BAD_FORMAT;

            *value = tok.int_value();
            t = tok.get_token(calc::TF_GET);
            return (t == calc::TT_EOF) ? STATUS_OK : STATUS_BAD_FORMAT;
        }

        status_t read_float(xml::PullParser *p, float *value)
        {
            LSPString tmp;
            status_t res = read_string(p, &tmp);
            if (res != STATUS_OK)
                return res;

            io::InStringSequence sq(&tmp);
            calc::Tokenizer tok(&sq);
            calc::token_t t = tok.get_token(calc::TF_GET);
            if ((t != calc::TT_IVALUE) && (t != calc::TT_FVALUE))
                return STATUS_BAD_FORMAT;

            *value = (t == calc::TT_IVALUE) ? tok.int_value() : tok.float_value();
            t = tok.get_token(calc::TF_GET);
            return (t == calc::TT_EOF) ? STATUS_OK : STATUS_BAD_FORMAT;
        }

        status_t read_bool(xml::PullParser *p, bool *value)
        {
            LSPString tmp;
            status_t res = read_string(p, &tmp);
            if (res != STATUS_OK)
                return res;

            io::InStringSequence sq(&tmp);
            calc::Tokenizer tok(&sq);
            calc::token_t t = tok.get_token(calc::TF_GET);
            if ((t != calc::TT_TRUE) && (t != calc::TT_FALSE))
                return STATUS_BAD_FORMAT;

            *value = (t == calc::TT_TRUE);
            t = tok.get_token(calc::TF_GET);
            return (t == calc::TT_EOF) ? STATUS_OK : STATUS_BAD_FORMAT;
        }

        status_t skip_tags(xml::PullParser *p)
        {
            size_t counter = 1;
            status_t item, res = STATUS_OK;

            do
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                    case xml::XT_COMMENT:
                    case xml::XT_ATTRIBUTE:
                        break;

                    case xml::XT_START_ELEMENT:
                        ++counter;
                        break;

                    case xml::XT_END_ELEMENT:
                        if ((--counter) <= 0)
                            return res;
                        break;

                    default:
                        return STATUS_CORRUPTED;
                }
            } while (res == STATUS_OK);

            return res;
        }

        status_t read_layer(xml::PullParser *p, layer_t *layer)
        {
            status_t item, res = STATUS_OK;

            do
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        const LSPString *name = p->name();

                        if (name->equals_ascii("filename"))
                            res = read_string(p, &layer->file_name);
                        else if (name->equals_ascii("min"))
                            res = read_float(p, &layer->min);
                        else if (name->equals_ascii("max"))
                            res = read_float(p, &layer->max);
                        else if (name->equals_ascii("gain"))
                            res = read_float(p, &layer->gain);
                        else if (name->equals_ascii("pitch"))
                            res = read_float(p, &layer->pitch);
                        else
                        {
                            lsp_warn("Unexpected tag: %s", name->get_native());
                            res = skip_tags(p);
                        }
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return res;

                    default:
                        return STATUS_CORRUPTED;
                }
            } while (res == STATUS_OK);

            return res;
        }

        status_t read_instrument_component(xml::PullParser *p,  instrument_t *inst)
        {
            status_t item, res = STATUS_OK;

            do
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        const LSPString *name = p->name();
                        if (name->equals_ascii("layer"))
                        {
                            layer_t *layer = new layer_t();
                            if (layer == NULL)
                                return STATUS_NO_MEM;
                            if (!inst->layers.add(layer))
                            {
                                delete layer;
                                return STATUS_NO_MEM;
                            }
                            res = read_layer(p, layer);
                        }
                        else
                        {
                            lsp_warn("Unexpected tag: %s", name->get_native());
                            res = skip_tags(p);
                        }
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return res;

                    default:
                        return STATUS_CORRUPTED;
                }
            } while (res == STATUS_OK);

            return res;
        }

        status_t read_instrument(xml::PullParser *p, instrument_t *inst)
        {
            status_t item, res = STATUS_OK;

            do
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        const LSPString *name = p->name();

                        if (name->equals_ascii("id"))
                            res = read_int(p, &inst->id);
                        else if (name->equals_ascii("filename"))
                            res = read_string(p, &inst->file_name);
                        else if (name->equals_ascii("name"))
                            res = read_string(p, &inst->name);
                        else if (name->equals_ascii("volume"))
                            res = read_float(p, &inst->volume);
                        else if (name->equals_ascii("isMuted"))
                            res = read_bool(p, &inst->muted);
                        else if (name->equals_ascii("isLocked"))
                            res = read_bool(p, &inst->locked);
                        else if (name->equals_ascii("pan_L"))
                            res = read_float(p, &inst->pan_left);
                        else if (name->equals_ascii("pan_R"))
                            res = read_float(p, &inst->pan_right);
                        else if (name->equals_ascii("randomPitchFactor"))
                            res = read_float(p, &inst->random_pitch_factor);
                        else if (name->equals_ascii("gain"))
                            res = read_float(p, &inst->gain);
                        else if (name->equals_ascii("filterActive"))
                            res = read_bool(p, &inst->filter_active);
                        else if (name->equals_ascii("filterCutoff"))
                            res = read_float(p, &inst->filter_cutoff);
                        else if (name->equals_ascii("filterResonance"))
                            res = read_float(p, &inst->filter_resonance);
                        else if (name->equals_ascii("Attack"))
                            res = read_float(p, &inst->attack);
                        else if (name->equals_ascii("Decay"))
                            res = read_float(p, &inst->decay);
                        else if (name->equals_ascii("Sustain"))
                            res = read_float(p, &inst->sustain);
                        else if (name->equals_ascii("Release"))
                            res = read_float(p, &inst->release);
                        else if (name->equals_ascii("muteGroup"))
                            res = read_int(p, &inst->mute_group);
                        else if (name->equals_ascii("isStopNote"))
                            res = read_bool(p, &inst->stop_note);
                        else if (name->equals_ascii("midiOutChannel"))
                            res = read_int(p, &inst->midi_out_channel);
                        else if (name->equals_ascii("midiOutNote"))
                            res = read_int(p, &inst->midi_out_note);
                        else if (name->equals_ascii("midiInChannel"))
                            res = read_int(p, &inst->midi_in_channel);
                        else if (name->equals_ascii("midiInNote"))
                            res = read_int(p, &inst->midi_in_note);
                        else if (name->equals_ascii("FX1Level"))
                            res = read_float(p, &inst->fx1_level);
                        else if (name->equals_ascii("FX2Level"))
                            res = read_float(p, &inst->fx2_level);
                        else if (name->equals_ascii("FX3Level"))
                            res = read_float(p, &inst->fx3_level);
                        else if (name->equals_ascii("FX4Level"))
                            res = read_float(p, &inst->fx4_level);
                        else if (name->equals_ascii("exclude"))
                        {
                            LSPString tmp;
                            res = read_string(p, &tmp);
                        }
                        else if (name->equals_ascii("layer"))
                        {
                            layer_t *layer = new layer_t();
                            if (layer == NULL)
                                return STATUS_NO_MEM;
                            if (!inst->layers.add(layer))
                            {
                                delete layer;
                                return STATUS_NO_MEM;
                            }
                            res = read_layer(p, layer);
                        }
                        else if (name->equals_ascii("instrumentComponent"))
                            res = read_instrument_component(p, inst);
                        else
                        {
                            lsp_warn("Unexpected tag: %s", name->get_native());
                            res = skip_tags(p);
                        }
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return res;

                    default:
                        return STATUS_CORRUPTED;
                }
            } while (res == STATUS_OK);

            return res;
        }

        status_t read_instruments(xml::PullParser *p, cvector<instrument_t> *list)
        {
            status_t item, res = STATUS_OK;

            do
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        const LSPString *name = p->name();
                        if (name->equals_ascii("instrument"))
                        {
                            instrument_t *inst = new instrument_t();
                            if (inst == NULL)
                                return STATUS_NO_MEM;
                            if (!list->add(inst))
                            {
                                delete inst;
                                return STATUS_NO_MEM;
                            }

                            res = read_instrument(p, inst);
                        }
                        else
                        {
                            lsp_warn("Unexpected tag: %s", name->get_native());
                            res = skip_tags(p);
                        }
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return res;

                    default:
                        return STATUS_CORRUPTED;
                }
            } while (res == STATUS_OK);

            return res;
        }

        status_t read_drumkit(xml::PullParser *p, drumkit_t *dst)
        {
            status_t item, res = STATUS_OK;

            do
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                    case xml::XT_COMMENT:
                    case xml::XT_ATTRIBUTE:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        const LSPString *name = p->name();
                        if (name->equals_ascii("name"))
                            res = read_string(p, &dst->name);
                        else if (name->equals_ascii("author"))
                            res = read_string(p, &dst->author);
                        else if (name->equals_ascii("info"))
                            res = read_string(p, &dst->info);
                        else if (name->equals_ascii("license"))
                            res = read_string(p, &dst->license);
                        else if (name->equals_ascii("instrumentList"))
                            res = read_instruments(p, &dst->instruments);
                        else
                        {
                            lsp_warn("Unexpected tag: %s", name->get_native());
                            res = skip_tags(p);
                        }
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return res;

                    default:
                        return STATUS_CORRUPTED;
                }
            } while (res == STATUS_OK);

            return res;
        }

        status_t read_document(xml::PullParser *p, drumkit_t *dst)
        {
            status_t item, res  = STATUS_OK;
            bool read           = false;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                if (item == xml::XT_END_DOCUMENT)
                    return (read) ? STATUS_OK : STATUS_BAD_FORMAT;

                switch (item)
                {
                    case xml::XT_END_DOCUMENT:
                        break;

                    case xml::XT_CHARACTERS:
                    case xml::XT_CDATA:
                    case xml::XT_COMMENT:
                    case xml::XT_DTD:
                    case xml::XT_START_DOCUMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                        if ((read) || (p->name() == NULL) || (!p->name()->equals_ascii("drumkit_info")))
                            return STATUS_BAD_FORMAT;

                        if ((res = read_drumkit(p, dst)) != STATUS_OK)
                            return res;

                        read = true;
                        break;

                    default:
                        return STATUS_BAD_FORMAT;
                }
            }
        }

        status_t load_document(xml::PullParser *p, drumkit_t *dst)
        {
            // Load drumkit
            drumkit_t tmp;
            status_t res    = read_document(p, &tmp);

            // Close document
            status_t res2   = p->close();
            if (res == STATUS_OK)
                res             = res2;

            // Update drumkit data on success
            if (res == STATUS_OK)
                tmp.swap(dst);

            return res;
        }

        //---------------------------------------------------------------------
        status_t load(const char *path, drumkit_t *dst)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            xml::PullParser p;
            status_t res = p.open(path);
            return (res == STATUS_OK) ? load_document(&p, dst) : res;
        }

        status_t load(const LSPString *path, drumkit_t *dst)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            xml::PullParser p;
            status_t res = p.open(path);
            return (res == STATUS_OK) ? load_document(&p, dst) : res;
        }

        status_t load(const io::Path *path, drumkit_t *dst)
        {
            if ((path == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            xml::PullParser p;
            status_t res = p.open(path);
            return (res == STATUS_OK) ? load_document(&p, dst) : res;
        }

        status_t load(io::IInStream *is, drumkit_t *dst)
        {
            if ((is == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            xml::PullParser p;
            status_t res = p.wrap(is, WRAP_NONE);
            return (res == STATUS_OK) ? load_document(&p, dst) : res;
        }

        status_t load(io::IInSequence *is, drumkit_t *dst)
        {
            if ((is == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            xml::PullParser p;
            status_t res = p.wrap(is, WRAP_NONE);
            return (res == STATUS_OK) ? load_document(&p, dst) : res;
        }
    }
}

