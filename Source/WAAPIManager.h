#pragma once

#include <AK/WwiseAuthoringAPI/AkAutobahn/Client.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/AkJson.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/autobahn.h>
#include <AK/Tools/Common/AkAssert.h>
#include <AK/WwiseAuthoringAPI/waapi.h>
#include <AK/WwiseAuthoringAPI/AkJsonBase.h>
#include <iostream>

using namespace AK::WwiseAuthoringAPI;

// Declare the global assert hook (but do NOT define it here)
extern AkAssertHook g_pAssertHook;

class WAAPIManager
{
public:
    WAAPIManager();
    ~WAAPIManager();

    void connectToWAAPI();
    void getSessionInfo();
    void GetDefaultWorkUnits();
   
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
};
