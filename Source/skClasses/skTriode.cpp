/*
  ==============================================================================

    skTriode.cpp
    Created: 15 Apr 2025 3:22:07pm
    Author:  jwh93

  ==============================================================================
*/

#include "skTriode.h"

//in Pirkle's original there are functions to mess with these
//but I prefer to keep them constant
SKTriode::SKTriode()
{
    params.lfShelfFreq = 100.0;
    params.lfShelfGain = 0.0;

    params.dcBlockFreq = 10.0;

    params.millerFreq = 10000.0;

    params.integratorFc = 5.0;

    params.clipPointPositive = 4.0;
    params.clipPointNegative = -1.5;
    params.gridThresh = 1.5;

    params.dcShiftCoeff = 1.0;
    params.waveSaturation = 1.0;

    params.inputGain = 1.5;
    params.outputGain = 1.0;
    params.amount = 1.0;
}

void SKTriode::setGain(double amt)
{
    params.amount = amt;
}

void SKTriode::setSaturation(bool amount)
{
    if (amount)
    {
        params.waveSaturation = 5.0;
    }

    else
    {
        params.waveSaturation = 1.0;
    }
}

void SKTriode::reset(double Fs)
{
    sampleRate = Fs;

    upperBandwidthFilter->calc(params.millerFreq, 1.0, 0.707, Fs);
    lowShelvingFilter->calc(params.lfShelfFreq, params.lfShelfGain, 1.0, Fs);

    lossyIntegrator[0].reset(Fs);
    lossyIntegrator[1].reset(Fs);

    ZVAFilterParameters params = lossyIntegrator[0].getParameters();
    params.filterAlgorithm = vaFilterAlgorithm::kLPF1;
    params.fc = 5.0;
    lossyIntegrator[0].setParameters(params);
    lossyIntegrator[1].setParameters(params);
}

juce::dsp::AudioBlock<float> SKTriode::clipBlock(juce::dsp::AudioBlock<float> block, int numChannels)
{
    for (int sample = 0; sample < block.getNumSamples(); sample++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            float* chunk = block.getChannelPointer(channel);

            chunk[sample] *= params.amount;

            chunk[sample] *= params.inputGain;

            chunk[sample] = gridConduction(chunk[sample], params.gridThresh);

            double dcOffset = lossyIntegrator[0].processAudioSample(chunk[sample]);

            dcOffset = fmin(dcOffset, 0.0);

            chunk[sample] = valveEmulation(chunk[sample],
                                           params.waveSaturation,
                                           params.gridThresh,
                                           dcOffset * params.dcShiftCoeff,
                                           params.clipPointPositive,
                                           params.clipPointNegative);

            //chunk[sample] = lowShelvingFilter->process(chunk[sample]);

            chunk[sample] = upperBandwidthFilter->process(chunk[sample]);

            chunk[sample] *= -params.outputGain;
        }
    }

    return block;

}

double SKTriode::gridConduction(double input, double thresh)
{
    if (input > 0.0)
    {
        double clipDelta = input - thresh;

        clipDelta = fmax(clipDelta, 0.0);

        double compressionFactor = 0.4473253 + 0.5451584 * exp(-0.3241584 * clipDelta);

        return compressionFactor * input;
    }
    else
    {
        return input;
    }
}

double SKTriode::valveEmulation(double input, double k, double gridConductionThresh, double dcOffset, double clipPointPos, double clipPointNeg)
{
    input += dcOffset;
    double yn;

    if (input > gridConductionThresh)
    {
        if (input > clipPointPos)
        {
            yn = clipPointPos;
        }
        else
        {
            input -= gridConductionThresh;

            if (clipPointPos > 1.0)
            {
                input /= (clipPointPos - gridConductionThresh);
            }

            yn = input * (3.0 / 2.0) * (1.0 - (input * input) / 3.0);

            yn *= (clipPointPos - gridConductionThresh);

            yn += gridConductionThresh;
        }
    }

    else if(input > 0.0)
    {
        yn = input;
    }

    else
    {
        if (input < clipPointNeg)
        {
            yn = clipPointNeg;
        }

        else
        {
            if (clipPointNeg < -1.0)
            {
                input /= fabs(clipPointNeg);
            }

            yn = tanh(k * input) / tanh(k);

            yn *= fabs(clipPointNeg);
        }
    }

    return yn;
}

///////////////////////////////////////////////

