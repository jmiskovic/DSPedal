#include "board.h"
#include "adxl345.h"
#include "CUI.h"
#include "interface.h"
#include "faust.h"

Slider slider[10];
UIGlue glue;
int active_sliders = 0;

Slider * xSlider = NULL;
Slider * ySlider = NULL;

Slider * keyboardSlider = NULL;

void openTabBox (void* ui_interface, const char* label) {

}
void openHorizontalBox (void* ui_interface, const char* label) {

}
void openVerticalBox (void* ui_interface, const char* label) {

}
void closeBox (void* ui_interface) {

}

/* -- active widgets */

void addButton (void* ui_interface, const char* label, FAUSTFLOAT* zone) {

}

void addCheckButton (void* ui_interface, const char* label, FAUSTFLOAT* zone) {

}

void addSlider (void* ui_interface, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {
    slider[active_sliders].label = label;
    slider[active_sliders].zone = zone;
    slider[active_sliders].init = init;
    slider[active_sliders].min = min;
    slider[active_sliders].max = max;
    slider[active_sliders].step = step;
    if (!keyboardSlider)
        keyboardSlider = &slider[active_sliders];
    else if (!xSlider)
        xSlider = &slider[active_sliders];
    else
        ySlider = &slider[active_sliders];
    active_sliders = active_sliders < sizeof(slider) / sizeof(slider[0]) ? active_sliders + 1 : active_sliders;
}

/* -- passive display widgets */

void addBargraph (void* ui_interface, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {

}

void declare (void* ui_interface, FAUSTFLOAT* zone, const char* key, const char* value) {

}


void interface_init() {
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
    ADXL345_Init(MR_2_g);
    ADXL345_power_on();
}

xyz min = {0};
xyz max = {0};
void update_min_max(xyz acc) {
    min.x = acc.x < min.x ? acc.x : min.x;
    min.y = acc.y < min.y ? acc.y : min.y;
    min.z = acc.z < min.z ? acc.z : min.z;

    max.x = acc.x > max.x ? acc.x : max.x;
    max.y = acc.y > max.y ? acc.y : max.y;
    max.z = acc.z > max.z ? acc.z : max.z;
}

/* calculate linear mapping of Aval in range [Amin, Amax] to range [Bmin, Bmax] */
FAUSTFLOAT mapRange(FAUSTFLOAT Amin, FAUSTFLOAT Amax, FAUSTFLOAT Bmin, FAUSTFLOAT Bmax, FAUSTFLOAT Aval) {
    return Bmin + (Aval-Amin)*(Bmax-Bmin)/(Amax-Amin);
}

void interface_process() {
    xyz acc = ADXL345_raw_read_acc_xyz();
    low_pass_filter (&acc);
    update_min_max(acc);
    if (xSlider)
        *(xSlider->zone) = mapRange(min.x, max.x, xSlider->min, xSlider->max, acc.x);
    else
        *(ySlider->zone) = mapRange(min.y, max.y, ySlider->min, ySlider->max, acc.y);

    uint32_t key = Buttons_GetStatus();
    if (keyboardSlider != NULL && key) {
        switch (key) {
        case BUTTONS_UP:
            *(keyboardSlider->zone) += keyboardSlider->step;
            if (*(keyboardSlider->zone) > keyboardSlider->max)
                *(keyboardSlider->zone) = keyboardSlider->max;
            break;
        case BUTTONS_DOWN:
            *(keyboardSlider->zone) -= keyboardSlider->step;
            if (*(keyboardSlider->zone) < keyboardSlider->min)
                *(keyboardSlider->zone) = keyboardSlider->min;
            break;
        case BUTTONS_RIGHT:
            keyboardSlider += 1;
            if (keyboardSlider >= &(slider[active_sliders]))
                keyboardSlider = &(slider[0]);
            DEBUGOUT("Selected param %d", (keyboardSlider - &(slider[0])) / sizeof(Slider));
            break;
        case BUTTONS_LEFT:
            keyboardSlider -= 1;
            if (keyboardSlider < &(slider[0]))
                keyboardSlider = &(slider[active_sliders - 1]);
            break;
        default:
            break;
        }
    }
}