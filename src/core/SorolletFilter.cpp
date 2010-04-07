#include "SorolletFilter.h"
#include "../sys/MathUtils.h"
#include <cmath>

SorolletFilter::SorolletFilter(int type, float freq, float res)
{
    setType(type);
    frequency = freq;
    resonance = res;

    prepareFilter();
    resetMemory();
}

void SorolletFilter::setType(int type)
{
    miType = type;
    prepareFilter();
}

void SorolletFilter::setFrequency(float value)
{
    frequency = value;
    prepareFilter();
}

void SorolletFilter::setResonance(float value)
{
    resonance = value;
    prepareFilter(); // TODO ??? not sure
}

void SorolletFilter::setInverseSamplingRate(float isr)
{
    mfInverseSamplingRate = isr;
}

float SorolletFilter::filterValue(float value)
{
    if(miType != FILTER_NONE)
    {
        //out(n) = a1 * in + a2 * in(n-1) + a3 * in(n-2) - b1*out(n-1) - b2*out(n-2)
        xn[0] = value;
        yn[0] = a1 * xn[0] + a2 * xn[1] + a3 * xn[2] - b1 * yn[1] - b2 * yn[2];

        xn[2] = xn[1];
        xn[1] = xn[0];
        yn[2] = yn[1];
        yn[1] = yn[0];

        return yn[0];
    }
    else
    {
        // Bypass
        return value;
    }
}

void SorolletFilter::resetMemory()
{
    int i;

    for(i = 0; i < 3; i++)
    {
        xn[i] = 0.0f;
        yn[i] = 0.0f;
    }
}

/**
 * Should be called whenever the filter frequency/type changes
 */
void SorolletFilter::prepareFilter()
{
    if(miType == FILTER_LOW_PASS)
    {
        c = 1.0f / tanf(M_PI * frequency * mfInverseSamplingRate);
        csquare = c * c;
        a1 = 1.0f / ( 1.0f + resonance * c + csquare);
        a2 = 2.0f * a1;
        a3 = a1;
        b1 = 2.0f * ( 1.0f - csquare) * a1;
        b2 = ( 1.0f - resonance * c + csquare) * a1;
    }
    else if(miType == FILTER_HIGH_PASS)
    {
        c = tanf(M_PI * frequency * mfInverseSamplingRate);
        csquare = c * c;
        a1 = 1.0f / ( 1.0f + resonance * c + csquare);
        a2 = -2.0f * a1;
        a3 = a1;
        b1 = 2.0f * ( csquare - 1.0f) * a1;
        b2 = ( 1.0f - resonance * c + csquare) * a1;
    }
}
