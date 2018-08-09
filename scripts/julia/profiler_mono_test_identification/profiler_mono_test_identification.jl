using WAV
using Plots
using DSP

pyplot()

shift       = 0

fs          = Int(readcsv("/tmp/fSampleRate.csv")[1, 1])
order       = Int(readcsv("/tmp/fOrder.csv")[1, 1])
windowSize  = (1 << Int(readcsv("/tmp/fWindowOrder.csv")[1, 1]))
offset      = Int(readcsv("/tmp/fOffset.csv")[1, 1])
fₛ          = readcsv("/tmp/fInitialFrequency.csv")[1, 1]
fₑ          = readcsv("/tmp/fFinalFrequency.csv")[1, 1]
α           = readcsv("/tmp/fAlpha.csv")[1, 1]
γ           = readcsv("/tmp/gamma.csv")[1, 1]
H           = zeros(Complex{Float64}, order, windowSize)
G           = zeros(H)
C           = zeros(Complex{Float64}, order, order)

smthWindow  = ones(windowSize)

for r in 1:order
    for c in r:order

        if isequal(mod(c + r, 2), 0)
            C[r, c] = α^(c - 1) * exp2(1.0 - c) * binomial(c, div(c - r, 2)) *
                (-1.0 + 0.0im)^(2.0c - (r - 1.0) / 2.0)
            # C[r, c] = exp2(1.0 - c) * binomial(c, div(c - r, 2)) *
            #     (-1.0 + 0.0im)^(2.0c - (r - 1.0) / 2.0)
        end

    end
end

h           = readcsv("/tmp/cResult.csv")

origin      = div(length(h), 2)
maxSample   = length(h)
nyq         = div(windowSize, 2) + 1

for w in 1:order

    window  = zeros(eltype(h), windowSize)

    fhead   = origin - (fs * γ * log(w)) - offset
    nhead   = floor(Int, fhead)
    δ       = nhead - fhead + shift #+ offset # Add phase to align peak close to beginning.

    select  = nhead:min(nhead + windowSize - 1, maxSample)
    window[1:length(select)] = h[select] .* smthWindow

    H[w, :] = fft(window)

    for k in 1:nyq

        p   = mod(windowSize + 1 - k, windowSize) + 1

        F   = exp(-im * 2π * (k - 1) * δ / windowSize)

        H[w, k] *= F
        H[w, p] *= conj(F)

    end

    maxSample = nhead

end

f   = (0:(nyq - 1)) * fs  / 2nyq
Δf  = fs / (2 * nyq)

# This relation actually only works for the positive frequency
G = C \ H

# Let's force Hermitian Symmetry of G rows.
for r in 1:order
    for k in 1:nyq

        p = mod(windowSize + 1 - k, windowSize) + 1
        G[r, p] = conj(G[r, k])

    end
end

# Impose DC Block:
G[:, 1] = 0.0 + 0.0im #1.0e-6 + 0.0im

L = 20.0 * log10.(abs.(G))
P = unwrap(angle.(G))

magPlot = plot(f[2:end], transpose(L[:, 2:nyq]), xscale = :log10,
    linecolor = [:red :blue :green :orange],
    label = ["G1" "G2" "G3" "G4"],
    xlabel = "Frequency [Hz]",
    ylabel = "[dB]"
    )

phsPlot = plot(f[2:end], transpose(P[:, 2:nyq]), xscale = :log10,
    linecolor = [:red :blue :green :orange],
    label = ["G1" "G2" "G3" "G4"],
    xlabel = "Frequency [Hz]",
    ylabel = "[rad]"
    )

# Julia VS C++
Ccpp    = readcsv("/tmp/mCoeffsRe.csv") + im * readcsv("/tmp/mCoeffsIm.csv")
Cnorm   = sqrt(sum(abs2, C - Ccpp))

Hcpp    = readcsv("/tmp/mHigherRe.csv") + im * readcsv("/tmp/mHigherIm.csv")
Hnorm   = Δf * sqrt(sum(abs2, H - Hcpp))
Hnorms  = Δf * sqrt.(sum(abs2, H - Hcpp, 2))[:]

Gcpp    = readcsv("/tmp/mKernelsRe.csv") + im * readcsv("/tmp/mKernelsIm.csv")
Gnorm   = Δf * sqrt(sum(abs2, G - Gcpp))
Gnorms  = Δf * sqrt.(sum(abs2, G - Gcpp, 2))[:]

g       = real(ifft(G, 2))
gcpp    = readcsv("/tmp/mKernelsTaps.csv")
gnorm   = sqrt(sum(abs2, g - gcpp)) / fs
gnorms  = sqrt.(sum(abs2, g - gcpp, 2))[:] ./ fs

Lcpp    = 20.0 * log10.(abs.(Gcpp))
Pcpp    = unwrap(angle.(Gcpp))

