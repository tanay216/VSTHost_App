#pragma once

#include <AK/WwiseAuthoringAPI/AkAutobahn/Client.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/AkJson.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/autobahn.h>
#include <AK/Tools/Common/AkAssert.h>
#include <AK/WwiseAuthoringAPI/waapi.h>
#include <AK/WwiseAuthoringAPI/AkJsonBase.h>
#include <Ak/WwiseAuthoringAPI/AkAutobahn/JsonProvider.h> // For handling JsonProvider type in the callback
#include <iostream>
#include <map>
#include <vector>
#include <future>  // For std::promise, std::future
#include <atomic>  // For std::atomic_bool
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SoundEngine/Common/AkCallback.h>


using namespace AK::WwiseAuthoringAPI;

// Declare the global assert hook (but do NOT define it here)
extern AkAssertHook g_pAssertHook;

struct WwiseEventNode
{
    std::string name;
    std::string path;
    std::string guid;
    std::string type;
    std::string parentID;
    std::vector<WwiseEventNode> children;
};

class WAAPIManager
{
public:
    WAAPIManager();
    ~WAAPIManager();

    void WwiseAudioCaptureCallback(
        AkAudioBuffer* io_pBuffer, // Audio buffer
        AkUInt32 in_uBufferSize    // Buffer size
    );

    void RegisterWwiseAudioCapture();

    void connectToWAAPI();
    void getSessionInfo();
    void GetDefaultWorkUnits();
    void GetAllEvents();

    //void GetEventDescendants(const std::string& eventName, const std::string& parentPath);
    std::vector<WwiseEventNode> GetChildrenOfObject(const std::string& objectPath);
        
    std::vector<WwiseEventNode> GetEventDescendants(const std::string& eventName, const std::string& parentPath);

    void postWwiseEvent(const std::string& objectID);

   // void SubscribeToVoices();
    void GetVoices(const std::string& objectID);
    void GetOriginalFilePath( std::string& objectGUID);

   // void WAAPIManager::SendVoiceDataToHost(const std::string& soundName, const std::string& eventName, float volume, float pitch, int gameObject);

    // Sound Engine ======================================================================

    struct VoiceData {
        uint64_t gameObjectID;
        std::string objectName;
        std::string objectGUID;
        std::string originalFilePath;
        AkAudioBuffer* audioBuffer;
    };

    void InitializeSoundEngine();
    void ShutdownSoundEngine();
    void RegisterGameObjects();
    void CaptureAudioStreams();
    const std::vector<VoiceData>& GetCapturedVoices() const { return capturedVoices;}

    void WAAPIManager::CapturedAudioStreamsDetails();

    void AutoAttachCapturePlugin() {
        AkJson args(AkJson::Map{
            {"from", AkJson::Map{{"ofType", AkJson::Array{AkVariant("Sound")}}}}
            });

        AkJson options(AkJson::Map{
            {"return", AkJson::Array{AkVariant("id"), AkVariant("path")}}
            });

        AkJson result;
        if (waapiClient.Call(ak::wwise::core::object::get, args, options, result)) {
            auto& sounds = result["return"].GetArray();
            for (auto& sound : sounds) {
                AttachPluginToObject(
                    sound["id"].GetVariant().GetString(),
                    "CapturePlugin"
                );
            }
        }
    }



    //  Wwise Event Tree getter methods
    std::map<std::string, WwiseEventNode> getWwiseEventsTree() { return wwiseEventsTree; }

    // Wwise Project Info getter methods
    std::string getWwiseProjectName() const { return wwiseProjectName; }
    std::string getWwiseVersion() const { return wwiseVersion; }
    std::string getWwisePlatform() const { return wwisePlatform; }


    // Custom assertion handler for Wwise
    static void AkAssertHookFunc(const char* in_pszExpression, const char* in_pszFileName, int in_lineNumber)
    {
        std::cerr << "Wwise Assertion Failed!" << std::endl;
        std::cerr << "Expression: " << in_pszExpression << std::endl;
        std::cerr << "File: " << in_pszFileName << std::endl;
        std::cerr << "Line: " << in_lineNumber << std::endl;
    };



private:

    void AttachPluginToObject(const std::string& objectID, const std::string& pluginName) {
        AkJson args(AkJson::Map{
            {"object", objectID},
            {"effects", AkJson::Array{AkJson::Map{
                {"name", pluginName},
                {"id", "MyCompany::CapturePlugin"}
            }}}
            });

        waapiClient.Call(ak::wwise::core::object::set, args, AkJson(AkJson::Type::Map), AkJson(AkJson::Type::Map));
    }

    std::unordered_map<std::string, WwiseEventNode> wwiseObjects;
    std::vector<VoiceData> capturedVoices;
    AK::IAkGlobalPluginContext* soundEngineContext = nullptr;
    
    Client waapiClient;
    std::string originalFilePath;
    std::string wwiseProjectName = "";
    std::string wwiseVersion = "";
    std::string wwisePlatform = "";

    static void AudioRenderCallback(
        AK::IAkGlobalPluginContext* in_pContext,
        AkGlobalCallbackLocation in_eLocation,
        void* in_pCookie
    );
    

    std::map<std::string, WwiseEventNode> wwiseEventsTree;

};

