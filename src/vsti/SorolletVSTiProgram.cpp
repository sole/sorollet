
#include "SorolletVSTiProgram.h"

SorolletVSTiProgram::SorolletVSTiProgram()
{
    // Default Program Values
    
    fVolume1 = 0.5f;
    fOctave1 = 0.5f; // in the middle
    fWave1 = 0.0f; // First wave
    fPhase1 = 0.5f;
    fVolume2 = 0.5f;
    fOctave2 = 0.2f;
    fWave2 = 1.0f; // Last wave
    fPhase2 = 0.5f;
    fWaveMixType = 0.0f; // Add
    fNoiseAmount = 0.0f; // No noise
    fNoiseMixType = 0.0f; // Additive noise

    fAmpAttack = 0.0f;
    fAmpDecay = 0.0f;
    fAmpSustain = 0.8f;
    fAmpRelease = 0.0f;
    fAmpTimeScale = 1.0f;
    fAmpMinValue = 0.0f;
    fAmpMaxValue = 0.66f; // Not at the maximum -- otherwise it saturates like hell!

    fPitchAttack = 0.0f;
    fPitchDecay = 0.0f;
    fPitchSustain = 0.5f;
    fPitchRelease = 0.0f;
    fPitchTimeScale = 1.0f;
    fPitchMinValue = 0.0f;
    fPitchMaxValue = 1.0f;

    fFilterType = 0.0f;
    fFilterFrequency = 0.0f;
    fFilterResonance = 0.0f;

    fSaturate = 0.0f;

    fEQActive = 0.0f;
    fEQLowFrequency = 0.5f;
    fEQHighFrequency = 0.5f;
    fEQLowGain = 1.0f;
    fEQMidGain = 1.0f;
    fEQHiGain = 1.0f;

    vst_strncpy(name, "Default", kVstMaxProgNameLen);
}
