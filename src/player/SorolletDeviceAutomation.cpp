#include "SorolletDeviceAutomation.h"

SorolletDeviceAutomation::SorolletDeviceAutomation()
{
	setInstrumentIndex(0);
	setActive(false);
}

void SorolletDeviceAutomation::setActive(bool value)
{
	mbActive = value;
}

bool SorolletDeviceAutomation::isActive()
{
	return mbActive;
}

void SorolletDeviceAutomation::setInstrumentIndex(unsigned int index)
{
	miInstrumentIndex = index;
}

void SorolletDeviceAutomation::setParameterMapping(unsigned int position, unsigned int parameterIndex)
{
	mParameterMappings[position] = parameterIndex;
}

unsigned int SorolletDeviceAutomation::getInstrumentIndex()
{
	return miInstrumentIndex;
}

unsigned int *SorolletDeviceAutomation::getParameterMappings()
{
	return &(mParameterMappings[0]);
}