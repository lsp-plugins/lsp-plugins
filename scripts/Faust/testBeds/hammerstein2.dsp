fi = library("filters.lib");
ba = library("basics.lib");
si = library("signals.lib");

br1(brG, stG) = ^(_, 1) : fi.tf22t(1.000000000000000000, -2.000000000000000000, 1.000000000000000000, -1.815341082704567954, 0.831005589346757501) :  *(_, 0.911586668012831502) : *(_, brG) : *(_, stG);

branch1 = hgroup("[0]Branch1", br1(branchGain, branchState))
with{
    branchGain= hslider("[0]Gain Adjust Branch 1[unit:dB][style:knob]",0, -120, 120, 0.1) : ba.db2linear;
    branchState = checkbox("[1]Active") : si.smoo;
};

br2(brG, stG) = ^(_, 2) : fi.tf22t(1.000000000000000000, -2.000000000000000000, 1.000000000000000000, -1.815341082704567954, 0.831005589346757501) :  *(_, 0.911586668012831502) : *(_, brG) : *(_, stG);

branch2 = hgroup("[1]Branch2", br2(branchGain, branchState))
with{
    branchGain= hslider("[0]Gain Adjust Branch 2[unit:dB][style:knob]",0, -120, 120, 0.1) : ba.db2linear;
    branchState = checkbox("[1]Active") : si.smoo;
};

br3(brG, stG) = ^(_, 3) : fi.tf22t(1.000000000000000000, -2.000000000000000000, 1.000000000000000000, -1.815341082704567954, 0.831005589346757501) :  *(_, 0.911586668012831502) : *(_, brG) : *(_, stG);

branch3 = hgroup("[2]Branch3", br3(branchGain, branchState))
with{
    branchGain= hslider("[0]Gain Adjust Branch 3[unit:dB][style:knob]",0, -120, 120, 0.1) : ba.db2linear;
    branchState = checkbox("[1]Active") : si.smoo;
};

br4(brG, stG) = ^(_, 4) : fi.tf22t(1.000000000000000000, -2.000000000000000000, 1.000000000000000000, -1.815341082704567954, 0.831005589346757501) :  *(_, 0.911586668012831502) : *(_, brG) : *(_, stG);

branch4 = hgroup("[3]Branch4", br4(branchGain, branchState))
with{
    branchGain= hslider("[0]Gain Adjust Branch 4[unit:dB][style:knob]",0, -120, 120, 0.1) : ba.db2linear;
    branchState = checkbox("[1]Active") : si.smoo;
};

br5(brG, stG) = ^(_, 5) : fi.tf22t(1.000000000000000000, -2.000000000000000000, 1.000000000000000000, -1.815341082704567954, 0.831005589346757501) :  *(_, 0.911586668012831502) : *(_, brG) : *(_, stG);

branch5 = hgroup("[4]Branch5", br5(branchGain, branchState))
with{
    branchGain= hslider("[0]Gain Adjust Branch 5[unit:dB][style:knob]",0, -120, 120, 0.1) : ba.db2linear;
    branchState = checkbox("[1]Active") : si.smoo;
};

br6(brG, stG) = ^(_, 6) : fi.tf22t(1.000000000000000000, -2.000000000000000000, 1.000000000000000000, -1.815341082704567954, 0.831005589346757501) :  *(_, 0.911586668012831502) : *(_, brG) : *(_, stG);

branch6 = hgroup("[5]Branch6", br6(branchGain, branchState))
with{
    branchGain= hslider("[0]Gain Adjust Branch 6[unit:dB][style:knob]",0, -120, 120, 0.1) : ba.db2linear;
    branchState = checkbox("[1]Active") : si.smoo;
};

process = _ <: branch1, branch2, branch3, branch4, branch5, branch6 :> _;
