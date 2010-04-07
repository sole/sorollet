#include "SorolletVoice.h"
#include "SorolletConstants.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

#include "../sys/MathUtils.h"

SorolletVoice::SorolletVoice()
{
    mEQL = NULL;
    mEQR = NULL;

    mBufferOSC1L = NULL;
    mBufferOSC1R = NULL;
    mBufferOSC2L = NULL;
    mBufferOSC2R = NULL;
    mBufferTmpL = NULL;
    mBufferTmpR = NULL;
    mBufferNoiseL = NULL;
    mBufferNoiseR = NULL;
    mBufferAmp = NULL;
    mBufferPitch1 = NULL;
    mBufferPitch2 = NULL;
    
    setSampleRate(44100);
    miPosition = 0;
    mlfInternalSamplePosition = 0;

    setBufferLength(4096); // Same value that is used in VST instruments
    
    setVolume1(0.5f);
    setOctave1(4);
    setWave1(WAVE_SINE);
    setPhase1(0.5f);

    setVolume2(0.5f);
    setOctave1(3);
    setWave1(WAVE_SQUARE);
    setPhase2(0.5f);
    
    setWaveMixType(WAVE_ADD);

    setNoiseAmount(0);
    setNoiseMixType(NOISE_ADD);


    miCurrentNote = NOTE_NULL;
    mfAbsoluteClippingValue = 1.0f;
    
    // Like no envelope at all, actually.
    mAmpADSR = new SorolletADSR(0.0f, 0.0f, 0.8f, 0.0f, 1.0f);
    mAmpADSR->setAcceptableMinMaxValues(0.0f, 0.66f);
    mPitchADSR = new SorolletADSR(0.0f, 0.0f, 0.5f, 0.0f, 1.0f);
    mPitchADSR->setAcceptableMinMaxValues(-48.0f, 48.0f);

    mFilterFreqADSR = new SorolletADSR(0.0f, 0.0f, 0.5f, 0.0f, 1.0f);
    mFilterFreqADSR->setAcceptableMinMaxValues(-24.0f, 24.0f);
    mFilterL = new SorolletFilter(FILTER_NONE, 0, 0);
    mFilterL->setInverseSamplingRate(mfInvSampleRate);
    mFilterR = new SorolletFilter(FILTER_NONE, 0, 0);
    mFilterR->setInverseSamplingRate(mfInvSampleRate);
    setFilterType(FILTER_NONE);
    mfFilterFrequencyHertz = 0;
    setFilterFrequency(0);
    setFilterResonance(0);

    setSaturate(0);

    mEQL = new SorolletEQ();
    mEQL->setSampleRate(mfSampleRate);
    mEQL->prepare();

    mEQR = new SorolletEQ();
    mEQR->setSampleRate(mfSampleRate);
    mEQR->prepare();

    setEQActive(false);
    setEQLowFrequency(0.5f);
    setEQHighFrequency(0.5f);
    
}

SorolletVoice::~SorolletVoice()
{
    if(mBufferOSC1L != NULL) { delete[] mBufferOSC1L; }
    if(mBufferOSC1R != NULL) { delete[] mBufferOSC1R; }
    if(mBufferOSC2L != NULL) { delete[] mBufferOSC2L; }
    if(mBufferOSC2R != NULL) { delete[] mBufferOSC2R; }
    if(mBufferTmpL != NULL) { delete[] mBufferTmpL; }
    if(mBufferTmpR != NULL) { delete[] mBufferTmpR; }
    if(mBufferNoiseL != NULL) { delete[] mBufferNoiseL; }
    if(mBufferNoiseR != NULL) { delete[] mBufferNoiseR; }
    if(mBufferAmp != NULL) { delete[] mBufferAmp; }
    if(mBufferPitch1 != NULL) { delete[] mBufferPitch1; }
    if(mBufferPitch2 != NULL) { delete[] mBufferPitch2; }

	delete mAmpADSR;
    delete mPitchADSR;
    delete mFilterFreqADSR;

    delete mFilterL;
    delete mFilterR;

    delete mEQL;
    delete mEQR;
}

void SorolletVoice::setBufferLength(int newBufferLength)
{
    miBufferLength = newBufferLength;
    prepareBuffers();
}

void SorolletVoice::setSampleRate(int newSampleRate)
{
    mfSampleRate = (float) newSampleRate;
    mfInvSampleRate = 1.0f / newSampleRate;

    if(mEQL)
    {
        mEQL->setSampleRate(mfSampleRate);
    }
    if(mEQR)
    {
        mEQR->setSampleRate(mfSampleRate);
    }
}

