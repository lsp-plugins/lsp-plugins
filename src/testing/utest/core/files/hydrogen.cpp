/*
 * hydrogen.cpp
 *
 *  Created on: 12 июл. 2020 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/io/Dir.h>
#include <core/files/Hydrogen.h>

using namespace lsp;

UTEST_BEGIN("core.files", hydrogen)

    void read_test_file(const char *fname)
    {
        hydrogen::drumkit_t dk;
        hydrogen::instrument_t *inst;
        hydrogen::layer_t *layer;

        // Load the equalizer settings
        io::Path path;
        UTEST_ASSERT(path.fmt("res/test/%s", fname) > 0);
        printf("Reading file: %s\n", path.as_native());
        UTEST_ASSERT(hydrogen::load(&path, &dk) == STATUS_OK);

        // Check the contents
        UTEST_ASSERT(dk.name.equals_ascii("test"));
        UTEST_ASSERT(dk.author.equals_ascii("TEST"));
        UTEST_ASSERT(dk.info.equals_ascii("test drumkit"));
        UTEST_ASSERT(dk.license.equals_ascii("Creative Commons"));
        UTEST_ASSERT(dk.instruments.size() == 2);

        // Check instrument # 1
        UTEST_ASSERT(inst = dk.instruments.get(0));
        UTEST_ASSERT(inst->id == 0);
        UTEST_ASSERT(inst->name.equals_ascii("Kick"));
        UTEST_ASSERT(float_equals_absolute(inst->volume, 1.0f));
        UTEST_ASSERT(inst->muted == false);
        UTEST_ASSERT(float_equals_absolute(inst->pan_left, 1.1f));
        UTEST_ASSERT(float_equals_absolute(inst->pan_right, 0.9f));
        UTEST_ASSERT(float_equals_absolute(inst->random_pitch_factor, 0.1f));
        UTEST_ASSERT(float_equals_absolute(inst->gain, 2.85f));
        UTEST_ASSERT(inst->filter_active == false);
        UTEST_ASSERT(float_equals_absolute(inst->filter_cutoff, 1.1f));
        UTEST_ASSERT(float_equals_absolute(inst->filter_resonance, 0.5f));
        UTEST_ASSERT(float_equals_absolute(inst->attack, 10.0f));
        UTEST_ASSERT(float_equals_absolute(inst->decay, 20.0f));
        UTEST_ASSERT(float_equals_absolute(inst->sustain, 30.0f));
        UTEST_ASSERT(float_equals_absolute(inst->release, 40.0f));
        UTEST_ASSERT(inst->mute_group == 1);
        UTEST_ASSERT(inst->stop_note == false);
        UTEST_ASSERT(inst->midi_out_note == 35);
        UTEST_ASSERT(inst->midi_in_note == 35);
        UTEST_ASSERT(inst->layers.size() == 2);

        UTEST_ASSERT(layer = inst->layers.at(0));
        UTEST_ASSERT(layer->file_name.equals_ascii("Kick-0.wav"));
        UTEST_ASSERT(float_equals_absolute(layer->min, 0.0f));
        UTEST_ASSERT(float_equals_absolute(layer->max, 0.2f));
        UTEST_ASSERT(float_equals_absolute(layer->gain, 1.0f));
        UTEST_ASSERT(float_equals_absolute(layer->pitch, 0.0f));

        UTEST_ASSERT(layer = inst->layers.at(1));
        UTEST_ASSERT(layer->file_name.equals_ascii("Kick-1.wav"));
        UTEST_ASSERT(float_equals_absolute(layer->min, 0.2f));
        UTEST_ASSERT(float_equals_absolute(layer->max, 0.4f));
        UTEST_ASSERT(float_equals_absolute(layer->gain, 1.1f));
        UTEST_ASSERT(float_equals_absolute(layer->pitch, 0.0f));

        // Check instrument # 2
        UTEST_ASSERT(inst = dk.instruments.get(1));
        UTEST_ASSERT(inst->id == 1);
        UTEST_ASSERT(inst->name.equals_ascii("Snare"));
        UTEST_ASSERT(float_equals_absolute(inst->volume, 1.0f));
        UTEST_ASSERT(inst->muted == false);
        UTEST_ASSERT(float_equals_absolute(inst->pan_left, 1.0f));
        UTEST_ASSERT(float_equals_absolute(inst->pan_right, 0.9f));
        UTEST_ASSERT(float_equals_absolute(inst->random_pitch_factor, 0.0f));
        UTEST_ASSERT(float_equals_absolute(inst->gain, 2.0f));
        UTEST_ASSERT(inst->filter_active == false);
        UTEST_ASSERT(float_equals_absolute(inst->filter_cutoff, 1.0f));
        UTEST_ASSERT(float_equals_absolute(inst->filter_resonance, 0.0f));
        UTEST_ASSERT(float_equals_absolute(inst->attack, 0.0f));
        UTEST_ASSERT(float_equals_absolute(inst->decay, 0.0f));
        UTEST_ASSERT(float_equals_absolute(inst->sustain, 1.0f));
        UTEST_ASSERT(float_equals_absolute(inst->release, 100000.0f));
        UTEST_ASSERT(inst->mute_group == -1);
        UTEST_ASSERT(inst->stop_note == false);
        UTEST_ASSERT(inst->midi_out_note == 40);
        UTEST_ASSERT(inst->midi_in_note == 40);
        UTEST_ASSERT(inst->layers.size() == 2);

        UTEST_ASSERT(layer = inst->layers.at(0));
        UTEST_ASSERT(layer->file_name.equals_ascii("Snare-0.wav"));
        UTEST_ASSERT(float_equals_absolute(layer->min, 0.0f));
        UTEST_ASSERT(float_equals_absolute(layer->max, 0.5f));
        UTEST_ASSERT(float_equals_absolute(layer->gain, 1.0f));
        UTEST_ASSERT(float_equals_absolute(layer->pitch, 0.0f));

        UTEST_ASSERT(layer = inst->layers.at(1));
        UTEST_ASSERT(layer->file_name.equals_ascii("Snare-1.wav"));
        UTEST_ASSERT(float_equals_absolute(layer->min, 0.5f));
        UTEST_ASSERT(float_equals_absolute(layer->max, 1.0f));
        UTEST_ASSERT(float_equals_absolute(layer->gain, 1.0f));
        UTEST_ASSERT(float_equals_absolute(layer->pitch, 0.0f));
    }

    void read_all_files(const char *fpath)
    {
        hydrogen::drumkit_t dk;
        io::Path path, fdpath;
        io::Dir dir;
        size_t num_read = 0;
        status_t res;

        UTEST_ASSERT(path.fmt("res/test/%s", fpath) > 0);
        UTEST_ASSERT(dir.open(&path) == STATUS_OK);

        while ((res = dir.read(&fdpath, true)) == STATUS_OK)
        {
            if ((fdpath.is_dot()) || (fdpath.is_dotdot()))
                continue;

            printf("Reading file: %s\n", fdpath.as_native());
            UTEST_ASSERT(hydrogen::load(&fdpath, &dk) == STATUS_OK);
            UTEST_ASSERT(dk.instruments.size() > 0);
            ++ num_read;
        }

        UTEST_ASSERT(res == STATUS_EOF);
        UTEST_ASSERT(num_read == 18);
    }

    UTEST_MAIN
    {
        printf("Reading test drumkit file...\n");
        read_test_file("hydrogen/test-drumkit.xml");

        printf("Checking drumkit files...\n");
        read_all_files("hydrogen");
    }

UTEST_END

