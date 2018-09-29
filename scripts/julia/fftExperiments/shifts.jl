using Base.Test
using Primes
using DSP
using Plots

pyplot()

N               = 1024
delay           = rand([-1.0 1.0]) * rand() * (div(N, 2) + 1)

band            = sort(rand(2))
responsetype    = Bandpass(band[1], band[2])
designmethod    = Butterworth(4)
filter          = digitalfilter(responsetype, designmethod)
h               = impz(filter, N)

# Pretending that the time series h is just some pulse

H               = fft(h)

q               = div(N, 2) + 1

phase           = zeros(H)
F               = zeros(H)

t               = zeros(H)

for k = 1:q

    p           = mod(N + 1 - k, N) + 1

    phase[k]    = -im * 2π * (k - 1) * delay / N
    phase[p]    = conj(phase[k])

    F[k]        = exp(phase[k])
    F[p]        = conj(F[k]) #exp(phase[p])
    t[k]        = H[k] - conj(H[p])

end

G               = H .* F
g               = ifft(G)

plot([h real(g) imag(g)])
