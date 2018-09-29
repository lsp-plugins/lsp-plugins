clc; clear; close all;

pkg load signal;
pkg load control;

% Destination Filename:
faustDSPtarget  = fullfile('..', '..', 'Faust', 'testBeds', 'linearIIR.dsp');

% Define a linear filter:
[z, p, k] = cheby1(3, 5, [0.2 0.6], 'stop');
%[z, p, k] = cheby1(3, 5, 0.2);

[SOS, G] = zp2sos(z, p, k);

sos2Faust(faustDSPtarget, 'w', SOS, G, '%.16f');

% Compare with measurement results.
windowSize  = 1024;
customShift = 20;

sampleRate  = csvread('/tmp/fSampleRate.csv');
h           = csvread('/tmp/cResult.csv');
origin      = floor(length(h) / 2);

g           = h( ...
                (origin - floor(windowSize / 2)):...
                (origin + floor(windowSize / 2) - 1) ...
                );
                
gShift      = circshift(g, floor(windowSize / 2));

gCustom     = h( ...
                (origin - customShift): ...
                (origin - customShift + windowSize - 1) ...
                );

G           = fft(gShift);
nyq         = floor(length(G) / 2) + 1;
f           = (0:(nyq - 1)) * sampleRate / (2 * nyq);

H_lSave     = fft(h((origin - 0):end));
nyq_lSave   = floor(length(H_lSave) / 2) + 1;
f_lSave     = (0:(nyq_lSave - 1)) * sampleRate / (2 * nyq_lSave);

[b, a]      = zp2tf(z, p, k);
lSys        = freqz(b, a, nyq);

figure;
subplot(2, 1, 1);
semilogx(f, 20.0 * log10(abs([lSys G(1:nyq)])));
hold on;
semilogx(f_lSave, 20.0 * log10(abs(H_lSave(1:nyq_lSave))), '--');
hold off;
xlim([min(f) max(f)]);
xlabel('Frequency [Hz]');
ylabel('Magnitude [dB]');
legend('Expected', 'Measured (Windowed)', 'Measured (from origin)', ...
    "location", 'southwest');
grid on;
subplot(2, 1, 2);
semilogx(f, angle([lSys G(1:nyq)]));
hold on;
semilogx(f_lSave, angle(H_lSave(1:nyq_lSave)), '--');
hold off;
xlim([min(f) max(f)]);
xlabel('Frequency [Hz]');
ylabel('Phase [rad]');
grid on;