#include "SorolletVSTi.h"
#include "../sys/MathUtils.h"
//#include "gmnames.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

SorolletVSTi::SorolletVSTi(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, kNumPrograms, kNumParams)
{
	if (audioMaster)
	{
		// TODO: load default / more varied programs!

		// initialize programs
		programs = new SorolletVSTiProgram[kNumPrograms];
		for (VstInt32 i = 0; i < 16; i++)
			channelPrograms[i] = i;

		mSorolletVoice = new SorolletVoice();

		if (programs)
		{
			setProgram(0);
		}

		setNumInputs(0); // no inputs
		setNumOutputs(kNumOutputs); // 2 outputs
		canProcessReplacing();
		isSynth();
		setUniqueID('so02');

		currentEvent = EVENT_NULL;

		initProcess();
		suspend();
	}
}

SorolletVSTi::~SorolletVSTi()
{
	delete mSorolletVoice;
}

void SorolletVSTi::suspend()
{
	AudioEffectX::suspend();
	mSorolletVoice->resetSamplePosition();
}

void SorolletVSTi::setProgram(VstInt32 program)
{
	if (program < 0 || program >= kNumPrograms)
		return;
	SorolletVSTiProgram *ap = &programs[program];
	curProgram = program;

	fVolume1 = ap->fVolume1;
	mSorolletVoice->setVolume1(fVolume1);

	fOctave1 = ap->fOctave1;
	mSorolletVoice->setOctave1(floatToOctave(fOctave1));

	fWave1 = ap->fWave1;
	mSorolletVoice->setWave1(floatToWave(fWave1));

	fPhase1 = ap->fPhase1;
	mSorolletVoice->setPhase1(fPhase1);

	fVolume2 = ap->fVolume2;
	mSorolletVoice->setVolume2(fVolume2);

	fOctave2 = ap->fOctave2;
	mSorolletVoice->setOctave2(floatToOctave(fOctave2));

	fWave2 = ap->fWave2;
	mSorolletVoice->setWave2(floatToWave(fWave2));

	fPhase2 = ap->fPhase2;
	mSorolletVoice->setPhase2(fPhase2);

	fWaveMixType = ap->fWaveMixType;
	mSorolletVoice->setWaveMixType(floatToWaveMixType(fWaveMixType));

	fNoiseAmount = ap->fNoiseAmount;
	mSorolletVoice->setNoiseAmount(fNoiseAmount);

	fNoiseMixType = ap->fNoiseMixType;
	mSorolletVoice->setNoiseMixType(floatToNoiseMixType(fNoiseMixType));

	// Envelopes

	fAmpTimeScale = ap->fAmpTimeScale;
	mSorolletVoice->getAmpADSR()->setTimeScale(floatToEnvelopeScale(fAmpTimeScale));

	fAmpAttack = ap->fAmpAttack;
	mSorolletVoice->getAmpADSR()->setAttack(fAmpAttack);

	fAmpDecay = ap->fAmpDecay;
	mSorolletVoice->getAmpADSR()->setDecay(fAmpDecay);

	fAmpSustain = ap->fAmpSustain;
	mSorolletVoice->getAmpADSR()->setSustainLevel(fAmpSustain);

	fAmpRelease = ap->fAmpRelease;
	mSorolletVoice->getAmpADSR()->setRelease(fAmpRelease);

	fAmpMinValue = ap->fAmpMinValue;
	mSorolletVoice->getAmpADSR()->setMinValue(fAmpMinValue);

	fAmpMaxValue = ap->fAmpMaxValue;
	mSorolletVoice->getAmpADSR()->setMaxValue(fAmpMaxValue);

	// ~~ pitch

	fPitchTimeScale = ap->fPitchTimeScale;
	mSorolletVoice->getPitchADSR()->setTimeScale(floatToEnvelopeScale(fPitchTimeScale));

	fPitchAttack = ap->fPitchAttack;
	mSorolletVoice->getPitchADSR()->setAttack(fPitchAttack);

	fPitchDecay = ap->fPitchDecay;
	mSorolletVoice->getPitchADSR()->setDecay(fPitchDecay);

	fPitchSustain = ap->fPitchSustain;
	mSorolletVoice->getPitchADSR()->setSustainLevel(fPitchSustain);

	fPitchRelease = ap->fPitchRelease;
	mSorolletVoice->getPitchADSR()->setRelease(fPitchRelease);

	fPitchMinValue = ap->fPitchMinValue;
	mSorolletVoice->getPitchADSR()->setMinValue(fPitchMinValue);

	fPitchMaxValue = ap->fPitchMaxValue;
	mSorolletVoice->getPitchADSR()->setMaxValue(fPitchMaxValue);

	// ~~ filter

	fFilterType = ap->fFilterType;
	mSorolletVoice->setFilterType(floatToFilterType(fFilterType));

	fFilterFrequency = ap->fFilterFrequency;
	mSorolletVoice->setFilterFrequency(fFilterFrequency);

	fFilterResonance = ap->fFilterResonance;
	mSorolletVoice->setFilterResonance(fFilterResonance);

	// ~~~

	fSaturate = ap->fSaturate;
	mSorolletVoice->setSaturate(fSaturate);

	// ~~~

	fEQActive = ap->fEQActive;
	mSorolletVoice->setEQActive(fEQActive > 0.5f);

	fEQLowFrequency = ap->fEQLowFrequency;
	mSorolletVoice->setEQLowFrequency(fEQLowFrequency);

	fEQHighFrequency = ap->fEQHighFrequency;
	mSorolletVoice->setEQHighFrequency(fEQHighFrequency);

	fEQLowGain = ap->fEQLowGain;
	mSorolletVoice->setEQLowGain(fEQLowGain);

	fEQMidGain = ap->fEQMidGain;
	mSorolletVoice->setEQMidGain(fEQMidGain);

	fEQHiGain = ap->fEQHiGain;
	mSorolletVoice->setEQHiGain(fEQHiGain);

}

