#ifndef FAUST_H
#define FAUST_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#ifndef FAUSTCLASS
#define FAUSTCLASS mydsp
#endif

typedef struct  {
    float fVec0[2048];
    float fVec1[2048];
    float fVec2[2048];
    float fVec3[2048];
    float fVec4[2048];
    float fVec5[2048];
    float fVec6[2048];
    float fVec7[2048];
    float fVec12[2048];
    float fVec13[2048];
    float fVec14[2048];
    float fVec15[2048];
    float fVec16[2048];
    float fVec17[2048];
    float fVec18[2048];
    float fVec19[2048];
    float fVec8[1024];
    float fVec20[1024];
    float fVec9[512];
    float fVec10[512];
    float fVec21[512];
    float fVec22[512];
    float fVec11[256];
    float fVec23[256];
    float fRec9[2];
    float fRec8[2];
    float fRec11[2];
    float fRec10[2];
    float fRec13[2];
    float fRec12[2];
    float fRec15[2];
    float fRec14[2];
    float fRec17[2];
    float fRec16[2];
    float fRec19[2];
    float fRec18[2];
    float fRec21[2];
    float fRec20[2];
    float fRec23[2];
    float fRec22[2];
    float fRec6[2];
    float fRec4[2];
    float fRec2[2];
    float fRec0[2];
    float fRec33[2];
    float fRec32[2];
    float fRec35[2];
    float fRec34[2];
    float fRec37[2];
    float fRec36[2];
    float fRec39[2];
    float fRec38[2];
    float fRec41[2];
    float fRec40[2];
    float fRec43[2];
    float fRec42[2];
    float fRec45[2];
    float fRec44[2];
    float fRec47[2];
    float fRec46[2];
    float fRec30[2];
    float fRec28[2];
    float fRec26[2];
    float fRec24[2];
    FAUSTFLOAT fHslider0;
    FAUSTFLOAT fHslider1;
    FAUSTFLOAT fHslider2;
    int IOTA;
    int fSamplingFreq;
} mydsp;

void instanceInitmydsp(mydsp* dsp, int samplingFreq);
void computemydsp(mydsp* dsp, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs);

#endif
