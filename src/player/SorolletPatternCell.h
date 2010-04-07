/* 
 * File:   SorolletPatternCell.h
 * Author: sole
 *
 * Created on 22 February 2010, 20:43
 */

#ifndef _SOROLLETPATTERNCELL_H
#define	_SOROLLETPATTERNCELL_H

class SorolletPatternCell
{
public:
    SorolletPatternCell();
    inline void setNote(int value) { note = value; }
    inline int getNote() { return note; }
    inline void setInstrument(int value) { instrument = value; }
    inline int getInstrument() { return instrument; }
    inline void setVolume(int value) { volume = value; }
    inline int getVolume() { return volume; }
    inline void setEffect(int value) { effect = value; }
    inline int getEffect() { return effect; }
    inline void setEffectParameter(int value) { effectParameter = value; }
    inline int getEffectParameter() { return effectParameter; }

protected:
    int note;
    int instrument;
    int volume;
    int effect;
    int effectParameter;
};

#endif	/* _SOROLLETPATTERNCELL_H */

