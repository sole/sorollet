#include "SorolletPlayerEvent.h"

SorolletPlayerEvent::SorolletPlayerEvent()
{
	reset();
}

void SorolletPlayerEvent::reset()
{
	timestamp = 0;
	timestampSamples = 0;
	type = SOROLLET_EVENT_NULL;
	note = 0;
	instrument = 0;
	volume = 0;
	track = 0;
	effect = 0;
	effectValue = 0;
	row = 0;
	pattern = 0;
	orderPosition = 0;
}
