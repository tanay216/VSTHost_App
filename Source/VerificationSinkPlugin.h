//// VerificationSinkPlugin.h
//#ifndef VERIFICATION_SINK_PLUGIN_H
//#define VERIFICATION_SINK_PLUGIN_H
//
////#include "IAkSinkPlugin.h"
//#include <AK/SoundEngine/Common/AkTypes.h>       // For AkAudioBuffer, AKRESULT
//#include <AK/SoundEngine/Common/IAkPlugin.h>
//#include <AK/Tools/Common/AkAssert.h>
//#include <math.h>
//#include "VoiceRegistry.h"
//#include "SharedMemoryHandler.h"// For AK::IAkPluginMemAlloc
//
//class VerificationSinkPlugin : public AK::IAkSinkPlugin
//{
//public:
//    VerificationSinkPlugin();
//    virtual ~VerificationSinkPlugin();
//
//    // Initialize the sink plugin.
//    virtual AKRESULT Init(IAkPluginMemAlloc ) override;
//    // Terminate the sink plugin.
//    virtual void Term() override;
//    // Process the final output buffer.
//    virtual AKRESULT ProcessSink(
//        const AkAudioBuffer* in_pBuffer,
//        const AkSinkPluginContext& in_Context
//    ) override;
//
//private:
//    // Dummy verification function.
//    void VerifyAudioOutput(const AkAudioBuffer* buffer, const AkSinkPluginContext& context);
//};
//
//#endif // VERIFICATION_SINK_PLUGIN_H
