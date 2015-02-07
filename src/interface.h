#ifndef INTERFACE_H
#define INTERFACE_H value

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

void interface_init(void);
void interface_process(void);

typedef struct {
    const char * label;
    FAUSTFLOAT * zone;
    FAUSTFLOAT init;
    FAUSTFLOAT min;
    FAUSTFLOAT max;
    FAUSTFLOAT step;
} Slider;

typedef struct {
    Slider * slider;
    int16_t min;
    int16_t max;
    int16_t val;
} AnalogController;

#endif /* INTERFACE_H */