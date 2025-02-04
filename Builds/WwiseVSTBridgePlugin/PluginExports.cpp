#include "/VSTHost_App/Source/CapturePlugin.h"
#include "/VSTHost_App/Source/SinkPlugin.h"
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include "pch.h"




using namespace AK;
// Capture Plugin Factory
AK::IAkPlugin* CreateCapturePlugin(AK::IAkPluginMemAlloc* alloc) {
    return AK_PLUGIN_NEW(alloc, CapturePlugin());
}

// Sink Plugin Factory
//AK::IAkPlugin* CreateSinkPlugin(AK::IAkPluginMemAlloc* alloc) {
//    return AK_PLUGIN_NEW(alloc, SinkPlugin());
//}
