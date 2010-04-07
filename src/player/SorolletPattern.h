/* 
 * File:   SorolletPattern.h
 * Author: sole
 *
 * Created on 22 February 2010, 20:38
 */

#ifndef _SOROLLETPATTERN_H
#define	_SOROLLETPATTERN_H

#include "SorolletPatternCell.h"
#include "SorolletEnvelope.h"

class SorolletPattern
{

public:
    SorolletPattern();
    SorolletPattern(int numTracks, int numRows);
    ~SorolletPattern();
    void setDimensions(int numTracks, int numRows);
    SorolletPatternCell* getRowData(int rowNumber);
    SorolletPatternCell* getCell(int row, int track);
    int getRowsNumber();
    void addTrackEnvelope(int trackNumber, SorolletEnvelope* envelope);
    SorolletEnvelope** getTrackEnvelopes(int trackNumber);
    int getTrackEnvelopesNumber(int trackNumber);
    bool trackHasEnvelopes(int trackNumber);

protected:
    int miNumRows;
    int miNumTracks;
    SorolletPatternCell** mRows;
    void clearData();
    int* mTracksNumEnvelopes;
    SorolletEnvelope*** mTracksEnvelopes;

};

#endif	/* _SOROLLETPATTERN_H */

