/* 
 * File:   SorolletPlayer.h
 * Author: sole
 *
 * Created on 19 February 2010, 20:39
 */

#ifndef _SOROLLET_PLAYER_H
#define	_SOROLLET_PLAYER_H

#include "../core/SorolletVoice.h"
#include "SorolletPattern.h"
#include "SorolletPlayerEvent.h"
#include "SorolletDeviceAutomation.h"

class SorolletPlayer
{
public:
    SorolletPlayer(float samplingRate, int bufferSize);
    ~SorolletPlayer();
    int loadSongFromArray(float* data);
    bool getBuffer(float *outL, float *outR, int numSamples);
    float getTimePosition();
    int getCurrentOrder();
    int getCurrentPattern();
    int getCurrentRow();

protected:
    void updateRowTiming();
    void addEvent(SorolletPlayerEvent* event);
    void buildEventList();
    void buildArpeggios(SorolletPatternCell* pCell, float timestamp, long timestampSamples, float secondsPerRow, int samplesPerRow, int instrument, int note, float volume);
    void processBuffer(float *outL, float *outR, int numSamples);

    float mfSamplingRate;
    float mfInvSamplingRate;
    int miBufferSize;
    float* mBufferL;
    float* mBufferR;
    float* mTmpBufferL;
    float* mTmpBufferR;
    float mfTimePosition;
    long mlPosition;

    int miNumEvents;
    SorolletPlayerEvent** mEventList;

    int miNextEventPosition;

    // Song stuff
    int miBPM;
    int miLinesPerBeat;
    int miTicksPerLine;
    float mfSecondsPerRow;
    float mfSecondsPerTick;
    float mfLastRowTime;
    int miCurrentRow;
    int miCurrentOrder;
    int miCurrentPattern;
    int miTrackCount;

    int miVoiceCount;
    SorolletVoice* mVoices;
    SorolletDeviceAutomation* mTracksAutomationDevices;

    int miPatternCount;
    SorolletPattern* mPatterns;

    int miOrderListCount;
    int* mOrderList;

};

#endif	/* _SOROLLET_PLAYER_H */

