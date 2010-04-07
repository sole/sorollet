#include "SorolletEnvelope.h"
#include "../sys/MathUtils.h"
#include <cstdlib>

SorolletEnvelopePoint::SorolletEnvelopePoint()
{
	row = 0;
	value = 0;
	prev = NULL;
	next = NULL;
}

SorolletEnvelopePoint* SorolletEnvelopePoint::searchByRow(int row)
{
	if(this->row == row)
	{
		return this;
	}
	// points are sorted, if we are asked for something less than our own value it doesn't exist anywhere
	else if(this->row > row)
	{
		return NULL;
	}
	else if(this->next == NULL)
	{
		return NULL;
	}
	else return this->next->searchByRow(row);
}


SorolletEnvelope::SorolletEnvelope()
{
	setAutomationParameterIndex(0);
	mPoints = NULL;
	setLength(0);
	setPointsLength(0);
	setPlayMode(PLAY_MODE_LINEAR);
}

SorolletEnvelope::~SorolletEnvelope()
{
	if(mPoints != NULL)
	{
		delete [] mPoints;
	}
}

void SorolletEnvelope::setAutomationParameterIndex(int index)
{
	miAutomationParameterIndex = index;
}

int SorolletEnvelope::getAutomationParameterIndex()
{
	return miAutomationParameterIndex;
}

void SorolletEnvelope::setPointsLength(int pointsLength)
{
	if(mPoints != NULL)
	{
		delete [] mPoints;
	}
	
	miPointsLength = pointsLength;
	mPoints = new SorolletEnvelopePoint[miPointsLength];
	miLastPoint = 0;
}

void SorolletEnvelope::setLength(int length)
{
	miLength = length;
}

void SorolletEnvelope::setPlayMode(int mode)
{
	miPlayMode = mode;
}

void SorolletEnvelope::addPoint(int row, float value)
{
	if(miLastPoint < miPointsLength)
	{
		mPoints[miLastPoint].row = row;
		mPoints[miLastPoint].value = value;
		
		if(miLastPoint > 0)
		{
			int prevPoint = miLastPoint - 1;
			mPoints[miLastPoint].prev = &(mPoints[prevPoint]);
			mPoints[prevPoint].next = &(mPoints[miLastPoint]);
		}
		
		miLastPoint++;
	}
}

float SorolletEnvelope::getValueAtRow(int row)
{
	float value = 0.0f;
	
	if(PLAY_MODE_POINTS == miPlayMode)
	{
		SorolletEnvelopePoint* p = mPoints[0].searchByRow(row); // start searching from the first point
		if(p != NULL)
		{
			return p->value;
		}
		else
		{
			int i;
			for(i = 0; i < miLastPoint; i++)
			{
				int pointRow = mPoints[i].row;
				if(i < pointRow)
				{
					value = mPoints[i].value;
				}
				else if(i > pointRow)
				{
					return value;
				}
			}
		}
	}
	else // if(PLAY_MODE_LINEAR == miPlayMode) // Linear and cubic are the same for the time being (AKA cubic not implemented)
	{
		SorolletEnvelopePoint* p = mPoints[0].searchByRow(row);
		if(p != NULL)
		{
			return p->value;
		}
		else
		{
			if(miPointsLength == 1) { return mPoints[0].value; }
			
			// Interpolate
			SorolletEnvelopePoint *p1, *p2;
			
			p1 = &(mPoints[0]);
			
			while(p1 != NULL && p1->next != NULL && (p1->next->row <= row))
			{
				p1 = p1->next;
			}
			
			p2 = p1->next;
			
			if(p2 != NULL)
			{
				while(p2 != NULL && p2->next != NULL && (p2->next->row <= row))
				{
					p2 = p2->next;
				}
				
				value = MathUtils::map((float) row, (float) p1->row, (float) p2->row, p1->value, p2->value);
				
				return value;
			}
			else
			{
				value = p1->value;
			}
			
		}
		
	}
	
	return value; 
}