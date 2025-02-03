#include "AudioInputSInkPlugin.h"
#include <iostream>

MySinkPlugin::MySinkPlugin() : m_pContext(nullptr), m_bIsRunning(false) {}

MySinkPlugin::~MySinkPlugin() {}

AKRESULT MySinkPlugin::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSinkPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pContext = in_pContext;
    m_bIsRunning.store(true);

    std::cout << "[MySinkPlugin] Initialized with Sample Rate: " << in_rFormat.uSampleRate << std::endl;
    return AK_Success;
}

AKRESULT MySinkPlugin::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    m_bIsRunning.store(false);
    std::cout << "[MySinkPlugin] Terminated." << std::endl;
}

AKRESULT MySinkPlugin::Reset()
{
    std::cout << "[MySinkPlugin] Reset." << std::endl;
    return AK_Success;
}

AKRESULT MySinkPlugin::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.bIsInPlace = false;
    out_rPluginInfo.bCanProcessObjects = false;
     
   
    return AK_Success;
}

 void MySinkPlugin::Consume(AkAudioBuffer* io_pBuffer, AkRamp in_gain)
{
    if (!m_bIsRunning.load() || !io_pBuffer || io_pBuffer->uValidFrames == 0)
        return AK_Fail;

    std::cout << "[MySinkPlugin] Consuming Audio: " << io_pBuffer->uValidFrames << " frames" << std::endl;

    // Send PCM data to JUCE VST Host
 
  //  SendAudioToJUCE(io_pBuffer);

    return AK_Success;
}

AKRESULT MySinkPlugin::IsDataNeeded()
{
    return m_bIsRunning.load();
}

AK::IAkPlugin* MySinkPlugin::CreateMySinkPlugin(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, MySinkPlugin());
}