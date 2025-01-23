#pragma once

#include <AK/WwiseAuthoringAPI/AkAutobahn/Client.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/AkJson.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/autobahn.h>
#include <AK/Tools/Common/AkAssert.h>
#include <AK/WwiseAuthoringAPI/waapi.h>
#include <AK/WwiseAuthoringAPI/AkJsonBase.h>
#include <iostream>
#include <map>
#include <vector>

using namespace AK::WwiseAuthoringAPI;

// Declare the global assert hook (but do NOT define it here)
extern AkAssertHook g_pAssertHook;

struct WwiseEventNode
{
    std::string name;
    std::string path;
    std::vector<WwiseEventNode> children;
};

class WAAPIManager
{
public:
    WAAPIManager();
    ~WAAPIManager();

    void connectToWAAPI();
    void getSessionInfo();
    void GetDefaultWorkUnits();
    void GetAllEvents();

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
    Client waapiClient;
    std::string wwiseProjectName = "";
    std::string wwiseVersion = "";
    std::string wwisePlatform = "";

    std::map<std::string, WwiseEventNode> wwiseEventsTree;

};
