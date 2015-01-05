#ifndef FAUST_H
#define FAUST_H

#include "CUI.h"

typedef struct mydsp mydsp;

mydsp* newmydsp();
void deletemydsp(mydsp* dsp);
void metadatamydsp(MetaGlue* m);
int getSampleRatemydsp(mydsp* dsp);
int getNumInputsmydsp(mydsp* dsp);
int getNumOutputsmydsp(mydsp* dsp);
int getInputRatemydsp(mydsp* dsp, int channel);
int getOutputRatemydsp(mydsp* dsp, int channel);
void classInitmydsp(int samplingFreq);
void instanceInitmydsp(mydsp* dsp, int samplingFreq);
void initmydsp(mydsp* dsp, int samplingFreq);
void buildUserInterfacemydsp(mydsp* dsp, UIGlue* interface);
void computemydsp(mydsp* dsp, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs);

#endif /* FAUST_H */