using DSP
using Plots

sampleRate  = 192000
plotFreqs   = 1:(div(sampleRate, 2) - 1)

# Branches Definitions
branches = [
                digitalfilter(Lowpass(50000.0; fs = sampleRate),           Butterworth(6));
                digitalfilter(Bandpass(220.0, 40000.0; fs = sampleRate),   Butterworth(1));
                digitalfilter(Bandpass(440.0, 20000.0; fs = sampleRate),   Butterworth(4));
                digitalfilter(Bandpass(880.0, 30000.0; fs = sampleRate),   Butterworth(5));
]

# Create Faust code
open("./../../Faust/testBeds/hammersteinFilter.dsp", "w") do f

    write(f, """fi = library("filters.lib");\n\n""")

    for n in 1:length(branches)

        write(f, "branchSection$n = ")

        biquadsInfo = convert(SecondOrderSections, branches[n])

        write(f, "*(_, $(biquadsInfo.g)) : ")

        for b in 1:length(biquadsInfo.biquads)

            write(f, "fi.tf2np(")

            coefficients = biquadsInfo.biquads[b]

            write(f, "$(coefficients.b0), $(coefficients.b1), $(coefficients.b2), $(coefficients.a1), $(coefficients.a2))")

            if isequal(b, length(biquadsInfo.biquads))
                write(f, ";\n")
            else
                write(f, " : ")
            end

        end

    end

    write(f, "\nbranch1 = pow(_, 1) : branchSection1;\n")

    for b in 2:length(branches)
        write(f, "branch$b = pow(_, $b) : branchSection$b;\n")
    end

    write(f, "\nprocess = _ <: ")

    for b in 1:length(branches)

        write(f, "branch$b")

        if isequal(b, length(branches))
            write(f, " ")
        else
            write(f, ", ")
        end

    end

    write(f, ":> *(_, $(1.0 / length(branches)));")

end

# run(`faust2jackconsole ./../../Faust/hammersteinFilters/hammersteinFilter.dsp`)

pyplot()

H       = freqz(branches[1], plotFreqs, sampleRate)
plotId  = plot(plotFreqs, 20.0 * log10.(abs.(H)), xscale = :log10)

for b in 2:length(branches)

    H = freqz(branches[b], plotFreqs, sampleRate)

    plot!(plotId, plotFreqs, 20.0 * log10.(abs.(H)), xscale = :log10)

end

ylims!(plotId, -100, 10)
