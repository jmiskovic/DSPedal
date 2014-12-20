#include <stdlib.h>
#include <math.h>
#include "faust.h"

#define max(a,b) ((a < b) ? b : a)
#define min(a,b) ((a < b) ? a : b)


void instanceInitmydsp(mydsp* dsp, int samplingFreq) {
    dsp->fSamplingFreq = samplingFreq;
    dsp->fHslider0 = (FAUSTFLOAT)0.4;
    dsp->fConst0 = (float)(0.001f * (float)min(192000, max(1, dsp->fSamplingFreq)));
    dsp->fHslider1 = (FAUSTFLOAT)0.5;
    dsp->IOTA = 0;
    /* C99 loop */
    {
        int i0;
        for (i0 = 0; (i0 < BUFFER_SIZE); i0 = (i0 + 1)) {
            dsp->fRec0[i0] = 0.f;
        }

    }
}

void computemydsp(mydsp* dsp, int count, FAUSTFLOAT ** inputs, FAUSTFLOAT ** outputs) {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 = (float)(0.01f * (float)dsp->fHslider0);
    int iSlow1 = (int)(int)(1 + (int)(int)((int)(int)((int)(float)(dsp->fConst0 * (float)dsp->fHslider1) - 1) & 65535));
    /* C99 loop */
    {
        int i;
        for (i = 0; (i < count); i = (i + 1)) {
            dsp->fRec0[(dsp->IOTA & (BUFFER_SIZE - 1))] = (float)((float)(fSlow0 * dsp->fRec0[((dsp->IOTA - iSlow1) & (BUFFER_SIZE - 1))]) + (float)input0[i]);
            output0[i] = (FAUSTFLOAT)dsp->fRec0[((dsp->IOTA - 0) & (BUFFER_SIZE - 1))];
            dsp->IOTA = (dsp->IOTA + 1);

        }

    }

}