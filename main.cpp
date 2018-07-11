#include "test/gtk_test.h"
#include "test/ladspa_test.h"

#define TEST gtk_test
//#define TEST ladspa_test

int main(int argc, const char**argv)
{
    return TEST::test(argc, argv);
}
