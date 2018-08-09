using WAV
using Plots

pyplot()

irFile               = "/tmp/allTimeResponse.wav"
chirpSamples         = 1034779
windowSize           = 8192
decayThreshold       = -60.0
rtRegLimUp           = -5
rtRegLimDo           = -25.0

ir, fs        = wavread(irFile)

fs            = Float64(fs)

timeOrigin    = Int(length(ir) / 2)

peakIdx       = indmax(abs.(ir))

noiseIdx1     = max(1, timeOrigin - chirpSamples)
noiseIdx2     = timeOrigin - 1

noise         = ir[noiseIdx1:noiseIdx2]

window        = zeros(windowSize)

peakLog       = zeros(ir)

maxNoisePeak  = 0.0
meanNoisePeak = 0.0
writePosition = 1
peakPosition  = 1
upCounter     = 0
noiseCounter  = 1
firstLow      = 1
lastUp        = 1
noiseThs      = 0
noiseThs2     = 0
noiseLevel2   = 0
upUntilFirstLow = 0
maxGap         = 0

sumAcc         = 0
gapAcc         = 0

gGap           = 0
gGapSum        = 0
centerMass     = 0

noiseLevel     = ceil(20.0 * log10(maximum(abs, ir[noiseIdx1:noiseIdx2])))
noiseThs       = exp10(noiseLevel / 20.0)

for n = noiseIdx1:length(ir)
  
  writePosition += 1  
  writePosition  = (writePosition > windowSize) ? 1 : writePosition

  window[writePosition] = abs(ir[n])

  if isequal(writePosition, peakPosition)
    peakPosition = indmax(window)
  elseif window[writePosition] > window[peakPosition]
    peakPosition = writePosition
  end
  
  peak           = window[peakPosition]
  peakLog[n]     = peak
  
  if (firstLow != 1)
    gap          = 20.0 * log10(peak / noiseThs)
    maxGap       = (gap > maxGap) ? gap : maxGap
  end
  
  if n < timeOrigin
#    maxNoisePeak   = (maxNoisePeak > peak) ? maxNoisePeak : peak
#    meanNoisePeak += ((peak - meanNoisePeak) / noiseCounter)
#    noiseLevel     = 20.0 * log10(meanNoisePeak)
#    noiseThs       = (noiseLevel > 0) ? exp10(floor(noiseLevel) / 20.0) : exp10(ceil(noiseLevel) / 20.0)
#    noiseLevel2    = ceil(20.0 * log10(meanNoisePeak))
#    noiseThs2      = exp10(noiseLevel2 / 20.0)
#    noiseCounter  += 1
  elseif peak > noiseThs
    upCounter   += 1
    lastUp       = n - timeOrigin
    gap          = 20 * log10(peak / noiseThs)
    sumAcc      += gap * lastUp
    gapAcc      += gap
    
    gGap         = 20 * log10(peak / noiseThs)
    gGapSum     += gGap
    centerMass  += (n - timeOrigin + 1) * gGap
  elseif (peak <= noiseThs) && (n > peakIdx) && (firstLow == 1)
    firstLow     = n - timeOrigin
    upUntilFirstLow = upCounter
  end
  
end

centerMass       /= gGapSum

ir_plot                       = ir
peakLog_plot                  = peakLog
ir_plot[ir .== 0.0]             = exp10(-150 / 20)
peakLog_plot[peakLog .== 0.0]   = exp10(-150 / 20)

fromFirstLowToLastUp = upCounter - upUntilFirstLow

origGap          = 20.0 * log10(maximum(abs, ir) / noiseThs)

integrationLimit = floor(Int, (firstLow * origGap + lastUp * maxGap) / (origGap + maxGap))#floor(Int, (upUntilFirstLow * firstLow + fromFirstLowToLastUp * lastUp) / upCounter)
FLM              = integrationLimit / fs

irPlus               = ir[timeOrigin:(timeOrigin + integrationLimit - 1)]

irNrg                = sum(irPlus.^2) / fs
irNormCoeff          = sqrt(1 / irNrg)
irPlusNorm           = irNormCoeff * irPlus
irPlusdB             = 20.0 * log10.(abs.(irPlusNorm))
noiseThsNorm         = noiseThs * irNormCoeff
noiseThsdB           = 20.0 * log10(noiseThsNorm)

erg_decay_avg        = flipdim(cumsum(flipdim(irPlus.^2, 1)), 1)
erg_decay_avg_norm   = erg_decay_avg / maximum(abs, erg_decay_avg)
erg_decay_dB         = 10.0 * log10.(erg_decay_avg_norm)
upIdx                = indmin(abs.(erg_decay_dB - rtRegLimUp))
doIdx                = indmin(abs.(erg_decay_dB - rtRegLimDo))
regData              = erg_decay_dB[upIdx:doIdx]
a, b                 = linreg(upIdx:doIdx, regData)
NRT                  = (decayThreshold - a) / b
FRT                  = NRT / fs
decayLine            = b * (1:length(irPlus)) + a

time                 = ((1:length(irPlus)) - 1) / fs

irPlusPlot           = irPlusNorm
decayPlot            = erg_decay_avg_norm

irPlusPlot[irPlusPlot .== 0.0]   = exp10(-150 / 20)
decayPlot[decayPlot .== 0.0]     = exp10(-150 / 20)

irPlot = plot(time, 20.0 * log10.(abs.(irPlusPlot)), label = "Impulse Response")
plot!(irPlot, time, 10.0 * log10.(decayPlot), label = "Energy Decay")
plot!(irPlot, time, decayLine, label = "Regression Line", linecolor = :orange)
hline!(irPlot, [noiseThsdB], label = "Noise Floor", linecolor = :black, linewidth = 2)
hline!(irPlot, [decayThreshold], label = "$decayThreshold dB", linecolor = :red)
vline!(irPlot, [min(FRT, maximum(time))], label = "Reverberation time", linecolor = :red)
title!(irPlot, "RT = $(@sprintf("%.3f", FRT)) s, IL = $(@sprintf("%.3f", FLM)) s")
xlabel!(irPlot, "Time [s]")
ylabel!(irPlot, "[dB]")


abovedB             = 3.0

irPos               = ir[timeOrigin:length(ir)]

startIdx            = indmax(abs.(irPos))
  
doSearch            = 20.0 * log10(abs(irPos[startIdx])) > (abovedB + 20.0 * log10(noiseThs))
  
rtItLim             = length(irPos)

info("here")

while (doSearch)
  
  window              = zeros(windowSize)
  writePosition       = 1
  peakPosition        = 1
  
  for n = startIdx:length(irPos)
    
      writePosition += 1  
	  writePosition  = (writePosition > windowSize) ? 1 : writePosition
	
	  window[writePosition] = abs(irPos[n])
	
	  if isequal(writePosition, peakPosition)
	    peakPosition = indmax(window)
	  elseif window[writePosition] > window[peakPosition]
	    peakPosition = writePosition
	  end
	  
	  peak           = window[peakPosition]
	  
	  if (peak <= noiseThs)
	    
	    info("here2")
	    println(n)
	    
	    rtItLim      = n
	    startIdx     = indmax(abs.(irPos[n:length(irPos)])) + n - 1
	    println(startIdx)
	    doSearch     = 20.0 * log10(abs(irPos[startIdx])) > (abovedB + 20.0 * log10(noiseThs))
	      
	    println(20.0 * log10(abs(irPos[startIdx])))
	    break;
	    
	  end
    
  end
  
end
