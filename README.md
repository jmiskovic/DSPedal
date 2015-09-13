# DSPedal #

DSPedal is a custom hardware platform for real-time sound effects. It is operated by few knobs, external expression pedal, couple of buttons and a small graphical LCD.

![block-diagram_dualcore.png](https://bitbucket.org/repo/Xzbg6G/images/842279159-block-diagram_dualcore.png)

## Hardware ##

Base board is Element14 LPC4357-EVB. Additional board holds knobs and graphical LCD (a salvaged nokia 3310 screen).

Everything is powered by NXP controller LPC4357 with integrated Cortex M4 and Cortex M0 cores. The M4 core is dedicated to DSP operations at 48 kHz sample rate. The M0 core operates the user interface (knobs, LCD, buttons).

Aside from controller the base board has 256 MB SDRAM, and accelerometer and an audio codec. All three components are supported by firmware.

## Real time effects ##

Processing effect is written in [Faust](http://faust.grame.fr/) programming language and compiled into C code. So far the Cortex M4 core clocked at 204 MHz has proven to be capable of executing relatively complex effects like wah, but effect-chaining quickly reaches the limits of processing power.

## Firmware ##

LPCOpen is used as HAL. The port to this platform is extracted as a separate project [here](https://bitbucket.org/jmiskovic/lpcopen-for-lpc4357-evb). The toolchain and workflow is described there.

For graphics, Andy Gock's driver for PCD8544 was reused. A library was written to support drawing of bitmaps and fonts. The user interface uses that library to display arbitrary number of effect parameters with some additional information for convenience. Each parameter can be assigned to a knob, expression pedal, accelerometer axis, or it can be manipulated with buttons.

![interface.jpg](https://bitbucket.org/repo/Xzbg6G/images/80519996-interface.jpg)

Faust compiler is not included in repository. It should be obvious from Makefile how to invoke the compilation of Faust effects. The whole compilation is rather involving because code has to be compiled for Cortex M0 and Cortex M4 architectures separately, and linked correctly so it can execute from internal flash and from RAM (for debugging).

The work in progress in *patcher* branch is the dynamic loading and chaining of different Faust effects. This requires even more complex linking through python scripts.