void SorolletVoice::resetSamplePosition()
{
    mlfInternalSamplePosition = 0.0f;
}

bool SorolletVoice::isActive()
{
    return (mAmpADSR->getState() == ADSR_DONE || miCurrentNote == NOTE_NULL);
}

void SorolletVoice::getBuffer(float* outL, float* outR, int numSamples)
{
    float currTime = getTime();
    if (mAmpADSR->getState() == ADSR_DONE)
    {
        miCurrentNote = NOTE_NULL;
    }

    memset(outL, 0, sizeof(float) * numSamples);
    memset(outR, 0, sizeof(float) * numSamples);

    // Return as soon as possible if required
    if (miCurrentNote < 0)
    {
        return;
    }

    int i;
    float notNoiseAmount = 1.0f - mfNoiseAmount;

    float* osc1_left = mBufferOSC1L;
    float* osc1_right = mBufferOSC1R;
    float* osc2_left = mBufferOSC2L;
    float* osc2_right = mBufferOSC2R;
    float* noise_left = mBufferNoiseL;
    float* noise_right = mBufferNoiseR;

    memset(osc1_left, 0, sizeof(float) * numSamples);
    memset(osc1_right, 0, sizeof(float) * numSamples);
    memset(osc2_left, 0, sizeof(float) * numSamples);
    memset(osc2_right, 0, sizeof(float) * numSamples);
    memset(noise_left, 0, sizeof(float) * numSamples);
    memset(noise_right, 0, sizeof(float) * numSamples);


    // Fill the amp and pitch buffers for this run
    float bufferTime = currTime;
    for (i = 0; i < numSamples; i++)
    {
        float pitchEnv = mPitchADSR->update(bufferTime);
        mBufferPitch1[i] = noteToFrequency(miCurrentNote + pitchEnv, miOctave1);
        mBufferPitch2[i] = noteToFrequency(miCurrentNote + pitchEnv, miOctave2);
        mBufferAmp[i] = mAmpADSR->update(bufferTime);
        bufferTime += mfInvSampleRate;
    }

    if (mfVolume1 > 0.0f)
    {
        float position = (float) miPosition;
        for (i = 0; i < numSamples; i++)
        {
            float frequency1 = mBufferPitch1[i];

            switch (miWave1)
            {
                case WAVE_SINE: getSineBuffer(osc1_left, osc1_right, 1, position, frequency1, mfPhaseRadians1);
                    break;
                case WAVE_TRIANGLE: getTriangleBuffer(osc1_left, osc1_right, 1, position, frequency1, mfPhaseRadians1);
                    break;
                case WAVE_SQUARE: getSquareBuffer(osc1_left, osc1_right, 1, position, frequency1, mfPhaseRadians1);
                    break;
                case WAVE_SAWTOOTH: getSawtoothBuffer(osc1_left, osc1_right, 1, position, frequency1, mfPhaseRadians1);
                    break;
            }
            osc1_left++;
            osc1_right++;
            position++;
        }

        // restore so nothing 'odd' happens later
        osc1_left = mBufferOSC1L;
        osc1_right = mBufferOSC1R;
    }

    if (mfVolume2 > 0.0f)
    {
        float position = (float) miPosition;
        for (i = 0; i < numSamples; i++)
        {
            float frequency2 = mBufferPitch2[i]; //noteToFrequency(miCurrentNote + mPitchADSR->update(t), miOctave1);

            switch (miWave2)
            {
                case WAVE_SINE: getSineBuffer(osc2_left, osc2_right, 1, position, frequency2, mfPhaseRadians2);
                    break;
                case WAVE_TRIANGLE: getTriangleBuffer(osc2_left, osc2_right, 1, position, frequency2, mfPhaseRadians2);
                    break;
                case WAVE_SQUARE: getSquareBuffer(osc2_left, osc2_right, 1, position, frequency2, mfPhaseRadians2);
                    break;
                case WAVE_SAWTOOTH: getSawtoothBuffer(osc2_left, osc2_right, 1, position, frequency2, mfPhaseRadians2);
                    break;
            }
            osc2_left++;
            osc2_right++;
            position++;
        }

        // restore so nothing 'odd' happens later
        osc2_left = mBufferOSC2L;
        osc2_right = mBufferOSC2R;
    }

    if (mfNoiseAmount > 0.0f)
    {
        getNoiseBuffer(noise_left, noise_right, numSamples);
    }

    float osc1L, osc1R, osc2L, osc2R, valueL, valueR;

    for (i = 0; i < numSamples; i++)
    {
        osc1L = osc1_left[i] * mfVolume1;
        osc1R = osc1_right[i] * mfVolume1;
        osc2L = osc2_left[i] * mfVolume2;
        osc2R = osc2_right[i] * mfVolume2;

        switch (miWaveMixType)
        {
            case WAVE_ADD:
                valueL = osc1L + osc2L;
                valueR = osc1R + osc2R;
                break;
            case WAVE_SUBSTRACT:
                valueL = osc1L - osc2L;
                valueR = osc1R - osc2R;
                break;
            case WAVE_MULTIPLY:
                valueL = osc1L * osc2L;
                valueR = osc1R * osc2R;
                break;
            case WAVE_DIVIDE:
                // Slightly more expensive because we check for non divide by zero!
                if (osc2L == 0) osc2L = 0.0001f;
                if (osc2R == 0) osc2R = 0.0001f;
                valueL = osc1L / osc2L;
                valueR = osc1R / osc2R;
                break;
        }

        // What about noise?
        if (mfNoiseAmount > 0.0f)
        {
            float noiseValueL = noise_left[i] * mfNoiseAmount;
            float noiseValueR = noise_right[i] * mfNoiseAmount;

            switch (miNoiseMixType)
            {
                case NOISE_ADD:
                    valueL += noiseValueL;
                    valueR += noiseValueR;
                    break;
                case NOISE_MIX:
                    valueL = valueL * notNoiseAmount + noiseValueL;
                    valueR = valueR * notNoiseAmount + noiseValueR;
                    break;
                case NOISE_MULTIPLY:
                    valueL *= noiseValueL;
                    valueR *= noiseValueR;
                    break;
            }
        }

        // Apply amp envelope
        valueL *= mBufferAmp[i] * mfCurrentVolume;
        valueR *= mBufferAmp[i] * mfCurrentVolume;

        // Filter
        valueL = mFilterL->filterValue(valueL);
        valueR = mFilterR->filterValue(valueR);

        // Saturate
        valueL = saturateValue(valueL);
        valueR = saturateValue(valueR);

        valueL = mEQL->update(valueL);
        valueR = mEQR->update(valueR);

        valueL *= mfAbsoluteClippingValue;
        valueR *= mfAbsoluteClippingValue;

        (*outL++) = MathUtils::clipf(valueL, -1.0f, 1.0f);
        (*outR++) = MathUtils::clipf(valueR, -1.0f, 1.0f);
    }

    mlfInternalSamplePosition += numSamples;
    miPosition += numSamples;

}

