<?php
	plugin_header();
	
	$s      =   (strpos($PAGE, '_stereo') > 0);
?>

<p>
	A simple plugin for audio systems profiling. The profiling is performed by an algorithm based on the
	<a href="https://ant-novak.com/pages/sss/">Synchronized Swept Sine method by Antonin Novak</a>.
</p>
<p>
    The profiler plugin allows to profile audio systems. These properties of an audio system can be currently profiled:
</p>
<ul>
    <li>Latency.</li>
    <li>Linear Impulse Response.</li>
    <li>Nonlinear Characteristics.</li>
</ul>
<p>A brief description of the plugin usage is provided below. For a summary of controls, see the <b>Controls</b> section.</p>
<p><b>1: Connection</b></p>
<p>The audio system to be profiled should be connected as in the measurement chain below:</p>
<ul>
    <li>profiler output -> audio system input - audio system output -> profiler input</li>
</ul>
<p>
    It is advisable to set the audio stack for stable operation (least amount of buffer over/underruns) when profiling.
    By default, in order to remove unwanted latency from the profile, the plugin operates a profiling sequence that includes latency detection.
    This will remove the average group delay of the entire chain above.
</p>
<p>
    This might affect the accuracy of the phase response measurement of the audio system under test.
    If this is undesired, it is recommended to assess the systemless latency first, and disable automatic latency measurement during profiling.
    To do so, set up this latency measurement loopback:
</p>
<ul>
    <li>profiler output -> straight connection -> profiler input</li>
</ul>
<p>More details are provided in Section 3.</p>
<p><b>2: Calibration</b></p>
<p>
    The plugin has a built-in calibration tone (sine wave) generator whose controls are accessible in the <b>'Calibrator'</b> section.
    After the measurement chain in Section 1 has been realised, it is suggested to set all hardware level controls (if any) to -Inf dB and then enable the calibrator by pressing <b>Enable</b> in the <b>'Calibrator'</b> section (led shines). This forces the plugin to transition into the CALIBRATING state, and
    the corresponding LED in the <b>'Results'</b> section will shine.
</p>
<p>
    Raise the hardware level controls and/or operate the calibrator <b>Amplitude</b> control until the Input Level meter provides a good level.
    All test signals levels will be set by the calibrator <b>Amplitude</b> control.
</p>
<p>
	The test signals level is good when:
</p>
    <ul>
        <li>The monitored plugin input is significantly higher than background noise.</li>
        <li>The calibration tone is not producing any unwanted distortion in the audio system.</li>
        <li>The calibration tone is not producing clipping in the plugin.</li>
    </ul>
<p>
    When the calibration tone is streaming, it is recommended to try a few different values of
    <b>Frequency</b> and select for the calibration the frequency that yields the maximum output of the audio system under test. The use of the frequency
    analyser plugin can facilitate this task.
</p>
<p>After the calibration is concluded, the calibrator should be disabled.</p>
<p><b>3: Latency Measurement</b></p>
<p>
    The plugin automatically detects latency at each profiling cycle by default. However, there are cases in which it is advisable to measure latency only
    once, and disable Latency Detection from all subsequent profilngs. This is useful when:
</p>
<ul>
    <li>The phase response of the system under test needs to be profiled as accurately as possible.</li>
    <li>The signal level at which latency detection is most reliable is different from the level at which profiling is wanted.</li>
</ul>
<p>
    To operate a one time latency measurement, set up the latency measurement loopback of Section 1.
    Then, measure latency once by pressing <b>Measure</b> in the <b>'Latency Detector'</b> section, where all of the plugin Latency Detector controls are available.
    After a successful measurement, disable the latency detection from the system profiling sequence by pressing the <b>Enable</b> button (ensure the LED is off).
    By doing so the Latency Detection step will be omitted in subsequent profiling measurements, and the systemless latency will used to time align the result,
    thus preventing alteration of the system under test phase response.
</p>
<p>
    One time latency measurements are also useful when the signal level for optimal latency detection is higher than that for system profiling.
    If this is the case, set the test signal amplitude for optimal latency measurement by operating the <b>Amplitude</b> control in the <b>'Calibrator'</b> section, 
    measure latency, disable the Latency Detector by disabling the <b>Enable</b> button in the <b>'Latency Detector'</b> section,
    and reset the amplitude control to the optimal value for audio system response profile (as discussed in Section 2).
</p>
<p>
	To be noted that, if the audio stack is not stable, buffer over/underruns (for example, JACK xruns) can produce change
    of latency and invalidate the one time latency measurement (for this reason latency is part of the profiling sequence by default).
</p>
<p>
	Whenever the plugin is detecting latency, its state will be DETECTING LATENCY and the corresponding led in the <b>'Results'</b> section will shine.
</p>
<p><b>4: Set profiling test signal duration</b></p>
<p>
    The control <b>Coarse Duration</b> in the <b>'Test Signal'</b> section is to be used to set the profiling chirp coarse duration. The actual chirp duration will be
    optimised during the pre-processing phase and shown in the <b>Actual Duration</b> indicator. Most measurements will be accurate with 10s Coarse Duration.
    Longer test duration increases the signal to noise ratio. If a reverberant system is being measured,
    the <b>Coarse Duration</b> should be longer than the expected reverberation time.
