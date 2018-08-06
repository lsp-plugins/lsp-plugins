<?php
	plugin_header();
?>

<p>
	A simple plugin for audio systems profiling. The profiling is performed by an algorithm based on the
	<a href="http://ant-novak.com/swept-sine.php">Synchronized Swept Sine method by Antonin Novak</a>.
</p>
<p>
    The profiler plugin allows to profile audio systems. These properties of an audio system can be currently profiled:
    <ul>
        <li>Latency</li>
        <li>Linear Impulse Response</li>
        <li>Nonlinear Characteristics</li>
    </ul>
    Profiling should be performed as follows.
    <p><b>1: Connection</b></p>
    The audio system to be profiled should be connected as in the measurement chain below:
    <ul>
        <li>profiler output -> audio system input - audio system output -> profiler input</li>
    </ul>
    It is advisable to set the audio stack for stable operation (least amount of buffer over/underruns) when profiling.
    By default, the plugin will remove the latency of the measurement chain. This might alter the phase response of the system under test.
    If this is undesired, it is recommended to assess the systemless latency first, and skip automatic latency measurement during profiling.
    To do so, set up this latency measurement loopback:
	<ul>
        <li>profiler output -> straight wire -> profiler input</li>
    </ul>
    Measure latency once by pressing <b>Measure Latency</b> in the <b>'Controls'</b> section. The latency detector controls are available in the <b>'Latency Detector'</b> 
    section. In the <b>'Latency Detector'</b> section, after a successful measurement has been performed (the result is displayed in the <b>'Results'</b> section),
    press the <b>Skip</b> button. This will remove only the loopback latency from the subsequent measurements, and leave the system response unaltered.
    <p><b>2: Calibration</b></p>
    The plugin has a built-in calibration tone (sine wave) generator whose control are accessible in the <b>Calibrator</b> section.
    After the measurement chain in Section 1 has been realised, it is suggested to set all hardware level controls (if any) to -Inf dB and then turn <b>ON</b> the calibrator.
    Raise the hardware level controls and/or operate the calibrator <b>Amplitude</b> control until the profiler input level meter provides a good level.
    All test signals levels will be set by the calibrator <b>Amplitude</b> control.
    <p>
    The test signals level is good when:
    </p>
    <ul>
        <li>The monitored plugin input is significantly higher than background noise.</li>
        <li>The calibration tone is not producing any unwanted distortion in the audio system.</li>
        <li>The calibration tone is not producing clipping in the plugin.</li>
    </ul>
    In order to monitor the audio system distortion, the frequency analyser plugin can be used to scope the frequency content of the signal
    fed to the profiler input by looking at the magnitude of harmonics. When the calibration tone is streaming, it is recommended to try a few different values of
    <b>Frequency</b> and select for the calibration the frequency that yields the maximum output of the audio system under test.
    <p>After the calibration is concluded, the calibrator should be turned <b>OFF</b>.</p>
    <p><b>3: Latency Measurement</b></p>
    It is advisable to attempt measuring systemless latency alone first. This can be done by realising the latency measurement loopback of Section 1 and operating the
    <b>Measure Latency</b> button in the <b>Controls</b> section.
    If needed, the latency detector parameters in the <b>'Latency Detector'</b> section can be tweaked. The measured value of latency will appear in the <b>Results</b> section.
    <p>The latency can be measured one single time before to profile the audio system. Notice that the test signal amplitude for optimal latency
    measurement might be higher than that for system profiling.
    To do so, set the test signal amplitude for optimal latency measurement (if necessary) by operating the <b>Amplitude</b> control in the <b>'Calibrator'</b> section, 
    measure latency, press the <b>Skip</b> button in the <b>Latency Detector</b> section,
    and reset the amplitude control to the optimal value for audio system response profile (as discussed in Section 2).</p>
    <p>This way, the latency detection step is omitted by the normal profiling sequence,
    and the previously measured value is used instead.</p>
    <p>To be noted that, if the audio stack is not stable, buffer over/underruns (for example, JACK xruns) can produce change
    of latency and invalidate the one time latency measurement (for this reason latency is part of the profiling measurement cycle by default).</p>
    <p><b>4: Set profiling test signal duration</b></p>
    The control <b>Coarse Duration</b> in the <b>'Test Signal'</b> section is to be used to set the profiling chirp coarse duration. The actual chirp duration will be
    optimised during the pre-processing phase and shown in the <b>Actual Duration</b> indicator. Most measurements will be accurate with 10s Coarse Duration.
    Longer test duration increases the signal to noise ratio. If a reverberant system is being measured,
    the <b>Coarse Duration</b> should be longer than the expected reverberation time.
    <p><b>5: Perform the profiling</b></p>
    By pressing <b>Profile</b> in the <b>'Controls'</b> section a single measurement is triggered.
    This will make the plugin to transition through the following states automatically:
    <ul>
        <li><b>LATENCY DETECTION</b> - In this state the latency of the audio system measurement chain is assessed. This step can be skipped by
        pressing the <b>Skip</b> button in the <b>Latency Detector</b> section. However, the plugin will perform the profiling measurement
        only if a value of latency has been previously successfully measured.</li>
        <li><b>PREPROCESSING</b> - In this state the plugin optimises the test signal parameters and generates the test chirp.</li>
        <li><b>WAIT</b> - In this state the plugin waits for a time set by the <b>Coarse Duration</b> control. For reverberant systems,
        this time should be longer than the expected reverberation time. This waiting state avoids the reverberant tail of the Latency Detection
        chirp to pollute the measurement.</li>
        <li><b>RECORDING</b> - In this state the profiling chirp is emitted and the audio system output recorded.
        A tail is also recorded in order to not truncate the high frequency reverberation recording.</li>
        <li><b>CONVOLVING</b> in this state the plugin is convoling the recorded output with an inverse filter in order
        to calculate the characteristics of the audio system.</li>
        <li><b>POSTPROCESSING</b> in this state the measurement result is being analysed to extract properties.</li>
    </ul>
    <p><b>6: Post processing the results</b></p>
    Post processing is performed automatically after each measurement or manually by pressing the
    <b>Post-process</b> button in the <b>Results</b> section. The post processing steps can calculate:
    <ul>
        <li>Background noise magnitude.</li>
        <li>Overall Reverberation Time (RT).</li>
        <li>Energy Decay Linear Correlation coefficient.</li>
        <li>Coarse Linear Impulse Response Duration</li>
    </ul>
    All the quantities above are mostly relevant for linear time invariant (LTI) systems. The result will be displayed and postprocessed from the middle of the
    Linear Impulse Response estimate of the system.
    To change this, operate the <b>Offset</b> control in the <b>Results</b> section, which allows to introduce a time offset.
    In case the spread of the Linear Impulse Response to the left of the middle is important, introducing a negative offset will increase accuracy of the calculations listed above.
    RT can be calculated with any of the algorithms in the <b>RT Algorithm</b> selector in the <b>Results</b> section. All the algorithms are based in calculating,
    from the Linear Impulse Response, the Energy Decay curve through backward integration, fitting a straight line in the Energy Decay curve in a
    specified interval and solving for the point at which the straight line intercepts -60 dB from the peak energy.
    See the <b>RT Algorithm</b> description for a list of the implemented algorithms, all based on ISO 3382-2 (but without filtering).
    <p>The algorithms supply the best results only if the background noise floor level in the Energy Decay curve is at least 10 dB below the lower
    limit of the regression line calculation. If this is true, the <b>Low Back Noise</b> led will shine. In order to improve the Signal To Noise ratio,
    it can be advised to:<p>
    <ul>
        <li>Make sure there is not unwanted clipping or distortion, as distortion products inflate the background noise assessment.</li>
        <li>Make sure the profiling chirp duration is long enough.</li>
        <li>Make sure the amplitude is high enough with respect the background noise, but not so high to produce unwanted distortion.</li>
        <li>Make sure that samples to the left of the middle are not being omitted by introducing some negative offset.</li>
    </ul>
    The calculated RT value is shown in the <b>Results</b> section.
    <p>R will show the correlation coefficient for the fitted regression line. For well fitted decaying lines, used for predicting RT,
    the value of R should be close to -1.</p>
    <p>The <b>Coarse IR Duration</b> will instead show the Linear Impulse Response duration based on the envelope of the Energy Decay curve.
    The value of the <b>Coarse IR Duration</b> is the time at which the envelope dives into the noise floor. This time is normally larger than the reverberation time.</p>
    <p><b>7: Saving the results</b></p>
    The measurement can be saved by using the Save button in the <b>Results</b> section.
    See <b>Save Mode</b> for the available saving modes. All saving ranges are rounded to the next tenth of second.
    Auto should be able to save all the meaningful parts of the Linear Impulse Response.
