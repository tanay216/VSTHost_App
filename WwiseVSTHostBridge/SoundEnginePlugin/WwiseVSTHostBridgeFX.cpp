/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2024 Audiokinetic Inc.
*******************************************************************************/

#include "WwiseVSTHostBridgeFX.h"
#include "../WwiseVSTHostBridgeConfig.h"

#include <AK/AkWwiseSDKVersion.h>
#include "VoiceRegistry.h"


AK::IAkPlugin* CreateWwiseVSTHostBridgeFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, WwiseVSTHostBridgeFX());
}

AK::IAkPluginParam* CreateWwiseVSTHostBridgeFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, WwiseVSTHostBridgeFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(WwiseVSTHostBridgeFX, AkPluginTypeEffect, WwiseVSTHostBridgeConfig::CompanyID, WwiseVSTHostBridgeConfig::PluginID)

WwiseVSTHostBridgeFX::WwiseVSTHostBridgeFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

WwiseVSTHostBridgeFX::~WwiseVSTHostBridgeFX()
{
}

AKRESULT WwiseVSTHostBridgeFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (WwiseVSTHostBridgeFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    return AK_Success;
}

AKRESULT WwiseVSTHostBridgeFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT WwiseVSTHostBridgeFX::Reset()
{
    return AK_Success;
}

AKRESULT WwiseVSTHostBridgeFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

//void WwiseVSTHostBridgeFX::Execute(AkAudioBuffer* io_pBuffer)
//{
//    //const AkUInt32 uNumChannels = io_pBuffer->NumChannels();
//
//    //AkUInt16 uFramesProcessed;
//    //for (AkUInt32 i = 0; i < uNumChannels; ++i)
//    //{
//    //   // m_playingID = AK::SoundEngine::
//    //    AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)io_pBuffer->GetChannel(i);
//
//    //    uFramesProcessed = 0;
//    //    while (uFramesProcessed < io_pBuffer->uValidFrames)
//    //    {
//    //        // Execute DSP in-place here
//    //        
//    //        
//    //      
//    //        ++uFramesProcessed;
//    //    }
//    //}
//
//    if (!io_pBuffer || io_pBuffer->uValidFrames == 0) return;
//
//    const AkUInt32 numChannels = io_pBuffer->NumChannels();
//    const AkUInt32 numSamples = io_pBuffer->uValidFrames;
//
//    // Write audio data to shared memory
//    sharedMem.WriteBuffer(io_pBuffer, m_playingID);
//
//    // Wait for processed data to be ready (polling approach)
//    int maxRetries = 5;
//    while (!sharedMem.readAvailable() && maxRetries-- > 0)
//    {
//        AKPLATFORM::AkSleep(1); // Avoid CPU overload
//    }
//
//    // Read processed audio from shared memory
//    for (AkUInt32 ch = 0; ch < numChannels; ++ch)
//    {
//        AkReal32* pBuf = io_pBuffer->GetChannel(ch);
//        const float* processedData = sharedMem.getProcessedSamples(ch);
//        memcpy(pBuf, processedData, numSamples * sizeof(float));
//    }
//
//
//   
//
//}

//void WwiseVSTHostBridgeFX::Execute(AkAudioBuffer* io_pBuffer) {
//    if (!io_pBuffer || io_pBuffer->uValidFrames == 0) return;
//
//    const AkUInt32 numChannels = io_pBuffer->NumChannels();
//    const AkUInt32 numSamples = io_pBuffer->uValidFrames;
//
//    // Write raw audio to shared memory
//    sharedMem.WriteBuffer(io_pBuffer, m_playingID);
//
//    // Wait for processed audio (but don’t get stuck if missing)
//    int maxRetries = 10;
//    while (!sharedMem.readAvailable() && maxRetries-- > 0) {
//        AKPLATFORM::AkSleep(1); // Allow time for processing
//    }
//
//    bool hasProcessedAudio = sharedMem.readAvailable(); // Check if valid processed data is available
//
//    // Read processed audio or pass dry input if no processed data exists
//    for (AkUInt32 ch = 0; ch < numChannels; ++ch) {
//        AkReal32* pBuf = io_pBuffer->GetChannel(ch);
//        const float* processedData = hasProcessedAudio ? sharedMem.getProcessedSamples(ch) : nullptr;
//
//        if (processedData) {
//            memcpy(pBuf, processedData, numSamples * sizeof(float)); // Use processed audio
//        }
//        else {
//            std::cout << "[Wwise FX] No processed audio. Using dry input." << std::endl;
//            memcpy(pBuf, io_pBuffer->GetChannel(ch), numSamples * sizeof(float)); // Use dry input
//        }
//    }
//
//    // Reset shared memory flag
//   // sharedMem.markProcessed();
//}

void WwiseVSTHostBridgeFX::Execute(AkAudioBuffer* io_pBuffer) {
    if (!io_pBuffer || io_pBuffer->uValidFrames == 0) return;

    const AkUInt32 numChannels = io_pBuffer->NumChannels();
    const AkUInt32 numSamples = io_pBuffer->uValidFrames;

    // Write raw audio to shared memory
    sharedMem.WriteBuffer(io_pBuffer, m_playingID);

    // Wait for processed audio (but don’t get stuck if missing)
    //int maxRetries = 10;
    //while (!sharedMem.readAvailable() && maxRetries-- > 0) {
    //    AKPLATFORM::AkSleep(1); // Allow time for processing
    //}

    bool hasProcessedAudio = sharedMem.readAvailable(); // Check if valid processed data is available

    // Read processed audio or pass dry input if no processed data exists
    for (AkUInt32 ch = 0; ch < numChannels; ++ch) {
        AkReal32* pBuf = io_pBuffer->GetChannel(ch);
        const float* processedData = hasProcessedAudio ? sharedMem.getProcessedSamples(ch) : nullptr;

        if (processedData) {
            memcpy(pBuf, processedData, numSamples * sizeof(float)); // Use processed audio
        }
        else {
            std::cout << "[Wwise FX] No processed audio. Using dry input." << std::endl;
            memcpy(pBuf, io_pBuffer->GetChannel(ch), numSamples * sizeof(float)); // Use dry input
        }
    }
    
    // Reset shared memory flag
   // sharedMem.markProcessed();
}

AKRESULT WwiseVSTHostBridgeFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
