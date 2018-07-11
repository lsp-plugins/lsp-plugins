//#include <stdio.h>
//#include <math.h>
//
//int main(int, const char **)
//{
//    float reactivity = 0.001f; // 1 msec (48 samples)
//    float sample_rate = 48000; // 48000 samples/sec
//    float tau = (reactivity * sample_rate) / log (1.0 - M_SQRT1_2);
//    float m__1_tau = ( 1.0 / tau); // -1 / tau
//    float vexp = expf(1.0 / tau); //(1.0 - M_SQRT1_2) * expf(1.0 / (reactivity * sample_rate));
//
////    printf("m_1_tau = %.5f, vexp=%.5f\n", m_1_tau, vexp);
//
//    float curr = 0.0f;
//
//    for (int i=0; i < 1000; ++i)
//    {
////        float arg = (i == 500) ? 1.0f : 0.0f;
////        arg *= arg;
//	float arg = (i < 500) ? sinf((2*M_PI * i)/100.0) : 0;
//        arg *= arg;
//
//        curr = arg - (arg - curr) * vexp;
//        printf("%d;%.6f;%.6f\n", i, arg, curr);
//    }
//
///*    float a = 0.0;
//    float b = 0.5;
//    float n = 100.0;
//    float m = powf(sqrtf(2.0) * 0.5, 1.0 / n) * n;
//
//    for (int i=0; i < 1000; ++i)
//    {
//        float arg = (i < 500) ? sinf((2*M_PI * i)/10.0) : 0;
//        arg *= arg;
//
//        a = ((a*m) + arg)/n;
//        printf("%d;%.6f;%.6f\n", i, arg, a);
//    }
//*/
//    return 0;
//}
//
