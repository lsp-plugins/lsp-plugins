using DSP
using Plots

pyplot()

fs      = 8000.0
up      = 5.0
f       = 220.0

fs_up   = up * fs

n       = 0:(fs - 1)
n_up    = 0:(fs_up - 1)

t       = n / fs
t_up    = n_up / fs_up

x       = sinpi.(2.0 * f * t)
x_up    = sinpi.(2.0 * f * t_up)

sigPlot = plot(t, x)
plot!(sigPlot, t_up, x_up)
display(sigPlot)

responsetype    = Lowpass(1000; fs = fs)
designmethod    = FIRWindow(hanning(512))
firFilt         = digitalfilter(responsetype, designmethod)
firFilt_up      = resample(firFilt, up) / up

responsetype2   = Lowpass(1000; fs = fs_up)
designmethod2   = FIRWindow(hanning(Int(up * 512)))
firFilt2        = digitalfilter(responsetype2, designmethod2)

tFir            = ((1:length(firFilt)) - 1) / fs
tFir_up         = ((1:length(firFilt_up)) - 1) / fs_up
tFir2           = ((1:length(firFilt2)) - 1) / fs_up

firPlot = plot(tFir, firFilt)
plot!(firPlot, tFir_up, firFilt_up)
plot!(firPlot, tFir2, firFilt2)
display(firPlot)

y       = filt(firFilt, x)
y_up    = filt(firFilt_up, x_up)

sigPlot2 = plot(t, y)
plot!(sigPlot2, t_up, y_up)
display(sigPlot2)