void SorolletVSTi::setProgramName(char* name)
{
	vst_strncpy(programs[curProgram].name, name, kVstMaxProgNameLen);
}

void SorolletVSTi::getProgramName(char* name)
{
	vst_strncpy(name, programs[curProgram].name, kVstMaxProgNameLen);
}

// In the external editor, values are labelled to the right as (display) (label)

void SorolletVSTi::getParameterLabel(VstInt32 index, char* label)
{
	switch (index)
	{
		case PARAM_VOLUME1: vst_strncpy(label, "dB", kVstMaxParamStrLen);
			break;
		case PARAM_OCTAVE1: vst_strncpy(label, "Octave", kVstMaxParamStrLen);
			break;
		case PARAM_WAVE1: vst_strncpy(label, "Wave", kVstMaxParamStrLen);
			break;
		case PARAM_PHASE1: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_VOLUME2: vst_strncpy(label, "dB", kVstMaxParamStrLen);
			break;
		case PARAM_OCTAVE2: vst_strncpy(label, "Octave", kVstMaxParamStrLen);
			break;
		case PARAM_WAVE2: vst_strncpy(label, "Wave", kVstMaxParamStrLen);
			break;
		case PARAM_PHASE2: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_WAVE_MIX_TYPE: vst_strncpy(label, "Operator", kVstMaxParamStrLen);
			break;
		case PARAM_NOISE_AMOUNT: vst_strncpy(label, "%", kVstMaxParamStrLen);
			break;
		case PARAM_NOISE_MIX_TYPE: vst_strncpy(label, "Operator", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_ATTACK: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_DECAY: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_SUSTAIN: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_RELEASE: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_TIME_SCALE: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_MIN_VALUE: vst_strncpy(label, "dB", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_MAX_VALUE: vst_strncpy(label, "dB", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_ATTACK: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_DECAY: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_SUSTAIN: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_RELEASE: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_TIME_SCALE: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_MIN_VALUE: vst_strncpy(label, "notes", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_MAX_VALUE: vst_strncpy(label, "notes", kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_TYPE: vst_strncpy(label, "type", kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_FREQUENCY: vst_strncpy(label, "Hz", kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_RESONANCE: vst_strncpy(label, "Hz", kVstMaxParamStrLen);
			break;
		case PARAM_SATURATE: vst_strncpy(label, "amount", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_ACTIVE: vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_LOW_FREQUENCY:
		case PARAM_EQ_HIGH_FREQUENCY: vst_strncpy(label, "Hz", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_LOW_GAIN:
		case PARAM_EQ_MID_GAIN:
		case PARAM_EQ_HI_GAIN:
			vst_strncpy(label, "", kVstMaxParamStrLen);
			break;
	}
}

void SorolletVSTi::getParameterDisplay(VstInt32 index, char* text)
{
	text[0] = 0;
	switch (index)
	{
		case PARAM_VOLUME1: dB2string(fVolume1, text, kVstMaxParamStrLen);
			break;
		case PARAM_OCTAVE1: int2string(floatToOctave(fOctave1), text, kVstMaxParamStrLen);
			break;
		case PARAM_WAVE1: floatToWaveName(fWave1, text);
			break;
		case PARAM_PHASE1: float2string(fPhase1, text, kVstMaxParamStrLen);
			break;
		case PARAM_VOLUME2: dB2string(fVolume2, text, kVstMaxParamStrLen);
			break;
		case PARAM_OCTAVE2: int2string(floatToOctave(fOctave2), text, kVstMaxParamStrLen);
			break;
		case PARAM_WAVE2: floatToWaveName(fWave2, text);
			break;
		case PARAM_PHASE2: float2string(fPhase2, text, kVstMaxParamStrLen);
			break;
		case PARAM_WAVE_MIX_TYPE: floatToWaveMixTypeName(fWaveMixType, text);
			break;
		case PARAM_NOISE_AMOUNT: float2string(fNoiseAmount, text, kVstMaxParamStrLen);
			break;
		case PARAM_NOISE_MIX_TYPE: floatToNoiseMixTypeName(fNoiseMixType, text);
			break;
		case PARAM_AMP_ATTACK: formatTime(mSorolletVoice->getAmpADSR()->getAttackTime(), text);
			break;
		case PARAM_AMP_DECAY: formatTime(mSorolletVoice->getAmpADSR()->getDecayTime(), text);
			break;
		case PARAM_AMP_SUSTAIN: float2string(fAmpSustain, text, kVstMaxParamStrLen);
			break;
		case PARAM_AMP_RELEASE: formatTime(mSorolletVoice->getAmpADSR()->getReleaseTime(), text);
			break;
		case PARAM_AMP_TIME_SCALE: float2string(floatToEnvelopeScale(fAmpTimeScale), text, kVstMaxParamStrLen);
			break;
		case PARAM_AMP_MIN_VALUE: float2string(mSorolletVoice->getAmpADSR()->getDisplayMinValue(), text, kVstMaxParamStrLen);
			break;
		case PARAM_AMP_MAX_VALUE: float2string(mSorolletVoice->getAmpADSR()->getDisplayMaxValue(), text, kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_ATTACK: formatTime(mSorolletVoice->getPitchADSR()->getAttackTime(), text);
			break;
		case PARAM_PITCH_DECAY: formatTime(mSorolletVoice->getPitchADSR()->getDecayTime(), text);
			break;
		case PARAM_PITCH_SUSTAIN: float2string(fPitchSustain, text, kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_RELEASE: formatTime(mSorolletVoice->getPitchADSR()->getReleaseTime(), text);
			break;
		case PARAM_PITCH_TIME_SCALE: float2string(floatToEnvelopeScale(fPitchTimeScale), text, kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_MIN_VALUE: float2string(mSorolletVoice->getPitchADSR()->getDisplayMinValue(), text, kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_MAX_VALUE: float2string(mSorolletVoice->getPitchADSR()->getDisplayMaxValue(), text, kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_TYPE: floatToFilterTypeName(fFilterType, text);
			break;
		case PARAM_FILTER_FREQUENCY: dB2string(mSorolletVoice->getFilterFrequencyDisplay(), text, kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_RESONANCE: dB2string(mSorolletVoice->getFilterResonanceDisplay(), text, kVstMaxParamStrLen);
			break;
		case PARAM_SATURATE: float2string(fSaturate, text, kVstMaxParamStrLen);
			break;
		case PARAM_EQ_ACTIVE:
			if (fEQActive > 0.5f)
			{
				vst_strncpy(text, "on", kVstMaxParamStrLen);
			}
			else
			{
				vst_strncpy(text, "off", kVstMaxParamStrLen);
			}
			break;
		case PARAM_EQ_LOW_FREQUENCY: float2string(mSorolletVoice->getEQLowFrequencyDisplay(), text, kVstMaxParamStrLen);
			break;
		case PARAM_EQ_HIGH_FREQUENCY: float2string(mSorolletVoice->getEQHighFrequencyDisplay(), text, kVstMaxParamStrLen);
			break;
		case PARAM_EQ_LOW_GAIN: float2string(mSorolletVoice->getEQLowGainDisplay(), text, kVstMaxParamStrLen);
			break;
		case PARAM_EQ_MID_GAIN: float2string(mSorolletVoice->getEQMidGainDisplay(), text, kVstMaxParamStrLen);
			break;
		case PARAM_EQ_HI_GAIN: float2string(mSorolletVoice->getEQHiGainDisplay(), text, kVstMaxParamStrLen);
			break;

	}
}

// also used for labelling the inputs in automation devices

void SorolletVSTi::getParameterName(VstInt32 index, char* label)
{
	switch (index)
	{
		case PARAM_VOLUME1: vst_strncpy(label, "Volume 1", kVstMaxParamStrLen);
			break;
		case PARAM_OCTAVE1: vst_strncpy(label, "Octave 1", kVstMaxParamStrLen);
			break;
		case PARAM_WAVE1: vst_strncpy(label, "Wave 1", kVstMaxParamStrLen);
			break;
		case PARAM_PHASE1: vst_strncpy(label, "Phase 1", kVstMaxParamStrLen);
			break;
		case PARAM_VOLUME2: vst_strncpy(label, "Volume 2", kVstMaxParamStrLen);
			break;
		case PARAM_OCTAVE2: vst_strncpy(label, "Octave 2", kVstMaxParamStrLen);
			break;
		case PARAM_WAVE2: vst_strncpy(label, "Wave 2", kVstMaxParamStrLen);
			break;
		case PARAM_PHASE2: vst_strncpy(label, "Phase 2", kVstMaxParamStrLen);
			break;
		case PARAM_WAVE_MIX_TYPE: vst_strncpy(label, "Wave Mix Type", kVstMaxParamStrLen);
			break;
		case PARAM_NOISE_AMOUNT: vst_strncpy(label, "Noise amount", kVstMaxParamStrLen);
			break;
		case PARAM_NOISE_MIX_TYPE: vst_strncpy(label, "Noise Mix Type", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_ATTACK: vst_strncpy(label, "Amp ATCK", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_DECAY: vst_strncpy(label, "Amp DECY", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_SUSTAIN: vst_strncpy(label, "Amp SUST", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_RELEASE: vst_strncpy(label, "Amp REL", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_TIME_SCALE: vst_strncpy(label, "Amp TIMES", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_MIN_VALUE: vst_strncpy(label, "Amp MinV", kVstMaxParamStrLen);
			break;
		case PARAM_AMP_MAX_VALUE: vst_strncpy(label, "Amp MaxV", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_ATTACK: vst_strncpy(label, "Pitch Attack", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_DECAY: vst_strncpy(label, "Pitch Decay", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_SUSTAIN: vst_strncpy(label, "Pitch Sustain", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_RELEASE: vst_strncpy(label, "Pitch Release", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_TIME_SCALE: vst_strncpy(label, "Pitch Time Scale", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_MIN_VALUE: vst_strncpy(label, "Pch MinV", kVstMaxParamStrLen);
			break;
		case PARAM_PITCH_MAX_VALUE: vst_strncpy(label, "Pch MaxV", kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_TYPE: vst_strncpy(label, "Filter type", kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_FREQUENCY: vst_strncpy(label, "FLT FREQ", kVstMaxParamStrLen);
			break;
		case PARAM_FILTER_RESONANCE: vst_strncpy(label, "FLT RES", kVstMaxParamStrLen);
			break;
		case PARAM_SATURATE: vst_strncpy(label, "Saturate", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_ACTIVE: vst_strncpy(label, "EQ status", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_LOW_FREQUENCY: vst_strncpy(label, "EQ Lo frq", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_HIGH_FREQUENCY: vst_strncpy(label, "EQ Hi frq", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_LOW_GAIN: vst_strncpy(label, "EQ Lo gain", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_MID_GAIN: vst_strncpy(label, "EQ Mid gain", kVstMaxParamStrLen);
			break;
		case PARAM_EQ_HI_GAIN: vst_strncpy(label, "EQ Hi gain", kVstMaxParamStrLen);
			break;
	}
}

void SorolletVSTi::setParameter(VstInt32 index, float value)
{
	int r;
	SorolletVSTiProgram *ap = &programs[curProgram];
	switch (index)
	{
		case PARAM_VOLUME1:
			fVolume1 = ap->fVolume1 = value;
			mSorolletVoice->setVolume1(fVolume1);
			break;
		case PARAM_OCTAVE1:
			fOctave1 = ap->fOctave1 = value;
			mSorolletVoice->setOctave1(floatToOctave(fOctave1));
			break;
		case PARAM_WAVE1:
			fWave1 = ap->fWave1 = value;
			mSorolletVoice->setWave1(floatToWave(fWave1));
			break;
		case PARAM_PHASE1:
			fPhase1 = ap->fPhase1 = value;
			mSorolletVoice->setPhase1(fPhase1);
			break;
		case PARAM_VOLUME2:
			fVolume2 = ap->fVolume2 = value;
			mSorolletVoice->setVolume2(fVolume2);
			break;
		case PARAM_OCTAVE2:
			fOctave2 = ap->fOctave2 = value;
			mSorolletVoice->setOctave2(floatToOctave(fOctave2));
			break;
		case PARAM_WAVE2:
			fWave2 = ap->fWave2 = value;
			mSorolletVoice->setWave2(floatToWave(fWave2));
			break;
		case PARAM_PHASE2:
			fPhase2 = ap->fPhase2 = value;
			mSorolletVoice->setPhase2(fPhase2);
			break;
		case PARAM_WAVE_MIX_TYPE:
			fWaveMixType = ap->fWaveMixType = value;
			mSorolletVoice->setWaveMixType(floatToWaveMixType(fWaveMixType));
			break;
		case PARAM_NOISE_AMOUNT:
			fNoiseAmount = ap->fNoiseAmount = value;
			mSorolletVoice->setNoiseAmount(fNoiseAmount);
			break;
		case PARAM_NOISE_MIX_TYPE:
			fNoiseMixType = ap->fNoiseMixType = value;
			mSorolletVoice->setNoiseMixType(floatToNoiseMixType(fNoiseMixType));
			break;
		case PARAM_AMP_ATTACK:
			fAmpAttack = ap->fAmpAttack = value;
			mSorolletVoice->getAmpADSR()->setAttack(value);
			break;
		case PARAM_AMP_DECAY:
			fAmpDecay = ap->fAmpDecay = value;
			mSorolletVoice->getAmpADSR()->setDecay(value);
			break;
		case PARAM_AMP_SUSTAIN:
			fAmpSustain = ap->fAmpSustain = value;
			mSorolletVoice->getAmpADSR()->setSustainLevel(value);
			break;
		case PARAM_AMP_RELEASE:
			fAmpRelease = ap->fAmpRelease = value;
			mSorolletVoice->getAmpADSR()->setRelease(value);
			break;
		case PARAM_AMP_TIME_SCALE:
			fAmpTimeScale = ap->fAmpTimeScale = value;
			mSorolletVoice->getAmpADSR()->setTimeScale(floatToEnvelopeScale(value));
			r = updateDisplay(); // b/c this affects the 4 previous display values
			break;
		case PARAM_AMP_MIN_VALUE:
			fAmpMinValue = ap->fAmpMinValue = value;
			mSorolletVoice->getAmpADSR()->setMinValue(value);
			break;
		case PARAM_AMP_MAX_VALUE:
			fAmpMaxValue = ap->fAmpMaxValue = value;
			mSorolletVoice->getAmpADSR()->setMaxValue(value);
			break;
		case PARAM_PITCH_ATTACK:
			fPitchAttack = ap->fPitchAttack = value;
			mSorolletVoice->getPitchADSR()->setAttack(value);
			break;
		case PARAM_PITCH_DECAY:
			fPitchDecay = ap->fPitchDecay = value;
			mSorolletVoice->getPitchADSR()->setDecay(value);
			break;
		case PARAM_PITCH_SUSTAIN:
			fPitchSustain = ap->fPitchSustain = value;
			mSorolletVoice->getPitchADSR()->setSustainLevel(value);
			break;
		case PARAM_PITCH_RELEASE:
			fPitchRelease = ap->fPitchRelease = value;
			mSorolletVoice->getPitchADSR()->setRelease(value);
			break;
		case PARAM_PITCH_TIME_SCALE:
			fPitchTimeScale = ap->fPitchTimeScale = value;
			mSorolletVoice->getPitchADSR()->setTimeScale(floatToEnvelopeScale(value));
			updateDisplay(); // b/c this affects the 4 previous display values
			break;
		case PARAM_PITCH_MIN_VALUE:
			fPitchMinValue = ap->fPitchMinValue = value;
			mSorolletVoice->getPitchADSR()->setMinValue(value);
			break;
		case PARAM_PITCH_MAX_VALUE:
			fPitchMaxValue = ap->fPitchMaxValue = value;
			mSorolletVoice->getPitchADSR()->setMaxValue(value);
			break;
		case PARAM_FILTER_TYPE:
			fFilterType = ap->fFilterType = value;
			mSorolletVoice->setFilterType(floatToFilterType(fFilterType));
			break;
		case PARAM_FILTER_FREQUENCY:
			fFilterFrequency = ap->fFilterFrequency = value;
			mSorolletVoice->setFilterFrequency(fFilterFrequency);
			break;
		case PARAM_FILTER_RESONANCE:
			fFilterResonance = ap->fFilterResonance = value;
			mSorolletVoice->setFilterResonance(fFilterResonance);
			break;
		case PARAM_SATURATE:
			fSaturate = ap->fSaturate = value;
			mSorolletVoice->setSaturate(fSaturate);
			break;
		case PARAM_EQ_ACTIVE:
			fEQActive = ap->fEQActive = value;
			mSorolletVoice->setEQActive(value > 0.5f);
			break;
		case PARAM_EQ_LOW_FREQUENCY:
			fEQLowFrequency = ap->fEQLowFrequency = value;
			mSorolletVoice->setEQLowFrequency(value);
			break;
		case PARAM_EQ_HIGH_FREQUENCY:
			fEQHighFrequency = ap->fEQHighFrequency = value;
			mSorolletVoice->setEQHighFrequency(value);
			break;
		case PARAM_EQ_LOW_GAIN:
			fEQLowGain = ap->fEQLowGain = value;
			mSorolletVoice->setEQLowGain(value);
			break;
		case PARAM_EQ_MID_GAIN:
			fEQMidGain = ap->fEQMidGain = value;
			mSorolletVoice->setEQMidGain(value);
			break;
		case PARAM_EQ_HI_GAIN:
			fEQHiGain = ap->fEQHiGain = value;
			mSorolletVoice->setEQHiGain(value);
			break;
	}
}

float SorolletVSTi::getParameter(VstInt32 index)
{
	float value = 0;
	switch (index)
	{
		case PARAM_VOLUME1: value = fVolume1;
			break;
		case PARAM_OCTAVE1: value = fOctave1;
			break;
		case PARAM_WAVE1: value = fWave1;
			break;
		case PARAM_PHASE1: value = fPhase1;
			break;
		case PARAM_VOLUME2: value = fVolume2;
			break;
		case PARAM_OCTAVE2: value = fOctave2;
			break;
		case PARAM_WAVE2: value = fWave2;
			break;
		case PARAM_PHASE2: value = fPhase2;
			break;
		case PARAM_WAVE_MIX_TYPE: value = fWaveMixType;
			break;
		case PARAM_NOISE_AMOUNT: value = fNoiseAmount;
			break;
		case PARAM_NOISE_MIX_TYPE: value = fNoiseMixType;
			break;
		case PARAM_AMP_ATTACK: value = fAmpAttack;
			break;
		case PARAM_AMP_DECAY: value = fAmpDecay;
			break;
		case PARAM_AMP_SUSTAIN: value = fAmpSustain;
			break;
		case PARAM_AMP_RELEASE: value = fAmpRelease;
			break;
		case PARAM_AMP_TIME_SCALE: value = fAmpTimeScale;
			break;
		case PARAM_AMP_MIN_VALUE: value = fAmpMinValue;
			break;
		case PARAM_AMP_MAX_VALUE: value = fAmpMaxValue;
			break;
		case PARAM_PITCH_ATTACK: value = fPitchAttack;
			break;
		case PARAM_PITCH_DECAY: value = fPitchDecay;
			break;
		case PARAM_PITCH_SUSTAIN: value = fPitchSustain;
			break;
		case PARAM_PITCH_RELEASE: value = fPitchRelease;
			break;
		case PARAM_PITCH_TIME_SCALE: value = fPitchTimeScale;
			break;
		case PARAM_PITCH_MIN_VALUE: value = fPitchMinValue;
			break;
		case PARAM_PITCH_MAX_VALUE: value = fPitchMaxValue;
			break;
		case PARAM_FILTER_TYPE: value = fFilterType;
			break;
		case PARAM_FILTER_FREQUENCY: value = fFilterFrequency;
			break;
		case PARAM_FILTER_RESONANCE: value = fFilterResonance;
			break;
		case PARAM_SATURATE: value = fSaturate;
			break;
		case PARAM_EQ_ACTIVE: value = fEQActive;
			break;
		case PARAM_EQ_LOW_FREQUENCY: value = fEQLowFrequency;
			break;
		case PARAM_EQ_HIGH_FREQUENCY: value = fEQHighFrequency;
			break;
		case PARAM_EQ_LOW_GAIN: value = fEQLowGain;
			break;
		case PARAM_EQ_MID_GAIN: value = fEQMidGain;
			break;
		case PARAM_EQ_HI_GAIN: value = fEQHiGain;
			break;
	}
	return value;
}

int SorolletVSTi::floatToOctave(float value)
{
	return ((int) MathUtils::map(value, 0.0f, 1.0f, -1.0f, 9.0f));
}

int SorolletVSTi::floatToWave(float value)
{
	return (int) MathUtils::map(value, 0.0f, 1.0f, 0, 3);
}

void SorolletVSTi::floatToWaveName(float value, char* text)
{
	int wave = floatToWave(value);
	switch (wave)
	{
		case WAVE_SINE: vst_strncpy(text, "Sine", kVstMaxParamStrLen);
			break;
		case WAVE_TRIANGLE: vst_strncpy(text, "Triangle", kVstMaxParamStrLen);
			break;
		case WAVE_SQUARE: vst_strncpy(text, "Square", kVstMaxParamStrLen);
			break;
		case WAVE_SAWTOOTH: vst_strncpy(text, "Sawtooth", kVstMaxParamStrLen);
			break;
	}
}

int SorolletVSTi::floatToWaveMixType(float value)
{
	return (int) MathUtils::map(value, 0.0f, 1.0f, 0, 3);
}

void SorolletVSTi::floatToWaveMixTypeName(float value, char* text)
{
	int wave = floatToWaveMixType(value);
	switch (wave)
	{
		case WAVE_ADD: vst_strncpy(text, "+", kVstMaxParamStrLen);
			break;
		case WAVE_SUBSTRACT: vst_strncpy(text, "-", kVstMaxParamStrLen);
			break;
		case WAVE_MULTIPLY: vst_strncpy(text, "*", kVstMaxParamStrLen);
			break;
		case WAVE_DIVIDE: vst_strncpy(text, "/", kVstMaxParamStrLen);
			break;
	}
}

int SorolletVSTi::floatToNoiseMixType(float value)
{
	return (int) MathUtils::map(value, 0.0f, 1.0f, 0, 2);
}

void SorolletVSTi::floatToNoiseMixTypeName(float value, char* text)
{
	int wave = floatToWaveMixType(value);
	switch (wave)
	{
		case NOISE_ADD: vst_strncpy(text, "+", kVstMaxParamStrLen);
			break;
		case NOISE_MIX: vst_strncpy(text, "%", kVstMaxParamStrLen);
			break;
		case NOISE_MULTIPLY: vst_strncpy(text, "*", kVstMaxParamStrLen);
			break;
	}
}

void SorolletVSTi::formatTime(float timeInSeconds, char* text)
{
	int h, m, s, ms;
	float ms_fract, remaining;
	double s_fract;
	char strTime[128];

	remaining = timeInSeconds;

	h = (int) (remaining / 3600.0f);
	remaining -= h * 3600;

	m = (int) (remaining / 60.0f);
	remaining -= m * 60;

	ms_fract = modf(remaining, &s_fract);
	s = (int) s_fract;
	ms = (int) (ms_fract * 1000);

	if (h > 0)
	{
		sprintf(strTime, "%02d:%02d:%02d.%03d", h, m, s, ms);
	}
	else if (m > 0)
	{
		sprintf(strTime, "%02d:%02d.%03d", m, s, ms);
	}
	else if (s > 0)
	{
		sprintf(strTime, "%02d.%03d s", s, ms);
	}
	else
	{
		sprintf(strTime, "%d ms", ms);
	}

	vst_strncpy(text, strTime, kVstMaxParamStrLen);
}

float SorolletVSTi::floatToEnvelopeScale(float value)
{
	return MathUtils::map(value, 0, 1, 1, 10);
}

int SorolletVSTi::floatToFilterType(float value)
{
	return (int) MathUtils::map(value, 0.0f, 1.0f, 0, 2);
}

void SorolletVSTi::floatToFilterTypeName(float value, char* text)
{
	int type = floatToFilterType(value);
	switch (type)
	{

		case FILTER_LOW_PASS: vst_strncpy(text, "low", kVstMaxParamStrLen);
			break;
		case FILTER_HIGH_PASS: vst_strncpy(text, "high", kVstMaxParamStrLen);
			break;
		default:
		case FILTER_NONE: vst_strncpy(text, "none", kVstMaxParamStrLen);
			break;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool SorolletVSTi::getOutputProperties(VstInt32 index, VstPinProperties* properties)
{
	if (index < kNumOutputs)
	{
		vst_strncpy(properties->label, "Sorollet ", 63);
		char temp[11] = {0};
		int2string(index + 1, temp, 10);
		vst_strncat(properties->label, temp, 63);

		properties->flags = kVstPinIsActive;
		if (index < 2)
			properties->flags |= kVstPinIsStereo; // make channel 1+2 stereo
		return true;
	}
	return false;
}

bool SorolletVSTi::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text)
{
	if (index < kNumPrograms)
	{
		vst_strncpy(text, programs[index].name, kVstMaxProgNameLen);
		return true;
	}
	return false;
}

bool SorolletVSTi::getEffectName(char* name)
{
	vst_strncpy(name, "SorolletVSTi", kVstMaxEffectNameLen);
	return true;
}

bool SorolletVSTi::getVendorString(char* text)
{
	vst_strncpy(text, "supersole", kVstMaxVendorStrLen);
	return true;
}

bool SorolletVSTi::getProductString(char* text)
{
	vst_strncpy(text, "Sorollet", kVstMaxProductStrLen);
	return true;
}

VstInt32 SorolletVSTi::getVendorVersion()
{
	return 1000;
}

VstInt32 SorolletVSTi::canDo(char* text)
{
	if (!strcmp(text, "receiveVstEvents"))
		return 1;
	if (!strcmp(text, "receiveVstMidiEvent"))
		return 1;
	return -1; // explicitly can't do; 0 => don't know
}

VstInt32 SorolletVSTi::getNumMidiInputChannels()
{
	return 1; // we are monophonic
}

VstInt32 SorolletVSTi::getNumMidiOutputChannels()
{
	return 0; // no MIDI output back to Host app
}

void SorolletVSTi::setSampleRate(float sampleRate)
{
	AudioEffectX::setSampleRate(sampleRate);
}

void SorolletVSTi::setBlockSize(VstInt32 blockSize)
{
	AudioEffectX::setBlockSize(blockSize);
	mSorolletVoice->setBufferLength(blockSize);
}

void SorolletVSTi::initProcess()
{
	noteIsOn = false;
	currentDelta = currentNote = currentDelta = 0;
	VstInt32 i;
}

void SorolletVSTi::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* outL = outputs[0];
	float* outR = outputs[1];

	VstTimeInfo* timeInfo;

	if (currentDelta > 0)
	{
		if (currentDelta >= sampleFrames) // future
		{
			currentDelta -= sampleFrames;
			return;
		}

		timeInfo = getTimeInfo(0);
		if (timeInfo)
		{
			mSorolletVoice->setCurrentSamplePosition(timeInfo->samplePos);
		}
		mSorolletVoice->getBuffer(outL, outR, currentDelta);
		outL += currentDelta;
		outR += currentDelta;
		sampleFrames -= currentDelta;
		currentDelta = 0;
	}

	timeInfo = getTimeInfo(0);
	if (timeInfo)
	{
		mSorolletVoice->setCurrentSamplePosition(timeInfo->samplePos);
	}

	if (currentEvent != EVENT_NULL)
	{
		if (currentEvent == EVENT_NOTE_ON)
		{
			mSorolletVoice->sendNoteOn(currentNote, MathUtils::map(currentVelocity, 0, 128, 0.0f, 1.0f));
		}
		else if (currentEvent == EVENT_NOTE_OFF)
		{
			mSorolletVoice->sendNoteOff();
		}

		currentEvent = EVENT_NULL;
	}

	mSorolletVoice->getBuffer(outL, outR, sampleFrames);

}

VstInt32 SorolletVSTi::processEvents(VstEvents* ev)
{
	for (VstInt32 i = 0; i < ev->numEvents; i++)
	{
		if ((ev->events[i])->type != kVstMidiType)
			continue;

		VstMidiEvent* event = (VstMidiEvent*) ev->events[i];
		char* midiData = event->midiData;
		VstInt32 status = midiData[0] & 0xf0; // ignoring channel
		if (status == 0x90 || status == 0x80) // we only look at notes
		{
			VstInt32 note = midiData[1] & 0x7f;
			VstInt32 velocity = midiData[2] & 0x7f;
			if (status == 0x80)
				velocity = 0; // note off by velocity 0
			if (!velocity && (note == currentNote))
				noteOff(event->deltaFrames);
			else
				noteOn(note, velocity, event->deltaFrames);
		}/*else if(status == 0xA0)
        {
            printf("polyphonic Aftertouch\n");
        }
        else if(status == 0xD0)
        {
            printf("channel Aftertouch\n");
        }*/
		else if (0xC0 == status)
		{
			// Control change
			int number = midiData[1] & 0x7F;
			float value = (float) ((midiData[2] & 0x7F));
			printf("CC %d value = %d , float = %f\n", number, midiData[2] & 0x7F, value);
		}
		else if (status == 0xb0)
		{
			if (midiData[1] == 0x7e || midiData[1] == 0x7b) // all notes off
				noteOff(event->deltaFrames);
		}

		event++;
	}
	return 1;
}

void SorolletVSTi::noteOn(VstInt32 note, VstInt32 velocity, VstInt32 delta)
{
	currentNote = note;
	currentVelocity = velocity;
	currentDelta = delta;
	currentEvent = EVENT_NOTE_ON;

	noteIsOn = true;
}

void SorolletVSTi::noteOff(VstInt32 delta)
{
	noteIsOn = false;
	currentDelta = delta;
	currentEvent = EVENT_NOTE_OFF;
}
