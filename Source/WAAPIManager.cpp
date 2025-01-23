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
        GetDefaultWorkUnits();
        // Disconnect when done
        waapiClient.Disconnect();
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

        if (waapiClient.Call(ak::wwise::core::getInfo, args, AkJson(AkJson::Type::Map), queryResult, 10))
        {
            std::cout << "Session info: " << queryResult["displayName"].GetVariant().GetString() << std::endl;
            std::cout << "Wwise Version: " << queryResult["version"]["displayName"].GetVariant().GetString() << std::endl;
            std::cout << "Platform: " << queryResult["platform"].GetVariant().GetString() << std::endl;
            
            
        }
        else
        {
            std::cerr << "Failed to retrieve session info!" << std::endl;
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








   


