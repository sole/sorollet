#include "SorolletVSTi.h"

#ifdef __GNUC__
	AEffect* main_plugin (audioMasterCallback audioMaster) asm ("main");
	#define main main_plugin
	
	AEffect *main (audioMasterCallback audioMaster)
	{ 
		SorolletVSTi* sorollet = new SorolletVSTi(audioMaster);
		if(!sorollet)
		{
			return 0;
		}
		return sorollet->getAeffect();
	}

	AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
	{
		return new SorolletVSTi(audioMaster);
	}
	
#else
	AudioEffect* createEffectInstance( audioMasterCallback audioMaster ) 
	{ 
		return new SorolletVSTi(audioMaster); 
	}
#endif
