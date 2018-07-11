#include "test/gtk_test.h"
#include "test/ladspa_test.h"
#include "test/vst_test.h"
#include "test/res_gen.h"

#define TEST gtk_test
//#define TEST ladspa_test
//#define TEST vst_test
//#define TEST res_gen

int main(int argc, const char**argv)
{
    return TEST::test(argc, argv);
}