SorolletADSR* SorolletVoice::getAmpADSR()
{
    return this->mAmpADSR;
}

SorolletADSR* SorolletVoice::getPitchADSR()
{
    return this->mPitchADSR;
}

SorolletADSR* SorolletVoice::getFilterFreqADSR()
{
    return this->mFilterFreqADSR;
}

// PROTECTED ~~~~~~~~~~~~~~~~

void SorolletVoice::prepareBuffers()
{
    if(mBufferOSC1L != NULL) { delete[] mBufferOSC1L; }
    if(mBufferOSC1R != NULL) { delete[] mBufferOSC1R; }
    if(mBufferOSC2L != NULL) { delete[] mBufferOSC2L; }
    if(mBufferOSC2R != NULL) { delete[] mBufferOSC2R; }
    if(mBufferTmpL != NULL) { delete[] mBufferTmpL; }
    if(mBufferTmpR != NULL) { delete[] mBufferTmpR; }
    if(mBufferNoiseL != NULL) { delete[] mBufferNoiseL; }
    if(mBufferNoiseR != NULL) { delete[] mBufferNoiseR; }
    if(mBufferAmp != NULL) { delete[] mBufferAmp; }
    if(mBufferPitch1 != NULL) { delete[] mBufferPitch1; }
    if(mBufferPitch2 != NULL) { delete[] mBufferPitch2; }
    
    mBufferOSC1L = new float[miBufferLength];
    mBufferOSC1R = new float[miBufferLength];
    mBufferOSC2L = new float[miBufferLength];
    mBufferOSC2R = new float[miBufferLength];
    mBufferTmpL = new float[miBufferLength];
    mBufferTmpR = new float[miBufferLength];
    mBufferNoiseL = new float[miBufferLength];
    mBufferNoiseR = new float[miBufferLength];
    mBufferAmp = new float[miBufferLength];
    mBufferPitch1 = new float[miBufferLength];
    mBufferPitch2 = new float[miBufferLength];
    
}

