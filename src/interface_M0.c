#include "board.h"
#include "adxl345.h"
#include "CUI.h"
#include "interface.h"
#include "pots.h"
#include "faust.h"

#define SLIDER_COUNT 10

static Slider slider[SLIDER_COUNT];
static UIGlue glue;
static int active_sliders = 0;

static AnalogController pot[3];
static AnalogController angle;

static Slider * keys;

static void openTabBox (void* ui_interface, const char* label);
static void openHorizontalBox (void* ui_interface, const char* label);
static void openVerticalBox (void* ui_interface, const char* label);
static void closeBox (void* ui_interface);
static void addButton (void* ui_interface, const char* label, FAUSTFLOAT* zone);
static void addSlider (void* ui_interface, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);
static void addCheckButton (void* ui_interface, const char* label, FAUSTFLOAT* zone);
static void addBargraph (void* ui_interface, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max);
static void declare (void* ui_interface, FAUSTFLOAT* zone, const char* key, const char* value);

void interface_init() {
    pots_init();
    //ADXL345_Init(MR_2_g);
    //ADXL345_power_on();
    glue.uiInterface = 0;
    glue.openTabBox = openTabBox;
    glue.openHorizontalBox = openHorizontalBox;
    glue.openVerticalBox = openVerticalBox;
    glue.closeBox = closeBox;
    glue.addButton = addButton;
    glue.addCheckButton = addCheckButton;
    glue.addVerticalSlider = addSlider;
    glue.addHorizontalSlider = addSlider;
    glue.addNumEntry = addSlider;
    glue.addHorizontalBargraph = addBargraph;
    glue.addVerticalBargraph = addBargraph;
    glue.declare = declare;

    mydsp * dsp = (mydsp * ) DSP_MEMORY_BLOCK_START;
    buildUserInterfacemydsp(dsp, &glue);

}


static void addSlider (void* ui_interface, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {
    slider[active_sliders].label = label;
    slider[active_sliders].zone = zone;
    slider[active_sliders].init = init;
    slider[active_sliders].min = min;
    slider[active_sliders].max = max;
    slider[active_sliders].step = step;
    /* first couple of sliders are automatically assigned to pots */
    if (active_sliders < POTS_COUNT) {
        pot[active_sliders].min = 0;
        pot[active_sliders].max = 1024;
        pot[active_sliders].slider = &(slider[active_sliders]);
    }
    if (active_sliders == 0)
        keys = &slider[active_sliders];
    active_sliders = active_sliders < SLIDER_COUNT - 1 ? active_sliders + 1 : 0;
}

/* calculate linear mapping of Aval in range [Amin, Amax] to range [Bmin, Bmax] */
FAUSTFLOAT mapRange(FAUSTFLOAT Amin, FAUSTFLOAT Amax, FAUSTFLOAT Bmin, FAUSTFLOAT Bmax, FAUSTFLOAT Aval) {
    return Bmin + (Aval-Amin)*(Bmax-Bmin)/(Amax-Amin);
}

void interface_process() {
    int i;
    //xyz acc = ADXL345_raw_read_acc_xyz();

    //if (xSlider)
    //    *(xSlider->zone) = mapRange(min.x, max.x, xSlider->min, xSlider->max, acc.x);

    for (i = 0; i < POTS_COUNT; i++) {
        Slider * s = pot[i].slider;
        if (s == NULL)
            continue;
        pot[i].val = pots_read(i);
        *(s->zone) = mapRange(pot[i].min, pot[i].max, s->min, s->max, pot[i].val);
    }

    uint32_t key = Buttons_GetStatus();
    if (keys != NULL && key) {
        switch (key) {
        case BUTTONS_UP:
            *(keys->zone) += keys->step;
            if (*(keys->zone) > keys->max)
                *(keys->zone) = keys->max;
            break;
        case BUTTONS_DOWN:
            *(keys->zone) -= keys->step;
            if (*(keys->zone) < keys->min)
                *(keys->zone) = keys->min;
            break;
        case BUTTONS_RIGHT:
            keys += 1;
            if (keys >= &(slider[active_sliders]))
                keys = &(slider[0]);
            DEBUGOUT("Selected param %d", (keys - &(slider[0])) / sizeof(Slider));
            break;
        case BUTTONS_LEFT:
            keys -= 1;
            if (keys < &(slider[0]))
                keys = &(slider[active_sliders - 1]);
            break;
        default:
            break;
        }
    }
}

/* stubs */

static void openTabBox (void* ui_interface, const char* label) {
}

static void openHorizontalBox (void* ui_interface, const char* label) {
}

static void openVerticalBox (void* ui_interface, const char* label) {
}

static void closeBox (void* ui_interface) {
}

static void addButton (void* ui_interface, const char* label, FAUSTFLOAT* zone) {
}

static void addCheckButton (void* ui_interface, const char* label, FAUSTFLOAT* zone) {
}

static void addBargraph (void* ui_interface, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {
}

static void declare (void* ui_interface, FAUSTFLOAT* zone, const char* key, const char* value) {
}