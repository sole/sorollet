#include "SorolletPlayer.h"
#include "../core/SorolletConstants.h"
#include "../sys/MathUtils.h"

#include <cstring>
#include <cstdlib>

#define MAX_EVENTS 15000

SorolletPlayer::SorolletPlayer(float samplingRate, int bufferSize)
{
	mfSamplingRate = samplingRate;
	mfInvSamplingRate = 1.0f / mfSamplingRate;
	miBufferSize = bufferSize;
	mBufferL = (float*) new float[bufferSize];
	mBufferR = (float*) new float[bufferSize];
	mTmpBufferL = (float*) new float[bufferSize];
	mTmpBufferR = (float*) new float[bufferSize];
	mfTimePosition = 0.0f;
	mlPosition = 0;
	mfLastRowTime = 0.0f;
	miCurrentOrder = 0;
	miCurrentRow = 0;
	mVoices = NULL;
	mTracksAutomationDevices = NULL;
	mPatterns = NULL;
	mOrderList = NULL;
	mEventList = NULL;
	miNumEvents = 0;
}

SorolletPlayer::~SorolletPlayer()
{
	delete[] mBufferL;
	delete[] mBufferR;
	delete[] mTmpBufferL;
	delete[] mTmpBufferR;

	if (mVoices != NULL)
	{
		delete[] mVoices;
	}

	if (mTracksAutomationDevices != NULL)
	{
		delete[] mTracksAutomationDevices;
	}

	if (mPatterns != NULL)
	{
		delete[] mPatterns;
	}

	if (mOrderList != NULL)
	{
		delete[] mOrderList;
	}

	if (mEventList != NULL)
	{
		for(int i = 0; i < miNumEvents; i++)
		{
			delete mEventList[i];
		}
		delete[] mEventList;
	}
}

int SorolletPlayer::loadSongFromArray(float* data)
{
	int i, j, k, m;
	float *ptr = data;

	// Global settings
	miBPM = (int) (*ptr++);
	miLinesPerBeat = (int) (*ptr++);
	miTicksPerLine = (int) (*ptr++);
	updateRowTiming();

	// Instruments
	miVoiceCount = (int) (*ptr++);
	if (mVoices != NULL)
	{
		delete[] mVoices;
	}
	mVoices = new SorolletVoice[miVoiceCount];

	for (i = 0; i < miVoiceCount; i++)
	{
		SorolletVoice* v = &(mVoices[i]);

		int parameterCount = (int) (*ptr++);

		for (j = 0; j < parameterCount; j++)
		{
			v->setVSTParameter(j, (*ptr++));
		}

		v->setAbsoluteClippingValue(0.5f);
	}

	// Tracks
	miTrackCount = (int) (*ptr++);

	if (mTracksAutomationDevices != NULL)
	{
		delete[] mTracksAutomationDevices;
	}

	mTracksAutomationDevices = new SorolletDeviceAutomation[miTrackCount];

	for (i = 0; i < miTrackCount; i++)
	{
		float trackIsAutomated = (*ptr++);

		if (trackIsAutomated)
		{
			SorolletDeviceAutomation* device = &mTracksAutomationDevices[i];

			int automatedInstrument = (int) (*ptr++);
			device->setInstrumentIndex(automatedInstrument);
			device->setActive(true);

			for (j = 0; j < 14; j++)
			{
				int paramIndex = (int) (*ptr++);
				device->setParameterMapping(j, paramIndex);
			}
		}
	}

	k = 0;
	m = 0;

	// Patterns
	miPatternCount = (int) (*ptr++);
	if (mPatterns != NULL)
	{
		delete[] mPatterns;
	}

	mPatterns = new SorolletPattern[miPatternCount];

	for (i = 0; i < miPatternCount; i++)
	{
		SorolletPattern* p = &(mPatterns[i]);
		int numRows = (int) (*ptr++);
		p->setDimensions(miTrackCount, numRows);

		for (j = 0; j < miTrackCount; j++)
		{
			float patternTrackIsAutomated = (*ptr++);

			if (patternTrackIsAutomated)
			{
				int envelopeCount = (int) (*ptr++);

				for (k = 0; k < envelopeCount; k++)
				{
					SorolletEnvelope* envelope = new SorolletEnvelope();

					int automationParameterIndex = (int) (*ptr++);
					int playMode = (int) (*ptr++);
					int length = (int) (*ptr++);
					envelope->setAutomationParameterIndex(automationParameterIndex);
					envelope->setPlayMode(playMode);
					envelope->setLength(length);

					int envelopeLengthPoints = (int) (*ptr++);
					envelope->setPointsLength(envelopeLengthPoints);

					for (m = 0; m < envelopeLengthPoints; m++)
					{
						int pointRow = (int) (*ptr++);
						float pointValue = (*ptr++);

						envelope->addPoint(pointRow, pointValue);
					}

					p->addTrackEnvelope(j, envelope);
				}
			}

			// How many rows with data in this track?
			int numDataRows = (int) (*ptr++);

			for (k = 0; k < numDataRows; k++)
			{
				SorolletPatternCell* cell;

				int cellRow = (int) (*ptr++);
				cell = p->getCell(cellRow, j);

				if (cell != NULL)
				{
					int cellNote = (int) (*ptr++);
					int cellInstrument = (int) (*ptr++);

					cell->setNote(cellNote);
					cell->setInstrument(cellInstrument);
					int cellVolume = (int) (*ptr++);
					if (cellVolume >= 0)
					{
						cell->setVolume(cellVolume);
					}

					int cellEffect = (int) (*ptr++);

					if (cellEffect >= 0)
					{
						cell->setEffect(cellEffect);
						int cellEffectParameter = (int) (*ptr++);
						cell->setEffectParameter(cellEffectParameter);
					}
				}
			}
		}
	}

	// Order list
	if (mOrderList != NULL)
	{
		delete[] mOrderList;
	}

	miOrderListCount = (int) (*ptr++);
	mOrderList = new int[miOrderListCount];

	for (i = 0; i < miOrderListCount; i++)
	{
		mOrderList[i] = (int) (*ptr++);
	}

	buildEventList();

	return 0;
}

