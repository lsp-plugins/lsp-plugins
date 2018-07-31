clc; clear; close all;

dataTarget = fullfile(                             ...
                            '..',                       ...
                            '..',                       ...
                            'Faust',                    ...
                            'testBeds',                 ...
                            'wienerHammerstein2.mat'     ...
                            );

% Select inner and outer windows and their parameters
outerWindowFcn  = @hanning; % @(x) ones(x, 1);
innerWindowFcn  = @novakWindow; % @(x, y, z) ones(x, 1);
innerFadeIn     = Inf;
innerFadeOut    = Inf;

% Decide if scaling the coeff matrix columns (preScaling = true) or the end
% result (the kernels).
preScaling      = false;

% Use  this threshold to check if higher order responses have high enough
% Magnitude [dB]
checkThreshold  = 100.0;

hcpp    = csvread('/tmp/cResult.csv');
Hcpp    = csvread('/tmp/mHigherRe.csv') + 1i * csvread('/tmp/mHigherIm.csv');
Gcpp    = csvread('/tmp/mKernelsRe.csv') + 1i * csvread('/tmp/mKernelsIm.csv');
fs      = csvread('/tmp/fSampleRate.csv');
gamma   = csvread('/tmp/gamma.csv');
order   = csvread('/tmp/fOrder.csv');
wSize   = 2^csvread('/tmp/fWindowOrder.csv');
offset  = csvread('/tmp/fOffset.csv');
alpha   = csvread('/tmp/fAlpha.csv');
fstart  = csvread('/tmp/fInitialFrequency.csv');
fend    = csvread('/tmp/fFinalFrequency.csv');

hStackCpp = real(ifft(Hcpp, [], 2));

center  = floor(length(hcpp) / 2);

hStack  = zeros(order, wSize);
HStack  = zeros(order, wSize);

nyq     = floor(wSize / 2) + 1;

f       = (0:(nyq - 1)) * fs / (2 * nyq);
fpi     = (0:(nyq - 1)) * pi / nyq;

data    = load(dataTarget);

for o = 1:order
    
    w               = zeros(wSize, 1);
    
    fCenter         = center - (fs * gamma * log(o));
    
    fGap2Next       = fs * gamma * log(1 + 1 / o);
    fGap2Previous   = fs * gamma * log(o / (o - 1));
    
    % Optimal Gap
    
    fOptHead        = fCenter - min(fGap2Next / 2, wSize / 2);
    fOptTail        = fCenter + min(fGap2Previous / 2, wSize / 2);
    fOptLength      = fOptTail - fOptHead;
    nMaxData        = floor(fOptLength);
    
    nOptHead        = floor(fOptHead);
    nOptTail        = floor(fOptTail);
    fOptCount       = floor(fOptLength);
    
    fHead           = fOptHead;
    nHead           = nOptHead;
    
    fWhead          = 0.5 * (wSize - min(fGap2Next, wSize)) + 1;
    nWhead          = floor(fWhead);
    
    innerFadeIn     = min(innerFadeIn, floor(min(fGap2Next, wSize) / 2));
    innerFadeOut    = min(innerFadeOut, floor(min(fGap2Previous, wSize) / 2));
    
%    % Simmetric up to +/- Gap2Next / 2 with respect impulse response center.
%    
%    fMaxWindowable  = fGap2Next;
%    
%    fMaxData        = min(fMaxWindowable, wSize);
%    nMaxData        = round(fMaxData);
%    
%    fHead           = fCenter - (fMaxData / 2);
%    nHead           = floor(fHead);
%    
%    fWhead          = ((wSize - fMaxData) / 2) + 1;
%    nWhead          = floor(fWhead);

%    innerFadeIn     = min(innerFadeIn, floor(fMaxData / 2));
%    innerFadeOut    = min(innerFadeOut, floor(fMaxData / 2));
    
    cShift          = nHead - fHead;
    wShift          = fWhead - nWhead;
    
    % This is the overall shift to align the responses to the middle of the
    % window.
    alignShift      = cShift + wShift;
    totalShift      = alignShift;
    
%    % If there are antialising FIRs, this will remove their added delay
%    % (if the direct loop latency has been assessed independently before the
%    % measurement). This just to check that the phase response is correct (
%    % expect deviation at high frequency, as the resulting Hammerstein kernel
%    % is actually made equivalent to a Wiener Hammerstein branch).
%    if isfield(data, 'antialFIRTaps')
%        totalShift  = totalShift - ((data.antialFIRTaps - 1) / 2);
%    endif

    w(nWhead:(nWhead + nMaxData - 1)) = ...
        hcpp(nHead:(nHead + nMaxData - 1)) .* ...
        innerWindowFcn(length(nHead:(nHead + nMaxData - 1)), innerFadeIn, innerFadeOut);
    
    phFactor        = ones(size(w));
    phFactor(1:nyq) = exp(-1i * 2 * pi * ((1:nyq) - 1) * totalShift / wSize);
    phFactor(mod(wSize + 1 - (1:nyq), wSize) + 1) = conj(phFactor(1:nyq));
    
    HStack(o, :)    = fft(w .* outerWindowFcn(length(w))) .* phFactor;
    hStack(o, :)    = real(ifft(HStack(o, :)));
    
endfor

C = zeros(order);

if preScaling
    scaleAmplitude = alpha;
else
    scaleAmplitude = 1.0;
endif

