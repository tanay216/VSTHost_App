// SinkPlugin.h - Output verification sink
#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>       // For AkAudioBuffer, AKRESULT
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Tools/Common/AkAssert.h>
#include <math.h>
#include "VoiceRegistry.h"
#include "SharedMemoryHandler.h"// For AK::IAkPluginMemAlloc

class SinkPlugin : public AK::IAkSinkPlugin {
public:
    SinkPlugin() = default;
    ~SinkPlugin() = default;

    AKRESULT Init(
        AK::IAkPluginMemAlloc* in_pAllocator,
        AK::IAkSinkPluginContext* in_pContext,
        AK::IAkPluginParam* in_pParams,
        AkAudioFormat& in_rFormat
    ) override { return AK_Success;
    }

    AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override {
        AK_PLUGIN_DELETE(in_pAllocator, this);
        return AK_Success;
    }

    AKRESULT GetPluginInfo(AkPluginInfo& out_rPluginInfo) override {
        out_rPluginInfo.eType = AkPluginTypeSink;
        out_rPluginInfo.bIsInPlace = true;
        return AK_Success;
    }

    // Corrected Execute signature
  

    
    AKRESULT Reset() override {return AK_Success;};

    void OnFrameEnd() override;
    bool IsStarved() override;
    void ResetStarved() override;
};