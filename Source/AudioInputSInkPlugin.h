#pragma once

#include <AK/Plugin/AkSinkFactory.h>
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <atomic>
//#include <IAkPlugin.h>

class MySinkPlugin : public AK::IAkSinkPlugin
{
public:
    MySinkPlugin();
    ~MySinkPlugin() override;

    // Wwise Plugin Interface
    AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSinkPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat) override;
    AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;
    AKRESULT Reset() override;
    AKRESULT GetPluginInfo(AkPluginInfo& out_rPluginInfo) override;


    // Audio Processing
   virtual void Consume(AkAudioBuffer* io_pBuffer, AkRamp in_gain) override;
   AKRESULT IsDataNeeded() ;

   // Required overrides from `IAkSinkPluginBase`
  
   void OnFrameEnd() override;
   bool IsStarved() override;
   void ResetStarved() override;

   AK::IAkPlugin* CreateMySinkPlugin(AK::IAkPluginMemAlloc* in_pAllocator);

private:
    AK::IAkSinkPluginContext* m_pContext;

    std::atomic<bool> m_bIsRunning;
    std::atomic<bool> m_bIsStarved;



};

