ba = library("basics.lib");

gainStage = *(_, amplitude)
with {
    amplitude = hslider("Gain [unit:dB][style:knob]", 0, -120, 120, 0.1) : ba.db2linear;
};

process = gainStage;
