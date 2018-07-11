#include "test/gtk_test.h"
#include "test/ladspa_test.h"
#include "test/vst_test.h"
#include "test/res_gen.h"
#include "test/audiofile_test.h"
#include "test/convolution_test.h"
#include "test/fft_test.h"
#include "test/fft_speed_test.h"
#include "test/window_test.h"
#include "test/frac_test.h"

#define TEST gtk_test
//#define TEST ladspa_test
//#define TEST vst_test
//#define TEST res_gen
//#define TEST audiofile_test
//#define TEST convolution_test
//#define TEST fft_test
//#define TEST fft_speed_test
//#define TEST window_test
//#define TEST frac_test

int main(int argc, const char**argv)
{
    return TEST::test(argc, argv);
}