</p>
<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (led indicator is shining), the plugin bypasses signal.
	</li>
</ul>
<p><b>'Results' section:</b></p>
<ul>
	<li><b>Input Level (dB)</b> - Indicator that reports the input level in dB.</li>
	<li><b>Results Graph</b> - Graph that shows the results of the profiling process.</li>
	<li><b>IR Offset</b> - Introduce an offset from the middle of the Linear Impulse Response, for post processing purposes, milliseconds.</li>
    <li><b>Latency (ms)</b> - Indicator that reports the measured latency in milliseconds.</li>
    <li><b>RT Algorithm</b> - Reverberation Time (RT) is calculated by linear regression of the Energy Decay curve.
    The limits are chosen by this selector. All saving ranges are rounded to the next tenth of second.</li>
    <ul>
	    <li><b>EDT0</b> - Early Decay Time, Linear Regression algorithm on values of Energy Decay between 0 dB and -10 dB from peak.</li>
	    <li><b>EDT1</b> - Early Decay Time, Linear Regression algorithm on values of Energy Decay between -1 dB and -10 dB from peak.</li>
	    <li><b>RT10</b> - Reverberation Time, Linear Regression algorithm on values of Energy Decay between -5 dB and -15 dB from peak.</li>
	    <li><b>RT20</b> - Reverberation Time, Linear Regression algorithm on values of Energy Decay between -5 dB and -25 dB from peak.</li>
	    <li><b>RT30</b> - Reverberation Time, Linear Regression algorithm on values of Energy Decay between -5 dB and -35 dB from peak.</li>
    </ul>
    <li><b>Low Back Noise</b> - If shining, the background noise and/or offset are optimal for the selected RT algorithm accuracy.</li>
    <li><b>Reverberation Time (s)</b> - Indicator that reports the estimated overall Reverberation Time in seconds, according to the selected RT algorithm and Offset.</li>
    <li><b>Energy Decay Linear Correlation</b> - Indicator that reports the Pearson correlation coefficient of the Energy Decay linear regression line used for Reverberation Time calculation.</li>
    <li><b>Coarse IR Duration (s)</b> - Indicator that reports the coarse value of measured Linear Impulse Response duration,
    estimated by Energy Decay envelope, in seconds.</li>
    <li><b>Post-process</b> - Button that forces the plugin to post-process the measurement result.</li>
    <li><b>Save Mode</b></li>
    <ul>
	    <li><b>LTI Auto (*.wav)</b> - Save, as a WAV file, the Linear Impulse Response from the offset value up to the largest value of time between RT and Coarse IR Duration.</li>
	    <li><b>LTI RT (*.wav)</b> - Save, as a WAV file, the Linear Impulse Response from the offset value up to to the RT value.</li>
	    <li><b>LTI Coarse (*.wav)</b> - Save, as a WAV file, the Linear Impulse Response from the offset value up to the Coarse IR Duration value.</li>
	    <li><b>LTI All (*.wav)</b> - Save, as a WAV file, all the measured samples of Linear Impulse Response to the right of the offset value.</li>
	    <li><b>All Info (*.lspc)</b> - Save, as an LSPC file, all the measured information. In this case the offset control does not affect the saved quantities.</li>
    </ul>
    <li><b>Save</b> - Save button.</li>
