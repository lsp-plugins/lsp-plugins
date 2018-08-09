#!/usr/bin/env julia

# Calculate and plot the frequency response from profiler_mono results files.
# (for linear system measurements).

# Loading the required packages
using WAV
using Plots

# Telling plots to use the PyPlot backend
pyplot()

# Window Size for both positive times and all times responses
windowSize      = 524288
# For all time responses, this variable sets where the beginning of the window 
# is. Units: samples before the origin of time.
allTimeOffset   = Int(windowSize / 2) - 1
# Reverberation time threshold
decayThreshold  = -60.0
# Reverberation time limit
decayLimit      = Inf
#RT regression line limits
rtRegLimUp      = -5.0
rtRegLimDo      = -25.0

# Loading the data
allTimeResponse, fs         = wavread("/tmp/allTimeResponse.wav")
positiveTimeResponse, fs    = wavread("/tmp/positiveTimeResponse.wav")

# Making sure that the all times window does not go way too far in the negative
# times. Max lag: the one that allows the window to be centred on the origin of
# time.
allTimeOff = min(allTimeOffset, fld(windowSize, 2))

# Centre, or origin of time
c     = (length(allTimeResponse) >> 1)

# All times
h_a   = allTimeResponse[(c - allTimeOffset):(c - allTimeOffset + windowSize - 1)]
H_a   = fft(h_a)

# Positive times
h_p   = positiveTimeResponse[1:windowSize]
H_p   = fft(h_p)

# Positive frequency only:
n     = fld(windowSize, 2) - 1
G_a   = H_a[1:n]
G_p   = H_p[1:n]

# dB Values
G_a_dB = 20 * log10.(abs.(G_a))
G_p_dB = 20 * log10.(abs.(G_p))

# Phase
G_a_rad = angle.(G_a)
G_p_rad = angle.(G_p)

# Frequency axis
f     = collect(((1:n) - 1) * (fs / (2 * n)))

# Limit frequency axis to the bandwidth of the test signal
pltIdx = pltIdx = (f .> 1) .& (f .< (fs / 2))

# Calculate reverberation time
limIdx              = Int(min(length(positiveTimeResponse), decayLimit * fs))
erg_decay_avg       = flipdim(cumsum(flipdim(positiveTimeResponse[1:limIdx].^2, 1)), 1)
erg_decay_avg_norm  = erg_decay_avg / maximum(abs, erg_decay_avg)
erg_decay_dB        = 10 * log10.(erg_decay_avg_norm[find(erg_decay_avg_norm)])
nRT                 = indmin(abs.(erg_decay_dB - decayThreshold))
fRT                 = nRT / fs

# Regression line of values between dB limits
upIdx               = indmin(abs.(erg_decay_dB - rtRegLimUp))
doIdx               = indmin(abs.(erg_decay_dB - rtRegLimDo))
regData             = erg_decay_dB[upIdx:doIdx]
a, b                = linreg(upIdx:doIdx, regData)
nRTreg              = (decayThreshold - a) / b
fRTreg              = nRTreg / fs

# Test another code for energy decay curve calculation
erg_decay_avg_test     = zeros(positiveTimeResponse[1:limIdx])
erg_decay_dB_test      = zeros(positiveTimeResponse[1:limIdx])
lookAheadSum           = sum(positiveTimeResponse[1:limIdx].^2)
erg_decay_norm         = lookAheadSum
erg_decay_avg_test[1]  = 1.0
erg_decay_dB_test[1]   = 0.0
nRT_test               = 0
fRT_test               = 0

for n = 2:limIdx
  
  value                 = positiveTimeResponse[n - 1] * positiveTimeResponse[n - 1]
  lookAheadSum         -= value
    
  erg_decay_avg_test[n] = lookAheadSum / erg_decay_norm
  erg_decay_dB_test[n]  = 10.0 * log10(erg_decay_avg_test[n])
  
  if (erg_decay_dB_test[n] < decayThreshold)
    nRT_test            = n
    fRT_test            = nRT_test / fs
    break
  end
  
end

# Do the plot
lay = grid(2, 1)

plot(
    plot(f[pltIdx], [G_a_dB[pltIdx] G_p_dB[pltIdx]], xscale = :log10, label = ["All Time" "Positive Time"], xlabel = "Frequency [Hz]", ylabel = "Magnitude [dB]"),
    plot(f[pltIdx], [G_a_rad[pltIdx] G_p_rad[pltIdx]], xscale = :log10, leg = false, xlabel = "Frequency [Hz]", ylabel = "Phase [rad]"),
    layout = lay
)