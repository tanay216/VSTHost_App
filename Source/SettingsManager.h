#pragma once
#include <juce_data_structures/juce_data_structures.h>

class SettingsManager
{
public:
    static SettingsManager& getInstance()
    {
        static SettingsManager instance;
        return instance;
    }

    void saveSetting(const juce::String& key, const juce::String& value)
    {
        settingsFile->setValue(key, value);
        settingsFile->save();
    }

    juce::String getSetting(const juce::String& key, const juce::String& defaultValue = "")
    {
        return settingsFile->getValue(key, defaultValue);
    }

private:
    SettingsManager()
    {
        juce::PropertiesFile::Options options;
        options.applicationName = "MyVSTHost";
        options.filenameSuffix = "settings";
        options.osxLibrarySubFolder = "Application Support";
       // options.folderName = "D:\\VSTHost_App\\Builds\\VisualStudio2022\\x64\\Debug\\App";
        

        settingsFile = std::make_unique<juce::PropertiesFile>(options);
    }

    ~SettingsManager() {}

    std::unique_ptr<juce::PropertiesFile> settingsFile;
};
