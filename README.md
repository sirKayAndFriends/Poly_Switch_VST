# PolyAmp VST AT LAST (v1.0)
A two channel amplifier plugin boasting a three band EQ with moveable mid frequency parameter, preamp boost with bass and mid control and a simple cab sim

## A Quick Introduction
### BOOST
A simple volume booster with variable "peak" frequency and bass cut.
- **Boost:** the amount of preamp volume.  Noon is flat gain, right is up to 10x volume boost and left is volume cut (0 - 1).  This means you can add "scoop" at your desired frequency as well as a boost peak
- **Peak:** the variable frequency of the boost.  From 400Hz to 2400Hz.
- **Cut:** a high pass filter.  Fully right is full bass (cutoff frequency @ 25Hz) and fully left is full cut (350Hz).

### TONE STACK
Three band EQ
- **Treble and Bass:** shelving filters for cut and boost.  Noon is once again "flat".  Treble is set at 2000Hz and Bass at 350Hz
- **Mid:** a mid boost pretty much identical to the one in the boost section (but not as powerful)
- **Voice:** the magic mid centre frequency.  Goes from 400Hz to 850Hz for a full range of amp mids sounds... you know, "American" and "British" :/

### GAIN
The gain bit
- **Gain:** amount of gain bit
- **Lo:** the lower gain channel - a simple hyperbolic tangent waveshaper
- **Hi:** high gain channel - a not-simple waveshaping function based on a transfer function i found at the [amp books](https://www.ampbooks.com/mobile/dsp/preamp/) website (thanks!)

### OUTPUT
Where the sound comes out
- **Level:** loudosity
- **Cabinet:** a low pass filter to simulate a speaker response.  There are so many EQ controls already that I think a bunch more for a cabsim would be ludicrous.  This is simply a lowpass fitler with a variable cutoff between 3000 and 6000 Hz.

## Suggestions for Getting Started
**Fender-y niceness**
- Boost @ 9 o'clock (this is a nice fender scoop but push it for a more "tweedy" sound)
- Peak @ 400 (this will give you the fendery thing with a scoop or boost, but I also like a 2400 treble boostery thing here too)
- Bass @ noon, Treble @ 3 o'clock
- Voice @ 400
- Mids sounds good anywhere - under noon = 60s, above noon = 50s
- Gain under noon
- Cabinet above noon 

**Sort of Hendrix-y Marshall-y**
- Boost @ 3 o'clock
- Peak @ 2400
- Voice @ 850
- Bass and Treble @ noon
- Mids @ 2 o'clock
- Gain @ 12 o'clock
- Cabinet @ 12 o'clock

**Big Heavy Metal Sound**
- Boost @ 3 o'clock
- Peak @ 740
- Voice @ 850
- Tone to Taste (I like mids pushed a little)
- Gain @ 2 o'clock
- Cabinet below noon

## Some Technical Bits for the Nerds
This was built in PlugData which is a fun wrapper for PureData that makes it a bit easier to use.  It also allows you to export C++ code via the HeavyCompiler which is exactly what i did.  This then interfaces in a slightly wobbly way with JUCE - a library for making audio software.  It all worked pretty well and I learned a lot!

I learned how to do the Heavy bits from [this repo](https://github.com/o-g-sus/JUCE-HEAVY) which very helpfully goes through how it all works.  My code ended up being a bit different (as there are a million ways to do even the simplest things in C) but I owe a lot to this work!

## Where Can I Find All the Stuff?
The VST3 and Plugdata files are at the top of this repo!  The source folder has the delicous c++ sauce and similar for the nerds.

The source folder holds everything you will need to view C++ code.  The important files are in the top level of this folder - HeavyFiles contains all the compiled code from the PD patch and is probably a bit esoteric.  All of the actual DSP is done in those but the "pluginProcessor.cpp" file handles the mediation.  If you want to mod this or play around you will need to build a project using the Projucer (the JUCE project manager) or CMake or something.  The JUCE library files are in there for that reason but I wouldn't play with them.

## Some Stuff for Future Versions
= MORE GAIN
- Definitely needs some better anti-aliasing stuff going on.  Heavy doesn't like some of the traditional ways of doing this in PD so... I'll have a think.
- I think the level controls and gain staging are a bit shagged so will sort this out soon.
- Maybe a mid control for the cab sim?  I'm feeling a "no" on this one...
- Is the colour scheme too jazzy?
- Some sort of peak meter?