void SorolletVoice::getSineBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase)
{
    float value;
    float cst = 2.0f * M_PI * frequency * mfInvSampleRate;
    int i;

    for (i = 0; i < numSamples; ++i)
    {
        value = ((float) sinf(cst * position + phase));

        (*outL++) = value;
        (*outR++) = value;

        position++;
    }
}

void SorolletVoice::getTriangleBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase)
{
    int i;
    float period = 1.0f / frequency;
    float semiperiod = period * 0.5f;
    float value;
    float ft = semiperiod * 0.5f;
    for (i=0; i<numSamples ; ++i)
    {
        float t = (i+position + phase) * mfInvSampleRate +ft;

        if (fmodf(t,period) < semiperiod)
                value = 2.0f * (fmodf(t, semiperiod) / semiperiod)-1.0f;
        else
                value = 1.0f - 2.0f *fmodf(t, semiperiod) / semiperiod;

        (*outL++) = value;
        (*outR++) = value;
    }
}

void SorolletVoice::getSquareBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase)
{
    float period = 1.0f / frequency;
    float halfPeriod = period / 2.0f;
    float value;
    int i;

    for (i = 0; i < numSamples; ++i)
    {
        float t = (i+position+phase) * mfInvSampleRate;
        if (fmodf(t, period) < halfPeriod)
        {
            value = 1.0f;
        } else
        {
            value = -1.0f;
        }

        (*outL++) = value;
        (*outR++) = value;

    }
}

void SorolletVoice::getSawtoothBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase)
{
    int i;
    float period = 1.0f / frequency;
    float value;
    
    for (i = 0; i < numSamples; ++i)
    {
        float t = (position+phase) * mfInvSampleRate;
        value = 2.0f * (fmodf(t, period) * frequency) - 1.0f;

        (*outL++) = value;
        (*outR++) = value;

        position++;
    }
}

void SorolletVoice::getNoiseBuffer(float* outL, float* outR, int numSamples)
{
    for (int i = 0; i < numSamples; i++)
    {
	(*outL++) = MathUtils::randf();
        (*outR++) = MathUtils::randf();
    }
}

float SorolletVoice::saturateValue(float value)
{
    // http://musicdsp.org/archive.php?classid=5#120
    if (mfSaturate > 0.01f)
    {
        if (fabsf(value) < mfSaturate)
        {
            return value;
        } else
        {
            if (value > 0)
            {
                return mfSaturate + (1.f - mfSaturate) * tanh((value - mfSaturate) / (1.f - mfSaturate));
            }
            else
            {
                return -(mfSaturate + (1.f - mfSaturate) * tanh((-value - mfSaturate) / (1.f - mfSaturate)));
            }
        }
    }
    return value;
}

float SorolletVoice::getTime()
{
    float time;

    time = (float) (mlfInternalSamplePosition * mfInvSampleRate);
    return time;
}

void SorolletVoice::setVolume1(float volume)
{
    mfVolume1 = volume;
}

void SorolletVoice::setVolume2(float volume)
{
    mfVolume2 = volume;
}

void SorolletVoice::setOctave1(int octave)
{
    miOctave1 = octave;
}

void SorolletVoice::setWave1(int waveType)
{
    miWave1 = waveType;
}

void SorolletVoice::setPhase1(float phase)
{
    mfPhase1 = phase;
    mfPhaseRadians1 = MathUtils::map(phase, 0.0f, 1.0f, -M_2_PI, M_2_PI);
}

void SorolletVoice::setOctave2(int octave)
{
    miOctave2 = octave;
}

void SorolletVoice::setWave2(int waveType)
{
    miWave2 = waveType;
}

void SorolletVoice::setPhase2(float phase)
{
    mfPhase1 = phase;
    mfPhaseRadians2 = MathUtils::map(phase, 0.0f, 1.0f, -M_2_PI, M_2_PI);
}

void SorolletVoice::setWaveMixType(int waveMixType)
{
    miWaveMixType = waveMixType;
}

void SorolletVoice::setNoiseAmount(float amount)
{
    mfNoiseAmount = amount;
}

void SorolletVoice::setNoiseMixType(int noiseMixType)
{
    miNoiseMixType = noiseMixType;
}

void SorolletVoice::setFilterType(int type)
{
    mFilterL->setType(type);
    mFilterR->setType(type);
}

