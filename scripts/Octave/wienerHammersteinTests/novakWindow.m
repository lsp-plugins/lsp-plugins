function[smoothingWindow] = novakWindow(windowLength, fadeInLength, fadeOutLength)

smoothingWindow = ones(windowLength, 1);

nIn1    = floor(-fadeInLength / 2);
nIn2    = floor((fadeInLength - 2) / 2);
nIn     = (nIn1:nIn2) * pi / fadeInLength;
fadeIn  = 0.5 * (sin(nIn) + 1);

nOt1    = floor(-fadeOutLength / 2);
nOt2    = floor((fadeOutLength - 2) / 2);
nOt     = (nOt1:nOt2) * pi / fadeOutLength;
fadeOut = 0.5 * (sin(nOt) + 1);

smoothingWindow(1:length(fadeIn))                       = fadeIn;
smoothingWindow(end:-1:(end - length(fadeOut) + 1))     = fadeOut;

end