#include "/VSTHost_App/Source/SinkPlugin.h"
#include "pch.h"


AKRESULT SinkPlugin::Term(AK::IAkPluginMemAlloc* in_pAllocator) {
    // Clean up resources
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}