for r = 1:order
    for c = r:order

        if isequal(mod(c + r, 2), 0)
            
            C(r, c) = ...
                    scaleAmplitude^(c - 1) * ...
                    2^(1 - c) * ...
                    nchoosek(c, (c - r) / 2) * ...
                    (-1)^(2 * c - ((r - 1) / 2));
                
        end

    end
end

% Let's check the levels:
Hlevels         = 20.0 * log10(abs(HStack));
HlevelsCheck    = max(Hlevels, [], 2) < checkThreshold;

% And Let's derive the Kernel Targets.
GStack = C \ HStack;

% Scaling the end results durectly if prescaling of coefficents was not made.
if ~preScaling
    GStack = bsxfun(@times, GStack, alpha.^(transpose(1 - (1:order)))); 
endif

% Forcing hermitian symmetry, as the system above is for the positive frequency.
GStack(:, mod(wSize + 1 - (1:nyq), wSize) + 1) = conj(GStack(:, 1:nyq));

% Let's make the kernels impulse responses before time shift, as they are easier
% to look at if centered.
gStack = real(ifft(GStack, [], 2));

% This is the shift to remove the latency that we would artificially add by
% having the higher order responses aligned in the middle of the window.
latencyShift = wSize / 2;
phFactor2 = ones(1, size(GStack, 2));
phFactor2(1:nyq) = exp(-1i * 2 * pi * ((1:nyq) - 1) * latencyShift / wSize);
phFactor2(mod(wSize + 1 - (1:nyq), wSize) + 1) = conj(phFactor2(1:nyq));
GStack = bsxfun(@times, GStack, phFactor2);

expected = transpose(freqz(data.b, data.a, nyq));

% If there are antialising FIRs, this will add their latency to the expected
% result, to ease comparison.
refShift = 0;

if isfield(data, 'antialFIRTaps')
    refShift  = (data.antialFIRTaps - 1) / 2;
endif

phFactor3   = exp(-1i * 2 * pi * ((1:nyq) - 1) * refShift / wSize);
expected    = expected .* phFactor3;

figure;
subplot(2, 1, 1);
semilogx(f(2:end), 20 * log10(abs(GStack(:, 2:nyq).')));
hold on;
semilogx(f(2:end), 20 * log10(abs(expected(2:end))), 'k--');
hold off;
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('Measured Kernels [dB]');
legend([cellfun(@num2str,num2cell(1:order),'un',0) 'Ref'], "Location", "northeastoutside");
subplot(2, 1, 2);
semilogx(f(2:end), angle(GStack(:, 2:nyq)).');
hold on;
semilogx(f(2:end), angle(expected(2:end)), 'k--');
hold off;
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('Measured Kernels [rad]');

figure;
subplot(2, 1, 1);
semilogx(f(2:end), 20 * log10(abs(HStack(:, 2:nyq).')));
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('Measured HORs [dB]');
legend(cellfun(@num2str,num2cell(1:order),'un',0), "Location", "northeastoutside");
subplot(2, 1, 2);
semilogx(f(2:end), unwrap(angle(bsxfun(@times, HStack(:, 2:nyq), phFactor2(2:nyq))), [], 2).');
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('Measured HORs [rad]');

figure;
subplot(2, 1, 1);
semilogx(f(2:end), 20 * log10(abs(Hcpp(:, 2:nyq).')));
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('Measured HORs C++ [dB]');
legend(cellfun(@num2str,num2cell(1:order),'un',0), "Location", "northeastoutside");
subplot(2, 1, 2);
semilogx(f(2:end), unwrap(angle(bsxfun(@times, Hcpp(:, 2:nyq), phFactor2(2:nyq))), [], 2).');
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('Measured HORs C++ [rad]');

figure;
subplot(2, 1, 1);
semilogx(f(2:end), 20 * log10(abs(HStack(:, 2:nyq).')), 'LineWidth', 2);
hold on;
semilogx(f(2:end), 20 * log10(abs(Hcpp(:, 2:nyq).')), '--', 'LineWidth', 2);
hold off;
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('HORs Test VS C++ [dB]');
subplot(2, 1, 2);
semilogx(f(2:end), (angle(bsxfun(@times, HStack(:, 2:nyq), phFactor2(2:nyq)))).', 'LineWidth', 2);
hold on;
semilogx(f(2:end), (angle(bsxfun(@times, Hcpp(:, 2:nyq), phFactor2(2:nyq)))).', '--', 'LineWidth', 2);
hold off;
grid on;
xlim([f(2) f(end)]);
xlabel('Frequency [Hz]');
ylabel('HORs Test VS C++ [rad]');

%% Test some biquad section identification
targetIdx   = 1;
bqdOrder    = 32;

targetIdx   = min(targetIdx, size(GStack, 1));

target      = GStack(targetIdx, 1:nyq);

W           = zeros(size(target));
W(f > 20.0 & f < 20000.0) = 1.0;

[b, a]      = invfreqz(target, fpi, bqdOrder, bqdOrder, W, 1e6, 1e-12, 'trace');
idH         = freqz(b, a, nyq);

[SOS, G]    = tf2sos (b, a);

figure;
subplot(2, 1, 1);
semilogx(f(2:end), 20 * log10(abs([idH(2:end) target(2:end).'])));
xlabel('Frequency [Hz]');
ylabel('[dB]');
legend('Identified', 'Target');
grid on;
xlim([f(2) f(end)]);
subplot(2, 1, 2);
semilogx(f(2:end), angle([idH(2:end) target(2:end).']))
xlabel('Frequency [Hz]');
ylabel('[rad]')
grid on;
xlim([f(2) f(end)]);