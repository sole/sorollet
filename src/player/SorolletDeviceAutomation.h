/* 
 * File:   SorolletDeviceAutomation.h
 * Author: sole
 *
 * Created on 11 March 2010, 11:35
 */

#ifndef SOROLLET_DEVICE_AUTOMATION_H
#define SOROLLET_DEVICE_AUTOMATION_H

class SorolletDeviceAutomation
{
	public:
		SorolletDeviceAutomation();
		void setActive(bool value);
		bool isActive();
		void setInstrumentIndex(unsigned int index);
		void setParameterMapping(unsigned int position, unsigned int parameterIndex);
		unsigned int getInstrumentIndex();
		unsigned int *getParameterMappings();
		
	
	protected:
		int miInstrumentIndex;
		unsigned int mParameterMappings[14];
		bool mbActive;
};

#endif