void SorolletPlayer::addEvent(SorolletPlayerEvent* event)
{
	if (miNumEvents < MAX_EVENTS)
	{
		mEventList[miNumEvents] = event;
		miNumEvents++;
	}
}

void SorolletPlayer::buildEventList()
{
	int i, j = 0;
	float t = 0;
	long samples = 0;
	long samplesPerRow;
	int order = 0;
	int pattern;
	SorolletPattern* pPattern;
	SorolletPatternCell* pCell;
	SorolletPlayerEvent* event;
	int note, instrument, volume;
	float fVolume;
	int *lastTrackInstrument = new int[miTrackCount];
	int *lastTrackNote = new int[miTrackCount];

	for (i = 0; i < miTrackCount; i++)
	{
		lastTrackInstrument[i] = INSTRUMENT_NULL;
		lastTrackNote[i] = NOTE_NULL;
	}

	if (mEventList != NULL)
	{
		delete[] mEventList;
	}
	mEventList = new SorolletPlayerEvent*[MAX_EVENTS];
	miNumEvents = 0;
	miNextEventPosition = 0;

	samplesPerRow = (int) ((mfSecondsPerRow * mfSamplingRate + 0.5f)); // Although this should change if speed commands are implemented

	while (order < miOrderListCount)
	{
		pattern = mOrderList[order];

		event = new SorolletPlayerEvent();
		event->timestamp = t;
		event->timestampSamples = samples;
		event->type = SOROLLET_EVENT_PATTERN_CHANGE;
		event->pattern = pattern;
		addEvent(event);

		pPattern = &mPatterns[pattern];

		for (i = 0; i < pPattern->getRowsNumber(); i++)
		{
			event = new SorolletPlayerEvent();
			event->timestamp = t;
			event->timestampSamples = samples;
			event->type = SOROLLET_EVENT_ROW_CHANGE;
			event->row = i;
			addEvent(event);

			for (j = 0; j < miTrackCount; j++)
			{
				pCell = pPattern->getCell(i, j);

				// Always get the volume
				volume = pCell->getVolume();
				if (volume != VOLUME_NULL)
				{
					fVolume = MathUtils::map((float) volume, 0.0f, (float) 0x80, 0.0f, 1.0f); // Convert from tracker style (0..0x80) to float value
				}
				else
				{
					fVolume = 1.0f;
				}

				// ~~ NOTE ON ~~ //
				if (pCell->getNote() != NOTE_NULL && pCell->getNote() != NOTE_OFF && pCell->getInstrument() != INSTRUMENT_NULL)
				{
					note = pCell->getNote();
					instrument = pCell->getInstrument();

					event = new SorolletPlayerEvent();
					event->timestamp = t;
					event->timestampSamples = samples;
					event->type = SOROLLET_EVENT_NOTE_ON;
					event->note = note;
					event->instrument = instrument;
					event->volume = fVolume;

					addEvent(event);

					buildArpeggios(pCell, t, samples, mfSecondsPerRow, samplesPerRow, instrument, note, fVolume);

					// Store this for later, so that if we get a new volume event we know to which instrument it applies
					lastTrackInstrument[j] = instrument;
					lastTrackNote[j] = note;
				}
				// ~~ NEW VOLUME ~~ //
				else if (
						(NOTE_NULL == pCell->getNote() || NOTE_OFF == pCell->getNote()) &&
						INSTRUMENT_NULL == pCell->getInstrument() &&
						VOLUME_NULL != pCell->getVolume())
				{
					if (lastTrackInstrument[j] != INSTRUMENT_NULL)
					{
						event = new SorolletPlayerEvent();
						event->timestamp = t;
						event->timestampSamples = samples;
						event->type = SOROLLET_EVENT_VOLUME;
						event->instrument = lastTrackInstrument[j];
						event->volume = fVolume;

						addEvent(event);

						if (lastTrackNote[j] != NOTE_NULL)
						{
							buildArpeggios(pCell, t, samples, mfSecondsPerRow, samplesPerRow, instrument, lastTrackNote[j], fVolume);
						}
					}
				}
				// ~~ NOTE OFF ~~ //
				else if (NOTE_OFF == pCell->getNote())
				{
					if (lastTrackInstrument[j] != INSTRUMENT_NULL)
					{
						event = new SorolletPlayerEvent();
						event->timestamp = t;
						event->timestampSamples = samples;
						event->type = SOROLLET_EVENT_NOTE_OFF;
						event->instrument = lastTrackInstrument[j];

						addEvent(event);
					}
				}
				else if (pCell->getNote() != NOTE_OFF && lastTrackNote[j] != NOTE_NULL && lastTrackInstrument[j] != INSTRUMENT_NULL)
				{
					buildArpeggios(pCell, t, samples, mfSecondsPerRow, samplesPerRow, lastTrackInstrument[j], lastTrackNote[j], 1.0f);
				}
			}

			t += mfSecondsPerRow;
			samples += samplesPerRow;
		}

		order++;
	}

	// End of the song --there can only be one of these events!!!
	event = new SorolletPlayerEvent();
	event->timestamp = t;
	event->timestampSamples = samples;
	event->type = SOROLLET_EVENT_SONG_END;
	addEvent(event);

	miNextEventPosition = 0;
	miCurrentRow = 0;
	miCurrentOrder = 0;
	miCurrentPattern = mOrderList[0];

	delete [] lastTrackInstrument;
	delete [] lastTrackNote;

}

