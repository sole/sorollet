/* 
 * File:   SorolletPlayerEvent.h
 * Author: sole
 *
 * Created on 26 February 2010, 23:36
 */

#ifndef _SOROLLETPLAYEREVENT_H
#define	_SOROLLETPLAYEREVENT_H


enum sorollet_player_event_types
{
    SOROLLET_EVENT_NULL = 0,
    SOROLLET_EVENT_NOTE_OFF,
    SOROLLET_EVENT_NOTE_ON,
    SOROLLET_EVENT_VOLUME,
    SOROLLET_EVENT_EFFECT,
    SOROLLET_EVENT_ROW_CHANGE,
    SOROLLET_EVENT_PATTERN_CHANGE,
    SOROLLET_EVENT_ORDER_POSITION_CHANGE,
    SOROLLET_EVENT_SONG_END
};

class SorolletPlayerEvent
{
public:
    SorolletPlayerEvent();
    void reset();

    float timestamp;
    long timestampSamples;
    int type;
    int note;
    int instrument;
    float volume;
    int track;
    int effect;
    int effectValue;
    int row;
    int pattern;
    int orderPosition;
};

#endif	/* _SOROLLETPLAYEREVENT_H */

