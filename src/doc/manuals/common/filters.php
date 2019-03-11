<p>
	Digital filters can be classified into two big categories - <b>Finite Impulse Response (FIR)</b> filters and <b>Infinite Impulse Response (IIR)</b> filters.
</p>
<p>
	<b>Finite impulse response filters</b> have limited impulse characteristic (the filter's reaction on the single pulse) in time domain.
	That causes that filters have weak resolution in frequency domain when the length of impulse response is short and have very good resolution
	in frequency domain when the length of impulse response is large. With large impulse response FIR filters can be times better than IIR filters
	but the main disadvantage of FIR filters is the necessity to do many calculations to perform convolution of input signal with impulse response
	of the filter. Thanks <b>Fast Fourier Transform (FFT)</b> algorithms the convolution may be calculated times faster rather than using direct 
	convolution algorithm.
</p>
<p>
	<b>Infinite impulse response filters</b> work as systems that process input signal and some part of already processed signal (feedback). Thanks
	such behaviour they have infinite impulse response in time domain. The final equation of these filters is times smaller than for similar by
	characteristics FIR filters, so they need less CPU time for processing. The disadvantage of IIR filters is that they are limited in form of their
	spectral characteristics. Also, IIR filters are mostly the digital representation of analog filters.
</p>
<p>
	FIR filters can be zero-phase (only when performing offline processing), linear-phase and non-linear phase (when performing offline and realtime
	processing). IIR filters never can have linear phase characteristic while performing realtime processing, only in offline mode.
	The difference of linear-phase and non-linear phase of filters is that when summarizing the same signal processed by linear-phase filter, we become no
	phase distortion that can cause unwanted frequency peaks or pits in some frequency range. With IIR filters frequency distortion is often practice rather
	than exception.
	The disadvantage of FIR filters is that due to long impulse response of the linear-phase FIR filter (that is symmetric in time line with peak at center)
	we get noticeably delayed signal after filtering stage.
</p>
<p>
	LSP Plugins provide three types of filters - already known IIR and two types of FIR filters - FIR and FFT. The difference between FIR and FFT filter
	modes is how these filters are built. FIR filters are built from impulse response of similar IIR filter, and then transformed so that they become linear
	phase characteristic. FFT filters are build by using frequency characteristic of FIR filter. Finally, both use FFT convolution to perform fast signal
	processing.
</p>

<p>
	All filters are build by using well-known methods of analog filter design (complex polynomials in top and bottom) or directly as digital filters.
    When the filters are build from analog designs, plugins first of all calculate parameters of ideal analog filter by using Laplace transform and then digitalize
    it by applying analog-to-digital transform also known as Z Transform. Each filter implements it's own set of poles and zeros, the configuration of poles and zeros
    can be controlled by using <b>quality factor</b> and <b>slope parameters</b>.
</p>
<p>
	There are set of rules to perform Laplace to Z-space transformation, for example Simpson rule etc. In LSP plugins, two types of rules are used: the most
	popular Bilinear Transform and less popular Matched Z Transform. The difference between these transforms is how the frequency and ampitude characteristics
	are moved from analog form into digital.
</p>
<p>
	<b>Bilinear Transform</b> is easy to implement, and in most plugins this type of transform is used. The main disadvantage of this transform is that it distorts
	the form of the filter's curve while moving it's poles and zeros from low to high frequencies. Also it's noticeable that low-pass filters transformed by using
	bilinear transform have	-INF dB amplification level at the Nyquist frequency.
</p>
<p>
	Completely different is the behaviour of filters transformed by <b>Matched Z Transform</b>. These filters have identical to the analog filters frequency and
	phase graph but have another disadvantage: they have aliasing at high frequencies that is caused by reflecting high frequencies from the range limited by
	Nyquist Frequency.
</p>
<p>
    Digital filters can also be generated directly in the digital domain, without needing to convert an analog prototype or by using design equations availble in
    literature, often coming from digitalization of canonic analog filter prototypes, but with the advantage of not having to perform digitalization in real-time.
    This is the case of Direct design IIR filters.
</p>
<p>
	Additional attention should be given to FIR filters because <b>Bilinear Transform</b> and <b>Matched Z Transform</b> advantages and disadvantages are only
	applicable to the IIR filters. FIR filters are free of this and just copy characteristics of IIR filters.
</p>
<p>
	All filters are mostly of all frequently used filter types that can be required in studio production.
	There are available simple ideal <b>RLC-filters</b> (Resistor, Inductivity and Capacitor) that have very smooth frequency characteristics,
	<b>Butterworth-Chebyshev-filters</b> based on Butterworth-Chebyshev polynomials (BWC) and <b>Linkwitz-Riley</b>-based filters (LRX).
	So, shortly, RLC filters can be used for accurate frequency correction, BWC-based filters can be used for strong frequency correction and
	LRX-filters for extreme frequency correction.
</p>
<p>
	All filters are designed to have clean sound and keep minimal phase shift if it's possible. They do not affect dynamics of the signal or colorize the
	original sound, they just perform correction in frequency domain. BT (Bilinear Transform) filters will have no distortion at high frequencies but will
	have distorted (relative to the similar analog filter) amplitude and frequency chart. MT (Matched Z Tranfrorm) filters can distort the signal at high
	frequencies because high frequencies can get reflected from Nyquist Frequency and that may cause aliasing effect. In fact, It's not audible in most cases.
</p>
