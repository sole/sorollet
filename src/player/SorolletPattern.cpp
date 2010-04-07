#include "SorolletPattern.h"

#include <cstdlib>
#include <cstring>

#define MAX_ENVELOPES 14 // because it's the max number of parameters that can be automated with one automation device instance in renoise

SorolletPattern::SorolletPattern()
{
	miNumRows = 0;
	mRows = NULL;
	miNumTracks = 0;
	mTracksNumEnvelopes = NULL;
	mTracksEnvelopes = NULL;

	setDimensions(0, 0);
}

SorolletPattern::SorolletPattern(int numTracks, int numRows)
{
    setDimensions(numTracks, numRows);
}

SorolletPattern::~SorolletPattern()
{
	clearData();
}

void SorolletPattern::clearData()
{
	if(mRows != NULL)
	{
		for(int i = 0; i < miNumRows; i++)
		{
			delete [] (mRows[i]);
		}
		delete [] mRows;
		mRows = NULL;
	}

	if(mTracksNumEnvelopes != NULL)
	{
		delete [] mTracksNumEnvelopes;
		mTracksNumEnvelopes = NULL;
	}

	if(mTracksEnvelopes != NULL)
	{
		for(int i = 0; i < miNumTracks; i++)
		{
			delete [] (mTracksEnvelopes[i]);
		}
		delete [] mTracksEnvelopes;
		mTracksEnvelopes = NULL;
	}
}

void SorolletPattern::setDimensions(int numTracks, int numRows)
{
	clearData();

	if(numTracks > 0 && numRows > 0)
	{
		mRows = new SorolletPatternCell*[numRows];
		for(int i = 0; i < numRows; i++)
		{
			mRows[i] = new SorolletPatternCell[numTracks];
		}

		mTracksNumEnvelopes = new int[numTracks];
		for(int i = 0; i < numTracks; i++)
		{
			mTracksNumEnvelopes[i] = 0;
		}

		mTracksEnvelopes = new SorolletEnvelope**[numTracks];
		for(int i = 0; i < numTracks; i++)
		{
			mTracksEnvelopes[i] = new SorolletEnvelope*[MAX_ENVELOPES];
		}
	}
    
    miNumRows = numRows;
    miNumTracks = numTracks;
}

SorolletPatternCell* SorolletPattern::getRowData(int rowNumber)
{
	return mRows[rowNumber];
}

SorolletPatternCell* SorolletPattern::getCell(int row, int track)
{
	if(row < this->miNumRows && track < this->miNumTracks)
	{
		return &mRows[row][track];
	}
	else
	{
		return NULL;
	}
}

int SorolletPattern::getRowsNumber()
{
    return miNumRows;
}

void SorolletPattern::addTrackEnvelope(int trackNumber, SorolletEnvelope* envelope)
{
	if(mTracksNumEnvelopes[trackNumber] >= MAX_ENVELOPES)
	{
		return;
	}
	
	int last = mTracksNumEnvelopes[trackNumber];
	
	mTracksEnvelopes[trackNumber][last] = envelope;
		
	mTracksNumEnvelopes[trackNumber]++;
}

SorolletEnvelope** SorolletPattern::getTrackEnvelopes(int trackNumber)
{
	return (mTracksEnvelopes[trackNumber]);
}

int SorolletPattern::getTrackEnvelopesNumber(int trackNumber)
{
	return mTracksNumEnvelopes[trackNumber];
}

bool SorolletPattern::trackHasEnvelopes(int trackNumber)
{
	if(trackNumber > miNumTracks) return false;
	
	return (mTracksNumEnvelopes[trackNumber] > 0);
}
