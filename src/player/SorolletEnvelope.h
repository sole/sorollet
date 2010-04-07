/* 
 * File:   SorolletEnvelope.h
 * Author: sole
 *
 * Created on 10 March 2010, 0:54
 */

#ifndef SOROLLET_ENVELOPE_H
#define SOROLLET_ENVELOPE_H

enum ENVELOPE_PLAY_MODES
{
	PLAY_MODE_POINTS,
	PLAY_MODE_LINEAR,
	PLAY_MODE_CUBIC
};

class SorolletEnvelopePoint
{
	public:
		SorolletEnvelopePoint();
		SorolletEnvelopePoint* searchByRow(int row);
		
		int row;
		float value;
		SorolletEnvelopePoint* prev;
		SorolletEnvelopePoint* next;
		
		
};

class SorolletEnvelope
{
	public:
		SorolletEnvelope();
		~SorolletEnvelope();
		void setAutomationParameterIndex(int index);
		int getAutomationParameterIndex();
		void setLength(int length);
		void setPointsLength(int pointsLength);
		void setPlayMode(int mode);
		void addPoint(int row, float value);
		float getValueAtRow(int row);
		
	private:
		int miAutomationParameterIndex; // TODO: strictly speaking, this shouldn't be here, but outside this class
		int miLength;
		int miPointsLength;
		int miPlayMode;
		
		SorolletEnvelopePoint* mPoints;
		int miLastPoint;
};

#endif