</p>
<p><b>5: Perform the profiling</b></p>
<p>
	Pressing <b>Profile</b> in the <b>'Test Signal'</b> section triggers a single profiling sequence.
    This will make the plugin to transition through the following states automatically:
</p>
<ul>
    <li><b>DETECTING LATENCY</b> - In this state the latency of the audio system measurement chain is assessed. This step can be omitted by
    disabling the <b>Enable</b> toggle in the <b>'Latency Detector'</b> section. If latency was never measured, the plugin will force
    latency detection.</li>
    <li><b>PREPROCESSING</b> - In this state the plugin optimises the test signal parameters and generates the test chirp.</li>
    <li><b>WAITING</b> - In this state the plugin waits for a time set by the <b>Coarse Duration</b> control. For reverberant systems,
    this time should be longer than the expected reverberation time. This waiting state avoids the reverberant tail of the Latency Detection
    chirp to pollute the measurement.</li>
    <li><b>RECORDING</b> - In this state the profiling chirp is emitted and the audio system output recorded.
    A tail is also recorded in order to not truncate the high frequency reverberation recording.</li>
    <li><b>CONVOLVING</b> in this state the plugin is convolving the recorded output with an inverse filter in order
    to calculate the characteristics of the audio system.</li>
    <li><b>POSTPROCESSING</b> - In this state the measurement result is being analysed to extract properties.</li>
</ul>
<p>
    The current state of the plugin is displayed at any time in the <b>'Results'</b> section.
</p>
<p><b>6: Post processing the results</b></p>
<p>
	Post processing is performed automatically after each measurement or manually by pressing the
    <b>Post-process</b> button in the <b>'Results'</b> section. Whenever the plugin is postprocessing, its state will be POSTPROCESSING and the corresponding
    LED in the <b>'Results'</b> section will shine. The post-processing steps can calculate:
</p>
<ul>
    <li>Background noise magnitude.</li>
    <li>Reverberation Time (RT).</li>
    <li>Energy Decay Linear Correlation coefficient.</li>
    <li>Coarse Linear Impulse Response Duration</li>
</ul>
<p>
    All the quantities above are mostly relevant for linear time invariant (LTI) systems. The result will be displayed and postprocessed from the 
    time originle of the Linear Impulse Response estimate of the system.
</p>
<p>
    To change this, operate the <b>Offset</b> control in the <b>'Results'</b> section, which allows to introduce a time offset.
    For numerical reasons, few details of the measured Linear Impulse Response are mapped into negative time samples to the left ot the origin of time.
    In case the spread of the Linear Impulse Response to the left of the origin of time is important, introducing a negative offset will increase accuracy of the calculations listed above, as well as providing a more accurate measurement.
    The Reverberation Time can be calculated with any of the algorithms in the <b>RT Algorithm</b> selector in the <b>'Results'</b> section. All the algorithms are based in calculating,
    from the Linear Impulse Response, the Energy Decay curve through backward integration, fitting a straight line in the Energy Decay curve in a
    specified interval and solving for the point at which the straight line intercepts -60 dB from the peak energy.
</p>
<p>
    See the <b>RT Algorithm</b> description for a list of the implemented algorithms, all based on ISO 3382-2 (but without filtering).
</p>
<p>
	The algorithms supply the best results only if the background noise floor level in the Energy Decay curve is at least 10 dB below the lower
    limit of the regression line calculation. If this is true, the relevant <b>Noise Floor</b> LED will shine. In order to improve the Signal To Noise ratio,
    it can be advised to:
</p>
<ul>
    <li>Make sure there is not unwanted clipping or distortion, as distortion products inflate the background noise assessment.</li>
    <li>Make sure the profiling chirp duration is long enough, as longer chirps provide better Signal To Noise ratio.</li>
    <li>Make sure the amplitude is high enough with respect the background noise, but not so high to produce unwanted distortion.</li>
    <li>Make sure that eventual Linear Impulse Response samples to the left of the origin of time are not being omitted by introducing some negative offset.</li>
</ul>
<p>
	The Energy Decay Linear Correlation coefficient is the Pearson correlation coefficient for the fitted regression line used for Reverberation Time
    calculation. For well fitted decaying lines this value is close to -1.
</p>
<p>
	The <b>Coarse IR Duration</b> is instead the Linear Impulse Response duration based on the envelope of the Energy Decay curve.
    The value of the <b>Coarse IR Duration</b> is the time at which the envelope dives into the noise floor.
</p>
<p><b>7: Saving the results</b></p>
<p>
    The profile can be saved by using the Save button in the <b>'Results'</b> section.
    See <b>Save Mode</b> for the available saving modes. All saving ranges are rounded to the next tenth of second.
    Auto should be able to save all the meaningful parts of the Linear Impulse Response.
</p>
<p>
	Whenever the plugin is saving to file, its state will be SAVING and the corresponding LED in the <b>'Results'</b> 
	section will shine.
