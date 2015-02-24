import("math.lib");

threshold = >(0.8);

commit  = hslider("[1]Commit", 0, 0, 1, 1) : threshold;
pan     = hslider("[2]Pan", 0.5, 0, 1, 0.05);
vol     = hslider("[3]Vol", 1.0, 0, 1, 0.05);
trigger = hslider("[4]Trigger", 1, 0, 1, 1) : 1 - _ : threshold;
reset   = hslider("[5]Reset", 0, 0, 1, 1) : threshold;

TABLE_SIZE = int(48000 * 21); // seconds at sample rate


/* UTILITY */
clamp(minv,maxv) = (_) : max(minv) : min(maxv) : (_);
pulse(on_trigger, off_trigger) = &(1 - off_trigger) ~ |(on_trigger) : (_);
visualize(sig, n) = sig : hbargraph("Signal %n[sytle:led]", 0, 1);

leveler = (_) : *(vol) : (_);
panner = (_) <: *(1 - pan : sqrt), *(pan : sqrt) : (_,_);

committed = pulse(commit, reset);
committing = pulse(commit, commit@loop_time);

resetting = pulse(reset, reset@loop_time);

trigger_time = countup(TABLE_SIZE, trigger == 0) : (_);
loop_time = selector * threshold(1 - (1 - trigger@1) * (trigger) + committed) ~ _ : clamp(1, TABLE_SIZE) : (_)
    with {
        selector(x) = if(committed, x, max(trigger_time, x));
    };

read_index = +(1) ~ %(loop_time + 1) : (_);
write_index = if(trigger, read_index@1, 0) : (_);

/* INSTA */
insta_in_gate = trigger : (_);
insta_feedback_gate = loop_time > 1 : mute_comitted : mute_retriggered : (_)
    with {
        mute_comitted = *(1 - committing@loop_time);
        mute_retriggered = *(1 - pulse(committed * trigger, (committed * trigger)@loop_time));
    };
insta_out_gate = loop_time == loop_time@1 : (_);
insta_looper = (_) : *(insta_in_gate) : + ~ ( _@loop_time : *(insta_feedback_gate)) : *(insta_out_gate) : (_);


/* PERMA */
perma_in_gate = committing : (_);
perma_feedback_gate = 1 - resetting : (_);
perma_out_gate = 1 : (_);
perma_looper = (_) : *(perma_in_gate) : + ~ ( _@loop_time : *(perma_feedback_gate)) : *(perma_out_gate) : (_);


stereo_looper = (_) : insta_looper : panner <: ((_,_), (perma_looper, perma_looper)) :> (leveler,leveler) : (_,_);

process = (_) <: (_ <: (_,_) , stereo_looper) :> (_ , _);
//process = visualize(committing, 0), visualize(commit, 1), visualize(commit@loop_time, 2), (loop_time : hbargraph("loop time", 0, TABLE_SIZE));