void SorolletVoice::setFilterFrequency(float frequency)
{
    mfFilterFrequency = frequency;
    float tmp = MathUtils::map(frequency, 0.0f, 1.0f, 10.0f, mfSampleRate * 0.5f - 10);
    if(tmp != mfFilterFrequencyHertz)
    {
        mfFilterFrequencyHertz = tmp;
        mFilterL->setFrequency(mfFilterFrequencyHertz);
        mFilterR->setFrequency(mfFilterFrequencyHertz);
    }
}

void SorolletVoice::setFilterResonance(float resonance)
{
    mfFilterResonance = resonance;
    mfFilterResonanceHertz = MathUtils::map(resonance, 0.0f, 1.0f, 0.707106781f, 0.01f);
    mFilterL->setResonance(mfFilterResonanceHertz);
    mFilterR->setResonance(mfFilterResonanceHertz);
}

float SorolletVoice::getFilterFrequencyDisplay()
{
    return mfFilterFrequencyHertz;
}

float SorolletVoice::getFilterResonanceDisplay()
{
    return mfFilterResonanceHertz;
}

void SorolletVoice::setSaturate(float value)
{
    mfSaturate = value;
}

void SorolletVoice::setEQActive(bool active)
{
    mEQL->setActive(active);
    mEQR->setActive(active);
}

void SorolletVoice::setEQLowFrequency(float value)
{
    mfEQCurrentLowFrequency = MathUtils::map(value, 0.0f, 1.0f, 0.1f, 800.0f);
    mfEQLowFrequency = value;
    mEQL->setLowFreq(mfEQCurrentLowFrequency);
    mEQR->setLowFreq(mfEQCurrentLowFrequency);
}

void SorolletVoice::setEQHighFrequency(float value)
{
    mfEQCurrentHighFrequency = MathUtils::map(value, 0.0f, 1.0f, 900.0f, 20000.0f);
    mfEQHighFrequency = value;
    mEQL->setHighFreq(mfEQCurrentHighFrequency);
    mEQR->setHighFreq(mfEQCurrentHighFrequency);
}

#define MIN_GAIN 0.0f
#define MAX_GAIN 10.0f

void SorolletVoice::setEQLowGain(float value)
{
    mfEQCurrentLowGain = MathUtils::map(value, 0.0f, 1.0f, MIN_GAIN, MAX_GAIN);
    mfEQLowGain = value;
    mEQL->setLowGain(mfEQCurrentLowGain);
    mEQR->setLowGain(mfEQCurrentLowGain);
}

void SorolletVoice::setEQMidGain(float value)
{
    mfEQCurrentMidGain = MathUtils::map(value, 0.0f, 1.0f, MIN_GAIN, MAX_GAIN);
    mfEQMidGain = value;
    mEQL->setMidGain(mfEQCurrentMidGain);
    mEQR->setMidGain(mfEQCurrentMidGain);
}

void SorolletVoice::setEQHiGain(float value)
{
    mfEQCurrentHiGain = MathUtils::map(value, 0.0f, 1.0f, MIN_GAIN, MAX_GAIN);
    mfEQHiGain = value;
    mEQL->setHiGain(mfEQCurrentHiGain);
    mEQR->setHiGain(mfEQCurrentHiGain);
}

float SorolletVoice::getEQLowFrequencyDisplay()
{
    return mfEQCurrentLowFrequency;
}

float SorolletVoice::getEQHighFrequencyDisplay()
{
    return mfEQCurrentHighFrequency;
}

float SorolletVoice::getEQLowGainDisplay()
{
    return mfEQCurrentLowGain;
}

float SorolletVoice::getEQMidGainDisplay()
{
    return mfEQCurrentMidGain;
}

float SorolletVoice::getEQHiGainDisplay()
{
    return mfEQCurrentHiGain;
}

void SorolletVoice::setAbsoluteClippingValue(float value)
{
	mfAbsoluteClippingValue = value;
}

