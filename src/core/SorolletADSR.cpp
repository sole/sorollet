#include "SorolletADSR.h"
#include "../sys/MathUtils.h"

#define MIN_VALUE 0.0001f

SorolletADSR::SorolletADSR(float attackTime, float decayTime, float sustainLevel, float releaseTime, float timeScale)
{
    setTimeScale(timeScale);
    setAttack(attackTime);
    setDecay(decayTime);
    setSustainLevel(sustainLevel);
    setRelease(releaseTime);
    setMinValue(0.0f);
    setMaxValue(1.0f);

    miState = ADSR_ATTACK;
}

void SorolletADSR::setAttack(float value)
{
    mfBaseAttack = value;
    mfAttackTime = mfBaseAttack * mfTimeScale;
}

void SorolletADSR::setDecay(float value)
{
    mfBaseDecay = value;
    mfDecayTime = mfBaseDecay * mfTimeScale;
    mfDecayEndTime = mfAttackEndTime + mfDecayTime;
}

void SorolletADSR::setSustainLevel(float level)
{
    mfSustainLevel = level;
}

void SorolletADSR::setRelease(float value)
{
    mfBaseRelease = value;
    mfReleaseTime = mfBaseRelease * mfTimeScale;
    mfReleaseEndTime = mfReleaseStartTime + mfReleaseTime;
}

void SorolletADSR::setTimeScale(float scale)
{
    mfTimeScale = scale;
    // --> update times
    mfAttackTime = mfBaseAttack * mfTimeScale;
    mfDecayTime = mfBaseDecay * mfTimeScale;
    mfReleaseTime = mfBaseRelease * mfTimeScale;
}

void SorolletADSR::setMinValue(float value)
{
    mfMinValue = value;
    mfRealMinValue = MathUtils::map(mfMinValue, 0.0f, 1.0f, mfAcceptableMinValue, mfAcceptableMaxValue);
}

void SorolletADSR::setMaxValue(float value)
{
    mfMaxValue = value;
    mfRealMaxValue = MathUtils::map(mfMaxValue, 0.0f, 1.0f, mfAcceptableMinValue, mfAcceptableMaxValue);
}

void SorolletADSR::setAcceptableMinMaxValues(float minV, float maxV)
{
    mfAcceptableMinValue = minV;
    mfAcceptableMaxValue = maxV;
}

float SorolletADSR::getAttackTime()
{
    return mfAttackTime;
}

float SorolletADSR::getDecayTime()
{
    return mfDecayTime;
}

float SorolletADSR::getReleaseTime()
{
    return mfReleaseTime;
}

float SorolletADSR::getDisplayMinValue()
{
    return mfRealMinValue;
}

float SorolletADSR::getDisplayMaxValue()
{
    return mfRealMaxValue;
}

void SorolletADSR::beginAttack(float startTime)
{
    miState = ADSR_ATTACK;
    mfStartTime = startTime;
    mfAttackEndTime = mfStartTime + mfAttackTime;
    mfDecayEndTime = mfAttackEndTime + mfDecayTime;
    mfValue = mfRealMinValue;
}

void SorolletADSR::beginRelease(float releaseStartTime)
{
    miState = ADSR_RELEASE;
    mfReleaseStartTime = releaseStartTime;
    mfReleaseEndTime = mfReleaseStartTime + mfReleaseTime;
}

float SorolletADSR::update(float time)
{
    float realSustainLevel;
    
    
    if(miState != ADSR_ATTACK || miState != ADSR_DONE)
    {
        realSustainLevel = MathUtils::map(mfSustainLevel, 0.0f, 1.0f, mfRealMinValue, mfRealMaxValue);
    }

   
    // Update state ~~~
    // Note how we don't switch to release here because that only happens
    // when we get a key_off/release event
	// (and the change is triggered from the outside of this class)
    if((miState == ADSR_ATTACK) && (time >= mfAttackEndTime))
    {
            miState = ADSR_DECAY;
    }
    else if((miState == ADSR_DECAY) && (time >= mfDecayEndTime))
    {
            miState = ADSR_SUSTAIN;
    }
    else if((miState == ADSR_RELEASE) && (time >= mfReleaseEndTime))
    {
            miState = ADSR_DONE;
    }

    // and calculate the value
    switch(miState)
    {
        case ADSR_ATTACK:
                mfValue = MathUtils::map(time, mfStartTime, mfAttackEndTime, mfRealMinValue, mfRealMaxValue);
                break;
        case ADSR_DECAY:
                mfValue = MathUtils::map(time, mfAttackEndTime, mfDecayEndTime, mfRealMaxValue, realSustainLevel);
                break;
        case ADSR_SUSTAIN:
                mfValue = realSustainLevel;
                break;
        case ADSR_RELEASE:
                mfValue = MathUtils::map(time, mfReleaseStartTime, mfReleaseEndTime, realSustainLevel, mfRealMinValue);
                break;
        case ADSR_DONE:
                mfValue = mfRealMinValue;
    }

    return mfValue;
}

float SorolletADSR::getValue()
{
    return mfValue;
}

int SorolletADSR::getState()
{
    return miState;
}
