/* 
 * File:   sorollet_adsr.h
 * Author: sole
 *
 * Created on 10 February 2010, 23:26
 */

#ifndef _SOROLLET_ADSR_H
#define	_SOROLLET_ADSR_H

enum adsr_states
{
    ADSR_ATTACK = 0,
    ADSR_DECAY,
    ADSR_SUSTAIN,
    ADSR_RELEASE,
    ADSR_DONE
};

class SorolletADSR
{
public:
    SorolletADSR(float attackTime, float decayTime, float sustainLevel, float releaseTime, float timeScale);
    void setAttack(float time);
    void setDecay(float time);
    void setSustainLevel(float level);
    void setRelease(float time);
    void setTimeScale(float scale);
    void setMinValue(float value);
    void setMaxValue(float value);
    void setAcceptableMinMaxValues(float minV, float maxV);
    void beginAttack(float startTime);
    void beginRelease(float releaseStartTime);
    float update(float time);
    float getValue();
    float getAttackTime();
    float getDecayTime();
    float getReleaseTime();
    float getDisplayMinValue();
    float getDisplayMaxValue();
    int getState();

protected:

    float mfBaseAttack;
    float mfBaseDecay;
    float mfBaseRelease;
    
    float mfAttackTime;
    float mfDecayTime;
    float mfSustainLevel;
    float mfReleaseTime;
    float mfTimeScale;
    float mfMinValue;
    float mfMaxValue;
    float mfRealMinValue;
    float mfRealMaxValue;
    float mfAcceptableMinValue;
    float mfAcceptableMaxValue;

    // Derived values
    float mfAttackEndTime;
    float mfDecayEndTime;
    float mfReleaseEndTime;
    // ~~~~~~~~

    float mfValue;
    float mfStartTime;
    float mfReleaseStartTime;

    int miState;
};

#endif	/* _SOROLLET_ADSR_H */

