using DSP
using Plots

pyplot()

responseType    = Lowpass(0.5)
designMethod    = FIRWindow(hanning(512))
firFilter       = digitalfilter(responseType, designMethod)

oscFreq         = 10000.0

firFilterFFT    = fft(firFilter)
firFilterFFT[1] = 0.0 + 0.1im

firFilter       = real(ifft(firFilterFFT))

# Deploy a FAUST DC Block Tester

open("./../../Faust/prototypes/testDCBlock.dsp", "w") do fileID

    write(fileID, """os = library("oscillators.lib");\nfi = library("filters.lib");\n\n""")

    write(fileID, """firTaps = (""")

    for n in 1:length(firFilter)

        write(fileID, "$(firFilter[n])")

        if isequal(n, length(firFilter))
            write(fileID, ");\n")
        else
            write(fileID, ", ")
        end

    end

    write(fileID, "\ndcOscillator = +(0.4 * os.osc($oscFreq), 0.5);\n")

    write(fileID, "\nprocess = dcOscillator : fi.fir(firTaps);")

    #write(fileID, "\nprocess = dcOscillator;")

end