void SorolletVoice::setVSTParameter(int index, float value)
{
    switch (index)
    {
        case PARAM_VOLUME1: setVolume1(value); break;
        case PARAM_OCTAVE1: setOctave1(floatToOctave(value)); break;
        case PARAM_WAVE1: setWave1(floatToWave(value)); break;
        case PARAM_PHASE1: setPhase1(value); break;
        case PARAM_VOLUME2: setVolume2(value); break;
        case PARAM_OCTAVE2: setOctave2(floatToOctave(value)); break;
        case PARAM_WAVE2: setWave2(floatToWave(value)); break;
        case PARAM_PHASE2: setPhase2(value); break;
        case PARAM_WAVE_MIX_TYPE: setWaveMixType(floatToWaveMixType(value)); break;
        case PARAM_NOISE_AMOUNT: setNoiseAmount(value); break;
        case PARAM_NOISE_MIX_TYPE: setNoiseMixType(floatToNoiseMixType(value)); break;
        case PARAM_AMP_ATTACK: mAmpADSR->setAttack(value); break;
        case PARAM_AMP_DECAY: mAmpADSR->setDecay(value); break;
        case PARAM_AMP_SUSTAIN: mAmpADSR->setSustainLevel(value); break;
        case PARAM_AMP_RELEASE: mAmpADSR->setRelease(value); break;
        case PARAM_AMP_TIME_SCALE: mAmpADSR->setTimeScale(floatToEnvelopeScale(value)); break;
        case PARAM_AMP_MIN_VALUE: mAmpADSR->setMinValue(value); break;
        case PARAM_AMP_MAX_VALUE: mAmpADSR->setMaxValue(value); break;
        case PARAM_PITCH_ATTACK: mPitchADSR->setAttack(value); break;
        case PARAM_PITCH_DECAY: mPitchADSR->setDecay(value); break;
        case PARAM_PITCH_SUSTAIN: mPitchADSR->setSustainLevel(value); break;
        case PARAM_PITCH_RELEASE: mPitchADSR->setRelease(value); break;
        case PARAM_PITCH_TIME_SCALE: mPitchADSR->setTimeScale(floatToEnvelopeScale(value)); break;
        case PARAM_PITCH_MIN_VALUE: mPitchADSR->setMinValue(value); break;
        case PARAM_PITCH_MAX_VALUE: mPitchADSR->setMaxValue(value); break;
        case PARAM_FILTER_TYPE: setFilterType(floatToFilterType(value)); break;
        case PARAM_FILTER_FREQUENCY: setFilterFrequency(value); break;
        case PARAM_FILTER_RESONANCE: setFilterResonance(value); break;
        case PARAM_SATURATE: setSaturate(value); break;
        case PARAM_EQ_ACTIVE: setEQActive(value > 0.5f); break;
        case PARAM_EQ_LOW_FREQUENCY: setEQLowFrequency(value); break;
        case PARAM_EQ_HIGH_FREQUENCY: setEQHighFrequency(value); break;
        case PARAM_EQ_LOW_GAIN: setEQLowGain(value); break;
        case PARAM_EQ_MID_GAIN: setEQMidGain(value); break;
        case PARAM_EQ_HI_GAIN: setEQHiGain(value); break;
    }
}

void SorolletVoice::setCurrentSamplePosition(double position)
{
    mlfCurrentSamplePosition = position;
    mfCurrentTime = (float) (position * mfInvSampleRate);
}

void SorolletVoice::sendNoteOn(int note, float volume)
{
    miPosition = 0;
    miCurrentNote = note;
    mfCurrentVolume = volume;
    float currTime = getTime();
    mAmpADSR->beginAttack(currTime);
    mPitchADSR->beginAttack(currTime);
}

void SorolletVoice::sendCurrentNoteVolume(float volume)
{
	mfCurrentVolume = volume;
}

void SorolletVoice::sendNoteOff()
{
    float currTime = getTime();
    mAmpADSR->beginRelease(currTime);
    mPitchADSR->beginRelease(currTime);
}

float SorolletVoice::noteToFrequency(float note, int octave)
{
    return (440.0f * powf(2.0f, ((note - 57.0f + (octave - 4.0f) * 12.0f) / 12.0f)));
}

int SorolletVoice::floatToOctave(float value)
{
    return ((int) (MathUtils::map(value, 0.0f, 1.0f, -1.0f, 9.0f)));
}

int SorolletVoice::floatToWave(float value)
{
	
    return (int) (MathUtils::map(value, 0.0f, 1.0f, 0, 3));
}

int SorolletVoice::floatToWaveMixType(float value)
{
    return (int) (MathUtils::map(value, 0.0f, 1.0f, 0, 3));
}

int SorolletVoice::floatToNoiseMixType(float value)
{
    return (int) (MathUtils::map(value, 0.0f, 1.0f, 0, 2));
}

float SorolletVoice::floatToEnvelopeScale(float value)
{
    return MathUtils::map(value, 0, 1, 1, 10);
}

int SorolletVoice::floatToFilterType(float value)
{
    return (int) (MathUtils::map(value, 0.0f, 1.0f, 0, 2));
}

