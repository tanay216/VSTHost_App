#include "pch.h"
#include "CapturePlugin.h"
#include <AK/SoundEngine/Common/IAkPlugin.h>


using namespace AK;

CapturePlugin::CapturePlugin()
{
    // Initialize any members if necessary.
}
CapturePlugin::~CapturePlugin()
{
	// Clean up any members if necessary.
}

AKRESULT CapturePlugin::GetPluginInfo(AkPluginInfo& out_pPluginInfo) {
    // Provide plugin info
    out_pPluginInfo.eType = AkPluginTypeEffect;
    out_pPluginInfo.bIsInPlace = true;
    return AK_Success;
}

AKRESULT CapturePlugin::Reset() {
	return AK_Success;
}

AKRESULT CapturePlugin::TimeSkip(AkUInt32 in_uFrames) {
	return AK_Success;
}
