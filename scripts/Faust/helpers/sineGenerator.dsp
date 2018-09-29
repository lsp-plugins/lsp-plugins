os = library("oscillators.lib");
ba = library("basics.lib");

sineWaveOscillator = *(gain, os.osc(frequency))
with{
    gain        = nentry("[1]Peak Amplitude[unit:dB]:", -1, -150, 10, 0.1) : ba.db2linear;
    frequency   = nentry("[2]Frequency:[unit:Hz]", 440, 1, 20000, 0.001);
};

process = sineWaveOscillator;
