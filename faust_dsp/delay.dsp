// Analog delay pedal
import("filter.lib");

MAX_DELAY = 1.0 * 48000;
MIN_DELAY = 0.5 * 48000;

retain = hslider("[1]Retain",   0,    0,      1.0,    0.05);
delayp = hslider("[2]Delay",    MAX_DELAY,    MIN_DELAY,      MAX_DELAY,   1);
mix    = hslider("[3]Mix",      0.5,    0.0,    1.0,    0.005);
pan    = hslider("[4]Pan",      0.5,    0,      1,      0.05);
warmth = hslider("[5]Warmth",   0.5,    0.0,    0.8,    0.01);

heater = iir((1 - warmth), (warmth));

analog_delay = (_) : + ~ (heater : delayer : leveler(retain)) : leveler(mix) : panner : (_,_)
    with {
        delayer = _ @ delayp;
        panner = (_) <: *(1 - pan : sqrt), *(pan : sqrt) : (_,_);
        leveler(vol) = (_) : *(vol) : (_);
    };

process = (_) <: ((_ <: (_,_)) , analog_delay) :> (_ , _);