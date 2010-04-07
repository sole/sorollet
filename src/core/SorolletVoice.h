#ifndef SOROLLET_VOICE_H
#define SOROLLET_VOICE_H

#include "SorolletADSR.h"
#include "SorolletFilter.h"
#include "SorolletEQ.h"

enum waves
{
    WAVE_SINE = 0, //0
    WAVE_TRIANGLE, // 1
    WAVE_SQUARE, // 2
    WAVE_SAWTOOTH // 3
};

enum wave_mix_type
{
    WAVE_ADD = 0,
    WAVE_SUBSTRACT,
    WAVE_MULTIPLY,
    WAVE_DIVIDE
};

enum noise_mix_type
{
    NOISE_ADD = 0,
    NOISE_MIX,
    NOISE_MULTIPLY
};

enum vst_param_names
{
    PARAM_VOLUME1 = 0,
    PARAM_OCTAVE1,
    PARAM_WAVE1,
    PARAM_PHASE1,
    PARAM_VOLUME2,
    PARAM_OCTAVE2,
    PARAM_WAVE2,
    PARAM_PHASE2,
    PARAM_WAVE_MIX_TYPE,
    PARAM_NOISE_AMOUNT,
    PARAM_NOISE_MIX_TYPE,
    PARAM_AMP_TIME_SCALE,
    PARAM_AMP_ATTACK,
    PARAM_AMP_DECAY,
    PARAM_AMP_SUSTAIN,
    PARAM_AMP_RELEASE,
    PARAM_AMP_MIN_VALUE,
    PARAM_AMP_MAX_VALUE,
    PARAM_PITCH_TIME_SCALE,
    PARAM_PITCH_ATTACK,
    PARAM_PITCH_DECAY,
    PARAM_PITCH_SUSTAIN,
    PARAM_PITCH_RELEASE,
    PARAM_PITCH_MIN_VALUE,
    PARAM_PITCH_MAX_VALUE,
    PARAM_FILTER_TYPE,
    PARAM_FILTER_FREQUENCY,
    PARAM_FILTER_RESONANCE,
    PARAM_SATURATE,
    PARAM_EQ_ACTIVE,
    PARAM_EQ_LOW_FREQUENCY,
    PARAM_EQ_HIGH_FREQUENCY,
    PARAM_EQ_LOW_GAIN,
    PARAM_EQ_MID_GAIN,
    PARAM_EQ_HI_GAIN,
};

class SorolletVoice
{
public:
    SorolletVoice();
    ~SorolletVoice();
    void setBufferLength(int newBufferLength);
    void setSampleRate(int newSampleRate);
    void resetSamplePosition();

    bool isActive();
    void getBuffer(float *outL, float *outR, int numSamples);
    SorolletADSR* getAmpADSR();
    SorolletADSR* getPitchADSR();
    SorolletADSR* getFilterFreqADSR();

    void setVolume1(float volume);
    void setOctave1(int octave);
    void setWave1(int waveType);
    void setPhase1(float phase);
    void setVolume2(float volume);
    void setOctave2(int octave);
    void setWave2(int waveType);
    void setPhase2(float phase);
    void setWaveMixType(int waveMixType);
    void setNoiseAmount(float amount);
    void setNoiseMixType(int noiseMixType);
    void setFilterType(int type);
    void setFilterFrequency(float frequency);
    void setFilterResonance(float resonance);
    float getFilterFrequencyDisplay();
    float getFilterResonanceDisplay();
    
    void setSaturate(float value);

    void setEQActive(bool active);
    void setEQLowFrequency(float value);
    void setEQHighFrequency(float value);
    void setEQLowGain(float value);
    void setEQMidGain(float value);
    void setEQHiGain(float value);
    float getEQLowFrequencyDisplay();
    float getEQHighFrequencyDisplay();
    float getEQLowGainDisplay();
    float getEQMidGainDisplay();
    float getEQHiGainDisplay();
    
    void setAbsoluteClippingValue(float value);

    void setVSTParameter(int index, float value);

    void setCurrentSamplePosition(double position);
    void sendNoteOn(int note, float volume);
    void sendCurrentNoteVolume(float volume);
    void sendNoteOff();
    float noteToFrequency(float note, int octave);

    // Aux!
     int floatToOctave(float value);
    int floatToWave(float value);
    int floatToWaveMixType(float value);
    int floatToNoiseMixType(float value);
    float floatToEnvelopeScale(float value);
    int floatToFilterType(float value);

protected:
    double mlfCurrentSamplePosition;
    double mlfInternalSamplePosition;
    float mfCurrentTime; // (derived from above)

    int miCurrentNote;
    float mfCurrentVolume;

    float mfVolume1;
    int miOctave1;
    int miWave1;
    float mfPhase1;
    float mfPhaseRadians1;
    float mfVolume2;
    int miOctave2;
    int miWave2;
    float mfPhase2;
    float mfPhaseRadians2;
    int miWaveMixType;
    float mfNoiseAmount;
    int miNoiseMixType;
    float mfFilterFrequency;
    float mfFilterFrequencyHertz;
    float mfFilterResonance;
    float mfFilterResonanceHertz;
    float mfSaturate;

    float mfEQLowFrequency;
    float mfEQHighFrequency;
    float mfEQLowGain;
    float mfEQMidGain;
    float mfEQHiGain;
    float mfEQCurrentLowFrequency;
    float mfEQCurrentHighFrequency;
    float mfEQCurrentLowGain;
    float mfEQCurrentMidGain;
    float mfEQCurrentHiGain;
    
    float mfAbsoluteClippingValue;

    SorolletADSR *mAmpADSR;
    SorolletADSR *mPitchADSR;
    SorolletADSR *mFilterFreqADSR;

    SorolletFilter *mFilterL;
    SorolletFilter *mFilterR;

    SorolletEQ *mEQL;
    SorolletEQ *mEQR;

    float mfSampleRate;
    float mfInvSampleRate;
    
    int miPosition;

    int miBufferLength;
    
    float* mBufferOSC1L;
    float* mBufferOSC1R;
    float* mBufferOSC2L;
    float* mBufferOSC2R;
    float* mBufferTmpL;
    float* mBufferTmpR;
    float* mBufferNoiseL;
    float* mBufferNoiseR;
    float* mBufferAmp;
    float* mBufferPitch1;
    float* mBufferPitch2;

    void getSineBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase);
    void getTriangleBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase);
    void getSquareBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase);
    void getSawtoothBuffer(float* outL, float* outR, int numSamples, float position, float frequency, float phase);
    void getNoiseBuffer(float* outL, float* outR, int numSamples);

    float saturateValue(float value);

    void prepareBuffers();

    float getTime();
};

#endif
