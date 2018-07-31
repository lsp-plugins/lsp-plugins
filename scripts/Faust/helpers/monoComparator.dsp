ma = library("maths.lib");
si = library("signals.lib");

comparator = *(_, gain1), *(_, gain2) :> _
with {
        gain1 = checkbox("in_0 (in_1)") : si.smoo;
        gain2 = ma.fabs(1 - gain1);
};

process = comparator;
