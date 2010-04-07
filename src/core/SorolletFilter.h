/* 
 * File:   sorollet_filter.h
 * Author: sole
 *
 * Created on 16 February 2010, 22:13
 */

#ifndef _SOROLLET_FILTER_H
#define	_SOROLLET_FILTER_H

enum filter_types
{
    FILTER_NONE = 0,
    FILTER_LOW_PASS,
    FILTER_HIGH_PASS
};

class SorolletFilter
{
public:
    SorolletFilter(int type, float frequency, float resonance);
    void setType(int type);
    void setFrequency(float frequency);
    void setResonance(float resonance);
    void setInverseSamplingRate(float isr);
    float filterValue(float value);

protected:
    void prepareFilter();
    void resetMemory();
    int     miType;
    float   mfInverseSamplingRate;
    float   frequency;
    float   resonance;
    float   xn[3];
    float   yn[3];
    float   a1, a2, a3, b1, b2, c, csquare;
};

#endif	/* _SOROLLET_FILTER_H */