# Final FIRs
firsH   = fft(gcpp, 2)
firsL   = 20.0 * log10.(abs.(firsH))
firsP   = unwrap(angle.(firsH))

plot!(magPlot, f[2:end], transpose(firsL[:, 2:nyq]), xscale = :log10, linestyle = :dot,
    linecolor = [:red :blue :green :orange],
    label = ["G1 (C++)" "G2 (C++)" "G3 (C++)" "G4 (C++)"],
    xlabel = "Frequency [Hz]",
    ylabel = "[dB]"
    )

plot!(phsPlot, f[2:end], transpose(firsP[:, 2:nyq]), xscale = :log10, linestyle = :dot,
    linecolor = [:red :blue :green :orange],
    label = ["G1 (C++)" "G2 (C++)" "G3 (C++)" "G4 (C++)"],
    xlabel = "Frequency [Hz]",
    ylabel = "[rad]"
    )

# hamm1.dsp

gain        = 0.25

branch1     = digitalfilter(Lowpass(50000.0; fs = fs),           Butterworth(6))
branch2     = digitalfilter(Bandpass(220.0, 40000.0; fs = fs),   Butterworth(1))
branch3     = digitalfilter(Bandpass(440.0, 20000.0; fs = fs),   Butterworth(4))
branch4     = digitalfilter(Bandpass(880.0, 30000.0; fs = fs),   Butterworth(5))

# dcBlock     = ZeroPoleGain([1.0], [0.995], 1.0025)
# dcB         = freqz(dcBlock, f, fs)
#
# antiAl      = digitalfilter(Lowpass(23000;  fs = fs), Butterworth(20))
# aAl         = freqz(antiAl, f, fs)

bG1         = freqz(branch1, f, fs) #.* dcB #.* aAl
bG2         = freqz(branch2, f, fs) #.* dcB #.* aAl
bG3         = freqz(branch3, f, fs) #.* dcB #.* aAl
bG4         = freqz(branch4, f, fs) #.* dcB #.* aAl

bG          = gain * transpose([bG1 bG2 bG3 bG4])

bL          = 20.0 * log10.(abs.(bG))
bP          = unwrap(angle.(bG))

plot!(magPlot, f[2:end], transpose(bL[:, 2:nyq]), xscale = :log10, linestyle = :dash,
    linecolor = [:red :blue :green :orange],
    label = ["Expected G1" "Expected G2" "Expected G3" "Expected G4"],
    xlabel = "Frequency [Hz]",
    ylabel = "[dB]"
    )

plot!(phsPlot, f[2:end], transpose(bP[:, 2:nyq]), xscale = :log10, linestyle = :dash,
    linecolor = [:red :blue :green :orange],
    label = ["Expected G1" "Expected G2" "Expected G3" "Expected G4"],
    xlabel = "Frequency [Hz]",
    ylabel = "[rad]"
    )

# Write FAUST source for each resulting FIR (from C++ or Julia as needed).
firs        = g # gcpp

# for nf in 1:size(firs, 1)
#
#     open("./../../Faust/hammersteinFilters/fir$nf.dsp", "w") do fileID
#
#         write(fileID, """fi = library("filters.lib");\n\nprocess = fi.fir((""")
#
#         for n in 1:size(firs, 2)
#
#             value = Float32(firs[nf, n])
#             write(fileID, "$(value)")
#
#             if isequal(n, size(firs, 2))
#                 write(fileID, "));")
#             else
#                 write(fileID, ", ")
#             end
#
#         end
#
#     end
#
# end
#
# open("./../../Faust/hammersteinFilters/identifiedHammersteinFilter.dsp", "w") do fileID
#
#     write(fileID, """fi = library("filters.lib");\n\n""")
#
#     for o in 1:size(firs, 1)
#
#         write(fileID, "fir$(o)Coeffs = (")
#
#         for n in 1:size(firs, 2)
#
#             write(fileID, "$(firs[o, n])")
#
#             if isequal(n, size(firs, 2))
#                 write(fileID, ");\n")
#             else
#                 write(fileID, ", ")
#             end
#
#         end
#
#     end
#
#     write(fileID, "\nbranch1 = pow(_, 1) : fi.fir(fir1Coeffs);\n")
#
#     for o in 2:size(firs, 1)
#
#         write(fileID, "branch$(o) = pow(_, $(o)) : fi.fir(fir$(o)Coeffs);\n")
#
#     end
#
#     write(fileID, "\nprocess = _ <: ")
#
#     for o in 1:size(firs, 1)
#
#         write(fileID, "branch$(o)")
#
#         if isequal(o, size(firs, 1))
#             write(fileID, " :> _;\n")
#         else
#             write(fileID, ", ")
#         end
#
#     end
#
# end

ylims!(magPlot, -150, 50)

plot(magPlot, phsPlot)
