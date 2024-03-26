# Universal Comb Filter

The universal comb filter is a simple demonstration the vast array of effects made possible by delay-based filtering. This filter is **universal** due to its innate ability to function as a delay, chorus, flanger, vibrato, ring mod, and (in this implementation) tremolo. Vibrato and tremolo are made possible by a variable-length delay (causing time stretching and compression, resulting in pitch bend), which can be modulated by a built-in LFO with knobs corresponding to frequency and depth.

The GUI is designed to mimic the block diagram reresentation of the filter, allowing users to tweak feedforward, feedback, and bleed coefficients in realtime with an accurate graphical representation of the data flow.

![Universal Comb Filter GUI](./img/ui.png)


## Usage

The variable delay in the centre can be extend from $(0, 500)$ ms, and the LFO modulating the delay length (min. delay) can be anywhere in the range $(0, 250)$ Hz (large frequencies like 250 are usually reserved for ring modulation). The modulation depth (or sweep width) is expressed in time, ranging from $(0,5)$ ms. This refers to the amount by which the LFO adjusts the delay length. For example, a modulation depth of 5 ms paired with a non-zero LFO frequency increases and decreases the delay by time 5 ms at a constant rate. This is audible as the amount of vibrato applied to the input signal.

Three knobs representing `feedforward` (delayed signal to output), `feedback` (delayed signal to input), and `bleed` (non-delayed signal) can be tweaked in the range $(0,1)$. Note that the bleed line is taken after the summation of the dry input and any feedback, so any feedback will also be present in this signal.

An additional `tremolo` toggle allows the LFO to modulate the amplitude of the output signal.

The following are presets for some common effects:
- Flanger:
    - `FF = 0.7, FB = 0.7, BL = 0.7`
    - `delay = 0 ms, depth = 2.0 ms, freq = 0.5 Hz`
-  Vibrato:
    - `FF = 1.0, FB = 0.0, BL = 0.0`
    - `delay = 0 ms, depth = 2.0 ms, freq = 4.0 Hz`
- Ring Mod:
    - `FF = 1.0, FB = 0.0, BL = 0.0`
    - `delay = 0 ms, depth = 5.0 ms, freq >= 100 Hz`
- Chorus:
    - `FF = 1.0, FB = 0.0, BL = 1.0`
    - `delay ~= 20 ms, depth = 0.0 ms, freq = 0.0 Hz`

The above presets can get you started, but please use this as a tool to experiment with the universal comb! Playing with feedback is fun. If you make cool sounds that aren't listed here, feel free to reach out!


## Installation

1. Download and unzip the `.vst3` file in the [latest release](https://github.com/isaiahdoyle/universalcombfilter/releases/tag/v0.9)
2. Locate your VST3 folder
    - MacOS default: `Library/Audio/Plugin-Ins/VST3`
    - Windows default: `C:\Program Files\Common Files\VST3`
3. Move/copy the unzipped `.vst3` file into the folder above


*note: this plugin has only been tested this on MacOS 14. it should be cross-platform to my knowledge (i.e., no mac-specific operations are used), but i've yet to experiment with cross-platform testing.*
