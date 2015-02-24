#include "board.h"
#include "adxl345.h"
#include "CUI.h"
#include "interface.h"
#include "pots.h"
#include "graphics.h"
#include "faust.h"

#define SLIDER_COUNT 15

static Slider sliders[SLIDER_COUNT];
static UIGlue glue;
static int active_sliders = 0;

static AnalogController pots[POTS_COUNT];
static AnalogController accel_x;
static AnalogController accel_y;

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

static void update_minmax(AnalogController * controller);
static AnalogController * select_next_controller(AnalogController * current);
static void draw_info(Slider * slider);
static void draw_icon(Slider * slider);
static void draw_bars(Slider * slider);
static void draw_knob(int8_t x, uint8_t y, uint8_t value);
static void interface_draw(Slider * slider);

void interface_init() {
    pots_init();
    graphics_init();
    ADXL345_Init(MR_2_g);
    ADXL345_power_on();
    accel_x.val = accel_x.min = accel_x.max = ADXL345_raw_read_acc(0);
    accel_y.val = accel_y.min = accel_y.max = ADXL345_raw_read_acc(1);
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
    sliders[active_sliders].label = label;
    sliders[active_sliders].zone = zone;
    sliders[active_sliders].init = init;
    sliders[active_sliders].min = min;
    sliders[active_sliders].max = max;
    sliders[active_sliders].step = step;
    /* first couple of sliders are automatically assigned to pots */
    if (active_sliders < POTS_COUNT) {
        pots[active_sliders].min = 0;
        pots[active_sliders].max = 1024;
        sliders[active_sliders].controller = &pots[active_sliders];
    }
    if (active_sliders == 0)
        keys = &sliders[active_sliders];
    active_sliders = active_sliders < SLIDER_COUNT - 1 ? active_sliders + 1 : 0;
}

/* calculate linear mapping of Aval in range [Amin, Amax] to range [Bmin, Bmax] */
FAUSTFLOAT mapRange(FAUSTFLOAT Amin, FAUSTFLOAT Amax, FAUSTFLOAT Bmin, FAUSTFLOAT Bmax, FAUSTFLOAT Aval) {
    return Bmin + (Aval-Amin)*(Bmax-Bmin)/(Amax-Amin);
}

static void update_minmax(AnalogController * controller) {
    controller->min = controller->val < controller->min ? controller->val : controller-> min;
    controller->max = controller->val > controller->max ? controller->val : controller-> max;
}

void interface_process() {
    static uint16_t debounce = 0;
    const uint16_t DEBOUNCE_TIME = 50;
    int i;

    /* read accelerometers */
    accel_x.val = ADXL345_raw_read_acc(0);
    update_minmax(&accel_x);
    accel_y.val = ADXL345_raw_read_acc(1);
    update_minmax(&accel_y);

    for (i = 0; i < POTS_COUNT; i++) {
        pots[i].val = pots_read(i);
    }

    for (i = 0; i < active_sliders; i++) {
        if (sliders[i].controller != NULL) {
            AnalogController * c = sliders[i].controller;
            *(sliders[i].zone) = mapRange(c->min, c->max, sliders[i].min, sliders[i].max, c->val);
        }
    }

    debounce -= debounce > 0;
    uint32_t key = Buttons_GetStatus();
    if (keys != NULL && !debounce && key) {
        switch (key) {
        case BUTTONS_UP:
            debounce = DEBOUNCE_TIME / 10;
            *(keys->zone) += keys->step;
            if (*(keys->zone) > keys->max)
                *(keys->zone) = keys->max;
            break;
        case BUTTONS_DOWN:
            debounce = DEBOUNCE_TIME / 10;
            *(keys->zone) -= keys->step;
            if (*(keys->zone) < keys->min)
                *(keys->zone) = keys->min;
            break;
        case BUTTONS_RIGHT:
            debounce = DEBOUNCE_TIME;
            keys += 1;
            if (keys >= &(sliders[active_sliders]))
                keys = &(sliders[active_sliders - 1]);
            DEBUGOUT("Selected param %d", (keys - &(sliders[0])) / sizeof(Slider));
            break;
        case BUTTONS_LEFT:
            debounce = DEBOUNCE_TIME;
            keys -= 1;
            if (keys < &(sliders[0]))
                keys = &(sliders[0]);
            break;
        case BUTTONS_PRESS:
            debounce = DEBOUNCE_TIME;
            keys->controller = select_next_controller(keys->controller);
            break;
        default:
            break;
        }
    }
    if (keys != NULL) {
        interface_draw(keys);
    }
}

