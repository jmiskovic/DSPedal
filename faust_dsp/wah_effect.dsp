crybaby(wah) = *(gs) : tf2(1,-1,0,a1s,a2s)
with {
  Q  = pow(2.0,(2.0*(1.0-wah)+1.0)); // Resonance "quality factor"
  fr = 450.0*pow(2.0,2.3*wah);       // Resonance tuning
  g  = 0.1*pow(4.0,wah);             // gain (optional)

  // Biquad fit using z = exp(s T) ~ 1 + sT for low frequencies:
  frn = fr/SR; // Normalized pole frequency (cycles per sample)
  R = 1 - PI*frn/Q; // pole radius
  theta = 2*PI*frn; // pole angle
  a1 = 0-2.0*R*cos(theta); // biquad coeff
  a2 = R*R;                // biquad coeff

  // dezippering of slider-driven signals:
  s = 0.999; // smoothing parameter (one-pole pole location)
  a1s = a1 : smooth(s);
  a2s = a2 : smooth(s);
  gs =  g  : smooth(s);

  tf2 = component("filter.lib").tf2;
};

process = (_) : bypass1(bp, crybaby(wah)) : (_) with {
   bp = wah_bp;
   wah = wah_sl;
};
