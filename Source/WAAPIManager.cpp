#include "WAAPIManager.h"


AkAssertHook g_pAssertHook = nullptr;  // Define the global variable
    
WAAPIManager::WAAPIManager()
{
    g_pAssertHook = WAAPIManager::AkAssertHookFunc;  // Assign in constructor
}

WAAPIManager::~WAAPIManager()
{
    waapiClient.Disconnect();
}


void WAAPIManager::connectToWAAPI()
{
    try
    {
        std::cout << "Attempting to connect to WAAPI..." << std::endl;

        if (!waapiClient.Connect("127.0.0.1", 8080))
        {
            std::cerr << "Could not connect to WAAPI." << std::endl;

            // Print more debug information
            if (!waapiClient.IsConnected())
            {
                std::cerr << "WAAPI Client is not connected." << std::endl;
            }
            else
            {
                std::cerr << "WAAPI Client is connected, but something else failed." << std::endl;
            }

            return;
        }
        std::cout << "******************" << std::endl;
        std::cout << "Connected to WAAPI!" << std::endl;
        std::cout << "******************" << std::endl;

        
        getSessionInfo();
        GetAllEvents();
      //  SubscribeToVoices();
      //  GetDefaultWorkUnits();
        // Disconnect when done
      //  waapiClient.Disconnect();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while connecting to WAAPI: " << e.what() << std::endl;
    }
}

