#define AK_DEFINE_GLOBALS
#include "pch.h"
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include "CapturePlugin.h"
#include "AK/SoundEngine/Common/AkMemoryMgr.h"
#include "CapturePluginParams.h"



void (*g_pAssertHook)(const char*, const char*, int) = nullptr;
using namespace AK;
// Capture Plugin Factory


IAkPluginParam* CreateCapturePluginParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, CapturePluginParams());
}

IAkPlugin* CreateCapturePlugin(AK::IAkPluginMemAlloc* alloc) {
    return AK_PLUGIN_NEW(alloc, CapturePlugin());
}

extern "C" {

    AK::IAkPluginParam* CreateCreateCapturePluginParams(AK::IAkPluginMemAlloc* in_pAllocator)
    {
        return CreateCapturePluginParams(in_pAllocator);
    }
    // Alias for the expected symbol.
    AK::IAkPlugin* CreateCreateCapturePlugin(AK::IAkPluginMemAlloc* in_pAllocator)
    {
        return CreateCapturePlugin(in_pAllocator);
    }



}




// Plugin Registration
AK_IMPLEMENT_PLUGIN_FACTORY(CreateCapturePlugin,AkPluginTypeEffect, 12345, 0  );
AK_STATIC_LINK_PLUGIN(CreateCapturePlugin);
DEFINE_PLUGIN_REGISTER_HOOK;