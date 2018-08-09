using WAV
using Plots

pyplot()

irFile               = "/tmp/positiveTimeResponse.wav"
decayFile            = "/tmp/decay.csv"
decayLineDataFile    = "/tmp/decayLineCoefficients.csv"
decayLimit           = 11.0
decayThreshold       = -60.0
rtRegLimUp           = -5.0
rtRegLimDo           = -35.0

ir, fs               = wavread(irFile)

t                    = ((1:length(ir)) - 1) / fs

irNrg                = sum(ir.^2) / fs
irNrgAvg             = irNrg / maximum(t)
irNormCoeff          = sqrt(1 / irNrg)
irNorm               = irNormCoeff * ir
irdB                 = 20.0 * log10.(abs.(irNorm))

limIdx               = Int(min(length(ir), decayLimit * fs))
erg_decay_avg        = flipdim(cumsum(flipdim(ir[1:limIdx].^2, 1)), 1)
erg_decay_avg_norm   = erg_decay_avg / maximum(abs, erg_decay_avg)
erg_decay_dB         = 10.0 * log10.(erg_decay_avg_norm[find(erg_decay_avg_norm)])
upIdx                = indmin(abs.(erg_decay_dB - rtRegLimUp))
doIdx                = indmin(abs.(erg_decay_dB - rtRegLimDo))
regData              = erg_decay_dB[upIdx:doIdx]
a, b                 = linreg(upIdx:doIdx, regData)
NRT                  = (decayThreshold - a) / b
FRT                  = NRT / fs
decayLine            = b * (1:length(ir)) + a

pluginDecay          = readcsv(decayFile)
decaySlope           = readcsv(decayLineDataFile)[1]
decayIntercept       = readcsv(decayLineDataFile)[2]
pluginNRT            = readcsv(decayLineDataFile)[3]
pluginFRT            = readcsv(decayLineDataFile)[4]
pluginDecayLine      = decaySlope * (1:length(ir)) + decayIntercept
  
tplug                = ((1:length(pluginDecay)) - 1) / fs
terg                 = ((1:length(erg_decay_dB)) - 1) / fs

irPlot               = plot(t, irdB,          label = "Impulse Response [dB]")
plot!(irPlot, terg, erg_decay_dB,             label = "Energy Decay Curve [dB] (Julia)", linecolor = :red, linestyle = :solid, linewidth = 1)
plot!(irPlot, tplug, pluginDecay,             label = "Energy Decay Curve [dB] (plugin)", linecolor = :red, linestyle = :dash, linewidth = 2)
plot!(irPlot, t, decayLine,                   label = "Energy Decay Line [dB] (Julia)", linecolor = :green, linestyle = :solid, linewidth = 1)
plot!(irPlot, t, pluginDecayLine,             label = "Energy Decay Line [dB] (plugin)", linecolor = :green, linestyle = :dash, linewidth = 2)
vline!(irPlot, [min(maximum(t), FRT)],        label = "Decay Time (Julia): $FRT s", linecolor = :orange, linestyle = :solid, linewidth = 1)
vline!(irPlot, [min(maximum(t), pluginFRT)],  label = "Decay Time (plugin): $pluginFRT s", linecolor = :orange, linestyle = :dash, linewidth = 2)
hline!(irPlot, [decayThreshold],              label = "Decay Threshold: $decayThreshold dB", linecolor = :black, linestyle = :solid, linewidth = 2)
hline!(irPlot, [rtRegLimUp],                  label = "Regression Up Limit: $rtRegLimUp dB", linecolor = :yellow, linestyle = :solid, linewidth = 2)
hline!(irPlot, [rtRegLimDo],                  label = "Regression Down Limit: $rtRegLimDo dB", linecolor = :cyan, linestyle = :solid, linewidth = 2)
ylims!(irPlot, -150, 50)
xlabel!(irPlot, "Time [s]")
ylabel!(irPlot, "Energy Level [dB]")
