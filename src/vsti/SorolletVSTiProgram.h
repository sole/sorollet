/* 
 * File:   SorolletVSTiProgram.h
 * Author: sole
 *
 * Created on 07 February 2010, 18:30
 */

#ifndef _SOROLLET_VSTI_PROGRAM_H
#define	_SOROLLET_VSTI_PROGRAM_H

#include "public.sdk/source/vst2.x/audioeffectx.h"

class SorolletVSTiProgram {

    public:
        SorolletVSTiProgram();
        ~SorolletVSTiProgram() {}

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

        // Envelopes
        float fAmpAttack;
        float fAmpDecay;
        float fAmpSustain;
        float fAmpRelease;
        float fAmpTimeScale;
        float fAmpMinValue;
        float fAmpMaxValue;

        float fPitchAttack;
        float fPitchDecay;
        float fPitchSustain;
        float fPitchRelease;
        float fPitchTimeScale;
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

        char name[kVstMaxProgNameLen + 1];
};

#endif

