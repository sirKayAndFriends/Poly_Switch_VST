/*
  ==============================================================================

    pirkleZVA.h
    Created: 15 Apr 2025 5:13:15pm
    Author:  jwh93

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "math.h"

enum class vaFilterAlgorithm {
    kLPF1, kHPF1, kAPF1, kSVF_LP, kSVF_HP, kSVF_BP, kSVF_BS
};

struct ZVAFilterParameters
{
	ZVAFilterParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	ZVAFilterParameters& operator=(const ZVAFilterParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		filterAlgorithm = params.filterAlgorithm;
		fc = params.fc;
		Q = params.Q;
		filterOutputGain_dB = params.filterOutputGain_dB;
		enableGainComp = params.enableGainComp;
		matchAnalogNyquistLPF = params.matchAnalogNyquistLPF;
		selfOscillate = params.selfOscillate;
		enableNLP = params.enableNLP;
		return *this;
	}

	// --- individual parameters
	vaFilterAlgorithm filterAlgorithm = vaFilterAlgorithm::kSVF_LP;	///< va filter algorithm
	double fc = 1000.0;						///< va filter fc
	double Q = 0.707;						///< va filter Q
	double filterOutputGain_dB = 0.0;		///< va filter gain (normally unused)
	bool enableGainComp = false;			///< enable gain compensation (see book)
	bool matchAnalogNyquistLPF = false;		///< match analog gain at Nyquist
	bool selfOscillate = false;				///< enable selfOscillation
	bool enableNLP = false;					///< enable non linear processing (use oversampling for best results)
};

class ZVAFilter
{
public:
	ZVAFilter();	/* C-TOR */
	~ZVAFilter();		/* D-TOR */

	/** reset members to initialized state */
	virtual bool reset(double _sampleRate);

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return ZVAFilterParameters custom data structure
	*/
	ZVAFilterParameters getParameters();

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param ZVAFilterParameters custom data structure
	*/
	void setParameters(const ZVAFilterParameters& params);

	/** return false: this object only processes samples */
	virtual bool canProcessAudioFrame();

	/** process input x(n) through the VA filter to produce return value y(n) */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn);
	

	/** recalculate the filter coefficients*/
	void calculateFilterCoeffs();

	/** set beta value, for filters that aggregate 1st order VA sections*/
	void setBeta(double _beta);

	/** get beta value,not used in book projects; for future use*/
	double getBeta();

protected:
	ZVAFilterParameters zvaFilterParameters;	///< object parameters
	double sampleRate = 44100.0;				///< current sample rate

	// --- state storage
	double integrator_z[2];						///< state variables

	// --- filter coefficients
	double alpha0 = 0.0;		///< input scalar, correct delay-free loop
	double alpha = 0.0;			///< alpha is (wcT/2)
	double rho = 0.0;			///< p = 2R + g (feedback)

	double beta = 0.0;			///< beta value, not used

	// --- for analog Nyquist matching
	double analogMatchSigma = 0.0; ///< analog matching Sigma value (see book)

};
