target          = G(10, 1:nyquistSample);
orderN          = 32;
orderD          = 32;

[~, l]          = min(abs(frequency - 20));
[~, u]          = min(abs(frequency - 20000));

We              = zeros(size(frequencyPI));
We(l:u)         = ones(length(l:u), 1);

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

[SOST, kT]      = tf2sos(B, A);
figure;
sosN = 10;
Hbqd = freqz(SOST(sosN, 1:3), SOST(sosN, 4:6), nyquistSample);
HbqdGdelay = -diff(unwrap(angle(Hbqd))) / (2 * pi * frequencyStep);
semilogx(frequency(2:(end - 1)), HbqdGdelay(2:end));