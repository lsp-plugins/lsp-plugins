//#include "plugin.h"
//
//#define DECLIPPER_MIN_SAMPLES           1
//#define DECLIPPER_MAX_SAMPLES           40
//
//#define DECLIPPER_DOTS                  2
//
//namespace forzee
//{
//    class declipper: public plugin
//    {
//        private:
//            enum Mode   { BYPASS, DETECT, LOOKUP, INTERPOLATE };
//
//            struct dot_t { float s, t; };
//
//        protected:
//            uint32_t    nSampleRate;
//            float      *vBuffer;
//            uint32_t    nBufSize;
//            uint32_t    nHead, nTail;
//            uint32_t    nClipStart, nDistance;
//            uint32_t    nDetect;
//            Mode        enMode;
//            float       nThreshold;
//            float       nMix;
//            float      *vMatrix[DECLIPPER_DOTS*2];
//            float       vpMatrix[(DECLIPPER_DOTS*2) * (DECLIPPER_DOTS*2 + 1)];
//
//        private:
//            void interpolate(uint32_t start, uint32_t end);
//            void print_matrix();
//
//        public:
//            declipper();
//            ~declipper();
//
//            void init(int sample_rate);
//            void fini();
//
//            void activate();
//            void deactivate();
//
//            void process(int samples);
//            void process(float *in, float *out, int samples);
//            void setup(float thresh, float mix, uint32_t samples, bool bypass);
//    };
//}
//
