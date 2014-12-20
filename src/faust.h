#ifndef FAUST_H
#define FAUST_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#ifndef FAUSTCLASS
#define FAUSTCLASS mydsp
#endif

#define BUFFER_SIZE 4357 //131072

typedef struct  {
    float fRec0[BUFFER_SIZE];
    FAUSTFLOAT fHslider0;    /* "feedback", &dsp->fHslider0, 0.f, 0.f, 100.f, 0.1f); */
    int fSamplingFreq;
    float fConst0;
    FAUSTFLOAT fHslider1;   /* "millisecond", &dsp->fHslider1, 0.f, 0.f, 1000.f, 0.1f); */
    int IOTA;
} mydsp;

void instanceInitmydsp(mydsp* dsp, int samplingFreq);
void computemydsp(mydsp* dsp, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs);

#endif
