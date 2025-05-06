/*
  ==============================================================================

    pirkleZVA.cpp
    Created: 15 Apr 2025 5:13:04pm
    Author:  jwh93

  ==============================================================================
*/

#include "pirkleZVA.h"

	ZVAFilter::ZVAFilter() {}		/* C-TOR */
	ZVAFilter::~ZVAFilter() {}		/* D-TOR */

	/** reset members to initialized state */
	bool ZVAFilter::reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		integrator_z[0] = 0.0;
		integrator_z[1] = 0.0;

		return true;
	}

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return ZVAFilterParameters custom data structure
	*/
	ZVAFilterParameters ZVAFilter::getParameters()
	{
		return zvaFilterParameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param ZVAFilterParameters custom data structure
	*/
	void ZVAFilter::setParameters(const ZVAFilterParameters& params)
	{
		if (params.fc != zvaFilterParameters.fc ||
			params.Q != zvaFilterParameters.Q ||
			params.selfOscillate != zvaFilterParameters.selfOscillate ||
			params.matchAnalogNyquistLPF != zvaFilterParameters.matchAnalogNyquistLPF)
		{
			zvaFilterParameters = params;
			calculateFilterCoeffs();
		}
		else
			zvaFilterParameters = params;
	}

	/** return false: this object only processes samples */
	bool ZVAFilter::canProcessAudioFrame() { return false; }

	/** process input x(n) through the VA filter to produce return value y(n) */
	/**
	\param xn input
	\return the processed sample
	*/
	double ZVAFilter::processAudioSample(double xn)
	{
		// --- with gain comp enabled, we reduce the input by
		//     half the gain in dB at resonant peak
		//     NOTE: you can change that logic here!
		vaFilterAlgorithm filterAlgorithm = zvaFilterParameters.filterAlgorithm;
		bool matchAnalogNyquistLPF = zvaFilterParameters.matchAnalogNyquistLPF;

		// --- for 1st order filters:
		if (filterAlgorithm == vaFilterAlgorithm::kLPF1 ||
			filterAlgorithm == vaFilterAlgorithm::kHPF1 ||
			filterAlgorithm == vaFilterAlgorithm::kAPF1)
		{
			// --- create vn node
			double vn = (xn - integrator_z[0]) * alpha;

			// --- form LP output
			double lpf = ((xn - integrator_z[0]) * alpha) + integrator_z[0];

			// double sn = integrator_z[0];

			// --- update memory
			integrator_z[0] = vn + lpf;

			// --- form the HPF = INPUT = LPF
			double hpf = xn - lpf;

			// --- form the APF = LPF - HPF
			double apf = lpf - hpf;

			// --- set the outputs
			if (filterAlgorithm == vaFilterAlgorithm::kLPF1)
			{
				// --- this is a very close match as-is at Nyquist!
				if (matchAnalogNyquistLPF)
					return lpf + alpha * hpf;
				else
					return lpf;
			}
			else if (filterAlgorithm == vaFilterAlgorithm::kHPF1)
				return hpf;
			else if (filterAlgorithm == vaFilterAlgorithm::kAPF1)
				return apf;

			// --- unknown filter
			return xn;
		}

		// --- form the HP output first
		double hpf = alpha0 * (xn - rho * integrator_z[0] - integrator_z[1]);

		// --- BPF Out
		double bpf = alpha * hpf + integrator_z[0];

		// --- LPF Out
		double lpf = alpha * bpf + integrator_z[1];

		// --- BSF Out
		double bsf = hpf + lpf;

		// --- finite gain at Nyquist; slight error at VHF
		double sn = integrator_z[0];

		// update memory
		integrator_z[0] = alpha * hpf + bpf;
		integrator_z[1] = alpha * bpf + lpf;

		double filterOutputGain = pow(10.0, zvaFilterParameters.filterOutputGain_dB / 20.0);

		// return our selected type
		if (filterAlgorithm == vaFilterAlgorithm::kSVF_LP)
		{
			if (matchAnalogNyquistLPF)
				lpf += analogMatchSigma * (sn);
			return filterOutputGain * lpf;
		}
		else if (filterAlgorithm == vaFilterAlgorithm::kSVF_HP)
			return filterOutputGain * hpf;
		else if (filterAlgorithm == vaFilterAlgorithm::kSVF_BP)
			return filterOutputGain * bpf;
		else if (filterAlgorithm == vaFilterAlgorithm::kSVF_BS)
			return filterOutputGain * bsf;

		// --- unknown filter
		return filterOutputGain * lpf;
	}

	/** recalculate the filter coefficients*/
	void ZVAFilter::calculateFilterCoeffs()
	{
		double fc = zvaFilterParameters.fc;
		double Q = zvaFilterParameters.Q;
		vaFilterAlgorithm filterAlgorithm = zvaFilterParameters.filterAlgorithm;

		// --- normal Zavalishin SVF calculations here
		//     prewarp the cutoff- these are bilinear-transform filters
		double wd = juce::MathConstants<float>::twoPi * fc;
		double T = 1.0 / sampleRate;
		double wa = (2.0 / T) * tan(wd * T / 2.0);
		double g = wa * T / 2.0;

		// --- for 1st order filters:
		if (filterAlgorithm == vaFilterAlgorithm::kLPF1 ||
			filterAlgorithm == vaFilterAlgorithm::kHPF1 ||
			filterAlgorithm == vaFilterAlgorithm::kAPF1)
		{
			// --- calculate alpha
			alpha = g / (1.0 + g);
		}
		else // state variable variety
		{
			// --- note R is the traditional analog damping factor zeta
			double R = zvaFilterParameters.selfOscillate ? 0.0 : 1.0 / (2.0 * Q);
			alpha0 = 1.0 / (1.0 + 2.0 * R * g + g * g);
			alpha = g;
			rho = 2.0 * R + g;

			// --- sigma for analog matching version
			double f_o = (sampleRate / 2.0) / fc;
			analogMatchSigma = 1.0 / (alpha * f_o * f_o);
		}
	}

	/** set beta value, for filters that aggregate 1st order VA sections*/
	void ZVAFilter::setBeta(double _beta) { beta = _beta; }

	/** get beta value,not used in book projects; for future use*/
	double ZVAFilter::getBeta() { return beta; }