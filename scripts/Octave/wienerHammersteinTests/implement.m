clc;
clear;
close all;

pkg load signal;
pkg load control;

faustDSPtarget  = fullfile('..', '..', 'Faust', 'modelFilters', 'model_IIR.dsp');
permission      = 'w';
precision       = '%.18f';
antialFIRTaps   = 16;
windowFcn       = @chebwin;
iirNumOrder     = 4;
iirDenOrder     = 4;

finalFrequency  = csvread('/tmp/fFinalFrequency.csv');
sampleRate      = csvread('/tmp/fSampleRate.csv');
alpha           = csvread('/tmp/fAlpha.csv');
g               = csvread('/tmp/mKernelsTaps.csv');

% Calcualte the best possible frequency response targets for implementation.
gCenter         = floor(size(g, 2) / 2) + 1;
%g               = circshift(g, gCenter, 2);
G               = fft(g, [], 2);
shift           = -gCenter; % - (antialFIRTaps - 1) / 2;
wSize           = size(G, 2);
nyq             = floor(wSize / 2) + 1;
phFactor        = ones(1, size(G, 2));
phFactor(1:nyq) = exp(-1i * 2 * pi * ((1:nyq) - 1) * shift / wSize);
phFactor(mod(wSize + 1 - (1:nyq), wSize) + 1) = conj(phFactor(1:nyq));
G               = bsxfun(@times, G, phFactor);

% Useful stuff:
nyquistSample   = floor(size(G, 2) / 2.0) + 1;
frequency       = (0:(nyquistSample - 1)) * sampleRate  / (2.0 * nyquistSample);
frequencyPI     = (0:(nyquistSample - 1)) * pi  / nyquistSample;
frequencyStep   = sampleRate / (2.0 * nyquistSample);

% Normalize gains with respect the linear order.
% G               = G / max(abs(G(1, 1:nyquistSample)));

% Get all normalized gains:
gains           = max(abs(G(:, 1:nyquistSample)), [], 2);

% This is the minimum adjustment gain applicable to the model:
minGain         = exp(log(max(gains(2:end))) / size(G, 1));

% Calculate IIR coefficiens:
B               = cell(size(G, 1), 1);
A               = cell(size(G, 1), 1);
sosData         = cell(size(G, 1), 1);
sosGains        = zeros(size(G, 1), 1); % For Plot
antialFIRs      = cell(size(G, 1), 1);

% Actual IIR responses:
H_Kernels       = zeros(size(G, 1), nyquistSample);
H_antial        = zeros(size(G, 1), nyquistSample);

for n = 1:size(G, 1)

    [B{n, 1}, A{n, 1}] = invfreqz(                          ...
                      G(n, 1:nyquistSample), .../ gains(n),     ...
                      frequencyPI,                          ...
                      iirNumOrder,                          ...
                      iirDenOrder                           ...
                      );

    sosItem = struct();

    [sosItem.SOS, sosItem.G] = tf2sos(B{n, 1}, A{n, 1});

    sosGains(n) = sosItem.G;

    sosData{n, 1} = sosItem;

    antialFIRs{n, 1} = fir1(                                              ...
                       antialFIRTaps - 1,                           ...
                       (2.0 * finalFrequency) / (n * sampleRate),   ...
                       'lowpass',                                   ...
                       windowFcn(antialFIRTaps)                     ...
                       );

    % sosItem.G       = sosItem.G * gains(n);
    H_Kernels(n, :) = freqz(B{n, 1}, A{n, 1}, nyquistSample); %gains(n) * freqz(B{n, 1}, A{n, 1}, nyquistSample);
    H_antial(n, :)  = freqz(antialFIRs{n, 1}, 1, nyquistSample);

endfor

groupDelays = -diff(unwrap(angle(H_Kernels), [], 2), 1, 2) / (2 * pi * frequencyStep);

% Plot the results.
maxPlotLevel = 10.0 * ceil(20.0 * log10(max(abs(H_Kernels(:)))) / 10);
minPlotLevel = -120;

figure;
subplot(2, 1, 1);
semilogx(frequency(2:end), transpose(20.0 * log10(abs(G(:, 2:nyquistSample)))));
xlim([frequency(2) frequency(end)]);
ylim([minPlotLevel maxPlotLevel]);
grid on;
xlabel('Frequency [Hz]');
ylabel('Measured Kernels [dB]');
subplot(2, 1, 2);
semilogx(frequency(2:end), transpose(angle(G(:, 2:nyquistSample))));
xlim([frequency(2) frequency(end)]);
grid on;
xlabel('Frequency [Hz]');
ylabel('Measured Kernels [rad]');

figure;
subplot(2, 1, 1);
semilogx(frequency(2:end), transpose(20.0 * log10(abs(H_Kernels(:, 2:end)))));
xlim([frequency(2) frequency(end)]);
ylim([minPlotLevel maxPlotLevel]);
grid on;
xlabel('Frequency [Hz]');
ylabel('Kernels IIR [dB]');
subplot(2, 1, 2);
semilogx(frequency(2:end), transpose(angle(H_Kernels(:, 2:end))));
xlim([frequency(2) frequency(end)]);
grid on;
xlabel('Frequency [Hz]');
ylabel('Kernels IIR [rad]');

figure;
semilogx(frequency(2:(end - 1)), transpose(groupDelays(:, 2:end)));
xlim([frequency(2) frequency(end)]);
grid on;
xlabel('Frequency [Hz]');
ylabel('Kernels IIR Group Delay [s]');

figure;
subplot(2, 1, 1);
semilogx(frequency(2:end), transpose(20.0 * log10(abs(H_antial(:, 2:end)))));
xlim([frequency(2) frequency(end)]);
ylim([minPlotLevel maxPlotLevel]);
grid on;
xlabel('Frequency [Hz]');
ylabel('Antialising FIR [dB]');
subplot(2, 1, 2);
semilogx(frequency(2:end), transpose(unwrap(angle(H_antial(:, 2:end)), [], 2)));
xlim([frequency(2) frequency(end)]);
grid on;
xlabel('Frequency [Hz]');
ylabel('Antialising FIR [rad]');

figure;
stem(sosGains);
grid on;
xlabel('Order');
ylabel('SOS Gain');
