Because of the legal status of the VST-SDK source code, I am not allowed to republish their code here.

You need to register in Steinberg's website as VST developer and download the VST SDK from there.

However, the code needs a change so that it will compile in Linux. 
I have provided a patch for this, it's in pluginterfaces/vst2.x.

Once you download the SDK, uncompress it to the vst-sdk folder (make sure the folders match, i.e.
the pluginterfaces folder is uncompressed to the pluginterfaces folder and etc).

Then duplicate aeffect.h and rename it to aeffect_original.h. You can apply the patch then.
