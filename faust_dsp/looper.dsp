math = library("math.lib");

threshold = >(0.8);

reset   = hslider("[1]Reset", 0, 0, 1, 1) : threshold;
pan     = hslider("[2]Pan", 0.5, 0, 1, 0.05);
vol     = hslider("[3]Vol", 1.0, 0, 1, 0.05);
trigger = hslider("[4]Trigger", 1, 0, 1, 1) : 1 - _ : threshold;
commit  = hslider("[5]Commit", 0, 0, 1, 1) : threshold;

TABLE_SIZE = int(48000 * 43); // seconds at sample rate

/* UTILITY */
clamp(minv,maxv) = (_) : max(minv) : min(maxv) : (_);
iter(count, trig) = math.if(trig>0, min(count, _+1), 0) ~_;
flipflop(on_trigger, off_trigger) = &(1 - off_trigger) ~ |(on_trigger) : (_);
pulse_duration(trigger, duration) = |(trigger) ~ (iter(duration) : in_count): (_)
    with {
        in_count(x) = (x > 0) & (x < loop_time);
    };
visualize(sig, n) = sig : hbargraph("Signal %n[sytle:led]", 0, 1);

leveler = (_) : *(vol) : (_);
panner = (_) <: *(1 - pan : sqrt), *(pan : sqrt) : (_,_);

committed = flipflop(commit, reset);
committing = pulse_duration(commit, loop_time);
resetting = pulse_duration(reset, loop_time);


trigger_time = iter(TABLE_SIZE, trigger) : (_);
loop_time = value_holder * resetter ~ _ : clamp(1, TABLE_SIZE) : (_)
    with {
        value_holder(x) = math.if(committed, x, max(trigger_time, x));   // if perma loop is active, trigger no longer sets the loop time
        resetter = 1 - (1 - trigger@1) & (trigger) | committed;
    };

/* INSTA */
insta_looper = (_) : *(in_gate) : + ~ ( _@loop_time : *(feedback_gate)) : *(out_gate) : (_)
    with {
        in_gate = trigger : (_);
        feedback_gate = loop_time > 1 : mute_comitted : mute_retriggered : mute_resetted : (_)
            with {
                mute_comitted = *(1 - pulse_duration((1 - committing) & committing@1, loop_time)); // mute feedback for single loop cycle after commiting
                mute_retriggered = *(1 - pulse_duration(committed * trigger, loop_time)); // mute feedback for single loop cycle if triggered
                mute_resetted = *(1 - resetting);   // mute feedback for duration of reset sequence
            };
        out_gate = loop_time == loop_time@1 :(_);
    };


/* PERMA */
perma_looper = (_) : *(in_gate) : + ~ ( _@loop_time : *(feedback_gate)) : *(out_gate) : (_)
    with {
        in_gate = committing : (_);
        feedback_gate = 1 - resetting : (_);
        out_gate = 1 : (_);
    };

committing_muter = (_) : *(1 - committing) : (_); // mutes signal while commit is in progress, so the volume level doesn't double

stereo_looper = (_) : insta_looper : panner <: ((committing_muter,committing_muter), (perma_looper, perma_looper)) :> (leveler,leveler) : (_,_);

process = (_) <: (_ <: (_,_) , stereo_looper) :> (_ , _);
//process = visualize(committing, 0), visualize(commit, 1), visualize(commit@loop_time, 2), (loop_time : hbargraph("loop time", 0, TABLE_SIZE));
