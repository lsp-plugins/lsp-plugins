using WAV
using Plots
using DSP

pyplot()

aaTaps  = 64
fₑ      = readcsv("/tmp/fFinalFrequency.csv")[1, 1]
fs      = Int(readcsv("/tmp/fSampleRate.csv")[1, 1])
gcpp    = readcsv("/tmp/mKernelsTaps.csv")
Gcpp    = readcsv("/tmp/mKernelsRe.csv") + im * readcsv("/tmp/mKernelsIm.csv")
Lcpp    = 20.0 * log10.(abs.(Gcpp))

nyq     = div(size(Gcpp, 2), 2) + 1
f       = (0:(nyq - 1)) * fs  / 2nyq
Δf      = fs / (2 * nyq)

labels  = ["G1 (C++)"]

for o in 2:size(Gcpp, 1)
    labels = [labels "G$(o) (C++)"]
end

maxLin  = maximum(abs, Gcpp[1, :])
GTrim   = Gcpp ./ maxLin
LTrim   = 20.0 * log10.(abs.(GTrim))
gTrim   = real(ifft(GTrim))

magPlot = plot(f[2:end], transpose(LTrim[:, 2:nyq]),
    xscale  = :log10,
    label   = labels,
    xlabel  = "Frequency [Hz]",
    ylabel  = "[dB]"
    )

display(magPlot)

firs    = gTrim

open("./../../Faust/modelFilters/model_FIR.dsp", "w") do fileID

    write(fileID, """fi = library("filters.lib");\n\n""")

    for o in 1:size(firs, 1)

        # Let's create this branch anti-aliasing linar phase FIR.
        write(fileID, "antiAl$(o)Coeffs = (")

        responseType    = Lowpass(fₑ / o; fs = fs)
        designMethod    = FIRWindow(hanning(aaTaps))
        branchAntial    = digitalfilter(responseType, designMethod)

        for t in 1:length(branchAntial)

            write(fileID, "$(branchAntial[t])")

            if isequal(t, length(branchAntial))
                write(fileID, ");\n")
            else
                write(fileID, ", ")
            end

        end

        write(fileID, "branch$(o)Proc = (")

        for t in 1:size(firs, 2)

            write(fileID, "$(firs[o, t])")

            if isequal(t, size(firs, 2))
                write(fileID, ");\n")
            else
                write(fileID, ", ")
            end

        end

        write(fileID, "branch$(o) = fi.fir(antiAl$(o)Coeffs) : pow(_, $(o)) : fi.fir(branch$(o)Proc);\n\n")

    end

    write(fileID, "process = _ <: ")

    for o in 1:size(firs, 1)

        write(fileID, "branch$(o)")

        if isequal(o, size(firs, 1))
            write(fileID, " :> _;\n")
        else
            write(fileID, ", ")
        end

    end

end
