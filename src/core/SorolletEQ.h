/* 
 * File:   sorollet_eq.h
 * Author: sole
 *
 * Created on 17 February 2010, 20:23
 */

#ifndef _SOROLLET_EQ_H
#define	_SOROLLET_EQ_H

// http://www.musicdsp.org/archive.php?classid=3#236
class SorolletEQ
{
public:
    SorolletEQ();
    void prepare();
    float update(float sample);
    void setLowFreq(float frequency);
    void setHighFreq(float frequency);
    void setLowGain(float gain);
    void setMidGain(float gain);
    void setHiGain(float gain);
    void setActive(bool value);
    void setSampleRate(float rate);

protected:
    
    bool isActive;

    static float verySmallAmount;
    float mfLowFreq;
    float mfHighFreq;
    float mfSampleRate;

    // Filter #1 (Low band)

    float  lf;       // Frequency
    float  f1p0;     // Poles ...
    float  f1p1;
    float  f1p2;
    float  f1p3;

    // Filter #2 (High band)

    float  hf;       // Frequency
    float  f2p0;     // Poles ...
    float  f2p1;
    float  f2p2;
    float  f2p3;

    // Sample history buffer

    float  sdm1;     // Sample data minus 1
    float  sdm2;     //                   2
    float  sdm3;     //                   3

    // Gain Controls

    float  lg;       // low  gain
    float  mg;       // mid  gain
    float  hg;       // high gain
};

#endif	/* _SOROLLET_EQ_H */

