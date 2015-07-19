filter = library("filter.lib");
music = library("music.lib");
math = library("math.lib");
effect = library("effect.lib");

crybaby(wah_pos) = *(gs) : tf2(1,-1,0,a1s,a2s)
with {
  Q  = pow(2.0,(2.0*(1.0-wah_pos)+1.0)); // Resonance "quality factor"
  fr = 450.0*pow(2.0,2.3*wah_pos);       // Resonance tuning
  g  = 0.1*pow(4.0,wah_pos);             // gain (optional)

  // Biquad fit using z = exp(s T) ~ 1 + sT for low frequencies:
  frn = fr/music.SR; // Normalized pole frequency (cycles per sample)
  R = 1 - math.PI*frn/Q; // pole radius
  theta = 2*math.PI*frn; // pole angle
  a1 = 0-2.0*R*cos(theta); // biquad coeff
  a2 = R*R;                // biquad coeff

  // dezippering of slider-driven signals:
  s = 0.999; // smoothing parameter (one-pole pole location)
  a1s = a1 : filter.smooth(s);
  a2s = a2 : filter.smooth(s);
  gs =  g  : filter.smooth(s);

  tf2 = component("filter.lib").tf2;
};

threshold = >(0.8);

process = (_) : effect.bypass1(bp, crybaby(wah_pos)) : (_) with {
   bp      = hslider("[0] Bypass", 0, 0, 1, 1) : threshold;
   wah_pos = hslider("[1] Wah",0.8,0,1,0.01);
};
