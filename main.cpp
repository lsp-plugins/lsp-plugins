#include "test/common.h"

#include "test/gtk_test.h"
#include "test/ladspa_test.h"
#include "test/vst_test.h"
#include "test/res_gen.h"
#include "test/audiofile_test.h"
#include "test/convolution_test.h"
#include "test/fft_test.h"
#include "test/fft_speed_test.h"
#include "test/dsp_speed_test.h"
#include "test/window_test.h"
#include "test/env_test.h"
#include "test/frac_test.h"
#include "test/sample_player_test.h"
#include "test/randgen_test.h"
#include "test/fade_test.h"
#include "test/resample_test.h"
#include "test/genttl.h"
#include "test/lv2_buf_size.h"
#include "test/alloc_test.h"

#define TEST gtk_test
//#define TEST ladspa_test
//#define TEST vst_test
//#define TEST res_gen
//#define TEST audiofile_test
//#define TEST convolution_test
//#define TEST fft_test
//#define TEST dsp_speed_test
//#define TEST fft_speed_test
//#define TEST window_test
//#define TEST env_test
//#define TEST sample_player_test
//#define TEST frac_test
//#define TEST randgen_test
//#define TEST fade_test
//#define TEST resample_test
//#define TEST genttl_test
//#define TEST lv2bufsize_test
//#define TEST alloc_test

int main(int argc, const char**argv)
{
    return TEST::test(argc, argv);
}
