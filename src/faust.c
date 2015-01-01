#include <stdlib.h>
#include <math.h>
#include "faust.h"

#define max(a,b) ((a < b) ? b : a)
#define min(a,b) ((a < b) ? a : b)


void instanceInitmydsp(mydsp* dsp, int samplingFreq) {
    dsp->fSamplingFreq = samplingFreq;
    dsp->fHslider0 = (FAUSTFLOAT)0.3333;
    dsp->fHslider1 = (FAUSTFLOAT)0.5;
    dsp->fHslider2 = (FAUSTFLOAT)0.5;
    /* C99 loop */
    {
        int i0;
        for (i0 = 0; (i0 < 2); i0 = (i0 + 1)) {
            dsp->fRec9[i0] = 0.f;

        }

    }
    dsp->IOTA = 0;
    /* C99 loop */
    {
        int i1;
        for (i1 = 0; (i1 < 2048); i1 = (i1 + 1)) {
            dsp->fVec0[i1] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i2;
        for (i2 = 0; (i2 < 2); i2 = (i2 + 1)) {
            dsp->fRec8[i2] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i3;
        for (i3 = 0; (i3 < 2); i3 = (i3 + 1)) {
            dsp->fRec11[i3] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i4;
        for (i4 = 0; (i4 < 2048); i4 = (i4 + 1)) {
            dsp->fVec1[i4] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i5;
        for (i5 = 0; (i5 < 2); i5 = (i5 + 1)) {
            dsp->fRec10[i5] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i6;
        for (i6 = 0; (i6 < 2); i6 = (i6 + 1)) {
            dsp->fRec13[i6] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i7;
        for (i7 = 0; (i7 < 2048); i7 = (i7 + 1)) {
            dsp->fVec2[i7] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i8;
        for (i8 = 0; (i8 < 2); i8 = (i8 + 1)) {
            dsp->fRec12[i8] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i9;
        for (i9 = 0; (i9 < 2); i9 = (i9 + 1)) {
            dsp->fRec15[i9] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i10;
        for (i10 = 0; (i10 < 2048); i10 = (i10 + 1)) {
            dsp->fVec3[i10] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i11;
        for (i11 = 0; (i11 < 2); i11 = (i11 + 1)) {
            dsp->fRec14[i11] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i12;
        for (i12 = 0; (i12 < 2); i12 = (i12 + 1)) {
            dsp->fRec17[i12] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i13;
        for (i13 = 0; (i13 < 2048); i13 = (i13 + 1)) {
            dsp->fVec4[i13] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i14;
        for (i14 = 0; (i14 < 2); i14 = (i14 + 1)) {
            dsp->fRec16[i14] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i15;
        for (i15 = 0; (i15 < 2); i15 = (i15 + 1)) {
            dsp->fRec19[i15] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i16;
        for (i16 = 0; (i16 < 2048); i16 = (i16 + 1)) {
            dsp->fVec5[i16] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i17;
        for (i17 = 0; (i17 < 2); i17 = (i17 + 1)) {
            dsp->fRec18[i17] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i18;
        for (i18 = 0; (i18 < 2); i18 = (i18 + 1)) {
            dsp->fRec21[i18] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i19;
        for (i19 = 0; (i19 < 2048); i19 = (i19 + 1)) {
            dsp->fVec6[i19] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i20;
        for (i20 = 0; (i20 < 2); i20 = (i20 + 1)) {
            dsp->fRec20[i20] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i21;
        for (i21 = 0; (i21 < 2); i21 = (i21 + 1)) {
            dsp->fRec23[i21] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i22;
        for (i22 = 0; (i22 < 2048); i22 = (i22 + 1)) {
            dsp->fVec7[i22] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i23;
        for (i23 = 0; (i23 < 2); i23 = (i23 + 1)) {
            dsp->fRec22[i23] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i24;
        for (i24 = 0; (i24 < 1024); i24 = (i24 + 1)) {
            dsp->fVec8[i24] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i25;
        for (i25 = 0; (i25 < 2); i25 = (i25 + 1)) {
            dsp->fRec6[i25] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i26;
        for (i26 = 0; (i26 < 512); i26 = (i26 + 1)) {
            dsp->fVec9[i26] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i27;
        for (i27 = 0; (i27 < 2); i27 = (i27 + 1)) {
            dsp->fRec4[i27] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i28;
        for (i28 = 0; (i28 < 512); i28 = (i28 + 1)) {
            dsp->fVec10[i28] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i29;
        for (i29 = 0; (i29 < 2); i29 = (i29 + 1)) {
            dsp->fRec2[i29] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i30;
        for (i30 = 0; (i30 < 256); i30 = (i30 + 1)) {
            dsp->fVec11[i30] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i31;
        for (i31 = 0; (i31 < 2); i31 = (i31 + 1)) {
            dsp->fRec0[i31] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i32;
        for (i32 = 0; (i32 < 2); i32 = (i32 + 1)) {
            dsp->fRec33[i32] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i33;
        for (i33 = 0; (i33 < 2048); i33 = (i33 + 1)) {
            dsp->fVec12[i33] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i34;
        for (i34 = 0; (i34 < 2); i34 = (i34 + 1)) {
            dsp->fRec32[i34] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i35;
        for (i35 = 0; (i35 < 2); i35 = (i35 + 1)) {
            dsp->fRec35[i35] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i36;
        for (i36 = 0; (i36 < 2048); i36 = (i36 + 1)) {
            dsp->fVec13[i36] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i37;
        for (i37 = 0; (i37 < 2); i37 = (i37 + 1)) {
            dsp->fRec34[i37] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i38;
        for (i38 = 0; (i38 < 2); i38 = (i38 + 1)) {
            dsp->fRec37[i38] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i39;
        for (i39 = 0; (i39 < 2048); i39 = (i39 + 1)) {
            dsp->fVec14[i39] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i40;
        for (i40 = 0; (i40 < 2); i40 = (i40 + 1)) {
            dsp->fRec36[i40] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i41;
        for (i41 = 0; (i41 < 2); i41 = (i41 + 1)) {
            dsp->fRec39[i41] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i42;
        for (i42 = 0; (i42 < 2048); i42 = (i42 + 1)) {
            dsp->fVec15[i42] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i43;
        for (i43 = 0; (i43 < 2); i43 = (i43 + 1)) {
            dsp->fRec38[i43] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i44;
        for (i44 = 0; (i44 < 2); i44 = (i44 + 1)) {
            dsp->fRec41[i44] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i45;
        for (i45 = 0; (i45 < 2048); i45 = (i45 + 1)) {
            dsp->fVec16[i45] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i46;
        for (i46 = 0; (i46 < 2); i46 = (i46 + 1)) {
            dsp->fRec40[i46] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i47;
        for (i47 = 0; (i47 < 2); i47 = (i47 + 1)) {
            dsp->fRec43[i47] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i48;
        for (i48 = 0; (i48 < 2048); i48 = (i48 + 1)) {
            dsp->fVec17[i48] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i49;
        for (i49 = 0; (i49 < 2); i49 = (i49 + 1)) {
            dsp->fRec42[i49] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i50;
        for (i50 = 0; (i50 < 2); i50 = (i50 + 1)) {
            dsp->fRec45[i50] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i51;
        for (i51 = 0; (i51 < 2048); i51 = (i51 + 1)) {
            dsp->fVec18[i51] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i52;
        for (i52 = 0; (i52 < 2); i52 = (i52 + 1)) {
            dsp->fRec44[i52] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i53;
        for (i53 = 0; (i53 < 2); i53 = (i53 + 1)) {
            dsp->fRec47[i53] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i54;
        for (i54 = 0; (i54 < 2048); i54 = (i54 + 1)) {
            dsp->fVec19[i54] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i55;
        for (i55 = 0; (i55 < 2); i55 = (i55 + 1)) {
            dsp->fRec46[i55] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i56;
        for (i56 = 0; (i56 < 1024); i56 = (i56 + 1)) {
            dsp->fVec20[i56] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i57;
        for (i57 = 0; (i57 < 2); i57 = (i57 + 1)) {
            dsp->fRec30[i57] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i58;
        for (i58 = 0; (i58 < 512); i58 = (i58 + 1)) {
            dsp->fVec21[i58] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i59;
        for (i59 = 0; (i59 < 2); i59 = (i59 + 1)) {
            dsp->fRec28[i59] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i60;
        for (i60 = 0; (i60 < 512); i60 = (i60 + 1)) {
            dsp->fVec22[i60] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i61;
        for (i61 = 0; (i61 < 2); i61 = (i61 + 1)) {
            dsp->fRec26[i61] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i62;
        for (i62 = 0; (i62 < 256); i62 = (i62 + 1)) {
            dsp->fVec23[i62] = 0.f;

        }

    }
    /* C99 loop */
    {
        int i63;
        for (i63 = 0; (i63 < 2); i63 = (i63 + 1)) {
            dsp->fRec24[i63] = 0.f;

        }

    }

}

float op1, op2;
void computemydsp(mydsp* dsp, int count, FAUSTFLOAT ** inputs, FAUSTFLOAT ** outputs) {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* input1 = inputs[1];
    FAUSTFLOAT* output0 = outputs[0];
    FAUSTFLOAT* output1 = outputs[1];
    float fSlow0 = (float)dsp->fHslider0;
    float fSlow1 = (float)(0.7f + (float)(0.28f * (float)dsp->fHslider1));
    float fSlow2 = (float)(0.4f * (float)dsp->fHslider2);
    float fSlow3 = (float)(1.f - fSlow2);
    float fSlow4 = (float)(1.f - fSlow0);
    /* C99 loop */
    {
        int i;
        for (i = 0; (i < count); i = (i + 1)) {
            dsp->fRec9[0] = (float)((float)(fSlow2 * dsp->fRec9[1]) + (float)(fSlow3 * dsp->fRec8[1]));
            float fTemp0 = (float)input0[i];
            float fTemp1 = (float)input1[i];
            float fTemp2 = (float)(0.015f * (float)(fTemp0 + fTemp1));
            dsp->fVec0[(dsp->IOTA & 2047)] = (float)((float)(fSlow1 * dsp->fRec9[0]) + fTemp2);
            dsp->fRec8[0] = dsp->fVec0[((dsp->IOTA - 1116) & 2047)];
            dsp->fRec11[0] = (float)((float)(fSlow2 * dsp->fRec11[1]) + (float)(fSlow3 * dsp->fRec10[1]));
            dsp->fVec1[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec11[0]));
            dsp->fRec10[0] = dsp->fVec1[((dsp->IOTA - 1188) & 2047)];
            dsp->fRec13[0] = (float)((float)(fSlow2 * dsp->fRec13[1]) + (float)(fSlow3 * dsp->fRec12[1]));
            dsp->fVec2[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec13[0]));
            dsp->fRec12[0] = dsp->fVec2[((dsp->IOTA - 1277) & 2047)];
            dsp->fRec15[0] = (float)((float)(fSlow2 * dsp->fRec15[1]) + (float)(fSlow3 * dsp->fRec14[1]));
            dsp->fVec3[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec15[0]));
            dsp->fRec14[0] = dsp->fVec3[((dsp->IOTA - 1356) & 2047)];
            dsp->fRec17[0] = (float)((float)(fSlow2 * dsp->fRec17[1]) + (float)(fSlow3 * dsp->fRec16[1]));
            dsp->fVec4[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec17[0]));
            dsp->fRec16[0] = dsp->fVec4[((dsp->IOTA - 1422) & 2047)];
            dsp->fRec19[0] = (float)((float)(fSlow2 * dsp->fRec19[1]) + (float)(fSlow3 * dsp->fRec18[1]));
            dsp->fVec5[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec19[0]));
            dsp->fRec18[0] = dsp->fVec5[((dsp->IOTA - 1491) & 2047)];
            dsp->fRec21[0] = (float)((float)(fSlow2 * dsp->fRec21[1]) + (float)(fSlow3 * dsp->fRec20[1]));
            dsp->fVec6[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec21[0]));
            dsp->fRec20[0] = dsp->fVec6[((dsp->IOTA - 1557) & 2047)];
            dsp->fRec23[0] = (float)((float)(fSlow2 * dsp->fRec23[1]) + (float)(fSlow3 * dsp->fRec22[1]));
            dsp->fVec7[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec23[0]));
            dsp->fRec22[0] = dsp->fVec7[((dsp->IOTA - 1617) & 2047)];
            float fTemp3 = (float)((float)((float)((float)((float)((float)((float)(dsp->fRec8[0] + dsp->fRec10[0]) + dsp->fRec12[0]) + dsp->fRec14[0]) + dsp->fRec16[0]) + dsp->fRec18[0]) + dsp->fRec20[0]) + dsp->fRec22[0]);
            dsp->fVec8[(dsp->IOTA & 1023)] = (float)(fTemp3 + (float)(0.5f * dsp->fRec6[1]));
            dsp->fRec6[0] = dsp->fVec8[((dsp->IOTA - 556) & 1023)];
            float fRec7 = (float)(0.f - (float)(fTemp3 - dsp->fRec6[1]));
            dsp->fVec9[(dsp->IOTA & 511)] = (float)(fRec7 + (float)(0.5f * dsp->fRec4[1]));
            dsp->fRec4[0] = dsp->fVec9[((dsp->IOTA - 441) & 511)];
            float fRec5 = (float)(dsp->fRec4[1] - fRec7);
            dsp->fVec10[(dsp->IOTA & 511)] = (float)(fRec5 + (float)(0.5f * dsp->fRec2[1]));
            dsp->fRec2[0] = dsp->fVec10[((dsp->IOTA - 341) & 511)];
            float fRec3 = (float)(dsp->fRec2[1] - fRec5);
            dsp->fVec11[(dsp->IOTA & 255)] = (float)(fRec3 + (float)(0.5f * dsp->fRec0[1]));
            dsp->fRec0[0] = dsp->fVec11[((dsp->IOTA - 225) & 255)];
            float fRec1 = (float)(dsp->fRec0[1] - fRec3);
            output0[i] = (FAUSTFLOAT)(float)((float)(fSlow0 * fRec1) + (float)(fSlow4 * fTemp0));
            dsp->fRec33[0] = (float)((float)(fSlow2 * dsp->fRec33[1]) + (float)(fSlow3 * dsp->fRec32[1]));
            dsp->fVec12[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec33[0]));
            dsp->fRec32[0] = dsp->fVec12[((dsp->IOTA - 1139) & 2047)];
            dsp->fRec35[0] = (float)((float)(fSlow2 * dsp->fRec35[1]) + (float)(fSlow3 * dsp->fRec34[1]));
            dsp->fVec13[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec35[0]));
            dsp->fRec34[0] = dsp->fVec13[((dsp->IOTA - 1211) & 2047)];
            dsp->fRec37[0] = (float)((float)(fSlow2 * dsp->fRec37[1]) + (float)(fSlow3 * dsp->fRec36[1]));
            dsp->fVec14[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec37[0]));
            dsp->fRec36[0] = dsp->fVec14[((dsp->IOTA - 1300) & 2047)];
            dsp->fRec39[0] = (float)((float)(fSlow2 * dsp->fRec39[1]) + (float)(fSlow3 * dsp->fRec38[1]));
            dsp->fVec15[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec39[0]));
            dsp->fRec38[0] = dsp->fVec15[((dsp->IOTA - 1379) & 2047)];
            dsp->fRec41[0] = (float)((float)(fSlow2 * dsp->fRec41[1]) + (float)(fSlow3 * dsp->fRec40[1]));
            dsp->fVec16[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec41[0]));
            dsp->fRec40[0] = dsp->fVec16[((dsp->IOTA - 1445) & 2047)];
            dsp->fRec43[0] = (float)((float)(fSlow2 * dsp->fRec43[1]) + (float)(fSlow3 * dsp->fRec42[1]));
            dsp->fVec17[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec43[0]));
            dsp->fRec42[0] = dsp->fVec17[((dsp->IOTA - 1514) & 2047)];
            dsp->fRec45[0] = (float)((float)(fSlow2 * dsp->fRec45[1]) + (float)(fSlow3 * dsp->fRec44[1]));
            dsp->fVec18[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec45[0]));
            dsp->fRec44[0] = dsp->fVec18[((dsp->IOTA - 1580) & 2047)];
            dsp->fRec47[0] = (float)((float)(fSlow2 * dsp->fRec47[1]) + (float)(fSlow3 * dsp->fRec46[1]));
            dsp->fVec19[(dsp->IOTA & 2047)] = (float)(fTemp2 + (float)(fSlow1 * dsp->fRec47[0]));
            dsp->fRec46[0] = dsp->fVec19[((dsp->IOTA - 1640) & 2047)];
            float fTemp4 = (float)((float)((float)((float)((float)((float)((float)(dsp->fRec32[0] + dsp->fRec34[0]) + dsp->fRec36[0]) + dsp->fRec38[0]) + dsp->fRec40[0]) + dsp->fRec42[0]) + dsp->fRec44[0]) + dsp->fRec46[0]);
            dsp->fVec20[(dsp->IOTA & 1023)] = (float)(fTemp4 + (float)(0.5f * dsp->fRec30[1]));
            dsp->fRec30[0] = dsp->fVec20[((dsp->IOTA - 579) & 1023)];
            float fRec31 = (float)(0.f - (float)(fTemp4 - dsp->fRec30[1]));
            dsp->fVec21[(dsp->IOTA & 511)] = (float)(fRec31 + (float)(0.5f * dsp->fRec28[1]));
            dsp->fRec28[0] = dsp->fVec21[((dsp->IOTA - 464) & 511)];
            float fRec29 = (float)(dsp->fRec28[1] - fRec31);
            dsp->fVec22[(dsp->IOTA & 511)] = (float)(fRec29 + (float)(0.5f * dsp->fRec26[1]));
            dsp->fRec26[0] = dsp->fVec22[((dsp->IOTA - 364) & 511)];
            float fRec27 = (float)(dsp->fRec26[1] - fRec29);
            dsp->fVec23[(dsp->IOTA & 255)] = (float)(fRec27 + (float)(0.5f * dsp->fRec24[1]));
            dsp->fRec24[0] = dsp->fVec23[((dsp->IOTA - 248) & 255)];
            float fRec25 = (float)(dsp->fRec24[1] - fRec27);
            output1[i] = (FAUSTFLOAT)(float)((float)(fSlow0 * fRec25) + (float)(fSlow4 * fTemp1));
            dsp->fRec9[1] = dsp->fRec9[0];
            dsp->IOTA = (dsp->IOTA + 1);
            dsp->fRec8[1] = dsp->fRec8[0];
            dsp->fRec11[1] = dsp->fRec11[0];
            dsp->fRec10[1] = dsp->fRec10[0];
            dsp->fRec13[1] = dsp->fRec13[0];
            dsp->fRec12[1] = dsp->fRec12[0];
            dsp->fRec15[1] = dsp->fRec15[0];
            dsp->fRec14[1] = dsp->fRec14[0];
            dsp->fRec17[1] = dsp->fRec17[0];
            dsp->fRec16[1] = dsp->fRec16[0];
            dsp->fRec19[1] = dsp->fRec19[0];
            dsp->fRec18[1] = dsp->fRec18[0];
            dsp->fRec21[1] = dsp->fRec21[0];
            dsp->fRec20[1] = dsp->fRec20[0];
            dsp->fRec23[1] = dsp->fRec23[0];
            dsp->fRec22[1] = dsp->fRec22[0];
            dsp->fRec6[1] = dsp->fRec6[0];
            dsp->fRec4[1] = dsp->fRec4[0];
            dsp->fRec2[1] = dsp->fRec2[0];
            dsp->fRec0[1] = dsp->fRec0[0];
            dsp->fRec33[1] = dsp->fRec33[0];
            dsp->fRec32[1] = dsp->fRec32[0];
            dsp->fRec35[1] = dsp->fRec35[0];
            dsp->fRec34[1] = dsp->fRec34[0];
            dsp->fRec37[1] = dsp->fRec37[0];
            dsp->fRec36[1] = dsp->fRec36[0];
            dsp->fRec39[1] = dsp->fRec39[0];
            dsp->fRec38[1] = dsp->fRec38[0];
            dsp->fRec41[1] = dsp->fRec41[0];
            dsp->fRec40[1] = dsp->fRec40[0];
            dsp->fRec43[1] = dsp->fRec43[0];
            dsp->fRec42[1] = dsp->fRec42[0];
            dsp->fRec45[1] = dsp->fRec45[0];
            dsp->fRec44[1] = dsp->fRec44[0];
            dsp->fRec47[1] = dsp->fRec47[0];
            dsp->fRec46[1] = dsp->fRec46[0];
            dsp->fRec30[1] = dsp->fRec30[0];
            dsp->fRec28[1] = dsp->fRec28[0];
            dsp->fRec26[1] = dsp->fRec26[0];
            dsp->fRec24[1] = dsp->fRec24[0];

        }

    }
}