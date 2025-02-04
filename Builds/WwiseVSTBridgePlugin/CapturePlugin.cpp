#include "/VSTHost_App/Source/CapturePlugin.h"
#include "pch.h"
#include <AK/SoundEngine/Common/IAkPlugin.h>


using namespace AK;

AKRESULT  AK::IAkPlugin::GetPluginInfo(AkPluginInfo& out_pPluginInfo) {
    // Provide plugin info
    out_pPluginInfo.eType = AkPluginTypeEffect;
    out_pPluginInfo.bIsInPlace = true;
    return AK_Success;
}

