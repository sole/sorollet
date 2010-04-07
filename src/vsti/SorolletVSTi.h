#ifndef __SorolletVSTi__
#define __SorolletVSTi__

#ifdef __GNUC__
	#define __cdecl
#endif

#include "public.sdk/source/vst2.x/audioeffectx.h"

#include "../core/SorolletVoice.h"
#include "SorolletVSTiProgram.h"

enum {
    // Global
    kNumPrograms = 16,
    kNumOutputs = 2,
    kNumParams = 34
};

enum eventTypes
{
    EVENT_NULL = 0,
    EVENT_NOTE_ON,
    EVENT_NOTE_OFF
};

class SorolletVSTi : public AudioEffectX {
public:
    SorolletVSTi(audioMasterCallback audioMaster);
    ~SorolletVSTi();

    virtual void suspend();

    // Processing
    virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);
    virtual VstInt32 processEvents(VstEvents* events);

    // Program
    virtual void setProgram(VstInt32 program);
    virtual void setProgramName(char* name);
    virtual void getProgramName(char* name);
    virtual bool getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text);

    // Parameters
    virtual void setParameter(VstInt32 index, float value);
    virtual float getParameter(VstInt32 index);
    virtual void getParameterLabel(VstInt32 index, char* label);
    virtual void getParameterDisplay(VstInt32 index, char* text);
    virtual void getParameterName(VstInt32 index, char* text);

    virtual void setSampleRate(float sampleRate);
    virtual void setBlockSize(VstInt32 blockSize);

    virtual bool getOutputProperties(VstInt32 index, VstPinProperties* properties);

    virtual bool getEffectName(char* name);
    virtual bool getVendorString(char* text);
    virtual bool getProductString(char* text);
    virtual VstInt32 getVendorVersion();
    virtual VstInt32 canDo(char* text);

    virtual VstInt32 getNumMidiInputChannels();
    virtual VstInt32 getNumMidiOutputChannels();

protected:
    
    float fVolume1;
    float fOctave1;
    float fWave1;
    float fPhase1;
    float fVolume2;
    float fOctave2;
    float fWave2;
    float fPhase2;
    float fWaveMixType;
    float fNoiseAmount;
    float fNoiseMixType;
    float fAmpTimeScale;
    float fAmpAttack;
    float fAmpDecay;
    float fAmpSustain;
    float fAmpRelease;
    float fAmpMinValue;
    float fAmpMaxValue;
    float fPitchTimeScale;
    float fPitchAttack;
    float fPitchDecay;
    float fPitchSustain;
    float fPitchRelease;    
    float fPitchMinValue;
    float fPitchMaxValue;
    float fFilterType;
    float fFilterFrequency;
    float fFilterResonance;
    float fSaturate;
    float fEQActive;
    float fEQLowFrequency;
    float fEQHighFrequency;
    float fEQLowGain;
    float fEQMidGain;
    float fEQHiGain;

    SorolletVoice *mSorolletVoice;
    SorolletVSTiProgram* programs;
    VstInt32 channelPrograms[16];

    VstInt32 currentEvent;
    VstInt32 currentNote;
    VstInt32 currentVelocity;
    VstInt32 currentDelta;
    bool noteIsOn;

    void initProcess();
    void noteOn(VstInt32 note, VstInt32 velocity, VstInt32 delta);
    void noteOff(VstInt32 delta);
    void fillProgram(VstInt32 channel, VstInt32 prg, MidiProgramName* mpn);

    // Aux, to communicate back and forth between SorolletVoice
    void formatTime(float timeInSeconds, char* text);
    int floatToOctave(float value);
    int floatToWave(float value);
    void floatToWaveName(float value, char* text);
    int floatToWaveMixType(float value);
    void floatToWaveMixTypeName(float value, char* text);
    int floatToNoiseMixType(float value);
    void floatToNoiseMixTypeName(float value, char* text);
    float floatToEnvelopeScale(float value);
    int floatToFilterType(float value);
    void floatToFilterTypeName(float value, char* text);
};

#endif