void SorolletPlayer::buildArpeggios(SorolletPatternCell* pCell, float timestamp, long timestampSamples, float secondsPerRow, int samplesPerRow, int instrument, int note, float volume)
{
	// ~~ Arpeggio?? ~~
	if (pCell->getEffect() == 0 && pCell->getEffectParameter() != 0)
	{
		// yeees
		SorolletPlayerEvent event;
		float timestamp2;
		long samples2;
		float noteTimeSeparation;
		long noteSamplesSeparation;

		#define NUMNOTES 3

		int newNotes[3];
		
		int note1 = (pCell->getEffectParameter() & 0xF0) / 16;
		int note2 = pCell->getEffectParameter() & 0x0F;

		newNotes[0] = note + note1;
		newNotes[1] = note + note2;
		newNotes[2] = note;

		int n = NUMNOTES + 1;
		noteTimeSeparation = secondsPerRow / n;
		noteSamplesSeparation = samplesPerRow / n;

		timestamp2 = timestamp + noteTimeSeparation;
		samples2 = timestampSamples + noteSamplesSeparation;

		for (int k = 0; k < NUMNOTES; k++)
		{
			SorolletPlayerEvent* event = new SorolletPlayerEvent();

			event->timestamp = timestamp2;
			event->timestampSamples = samples2;
			event->type = SOROLLET_EVENT_NOTE_ON;
			event->note = newNotes[k];
			event->instrument = instrument;
			event->volume = volume;
			addEvent(event);

			timestamp2 += noteTimeSeparation;
			samples2 += noteSamplesSeparation;
		}
	}
}

/**
 * Returns false when it has finished playing
 **/
