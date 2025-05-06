/*
  ==============================================================================

    distortion.cpp
    Created: 8 Apr 2025 6:29:11pm
    Author:  jwh93

  ==============================================================================
*/

#include "skDistortion.h"
#include "JuceHeader.h"

SKDistortion::SKDistortion(int setting)
{
    forwardClip = 1.0;
    reverseClip = -1.0;
    gain = 0.5;

    clipMode = setting;
}

void SKDistortion::setGain(double amount)
{
    gain = amount;
}

void SKDistortion::setMode(int setting)
{
    clipMode = setting;
}

void SKDistortion::setClipThreshold(double forward, double reverse)
{
    forwardClip = forward;
    reverseClip = reverse;
}

juce::dsp::AudioBlock<float> SKDistortion::clipBlock(juce::dsp::AudioBlock<float> block, int numChannels)
{

    switch (clipMode)
    {
    case soft_clip:
    {
        for (int sample = 0; sample < block.getNumSamples(); sample++)
        {
            for (int channel = 0; channel < numChannels; channel++)
            {
                float* data = block.getChannelPointer(channel);

                data[sample] *= gain;

                if (data[sample] < 0.0f)
                {
                    data[sample] = -1.0f + exp(data[sample]);
                }
                else if (data[sample] > 0.0f)
                {
                    data[sample] = 1.0f - exp(-data[sample]);
                }
                else if (data[sample] == 0.0f)
                {
                    data[sample] = 0.0f;
                }
            }
        }

        return block;
        break;
    }

    case hard_clip:
    {
        for (int sample = 0; sample < block.getNumSamples(); sample++)
        {
            for (int channel = 0; channel < numChannels; channel++)
            {
                float* data = block.getChannelPointer(channel);
                data[sample] *= gain;

                if (data[sample] >= forwardClip)
                {
                    data[sample] = forwardClip;
                }
                else if (data[sample] <= reverseClip)
                {
                    data[sample] = reverseClip;
                }

                else {
                    data[sample] = data[sample];
                }
            }
        }

        return block;
        break;
    }

    case tangent:
    {
        for (int sample = 0; sample < block.getNumSamples(); sample++)
        {
            for (int channel = 0; channel < numChannels; channel++)
            {
                float* data = block.getChannelPointer(channel);

                data[sample] *= gain * 2.0;

                data[sample] = tanh(data[sample]);
                
                data[sample] *= 1.5f;
            }
        }     

        return block;
        break;
    }

    case triode:
    {
        for (int sample = 0; sample < block.getNumSamples(); sample++)
        {
            for (int channel = 0; channel < numChannels; channel++)
            {
                float* data = block.getChannelPointer(channel);

                data[sample] *= (gain * 10.0);

                data[sample] = triodeClip(data[sample]);

                data[sample] = triodeClip(data[sample]);

                data[sample] *= 5.0f;
            }
        }

        return block;
        break;
    }

    default:
    {
        return block;
    }
    }
}

float SKDistortion::triodeClip(float input)
{
    if (input <= -0.98338) {
        input = -0.32623;

    }
    else if (input > -0.98338 && input <= -0.50698) {
        input = ((7.8101 * pow(10, -2)) * pow(input, 3)) + ((2.3041 * pow(10, -1)) * pow(input, 2)) + ((2.9710 * pow(10, -1)) * input) + (-1.8261 * pow(10, -1));

    }
    else if (input > -0.50698 && input <= -0.20759) {
        input = ((4.2263 * pow(10, -1)) * pow(input, 3)) + ((7.5441 * pow(10, -1)) * pow(input, 2)) + ((5.6276 * pow(10, -1)) * input) + (-1.3772 * pow(10, -1));

    }
    else if (input > -0.20759 && input <= -0.00212) {
        input = ((9.2483 * pow(10, -1)) * pow(input, 3)) + (1.0672 * pow(input, 2)) + ((6.2768 * pow(10, -1)) * input) + (-1.3322 * pow(10, -1));

    }
    else if (input > -0.00212 && input <= 0.20041) {
        input = ((-7.8522 * pow(10, -1)) * pow(input, 3)) + (1.0563 * pow(input, 2)) + ((6.2766 * pow(10, -1)) * input) + (-1.3322 * pow(10, -1));

    }
    else if (input > 0.20041 && input <= 0.50062) {
        input = (-1.0856 * pow(input, 3)) + (1.2369 * pow(input, 2)) + ((5.9147 * pow(10, -1)) * input) + (-1.3081 * pow(10, -1));

    }
    else if (input > 0.50062 && input <= 0.89961) {
        input = ((3.2873 * pow(10, -1)) * pow(input, 3)) + ((-8.8720 * pow(10, -1)) * pow(input, 2)) + (1.6548 * input) + (-3.0825 * pow(10, -1));

    }
    else if (input > 0.89961) {
        input = 0.70177;
    };

    return input * -1.0;
}



