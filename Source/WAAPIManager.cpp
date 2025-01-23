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

                        std::string eventName = itemMap.at("name").GetVariant().GetString();
                        std::string eventPath = itemMap.at("path").GetVariant().GetString();
                        std::string parentPath = "Events";  // Default root folder

                        std::string parentID = "N/A";  // Default value
                        if (itemMap.find("parent.id") != itemMap.end() && itemMap.at("parent.id").IsVariant())
                        {
                            parentID = itemMap.at("parent.id").GetVariant().GetString();
                            parentPath = itemMap.at("path").GetVariant().GetString();
                        }

                        std::cout << "- " << eventName << " (" << eventPath << ") | Parent ID: " << parentID << std::endl;

                       
                        // Create event node
                        WwiseEventNode eventNode = { eventName, eventPath, {} };

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










   


