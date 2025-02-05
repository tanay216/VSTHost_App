// CapturePlugin.h - Custom FX plugin for audio capture
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Tools/Common/AkAssert.h>
#include <math.h>
#include "VoiceRegistry.h"
#include "SharedMemoryHandler.h"



class CapturePlugin : public AK::IAkInPlaceEffectPlugin {
public:
   CapturePlugin();
   virtual ~CapturePlugin();

   
   AKRESULT GetPluginInfo(AkPluginInfo& out_rPluginInfo) override;

    AKRESULT Init(AK::IAkPluginMemAlloc* alloc, AK::IAkEffectPluginContext* context,
        AK::IAkPluginParam* params, AkAudioFormat& format) override {
        m_nodeID = context->GetAudioNodeID();
        return AK_Success;
    }

    void Execute(AkAudioBuffer* io_pBuffer) override {
        // Capture audio data
        AkPlayingID playingID = m_nodeID;

        // Update voice registry
        VoiceRegistry::Instance().UpdateBuffer(playingID, io_pBuffer);
        sharedMem.WriteBuffer(io_pBuffer, playingID);
        AKPLATFORM::OutputDebugMsg("CapturePlugin::Execute()");

       
    }

    AKRESULT Term(AK::IAkPluginMemAlloc* alloc) override { 
        AK_PLUGIN_DELETE(alloc, this);
        return AK_Success;
    }

    
  
  
    AKRESULT Reset() override;
   // AKRESULT  AK::IAkPlugin::GetPluginInfo(AkPluginInfo& out_pPluginInfo);
    AKRESULT TimeSkip(AkUInt32 in_uFrames) override;
   
    

private:
    AkPluginID m_nodeID;
    SharedMemoryManager sharedMem;
};