</p>

<p><b>Controls:</b></p>
<ul>
	<li>
		<b>Bypass</b> - bypass switch, when turned on (LED indicator is shining), the plugin bypasses signal.
	</li>
</ul>
<p><b>'Results' section:</b></p>
<ul>
	<li><b>Results Graph</b> - Graph that shows the Linear Impulse response, from the Offset value up to the Reverberation Time.</li>
	<li><b>PROFILER STATE</b> - Shows the internal state of the plugin.</li>
	<li><b>Reverberation Time (s)</b> - Indicator that reports the estimated overall Reverberation Time in seconds, according to the selected RT algorithm and Offset.</li>
    <li><b>Energy Decay Linear Correlation</b> - Indicator that reports the Pearson correlation coefficient of the Energy Decay linear regression line used for Reverberation Time calculation.</li>
    <li><b>Coarse IR Duration (s)</b> - Indicator that reports the coarse value of the Linear Impulse Response duration,
    estimated by Energy Decay envelope, in seconds.</li>
    <li><b>RT Algorithm</b> - Reverberation Time (RT) is calculated by linear regression of the Energy Decay curve.
    The limits are chosen by this selector.</li>
    <ul>
	    <li><b>EDT0</b> - Early Decay Time, Linear Regression algorithm on values of Energy Decay between 0 dB and -10 dB from peak.</li>
	    <li><b>EDT1</b> - Early Decay Time, Linear Regression algorithm on values of Energy Decay between -1 dB and -10 dB from peak.</li>
	    <li><b>RT10</b> - Reverberation Time, Linear Regression algorithm on values of Energy Decay between -5 dB and -15 dB from peak.</li>
	    <li><b>RT20</b> - Reverberation Time, Linear Regression algorithm on values of Energy Decay between -5 dB and -25 dB from peak.</li>
	    <li><b>RT30</b> - Reverberation Time, Linear Regression algorithm on values of Energy Decay between -5 dB and -35 dB from peak.</li>
    </ul>
    <li><b>Save Mode</b></li>
    <ul>
	    <li><b>LTI Auto (*.wav)</b> - Save, as a WAV file, the Linear Impulse Response from the Offset value up to the largest value of time between RT and Coarse IR Duration.</li>
	    <li><b>LTI RT (*.wav)</b> - Save, as a WAV file, the Linear Impulse Response from the Offset value up to to the RT value.</li>
	    <li><b>LTI Coarse (*.wav)</b> - Save, as a WAV file, the Linear Impulse Response from the Offset value up to the Coarse IR Duration value.</li>
	    <li><b>LTI All (*.wav)</b> - Save, as a WAV file, all the measured samples of Linear Impulse Response to the right of the Offset value.</li>
	    <li><b>All Info (*.lspc)</b> - Save, as an LSPC file, all the measured information.</li>
    </ul>
	<li><b>Offset</b> - Introduce an offset from the origin of time of the Linear Impulse Response, for post processing purposes, milliseconds.</li>
	<li><b>Post-process</b> - Button that forces the plugin to post-process the measurement result.</li>
	<li><b>Save</b> - Save button.</li>
    <li><b>Noise Floor</b> - If shining, the background noise and/or Offset value are optimal for the selected RT algorithm accuracy.</li>
</ul>
<p><b>'Calibrator' section:</b></p>
<ul>
    <li><b>Frequency</b> - Frequency of the Calibration tone.</li>
    <li><b>Amplitude</b> - Amplitude of the Calibration tone.</li>
    <li><b>Input Level (dB)</b> - Indicator that reports the input level in dB.</li>
    <li><b>Enable</b> - enables the Calibration tone.</li>
    <li><b>Feedback</b> - If off, the feedback loop between input and output is disabled to avoid gain buildup.</li>
</ul>
<p><b>'Latency Detector' section:</b></p>
<ul>
    <li><b>Max latency</b> - Maximum value of the expected transmission line latency, in milliseconds (needs to include buffering latency).</li>
    <li><b>Peak</b> - Peak threshold for early detection: if the gap between consecutive local convolution peaks is higher than this value
    the plugin returns the latency associated with the highest peak.</li>
    <li><b>Absolute</b> - Absolute threshold for the detection algorithm: values of convolution smaller than this are ignored by the algorithm.</li>
    <li><b>Latency (ms)</b> - Indicator that reports the measured latency in milliseconds.</li>
    <li><b>Enable</b> - Enables latency detection, if this control is deactivated, Latency Detection is omitted by the profiling sequence.</li>
    <li><b>Measure</b> - Button that forces the plugin to perform a single latency measurement.</li>
</ul>
<p><b>'Test Signal' section:</b></p>
<ul>
    <li><b>Coarse Duration</b> - Sets the coarse duration of the profiling Test Signal.</li>
    <li><b>Actual Duration</b> - Actual duration of the profiling Test Signal, after optimisation performed in pre-processing.</li>
    <li><b>Profile</b> - Button that forces the plugin to perform a single profiling measurement.</li>
</ul>
