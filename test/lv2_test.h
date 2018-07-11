#include <core/plugin.h>
#include <stdio.h>

//float in[]=
//{
//    0.0f, 0.25f, 0.5f, 0.75f,
//    1.0f, 1.0f, 1.0f, 0.75f,
//    0.5f, 0.25f, 1.0f, 1.0f,
//    0.75f, 0.5f, 0.25f, 0.0f
//};

#define BUFFER_SIZE 512
//2048

using namespace lsp;

dsp *proc;

float in_l[BUFFER_SIZE];
float in_r[BUFFER_SIZE];
float out_l[BUFFER_SIZE];
float out_r[BUFFER_SIZE];

#define FUNCTION_SIZE 8192
float f[FUNCTION_SIZE];

//float f(float t)
//{
//    return
//        + 0.1 * t*t*t*t*t
//        - 0.5 * t*t*t*t
//        + 0.3 * t*t*t
//        - 0.07* t*t
//        + 0.1 * t
//        - 0.2;
//}

//void test_declipper()
//{
//    float in[4096];
//    float in_l[2048];
//    float in_r[2048];
//
//    using namespace forzee;
//
//    declipper *p = new declipper();
//
//    p->init(48000);
//    p->setup(1.0, 0.5f, 40, false);
//
//    FILE *fd = fopen("Selection.raw", "r");
//
//    while (true)
//    {
//        int count = fread(in, sizeof(float), sizeof(in)/sizeof(float), fd);
//        if (count <= 0)
//            break;
//
//        int samples = count >> 1;
//        for (int i=0; i<count; i+=2)
//        {
//            in_l[i>>1] = in[i];
//            in_r[i>>1] = in[i+1];
//        }
//
//        p->process(in_r, out, samples);
//    }
//
//    delete p;
//
//    fclose(fd);
//}

float correlation(const float* a, const float* b, size_t len)
{
    // s1_len should be less or equal than s2_len
    float corr = 0;
    for (size_t i=0; i<len; ++i)
        corr += a[i] * b[i];
    return corr / ((float)len);
}

void test_phase_detector()
{
    float
        bypass = -1.0f,
        time = phase_detector::DETECT_TIME_DFL,
        reactivity = phase_detector::REACT_TIME_MAX,
        reset = -1.0f,
        sel_time = 0.0f;

    float
        best_time = 0.0f,
        best_value = 0.0f,
        worst_time = 0.0f,
        worst_value = 0.0f,
        sel_value = 0.0f;

    using namespace lsp;

    proc->fill_zero(in_l, BUFFER_SIZE);
    proc->fill_zero(in_r, BUFFER_SIZE);
    proc->fill_zero(out_l, BUFFER_SIZE);
    proc->fill_zero(out_r, BUFFER_SIZE);

    // Instantiate
    phase_detector *c = new phase_detector();
    c->init(48000);

    // Bind ports
    size_t port_id = 0;
    c->bind(port_id++, in_l);
    c->bind(port_id++, in_r);

    c->bind(port_id++, out_l);
    c->bind(port_id++, out_r);

    c->bind(port_id++, &bypass);
    c->bind(port_id++, &time);
    c->bind(port_id++, &reactivity);
    c->bind(port_id++, &reset);
    c->bind(port_id++, &sel_time);

    c->bind(port_id++, &best_time);
    c->bind(port_id++, &best_value);
    c->bind(port_id++, &worst_time);
    c->bind(port_id++, &worst_value);
    c->bind(port_id++, &sel_value);

    // Activate plugin
    c->activate();

    FILE *f1 = fopen("tmp/kick-inner.raw", "r");
    FILE *f2 = fopen("tmp/kick-inner.raw", "r");

    printf("step;best_msec;best_samples;best_cm\n");

    size_t i=0;
    while (true)
    {
        int read1 = fread(in_l, sizeof(float), BUFFER_SIZE, f1);
        if (read1 <= 0)
            break;
        int read2 = fread(in_r, sizeof(float), read1, f2);
        if (read2 < read1)
            read1 = read2;
        if (read1 <= 0)
            break;

        // Process data
        c->run(read1);

        printf("%d;%.3f;%.3f;%.3f;\n",
            (int)(i++), best_time, best_time * 48, best_time * 340.29 * 0.1);
//        printf("Processed %d samples\n", (int)read1);
    }

//    size_t items = c->readFunction(f, FUNCTION_SIZE);
//    for (size_t i=0; i< items; ++i)
//        printf("%d;%.4f;\n", (int)i, f[i]);
//    c->process(in, out, 32);

    fclose(f2);
    fclose(f1);

    c->deactivate();
    c->destroy();
    delete c;
}

void gen_impulse()
{
    FILE *f = fopen("tmp/impulse.raw", "w");

    proc->fill_zero(out_l, BUFFER_SIZE);
    out_l[0] = 1.0f;

    for (size_t i=0; i < 1; ++i)
    {
        fwrite(out_l, sizeof(float), BUFFER_SIZE, f);
        out_l[0] = 0.0f;
    }

    fclose(f);
}

//int main(int, const char**)
//{
//    proc       = dsp::getInstance();
//
////    test_declipper();
//    test_phase_detector();
////    gen_impulse();
//}
