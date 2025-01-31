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
//void WAAPIManager::SubscribeToVoices()
//{
//    using namespace AK::WwiseAuthoringAPI;
//
//    if (!waapiClient.IsConnected())
//    {
//        std::cerr << "Not connected to WAAPI!" << std::endl;
//        return;
//    }
//
//    std::cout << "Subscribing to Wwise Voices..." << std::endl;
//
//    // Define options for subscription (what data to return)
//    const AkJson& options(AkJson::Map{
//        { "return", AkJson::Array{
//            AkVariant("gameObject"),
//            AkVariant("sound"),
//            AkVariant("playingId"),
//            AkVariant("event"),
//            AkVariant("position"),
//            AkVariant("volume"),
//            AkVariant("pitch")
//        }}
//        });
//
//    // Future and promise setup
//    std::promise<bool> subscriptionPromise;
//    std::future<bool> subscriptionFuture = subscriptionPromise.get_future();
//    std::atomic_bool subscriptionCalled = false;
//
//    // Define callback to handle results
//    auto callback = [ &subscriptionCalled, &subscriptionPromise](uint64_t, const JsonProvider& result)
//        {
//            if (!subscriptionCalled)
//            {
//                subscriptionCalled = true;
//
//                // Extract voice data from result
//                const auto& voicesArray = result.GetAkJson()["voices"].GetArray();
//                for (const auto& voice : voicesArray)
//                {
//                    std::string soundName = voice["sound"].GetVariant().GetString();
//                    std::string eventName = voice["event"].GetVariant().GetString();
//
//                 //  float volume = voice["volume"].GetVariant().GetFloatValue();
//                    //float pitch = voice["pitch"].GetVariant().GetFloatValue();
//                    int gameObject = voice["gameObject"].GetVariant().GetInt32();
//
//                    std::cout << "[Voice Update] Sound: " << soundName
//                        << ", Event: " << eventName
//                        
//                        << ", GameObject ID: " << gameObject << std::endl;
//
//                    // Forward this data to the VST Host
//                   // SendVoiceDataToHost(soundName, eventName, volume, pitch, gameObject);
//                }
//
//                // Set the promise to signal that the subscription was processed
//                subscriptionPromise.set_value(true);
//            }
//        };
//
//    uint64_t subscriptionId = 0;
//    
//     
//    // Subscribe to the profiler for voices
//    AkJson subscribeResult;
//    if (!waapiClient.Subscribe(ak::wwise::core::profiler::getVoices, AkJson(AkJson::Type::Map), callback, subscriptionId, subscribeResult))
//    {
//        std::cerr << "Failed to subscribe to voice profiler." << std::endl;
//    }
//    else
//    {
//        std::cout << "Successfully subscribed to Wwise Voices!" << std::endl;
//        std::cout << "Subscription ID: " << subscriptionId << std::endl;
//        std::cout << "Waiting for voice updates..." << std::endl;
//
//       
//    }
//    
//
//    
//    // Block here until the subscription event has been processed
//  //  subscriptionFuture.get();
//
//    // Optionally unsubscribe or handle cleanup if needed
//   /* AkJson unsubscribeResult;
//    waapiClient.Unsubscribe(subscriptionId, unsubscribeResult);
//
//    std::cout << "Voice subscription completed." << std::endl;*/
//}


//void WAAPIManager::GetVoices()
//{
//    using namespace AK::WwiseAuthoringAPI;
//
//    if (!waapiClient.IsConnected())
//    {
//        std::cerr << "Not connected to WAAPI!" << std::endl;
//        return;
//    }
//
//    std::cout << "Retrieving Wwise Voices..." << std::endl;
//
//    // Define the query arguments
//    AkJson args(AkJson::Map{
//        { "time", AkVariant(0) }  // 0 means latest available profiler capture time
//        });
//
//    // Define the return options (valid property names)
//    AkJson result(AkJson::Map{
//        { "return", AkJson::Array{
//                 // GameObject ID
//            AkVariant("soundID"),           // Sound Name
//            AkVariant("playingID"),       // Unique playing instance ID
//                  // 3D position of the sound (if applicable)
//        }}
//        });
//
//    AkJson options;
//
//    // Execute the WAAPI call
//    if (waapiClient.Call(ak::wwise::core::profiler::getVoices, args, options, result))
//    {
//        if (!result.HasKey("voices"))
//        {
//            std::cerr << "No 'voices' key found in WAAPI response." << std::endl;
//            return;
//        }
//
//        const auto& voicesArray = result["voices"].GetArray();
//        if (voicesArray.empty())
//        {
//            std::cout << "No active voices found." << std::endl;
//            return;
//        }
//
//        for (const auto& voice : voicesArray)
//        {
//            std::string soundName = voice["sound"].GetVariant().GetString();
//            std::string eventName = voice["event"].GetVariant().GetString();
//            int gameObjectID = voice["gameObject"].GetVariant().GetInt32();
//            int playingID = voice["playingId"].GetVariant().GetInt32();
//
//            std::cout << "[Voice Data] Sound: " << soundName
//                << ", Event: " << eventName
//                << ", GameObject ID: " << gameObjectID
//                << ", Playing ID: " << playingID
//                << std::endl;
//        }
//    }
//    else
//    {
//        std::cerr << "Failed to retrieve voice data from Wwise." << std::endl;
//    }
//}

void WAAPIManager::GetVoices(const std::string& objectID)
{
    using namespace AK::WwiseAuthoringAPI;

    if (!waapiClient.IsConnected())
    {
        std::cerr << "Not connected to WAAPI!" << std::endl;
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



            /*std::string gameObjectID = voice["gameObjectID"].GetVariant().GetString();
            std::string soundID = voice["soundID"].GetVariant().GetString();
            std::string  playingID = voice["playingID"].GetVariant().GetString();
            std::string objectName = voice["objectName"].GetVariant().GetString();
            std::string playTargetName = voice["playTargetName"].GetVariant().GetString();*/

            // Extract data correctly based on type
            uint64_t gameObjectID = 0;
            int32_t soundID = 0;
            int32_t playingID = 0;
            std::string objectName;
            std::string playTargetName;


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



            // Filter by objectID (this ensures you only get voices triggered by this object)
           
               
                std::cout << "[Voice] Object: " << objectName
                    << ", Play Target: " << playTargetName
                    << ", GameObject ID: " << gameObjectID
                    << ", Sound ID: " << soundID
                    << ", Playing ID: " << playingID
                    << std::endl;
            

            
        }
    }
    else
    {
        std::cerr << "Failed to retrieve voice data from Wwise." << std::endl;
    }
}






//void WAAPIManager::SendVoiceDataToHost(const std::string& soundName, const std::string& eventName, float volume, float pitch, int gameObject)
//{
//    // Implement your method to send voice data to your JUCE-based VST host
//    // Example: Using JUCE Message Manager or Networking
//}