bool SorolletPlayer::getBuffer(float *outL, float *outR, int numSamples)
{
	float *outBufferL, *outBufferR;
	SorolletPlayerEvent* pEvent;
	float bufferEndTime;
	float segmentStartTime;
	int remainingSamples;
	long bufferEndSamples;
	int intervalSamples;
	long segmentStartSamples;

	memset(outL, 0, sizeof (float) * numSamples);
	memset(outR, 0, sizeof (float) * numSamples);

	outBufferL = outL;
	outBufferR = outR;
	remainingSamples = numSamples;
	bufferEndTime = mfTimePosition + (numSamples * mfInvSamplingRate);
	bufferEndSamples = mlPosition + numSamples;

	segmentStartTime = mfTimePosition;
	segmentStartSamples = mlPosition;

	do
	{
		if (miNextEventPosition == miNumEvents)
		{
			return false;
		}

		pEvent = mEventList[miNextEventPosition];

		if (pEvent->timestampSamples >= bufferEndSamples)
		{
			break;
		}

		intervalSamples = pEvent->timestampSamples - segmentStartSamples;

		// Get buffer UNTIL the event
		if (intervalSamples > 0)
		{
			processBuffer(outBufferL, outBufferR, intervalSamples);

			outBufferL += intervalSamples;
			outBufferR += intervalSamples;
			remainingSamples -= intervalSamples;
			segmentStartSamples = pEvent->timestampSamples;
			mlPosition += intervalSamples;
		}

		// Apply the event
		if (SOROLLET_EVENT_PATTERN_CHANGE == pEvent->type)
		{
			miCurrentPattern = pEvent->pattern;
		}
		else if (SOROLLET_EVENT_ROW_CHANGE == pEvent->type)
		{
			miCurrentRow = pEvent->row;
		}
		else if (SOROLLET_EVENT_NOTE_ON == pEvent->type)
		{
			SorolletVoice* pVoice = &mVoices[pEvent->instrument];
			pVoice->sendNoteOn(pEvent->note, pEvent->volume);
		}
		else if (SOROLLET_EVENT_VOLUME == pEvent->type)
		{
			SorolletVoice* pVoice = &mVoices[pEvent->instrument];
			pVoice->sendCurrentNoteVolume(pEvent->volume);
		}
		else if (SOROLLET_EVENT_NOTE_OFF == pEvent->type)
		{
			SorolletVoice* pVoice = &mVoices[pEvent->instrument];
			pVoice->sendNoteOff();
		}

		miNextEventPosition++;

	}
	while (
			miNextEventPosition < miNumEvents
			&& remainingSamples > 0
			);

	if (remainingSamples > 0)
	{
		processBuffer(outBufferL, outBufferR, remainingSamples);
	}

	mlPosition += remainingSamples;
	mfTimePosition += numSamples * mfInvSamplingRate;

	return (miNextEventPosition - 1 < miNumEvents);
}

void SorolletPlayer::processBuffer(float *outL, float *outR, int numSamples)
{
	float *tmpL, *tmpR;
	tmpL = mTmpBufferL;
	tmpR = mTmpBufferR;
	int i, j;

	// Process envelopes, if applicable
	SorolletPattern* pPattern = &mPatterns[miCurrentPattern];
	for (i = 0; i < miTrackCount; i++)
	{
		if (pPattern->trackHasEnvelopes(i) && mTracksAutomationDevices[i].isActive())
		{
			SorolletDeviceAutomation* deviceAutomation = &(mTracksAutomationDevices[i]);
			SorolletEnvelope** vEnvelopes = pPattern->getTrackEnvelopes(i);
			int numEnvelopes = pPattern->getTrackEnvelopesNumber(i);

			for (j = 0; j < numEnvelopes; j++)
			{
				SorolletEnvelope* pEnvelope = vEnvelopes[j];

				int instrumentIndex = deviceAutomation->getInstrumentIndex();
				int automationParameterIndex = pEnvelope->getAutomationParameterIndex();
				int instrumentParameterIndex = deviceAutomation->getParameterMappings()[automationParameterIndex];
				float value = pEnvelope->getValueAtRow(miCurrentRow);

				mVoices[i].setVSTParameter(instrumentParameterIndex, value);
			}
		}
	}

	// ~~~~

	memset(outL, 0, sizeof (float) * numSamples);
	memset(outR, 0, sizeof (float) * numSamples);

	for (i = 0; i < miVoiceCount; i++)
	{
		SorolletVoice* voice = &mVoices[i];

		voice->getBuffer(tmpL, tmpR, numSamples);

		for (j = 0; j < numSamples; ++j)
		{
			outL[j] += tmpL[j];
			outR[j] += tmpR[j];
		}
	}

	for (j = 0; j < numSamples; ++j)
	{
		outL[j] = MathUtils::clipf(outL[j], -1.0f, 1.0f);
		outR[j] = MathUtils::clipf(outR[j], -1.0f, 1.0f);
	}
}

float SorolletPlayer::getTimePosition()
{
	return mfTimePosition;
}

int SorolletPlayer::getCurrentOrder()
{
	return miCurrentOrder;
}

int SorolletPlayer::getCurrentPattern()
{
	return miCurrentPattern;
}

int SorolletPlayer::getCurrentRow()
{
	return miCurrentRow;
}

void SorolletPlayer::updateRowTiming()
{
	mfSecondsPerRow = 60.0f / (miLinesPerBeat * miBPM);
	mfSecondsPerTick = mfSecondsPerRow / miTicksPerLine;
}