</ul>
<p><b>'Calibrator' section:</b></p>
<ul>
    <li><b>Frequency</b> - Frequency of the Calibration tone.</li>
    <li><b>Amplitude</b> - Amplitude of the Calibration tone.</li>
    <li><b>Enable</b> - Switches ON and OFF the Calibration tone.</li>
</ul>
<p><b>'Latency Detector' section:</b></p>
<ul>
    <li><b>Max latency</b> - Maximum value of the expected transmission line latency, in milliseconds (needs to include buffering latency).</li>
    <li><b>Peak</b> - Peak threshold for early detection: if the gap between consecutive local convolution peaks is higher than this value
    the plugin returns the latency associated with the highest peak.</li>
    <li><b>Absolute</b> - Absolute threshold for the detection algorithm: values of convolution smaller than this are ignored by the algorithm.</li>
    <li><b>Enable</b> - Enables latency detection, if this control is deactivated, Latency Detection is omitted by the profiling sequence.</li>
</ul>
<p><b>'Test Signal' section:</b></p>
<ul>
    <li><b>Coarse Duration</b> - Sets the coarse duration of the profiling Test Signal.</li>
    <li><b>Actual Duration</b> - Actual duration of the profiling Test Signal, after optimisation performed in pre-processing.</li>
</ul>
<p><b>'Controls' section:</b></p>
<ul>
    <li><b>Feedback</b> - If off, the feedback loop between input and output is disabled to avoid gain buildup.</li>
    <li><b>Measure Latency</b> - Button that forces the plugin to perform a single latency measurement.</li>
    <li><b>Profile</b> - Button that forces the plugin to perform a single profiling measurement.</li>
</ul>
<p><b>'State' section:</b> - Shows the internal state of the plugin.</p>