SKPowerAmp::SKPowerAmp()
{
    params.inputGain = 0.25;
    params.outputGain = 0.53;

    params.fixedBiasVoltage = -1.5;
    params.clipPointPositive = 1.5;

    params.dcShiftCoeff = 0.5;
    params.waveSaturation = 1.2;

    //waveshaper

    params.asymWaveShaper_g = 1.70;
    params.asymWaveShaper_Lp = 23.6;
    params.asymWaveShaper_Ln = 0.5;

    params.symWaveShaper_g = 4.0;
    params.symWaveShaper_LpLn = 1.01;

    sampleRate = 44100.0;
    
}

void SKPowerAmp::reset(double Fs)
{
    sampleRate = Fs;

    lossyIntegrator[0].reset(Fs);
    lossyIntegrator[1].reset(Fs);

    ZVAFilterParameters params = lossyIntegrator[0].getParameters();
    params.filterAlgorithm = vaFilterAlgorithm::kLPF1;
    params.fc = 5.0;
    lossyIntegrator[0].setParameters(params);
    lossyIntegrator[1].setParameters(params);
}

double SKPowerAmp::gridConduction(double input, double thresh)
{
    if (input > 0.0)
    {
        double clipDelta = input - thresh;

        clipDelta = fmax(clipDelta, 0.0);

        double compressionFactor = 0.4473253 + 0.5451584 * exp(-0.3241584 * clipDelta);

        return compressionFactor * input;
    }
    else
    {
        return input;
    }
}

double SKPowerAmp::doPolettiWaveShaper(double input, double g, double Ln, double Lp)
{
    double out = 0.0;

    if (input <= 0)
    {
        out = (g * input) / (1.0 - ((g * input) / Ln));
    } 

    else
    {
        out = (g * input) / (1.0 + ((g * input) / Lp));
    }

    return out;
}

juce::dsp::AudioBlock<float> SKPowerAmp::processBlock(juce::dsp::AudioBlock<float> block, int numChannels)
{
    for (int sample = 0; sample < block.getNumSamples(); sample++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            float* chunk = block.getChannelPointer(channel);
            double out = 0.0;

            chunk[sample] *= params.inputGain;

            double out_pos = doPolettiWaveShaper(chunk[sample],
                                                 params.asymWaveShaper_g,
                                                 params.asymWaveShaper_Lp,
                                                 params.asymWaveShaper_Ln);

            double out_neg = doPolettiWaveShaper(chunk[sample],
                                                 params.asymWaveShaper_g,
                                                 params.asymWaveShaper_Lp,
                                                 params.asymWaveShaper_Ln);

            out_pos = doPolettiWaveShaper(out_pos,
                                                 params.symWaveShaper_g,
                                                 params.symWaveShaper_LpLn,
                                                 params.symWaveShaper_LpLn);

            out_neg = doPolettiWaveShaper(out_neg,
                                                 params.symWaveShaper_g,
                                                 params.symWaveShaper_LpLn,
                                                 params.symWaveShaper_LpLn);

            out = out_pos + out_neg;
            chunk[sample] = out;


            //PIRKLE POWER AMP
           /* double in_pos = chunk[sample];
            double in_neg = -chunk[sample];

            in_pos = gridConduction(in_pos, 0.0);
            in_neg = gridConduction(in_neg, 0.0);

            double dcOffsetPos = lossyIntegrator[0].processAudioSample(in_pos);
            double dcOffsetNeg = lossyIntegrator[1].processAudioSample(in_neg);

            dcOffsetPos = fmin(dcOffsetPos, 0.0);
            dcOffsetNeg = fmin(dcOffsetNeg, 0.0);

            double out_pos = doPirkleWaveshaper(in_pos,
                params.waveSaturation,
                params.fixedBiasVoltage,
                dcOffsetPos * params.dcShiftCoeff);

            double out_neg = doPirkleWaveshaper(in_neg,
                params.waveSaturation,
                params.fixedBiasVoltage,
                dcOffsetPos * params.dcShiftCoeff);

            chunk[sample] = out_pos - out_neg;*/

            chunk[sample] *= params.outputGain;
        }
    }

    return block;
}

double SKPowerAmp::doPirkleWaveshaper(double input, double g, double fixedDC, double variableDC)
{
    input += fixedDC;
    input += variableDC;

    double out = 1.5 * atan(g * input) / atan(g);

    return out;
}


