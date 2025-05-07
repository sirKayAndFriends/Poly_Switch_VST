/*
  ==============================================================================

    skFilters.cpp
    Created: 9 Apr 2025 2:16:53pm
    Author:  jwh93

  ==============================================================================
*/

//https://www.gcradix.org/post/introduction-the-biquad-filter


#include "skFilters.h"
#include "math.h"

skFilters::skFilters(int type)
{
    b0 = 1.0f;
    b1 = b2 = 0.0f;
    a0 = a1 = a2 = 0.0f;
    z1 = z2 = 0.0f;

    filterType = type;
}

void skFilters::calc(double freq, double gain, double q, double Fs)
{
    const double v = pow(10.0, (fabs(gain) / 20.0));
    const double k = tan(juce::MathConstants<float>::pi * (freq / Fs));
    const double kSquare = k * k;

    switch (filterType)
    {

        case lowpass: 
        {
            const double norm = 1.0 / (1.0 + k / q + kSquare);
            a0 = kSquare * norm;
            a1 = 2.0 * a0;
            a2 = a0;
            b1 = (2.0 * (kSquare - 1.0) * norm);
            b2 = ((1.0 - k / q + kSquare) * norm);

            break;
        }

        case highpass:
        {
            const double norm = 1.0 / (1.0 + k / q + kSquare);
            a0 = 1.0 * norm;
            a1 = -2.0 * a0;
            a2 = a0;
            b1 = (2.0 * (kSquare - 1.0) * norm);
            b2 = norm * (1.0 - k / q + kSquare);

            break;
        }

        case peak:
        {
            const double x1 = k / q;
            const double x2 = k * v / q;

            if (gain >= 0)
            {
                const double norm = 1.0/ (1.0 + x1 + kSquare);
                a0 = static_cast<float>((1.0 + x2 + kSquare) * norm);
                a1 = static_cast<float>(2.0 * (kSquare - 1) * norm);
                a2 = static_cast<float>((1.0 - x2 + kSquare) * norm);
                b1 = a1;
                b2 = static_cast<float>((1.0 - x1 + kSquare) * norm);
            }
            else
            {
                double const norm = 1 / (1 + x2 + kSquare);
                a0 = static_cast<float>((1 + x1 + kSquare) * norm);
                a1 = static_cast<float>((2 * (kSquare - 1)) * norm);
                a2 = static_cast<float>((1 - x1 + kSquare) * norm);
                b1 = a1;
                b2 = static_cast<float>((1 - x2 + kSquare) * norm);
            }

            break;
        }

        case highshelf:
        {
            const double x1 = sqrt(2.0 * v) * k;
            const double x2 = sqrt(2.0) * k;

            if (gain >= 0)
            {
                double const norm = 1 / (1 + x2 + kSquare);
               a0 = static_cast<float>((v + x1 + kSquare) * norm);
               a1 = static_cast<float>(2.0 * (kSquare - v) * norm);
               a2 = static_cast<float>((v - x1 + kSquare) * norm);
               b1 = static_cast<float>(2.0 * (kSquare - 1) * norm);
               b2 = static_cast<float>((1.0 - x2 + kSquare) * norm);
            }
            else
            {
                double const norm = 1.0 / (v + x1 + kSquare);
               a0 = static_cast<float>((1.0 + x1 + kSquare) * norm);
               a1 = static_cast<float>(2.0 * (kSquare - 1.0) * norm);
               a2 = static_cast<float>((1.0 - x1 + kSquare) * norm);
               b1 = static_cast<float>(2.0 * (kSquare - v) * norm);
               b2 = static_cast<float>((v - x1 + kSquare) * norm);
            }

            break;
        }

        case lowshelf:
        {
            const double x3 = sqrt(2.0) * k;
            const double x1 = sqrt(2.0 * v) * k;
            const double x2 = v * kSquare;

            if (gain >= 0)
            {
                double const norm = 1.0 / (1.0 + x3 + kSquare);
                a0 = static_cast<float>((1.0 + x1 + x2) * norm);
                a1 = static_cast<float>(2.0 * (x2 - 1.0) * norm);
                a2 = static_cast<float>((1.0 - x1 + x2) * norm);
                b1 = static_cast<float>(2.0 * (kSquare - 1.0) * norm);
                b2 = static_cast<float>((1.0 - x3 + kSquare) * norm);
            }
            else
            {
                double const norm = 1.0 / (1.0 + x1 + x2);
                a0 = static_cast<float>((1.0 + x3 + kSquare) * norm);
                a1 = static_cast<float>(2.0 * (kSquare - 1.0) * norm);
                a2 = static_cast<float>((1.0 - x3 + kSquare) * norm);
                b1 = static_cast<float>(2.0 * (x2 - 1.0) * norm);
                b2 = static_cast<float>((1.0 - x1 + x2) * norm);
            }

            break;
        }
    }
}

float skFilters::process(float input)
{
    float output = input * a0 + z1;
    z1 = input * a1 + z2 - b1 * output;
    z2 = input * a2 - b2 * output;

    return output;
}

juce::dsp::AudioBlock<float> skFilters::processBlock(juce::dsp::AudioBlock<float> block)
{
    for (int sample = 0; sample < block.getNumSamples(); sample++)
    {
        for (int channel = 0; channel < block.getNumChannels(); channel++)
        {
            float* inputWritePointer = block.getChannelPointer(channel);
            float inputSample = block.getSample(channel, sample);
            float outputSample = inputSample * a0 + z1;

            inputWritePointer[sample] = outputSample;
            z1 = inputSample * a1 + z2 - b1 * outputSample;
            z2 = inputSample * a2 - b2 * outputSample;
        }
    }

    return block;
}
