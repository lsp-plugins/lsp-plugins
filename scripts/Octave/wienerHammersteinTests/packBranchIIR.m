function[branchStruct] = packBranchIIR(z, p, k, branchGain)

[b, a]          = zp2tf(z, p, k * branchGain);
[S, G]          = zp2sos(z, p, k * branchGain);

branchStruct    = struct();
branchStruct.z  = z;
branchStruct.p  = p;
branchStruct.k  = k * branchGain;
branchStruct.a  = a;
branchStruct.b  = b;
branchStruct.S  = S;
branchStruct.G  = G;


endfunction