void WAAPIManager::getSessionInfo()
{
    using namespace AK::WwiseAuthoringAPI;
    try
    {
        if (!waapiClient.IsConnected())
        {
            std::cerr << "Not connected to WAAPI!" << std::endl;
            return;
        }

        std::cout << "Getting session info..." << std::endl;

        AkJson queryResult;
        AkJson args(AkJson::Map{});

        // Get Wwise Version & Platform
        if (waapiClient.Call(ak::wwise::core::getInfo, args, AkJson(AkJson::Type::Map), queryResult, 10))
        {
            wwiseVersion = queryResult["version"]["displayName"].GetVariant().GetString();
            wwisePlatform = queryResult["platform"].GetVariant().GetString();

            std::cout << "Wwise Version: " << wwiseVersion << std::endl;
            std::cout << "Platform: " << wwisePlatform << std::endl;
        }
        else
        {
            std::cerr << "Failed to retrieve session info!" << std::endl;
        }

        // Correct API Call to Get Project Info
        if (waapiClient.Call(ak::wwise::core::getProjectInfo, args, AkJson(AkJson::Type::Map), queryResult, 10))
        {
            wwiseProjectName = queryResult["name"].GetVariant().GetString();
            std::cout << "Project Name: " << wwiseProjectName << std::endl;
        }
        else
        {
            std::cerr << "Failed to retrieve project info!" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while querying session info: " << e.what() << std::endl;
    }
}

void WAAPIManager::GetDefaultWorkUnits()
{
    using namespace AK::WwiseAuthoringAPI;

    try
    {
        if (!waapiClient.IsConnected())
        {
            std::cerr << "Not connected to WAAPI!" << std::endl;
            return;
        }

        std::cout << "Retrieving default work units..." << std::endl;

        AkJson queryResult;

        
        AkJson args(AkJson::Map{
            { "from", AkJson::Map{
                { "ofType", AkJson::Array{ AkVariant("WorkUnit") } }  // Get all WorkUnits
            }}
            });

        AkJson options(AkJson::Map{
            { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("path") } }  // "category" removed
            });

        if (waapiClient.Call(ak::wwise::core::object::get, args, options, queryResult, 10))
        {
            if (queryResult.IsMap() && queryResult.HasKey("return"))
            {
                const auto& results = queryResult["return"].GetArray();

                if (results.empty())
                {
                    std::cout << "No default work units found." << std::endl;
                    return;
                }

                std::cout << "Default Work Units:" << std::endl;
                for (const auto& item : results)
                {
                    if (item.IsMap())
                    {
                        const auto& itemMap = item.GetMap();

                        std::string workUnitName = itemMap.at("name").GetVariant().GetString();
                        std::string workUnitID = itemMap.at("id").GetVariant().GetString();
                        std::string workUnitPath = itemMap.at("path").GetVariant().GetString();

                        std::cout << "- " << workUnitName << " (ID: " << workUnitID << ", Path: " << workUnitPath << ")" << std::endl;
                    }
                }
            }
            else
            {
                std::cerr << "Unexpected response format!" << std::endl;
            }
        }
        else
        {
            std::cerr << "Failed to retrieve default work units!" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while querying work units: " << e.what() << std::endl;
    }
}

void WAAPIManager::GetAllEvents()
{
    using namespace AK::WwiseAuthoringAPI;

    try
    {
        if (!waapiClient.IsConnected())
        {
            std::cerr << "Not connected to WAAPI!" << std::endl;
            return;
        }

        std::cout << "Retrieving all Wwise events..." << std::endl;

        AkJson queryResult;

        // Query all Event objects
        AkJson args(AkJson::Map{
            { "from", AkJson::Map{
                { "ofType", AkJson::Array{ AkVariant("Event") } }
            }}
            });

        AkJson options(AkJson::Map{
            { "return", AkJson::Array{
                AkVariant("id"),
                AkVariant("name"),
                AkVariant("path"),
                AkVariant("type"),
                AkVariant("parent.id")  
            }}
            });

        if (waapiClient.Call(ak::wwise::core::object::get, args, options, queryResult, 10))
        {
            if (queryResult.IsMap() && queryResult.HasKey("return"))
            {
                const auto& results = queryResult["return"].GetArray();

                if (results.empty())
                {
                    std::cout << "No events found in the project." << std::endl;
                    return;
                }

                std::cout << "Wwise Events:" << std::endl;
                for (const auto& item : results)
                {
                    if (item.IsMap())
                    {
                        const auto& itemMap = item.GetMap();

                        std::string eventID = itemMap.at("id").GetVariant().GetString();
                        std::string eventName = itemMap.at("name").GetVariant().GetString();
                        std::string eventPath = itemMap.at("path").GetVariant().GetString();
                        std::string eventType = itemMap.at("type").GetVariant().GetString();
                        std::string parentPath = "Events";  // Default root folder

                        std::string parentID = "N/A";  // Default value
                        if (itemMap.find("parent.id") != itemMap.end() && itemMap.at("parent.id").IsVariant())
                        {
                            parentID = itemMap.at("parent.id").GetVariant().GetString();
                            parentPath = itemMap.at("path").GetVariant().GetString();
                        }

                        std::cout << "- " << eventName <<"path:" << eventPath << " (ID: " << eventID << ", Type: " << eventType <<  std::endl;

                       
                        // Create event node
                        WwiseEventNode eventNode = { eventName, eventPath, eventID, eventType, {}};
                        // Fetch its descendants (Containers, Sound SFX)
                       // GetEventDescendants(eventName, eventPath);

                        // Insert into hierarchy
                        if (wwiseEventsTree.find(parentPath) == wwiseEventsTree.end())
                        {
                            wwiseEventsTree[parentPath] = { parentPath, parentPath, {} };
                        }
                        wwiseEventsTree[parentPath].children.push_back(eventNode);

                    }
                }
            }
            else
            {
                std::cerr << "Unexpected response format!" << std::endl;
            }
        }
        else
        {
            std::cerr << "Failed to retrieve events!" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while querying events: " << e.what() << std::endl;
    }
}

std::vector<WwiseEventNode> WAAPIManager::GetChildrenOfObject(const std::string& objectPath)
{
    using namespace AK::WwiseAuthoringAPI;

    std::vector<WwiseEventNode> children;

    AkJson queryResult;

    // WAQL query to get children of the given object
    std::string waqlQuery = "$ \"" + objectPath + "\" select children";

    AkJson args(AkJson::Map{
        { "waql", AkVariant(waqlQuery) }
        });

    AkJson options(AkJson::Map{
        { "return", AkJson::Array{
            AkVariant("id"),
            AkVariant("name"),
            AkVariant("path"),
            AkVariant("type")
        }}
        });

    if (waapiClient.Call(ak::wwise::core::object::get, args, options, queryResult, 10))
    {
        if (queryResult.IsMap() && queryResult.HasKey("return"))
        {
            const auto& results = queryResult["return"].GetArray();

            for (const auto& item : results)
            {
                if (item.IsMap())
                {
                    const auto& itemMap = item.GetMap();
                    std::string guid = itemMap.at("id").GetVariant().GetString();
                    std::string childName = itemMap.at("name").GetVariant().GetString();
                    std::string childPath = itemMap.at("path").GetVariant().GetString();
                    std::string type = itemMap.at("type").GetVariant().GetString();

                    if (type == "Event" || type == "Bus")
                    {
                        std::cout << " - Skipped: " << childName << " (" << type << ")" << std::endl;
                        continue;
                    }
                    WwiseEventNode childNode = { childName, childPath, guid, type };
                    children.push_back(childNode);

                    std::cout << " - Found child: " << childName << " (" << type << ")" << std::endl;
                }
            }
        }
    }

    return children;
}

std::vector<WwiseEventNode> WAAPIManager::GetEventDescendants(const std::string& eventName, const std::string& parentPath)
{
    using namespace AK::WwiseAuthoringAPI;

    std::vector<WwiseEventNode> descendants;

    if (!waapiClient.IsConnected())
    {
        std::cerr << "Not connected to WAAPI!" << std::endl;
        return descendants;
    }

    std::cout << "Retrieving descendants for event: " << eventName << std::endl;

    // WAQL query to get all children directly associated with the event
    AkJson queryResult;
    std::string waqlQuery = "$ from type event where name = \"" + eventName + "\" select children select target";

    AkJson args(AkJson::Map{
        { "waql", AkVariant(waqlQuery) }
        });

    AkJson options(AkJson::Map{
        { "return", AkJson::Array{
            AkVariant("id"),
            AkVariant("name"),
            AkVariant("path"),
            AkVariant("type")
        }}
        });

    if (waapiClient.Call(ak::wwise::core::object::get, args, options, queryResult, 10))
    {
        if (queryResult.IsMap() && queryResult.HasKey("return"))
        {
            const auto& results = queryResult["return"].GetArray();

            for (const auto& item : results)
            {
                if (item.IsMap())
                {
                    const auto& itemMap = item.GetMap();
                    std::string guid = itemMap.at("id").GetVariant().GetString();
                    std::string childName = itemMap.at("name").GetVariant().GetString();
                    std::string childPath = itemMap.at("path").GetVariant().GetString();
                    std::string type = itemMap.at("type").GetVariant().GetString();

                    // Create the current child node
                    WwiseEventNode childNode = { childName, childPath, guid, type };

                    // Recursive call to get children of this child
                    childNode.children = GetChildrenOfObject(childPath);

                    // For each child of this node, recursively get their children
                    for (auto& grandChild : childNode.children)
                    {
                        grandChild.children = GetChildrenOfObject(grandChild.path);
                    }

                    std::cout << " - Added descendant: " << childName << " (" << type << ")" << std::endl;

                    descendants.push_back(childNode);
                }
            }
        }
        else
        {
            std::cerr << "No descendants found for event: " << eventName << std::endl;
        }
    }
    else
    {
        std::cerr << "WAAPI query failed for event: " << eventName << std::endl;
    }

    return descendants;
}

void WAAPIManager::postWwiseEvent(const std::string& objectID)
{
    using namespace AK::WwiseAuthoringAPI;

    if (!waapiClient.IsConnected())
    {
        std::cerr << "WAAPI is not connected! Cannot post object." << std::endl;
        return;
    }

    if (objectID.empty())
    {
        std::cerr << "ERROR: Trying to play an object with an EMPTY ID!" << std::endl;
        return;
    }

    std::cout << "Posting Wwise Object: " << objectID << " to Wwise..." << std::endl;

    AkJson args(AkJson::Map{
        { "object", AkVariant(objectID) }
        });

    AkJson result;
    if (waapiClient.Call(ak::wwise::core::transport::create, args, AkJson(AkJson::Type::Map), result, 10))
    {
        uint64_t transportID = result["transport"].GetVariant().GetUInt32();
        std::cout << "Transport object created for: " << objectID << " with ID: " << transportID << std::endl;

        AkJson playArgs(AkJson::Map{
            { "action", AkVariant("play") },
            { "transport", AkVariant(transportID) }
            });

        
        if (waapiClient.Call(ak::wwise::core::transport::executeAction, playArgs, AkJson(AkJson::Type::Map), result, 10))
        {
            
            std::cout << "Successfully started playback for: " << objectID << std::endl;
        }
        else
        {
            std::cerr << "Failed to start playback for: " << objectID << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to create transport object for: " << objectID << std::endl;
    }

    GetVoices(objectID);
}

void WAAPIManager::GetVoices(const std::string& objectID)
{
    using namespace AK::WwiseAuthoringAPI;

    if (!waapiClient.IsConnected())
    {
        std::cerr << "Not connected to WAAPI!" << std::endl;
        return;
    }

    std::cout << "Starting Wwise Profiler Capture..." << std::endl;

    // Start profiler capture
    if (!waapiClient.Call(ak::wwise::core::profiler::startCapture, AkJson(AkJson::Map{}), AkJson(AkJson::Type::Map), AkJson(AkJson::Type::Map), 10))
    {
        std::cerr << "Failed to start Wwise profiler capture." << std::endl;
        return;
    }

    std::cout << "Retrieving Wwise Voices for Object: " << objectID << "..." << std::endl;

    // Define the query arguments
    AkJson args(AkJson::Map{
        { "time", AkVariant("capture")}  // 0 means latest available profiler capture time
        });

    // Define the return options (valid property names)
    AkJson options(AkJson::Map{
        { "return", AkJson::Array{
            AkVariant("gameObjectID"),
            AkVariant("soundID"),
            AkVariant("playingID"),
            AkVariant("objectName"),
            AkVariant("objectGUID"),
            AkVariant("playTargetName")  // Name of the target object playing
        }}
        });

    AkJson result;

    // Execute the WAAPI call
    if (waapiClient.Call(ak::wwise::core::profiler::getVoices, args, options, result))
    {
        
        if (!result.HasKey("return"))
        {
            std::cerr << "No 'return' key found in WAAPI response." << std::endl;
            return;
        }

        const auto& voicesArray = result["return"].GetArray();
        if (voicesArray.empty())
        {
            std::cout << "No active voices found." << std::endl;
            return;
        }
       
        for (const auto& voice : voicesArray)
        {
            std::cout << "===Voice Data===" << std::endl;
            if (!voice.IsMap()) continue;
            
            
            const auto& voiceMap = voice.GetMap();

            // Extract data correctly based on type
            uint64_t gameObjectID = 0;
            int32_t soundID = 0;
            int32_t playingID = 0;
            std::string objectName;
            std::string playTargetName;

            // store objectGUID as well, this is the variable we will pass to getOriginalFilePath
			std::string objectGUID;

               // Extract 'objectGUID'
			if (voiceMap.find("objectGUID") != voiceMap.end() && voiceMap.at("objectGUID").IsVariant())
			{
				const auto& var = voiceMap.at("objectGUID").GetVariant();
				if (var.IsString()) objectGUID = var.GetString();
			}

            // Extract `gameObjectID` (uint64_t)
            if (voiceMap.find("gameObjectID") != voiceMap.end() && voiceMap.at("gameObjectID").IsVariant())
            {
                const auto& var = voiceMap.at("gameObjectID").GetVariant();
                if (var.GetUInt64())
                    gameObjectID = var.GetUInt64();
                else if (var.GetInt32())
                    gameObjectID = static_cast<uint64_t>(var.GetInt32()); // Convert safely if needed
            }

            // Extract `soundID`
            if (voiceMap.find("soundID") != voiceMap.end() && voiceMap.at("soundID").IsVariant())
            {
                const auto& var = voiceMap.at("soundID").GetVariant();
                if (var.GetInt32()) soundID = var.GetInt32();
            }

            // Extract `playingID`
            if (voiceMap.find("playingID") != voiceMap.end() && voiceMap.at("playingID").IsVariant())
            {
                const auto& var = voiceMap.at("playingID").GetVariant();
                if (var.GetInt32()) playingID = var.GetInt32();
            }

            // Extract `objectName`
            if (voiceMap.find("objectName") != voiceMap.end() && voiceMap.at("objectName").IsVariant())
            {
                const auto& var = voiceMap.at("objectName").GetVariant();
                if (var.IsString()) objectName = var.GetString();
            }

            // Extract `playTargetName`
            if (voiceMap.find("playTargetName") != voiceMap.end() && voiceMap.at("playTargetName").IsVariant())
            {
                const auto& var = voiceMap.at("playTargetName").GetVariant();
                if (var.IsString()) playTargetName = var.GetString();
            }

            VoiceData newVoice;
            newVoice.gameObjectID = gameObjectID;
            newVoice.objectName = objectName;
            newVoice.objectGUID = objectGUID;
            newVoice.originalFilePath = originalFilePath;
            newVoice.audioBuffer = new AkAudioBuffer(); // Initialize buffer
            

            capturedVoices.push_back(newVoice);

            // Filter by objectID (this ensures you only get voices triggered by this object)
           
               GetOriginalFilePath(objectGUID);
                std::cout << "[Voice] Object: " << objectName
                    << " Object GUID: " << objectGUID
                    << ", Play Target: " << playTargetName
                    << ", GameObject ID: " << gameObjectID
                    << ", Sound ID: " << soundID
                    << ", Playing ID: " << playingID
                    <<" Original File Path: "<< originalFilePath
                    << std::endl;

                CapturedAudioStreamsDetails();

            
        }
    }
    else
    {
        std::cerr << "Failed to retrieve voice data from Wwise." << std::endl;
    }

    std::cout << "Stopping Wwise Profiler Capture..." << std::endl;

    // Start profiler capture
    if (!waapiClient.Call(ak::wwise::core::profiler::stopCapture, AkJson(AkJson::Map{}), AkJson(AkJson::Type::Map), AkJson(AkJson::Type::Map), 10))
    {
        std::cerr << "Failed to stop Wwise profiler capture." << std::endl;
        return;
    }
}

void WAAPIManager::GetOriginalFilePath( std::string& objectGUID)
{
    if (!waapiClient.IsConnected())
    {
        std::cerr << "Not connected to WAAPI!" << std::endl;
        return;
    }
    // store soundID as a string

    AkJson args(AkJson::Map{
        { "from", AkJson::Map{
            { "id", AkJson::Array{AkVariant(objectGUID)}}
        }}
        });

    AkJson options(AkJson::Map{
        { "return", AkJson::Array{
            AkVariant("type"),
            AkVariant("originalRelativeFilePath"),
            AkVariant("parent")
        }}
        });

    AkJson result;
    if (waapiClient.Call(ak::wwise::core::object::get, args, options, result))
    {
        if (!result.HasKey("return"))
        {
            std::cerr << "No 'return' key found in WAAPI response." << std::endl;
            return;
        }

        const auto& objects = result["return"].GetArray();
        if (objects.empty())
        {
            std::cerr << "No objects found for soundID: " << objectGUID << std::endl;
            return;
        }

        for (const auto& obj : objects)
        {
            if (!obj.IsMap()) continue;
            const auto& objMap = obj.GetMap();

            std::string type;
           

            if (objMap.find("type") != objMap.end() && objMap.at("type").IsVariant())
                type = objMap.at("type").GetVariant().GetString();

            if (objMap.find("originalRelativeFilePath") != objMap.end() && objMap.at("originalRelativeFilePath").IsVariant())
                originalFilePath = objMap.at("originalRelativeFilePath").GetVariant().GetString();

            if (type == "Sound")  // Ensure it's an actual Sound SFX
            {
                std::cout << "[File Path] Sound ID: " << objectGUID << " -> " << originalFilePath << std::endl;
            }
            else
            {
                std::cerr << "Sound ID: " << objectGUID << " is not a raw Sound object." << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Failed to retrieve file path for soundID: " << objectGUID << std::endl;
    }
}

void WAAPIManager::RegisterGameObjects() {

    std::cout << "[DEBUG] RegisterGameObjects() called!" << std::endl;
    // Use existing GetVoices data
    for (auto& voice : capturedVoices) {
        AK::SoundEngine::RegisterGameObj(voice.gameObjectID, voice.objectName.c_str());
    }


    AkGameObjectID gameObjectID = 100; // Arbitrary ID

    if (AK::SoundEngine::RegisterGameObj(gameObjectID, "VST_Host_GameObject") != AK_Success)
    {
        std::cerr << "[Wwise] Failed to register game object!" << std::endl;
        return;
    }

    std::cout << "[Wwise] Game Object Registered Successfully: " << gameObjectID << std::endl;
}


void WAAPIManager::InitializeSoundEngine() {

    std::cout << "[Wwise] Initializing Sound Engine..." << std::endl;
    // Memory Manager
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);
   
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        std::cerr << "[Wwise] Memory Manager Initialization Failed!" << std::endl;
        return;
    }

    

    // Stream Manager
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);
   
    if (!AK::StreamMgr::Create(stmSettings))
    {
        std::cerr << "[Wwise] Stream Manager Initialization Failed!" << std::endl;
        return;
    }
    

    // Sound Engine
    AkInitSettings initSettings;
    AkPlatformInitSettings platformSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformSettings);
    AK::SoundEngine::Init(&initSettings, &platformSettings);

    soundEngineContext = AK::SoundEngine::GetGlobalPluginContext();

    // Register audio render callback
    AK::SoundEngine::RegisterGlobalCallback(
        AudioRenderCallback,
        AkGlobalCallbackLocation_Begin,
        this
    );

    std::cout << "[Wwise] Sound Engine Initialized Successfully." << std::endl;
}

void WAAPIManager::ShutdownSoundEngine() {
    AK::SoundEngine::UnregisterGlobalCallback(AudioRenderCallback);
    AK::SoundEngine::Term();
   // AK::StreamMgr::Destroy();
    AK::MemoryMgr::Term();
}

void WAAPIManager::AudioRenderCallback(
    AK::IAkGlobalPluginContext* in_pContext,
    AkGlobalCallbackLocation in_eLocation,
    void* in_pCookie
) {
    if (in_eLocation == AkGlobalCallbackLocation_Begin) {
        WAAPIManager* manager = static_cast<WAAPIManager*>(in_pCookie);

      /*  if (manager->capturedVoices.empty()) {
            std::cerr << "[Wwise] No active voices in AudioRenderCallback!" << std::endl;
        }*/
       // std::cout << "[Wwise] Audio Render Callback Triggered!" << std::endl;



        manager->CaptureAudioStreams();

        
    }
}

void WAAPIManager::CaptureAudioStreams()
{
    /*std::cout << "========================" << std::endl;
    std::cout << "[Wwise] Capturing Audio Streams..." << std::endl;*/


    AK::SoundEngine::RenderAudio(); // This should update capturedVoices

   /* if (capturedVoices.empty()) {
        std::cerr << "[Wwise] No voices captured!" << std::endl;
    }*/

    for (auto& voice : capturedVoices) {
        if (!voice.audioBuffer) {
            voice.audioBuffer = new AkAudioBuffer();
        }

       
    }
}

void WAAPIManager::CapturedAudioStreamsDetails() {

    for (auto& voice : capturedVoices) {

        if (voice.audioBuffer->uValidFrames > 0) {
            std::cout << "[Wwise] Captured Voice: " << voice.objectName
                << " | Object GUID: " << voice.gameObjectID
                << " | Frames: " << voice.audioBuffer->uValidFrames
                << " | Channels: " << voice.audioBuffer->NumChannels() << std::endl;

        }
        else {
            std::cerr << "[Wwise] No valid audio captured for " << voice.objectName << std::endl;
        }

    }
   
}
