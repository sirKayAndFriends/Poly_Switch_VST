# Poly Switch Amplifier VST (v3.0)
A two channel amplifier with flexible EQ stuff and fun features for all ages.

## A Quick Introduction
### BOOST
A simple volume booster with variable "peak" frequency and bass cut.
- **Boost:** the amount of preamp volume.  Noon is flat gain, right is up to 10x volume boost and left is volume cut (0 - 1).  This means you can add "scoop" at your desired frequency as well as a boost peak
- **Peak:** the variable frequency of the boost.  From 400Hz to 2400Hz.
- **Cut:** a high pass filter.  Fully right is full bass (cutoff frequency @ 25Hz) and fully left is full cut (350Hz).
**Check out the visualiser for an easier time**

### TONE STACK
Three band EQ
- **Treble and Bass:** shelving filters for cut and boost.  Noon is once again "flat".  Treble is set at 2000Hz and Bass at 350Hz
- **Mid:** a mid boost pretty much identical to the one in the boost section (but not as powerful)
- **Voice:** the magic mid centre frequency.  Goes from 400Hz to 850Hz for a full range of amp mids sounds... you know, "American" and "British" :/

### GAIN
The gain bit.  This is based on Will Pirkle's triode simulation in and addendum to the programming tome *Designing Audio Effect Plugins in C++* which you can find [here!](https://www.willpirkle.com/fx-book-bonus-material/chapter-19-addendum/).  <sub>(This is BANANAS... also I love the style of those old plugins.  What were they thinking??)</sub>

If you have the compression function on (see "secret settings") you have to be a little careful about input volume.  The compressor waveshaper can start ducking wildly if the signal is too high going into the amp so be careful!  This thing acts a bit more like an op-amp distortion pedal when pushed too hard so don't be going will-nilly with the boosts beforehand.  As long as the gain level at the amp input is below 0db you'll be fine.

- **Gain:** amount of gain bit
- **LESS:** the lower gain channel
- **MORE:** high gain channel

### OUTPUT
Where the sound comes out
- **Level:** loudosity
- **Cabinet:** a low pass filter to simulate a speaker response.  There are so many EQ controls already that I think a bunch more for a cabsim would be ludicrous.  This is simply a lowpass filter with a variable cutoff between 3000 and 6000 Hz.

## SECRET SETTINGS!
Hover over the title.  Notice some hidden buttons?  Wow, good job finding the secret!  Clicking on the "POLY" button deactivates the power amp section if thats your thing.  The "SWITCH" button turns off Pirkle's compression function.  This kinda "pre-shapes" the guitar signal and compresses it.  This is cool because the following waveshaper then becomes "dynamic" as the incoming compressed guitar signal has a DC offset relative to its amplitude.  Basically the bias point moves around depending on how you play to get a more "interactive" distortion (something something triode self-bias).  BUT you have to be careful about the input volume as too louda signal will make this section go apeshit and you'll get some really weird ducking and stuff.  Easy enough to avoid but if you can't be arsed with proper gain-staging (like me) you can turn this off.  It will have less of the mythical "sag" but I'm sure we can all live with that.

## Some Technical Bits for the Nerds
So this has gone through a bunch of different versions and methods to get here (hi Adam! If you're reading this, made you look in the Nerd Section).  I eventually got annoyed with the PureData C++ compiler etc and grumped off to learn me some proper C++ with JUCE.  It now sounds way better and there are fewer source files to look through.

The overall all architecture is based on a certain pedal company's line of amp-like distortion pedals.  Lets say the name is a bit like *Catch-22*.  I spent the winter pulling one apart, diving into an incredibly nerdy and wonderful thread on the guts of the thing ([go there now](https://www.freestompboxes.org/viewtopic.php?t=6571&sid=04510b69376b549d3b9b9209d3f2c42b)).  I learnt a lot - most importantly that the dozen or so pedals were basically the same, differing only in the type of filtering at each stage.  Distortion? Identical.  EQ?  Treble and bass identical.  The main thing that set them apart was the frequency of the two mid controls and the "cab sim" filter stage and even then there were only a few base templates.  Very smart and probably saved them $$$

This is what this VST does.  The Boost and EQ section have moveable mid controls for a variety of different sounds (much easier to do with DSP) and common overdrive and filtering.  Also the EQ section in the pedals is fully active and doesn't model the usual "TMB" tonestack of most amplifiers.  You know what? I like this, it means you can get way more sounds out of the thing while still setting it up to sounds like your favourite tweed 1959 or whatever it is.  The "cab sim" section is just a lowpass filter to round things off - there are so many EQ controls that I doubt you really need a cab sim after this.  If you insist, the cab knob fully to the right is set at 6000hz which is way higher than a normal cab sim filter so your fave IR VST will sit very nicely after this.  I've tried it with Reaper's bog standard cab modeller at it sound areet.

The overdrive itself is, as mentioned above, based on Will Pirkle's modelling of class A triode amplifiers and class B pentode power amps.  It is crazy reading and, honestly, mostly went over my head.  I downloaded his code, tinkered and simplified a bit and it sounded great.  The LESS/MORE settings change the 'saturation' parameter without changing any of the architecture (like the amount of triodes).

## SOURCE FILES
Have at it if you want to mess around.  The filters are ripe for modding as they might not fit your needs!  Also I'm not a programmer so I get that my code probably looks like crap.  I'd like to get better and start doing some smarter stuff but nothing would ever get done if we wait for perfection - the program works fine as is.  I don't care, code-bros log off.

## Changelog - updated 07/05/2025
- altered gain structure.  Previously I was using three (!) different algorithms and that felt excessive so its only one now.
- option to switch off the "compression" function
- Some proper anti-aliasing.  4x oversampling and some aggressive filter.  Sounds areet.  Still fiddling with this so might change later.
- volume differences fixed! All channels should be in the same ballpark.
- more active cab filter - does more, same great price.

