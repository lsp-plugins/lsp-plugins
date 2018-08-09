clc;
clear;
close all;

pkg load signal;
pkg load control;

antialFIRTaps   = 64;
finalFrequency  = csvread('/tmp/fFinalFrequency.csv');
sampleRate      = csvread('/tmp/fSampleRate.csv');
G               = csvread('/tmp/mKernelsRe.csv') + 1i * csvread('/tmp/mKernelsIm.csv');
g               = csvread('/tmp/mKernelsTaps.csv');
alpha           = csvread('/tmp/fAlpha.csv');

nyquistSample   = floor(size(G, 2) / 2.0) + 1;
frequency       = (0:(nyquistSample - 1)) * sampleRate  / (2.0 * nyquistSample);
frequencyPI     = (0:(nyquistSample - 1)) * pi  / nyquistSample;
frequencyStep   = sampleRate / (2.0 * nyquistSample);

% scaleFactor     = max(abs(G(1, :)));
% G               = G / scaleFactor;
% L               = 20.0 * log10(abs(G));

% Have a try with overall gain identification only.
% What about always identifying with alpha = 1, but putting a model trim control
% which assigns to each branch a gain as if alpha was different?

gCenter         = floor(size(g, 2) / 2) + 1;
gShift          = circshift(g, -gCenter, 2);
GShift          = fft(gShift, [], 2);
LShift          = 20.0 * log10(abs(GShift));

GFix            = conj(GShift);
LFix            = 20.0 * log10(abs(GFix));

gFix            = circshift(real(ifft(GFix, [], 2)), gCenter, 2);

figure;
plot(transpose(gFix))

figure;
subplot(2, 1, 1);
plot(frequency, transpose(LFix(:, 1:nyquistSample)));
subplot(2, 1, 2);
plot(frequency, (angle(GFix(:, 1:nyquistSample))));

gDelay          = -diff(unwrap(angle(GFix(:, 1:nyquistSample))), 1, 2) ./ ...
                    (2 * pi * frequencyStep);
                    
figure;
plot(frequency(1:(end - 1)), transpose(gDelay))

% This works with GFix!

target          = GFix(9, 1:nyquistSample);
orderN          = 12;
orderD          = 15;

[B, A]          = invfreqz(target, frequencyPI, orderN, orderD);
[H, W]          = freqz(B, A, nyquistSample);

figure;
subplot(2, 1, 1);
plot(frequencyPI, 20.0 * log10(abs(target)));
hold on;
plot(W, 20.0 * log10(abs(H)));
subplot(2, 1, 2);
plot(frequencyPI, (angle(target)));
hold on;
plot(W, (angle(H)));

% Let's roll.

maxBiquads      = 32;
maxOrder        = 2 * maxBiquads;
Jvalues         = zeros(maxOrder, 1);

gains           = max(abs(GFix(:, 1:nyquistSample)), [], 2);

for testOrder = 1:maxOrder
    
    for n = 1:size(GFix, 1)
        
        [B, A] = invfreqz(                                      ...
                          GFix(n, 1:nyquistSample) / gains(n),  ...
                          frequencyPI,                          ...
                          testOrder,                            ...
                          testOrder                             ...
                          );
                          
        H = freqz(B, A, nyquistSample);
        
        Gn = sqrt(conj(GFix(n, 1:nyquistSample)) ...
            * transpose(GFix(n, 1:nyquistSample)));
        Hn = sqrt(ctranspose(H) * H);
   
        Jvalues(testOrder) = Jvalues(testOrder) + ...
            abs(sqrt((conj(GFix(n, 1:nyquistSample)) / Gn) * (H / Hn)));
        
    endfor
    
    Jvalues(testOrder) = Jvalues(testOrder) / size(GFix, 1);
    
endfor

[~, bestOrder] = max(Jvalues);

Bbests = zeros(size(GFix, 1), bestOrder);
Abests = zeros(size(GFix, 1), bestOrder);
Hbests = zeros(size(GFix, 1), nyquistSample);

for n = 1:size(GFix, 1)
    
    [Bbest(n, :), Abests(n, :)] = invfreqz(                  ...
                      GFix(n, 1:nyquistSample) / gains(n),  ...
                      frequencyPI,                          ...
                      bestOrder,                            ...
                      bestOrder                             ...
                      );
                      
    Hbests(n, :) = gains(n) * freqz(Bbest(n, :), Abest(n, :), nyquistSample);
    
endfor

dBbest      = 20.0 * log10(abs(Hbests));
anglebest   = angle(Hbests);
gDelaybest  = -diff(unwrap(anglebest, [], 2), 1, 2) / (2 * pi * frequencyStep);

figure;
subplot(3, 1, 1);
plot(frequency, dBbest);
subplot(3, 1, 2);
plot(frequency, anglebest);
subplot(3, 1, 3);
plot(frequency(1:(end - 1)), gDelaybest);