static AnalogController * select_next_controller(AnalogController * current) {
    if (current == NULL)
        return &accel_x;
    else if (current == &accel_x)
        return &accel_y;
    else if (current == &accel_y)
        return &pots[0];
    else if (current >= &pots[0] && current <= &pots[POTS_COUNT - 2])
        return current + 1;
    else
        return NULL;
}

static void draw_info(Slider * slider) {
    int16_t whole = (int16_t) (*(slider->zone));
    uint16_t decimal = (int16_t) (*(slider->zone) * 100) - whole * 100;
    char value_buffer[10];
    sprintf(value_buffer, "%3d.%02d", whole, decimal);
    graphics_write(slider->label, andale_font_bitmap, or_op, 5, LCD_PIXEL_HEIGHT - andale_font_bitmap.height);
    graphics_write(value_buffer, andale_font_bitmap, or_op, 26, LCD_PIXEL_HEIGHT - andale_font_bitmap.height - andale_font_bitmap.height - 1);
}

static void draw_knob(int8_t x, uint8_t y, uint8_t value) {
    uint8_t width = (knobbase_bitmap.width - 2) * value / 255;
    graphics_draw(knobbase_bitmap, or_op, x, y);
    graphics_draw_rect(x + 1, y + 1, width, knobbase_bitmap.height - 2);
    graphics_draw(knobmask_bitmap, and_op, x, y);
}

const bitmap * const pot_bitmaps[POTS_COUNT] = {&pot1_bitmap, &pot2_bitmap, &pot3_bitmap, &expression1_bitmap, &expression2_bitmap};

static void draw_icon(Slider * slider) {
    const bitmap * icon = NULL;
    for (int i = 0; i < POTS_COUNT; i++) {
        AnalogController * c = &pots[i];
        if (slider->controller == c) {
            icon = pot_bitmaps[i];
        }
    }
    if (slider->controller == &accel_x)
        icon = &accel_x_bitmap;
    else if (slider->controller == &accel_y)
        icon = &accel_y_bitmap;
    if (icon)
        graphics_draw(*icon, or_op, 38, 1);
}

static void draw_bars(Slider * slider) {
    uint8_t i;
    bool center = false;
    const uint8_t max_height = 30;
    const uint8_t width = 3;
    const uint8_t gap = 1;

    uint8_t height;
    for (i = 0; i < active_sliders; i++) {
        if (slider == &sliders[i]) { /* skip center knob (already drawn) */
            center = true;
            continue;
        }
        height = mapRange(sliders[i].min, sliders[i].max, 0, max_height, *(sliders[i].zone));
        if (!center) {
            graphics_draw_rect(i * (width + gap), max_height - height, width, height + 1);
        } else {
            graphics_draw_rect(84 - (active_sliders - i) * (width + gap), max_height - height, width, height + 1);
        }
    }
}

static void interface_draw(Slider * slider) {

    graphics_clear();
    /* write description and current value */
    draw_info(slider);
    /* draw knob */
    draw_knob(LCD_PIXEL_WIDTH / 2 - knobmask_bitmap.width/2, 14, mapRange(slider->min, slider->max, 0, 255, *(keys->zone)));
    /* draw control icon */
    draw_icon(slider);
    /* draw bars */
    draw_bars(slider);
    graphics_refresh();
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