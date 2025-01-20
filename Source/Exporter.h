#ifndef EXPORTER_H
#define EXPORTER_H

#include <juce_audio_formats/juce_audio_formats.h>
#include <string>
#include "VSTPluginComponent.h"
#include <regex>
#include "SettingsManager.h"

using namespace juce;

class Exporter {
public:

    Exporter();
    ~Exporter();
    void exportAudioToFile(const AudioBuffer<float>& buffer, double sampleRate, const std::string& originalFileName, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex,
        const std::string& regexPattern,
        const std::string& regexReplacement);
    void exportFileName(const std::string& currentFileName, const std::string& prefix, const std::string& insert, int insertIndex, const std::string& suffix, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex,
        const std::string& regexPattern,
        const std::string& regexReplacement);
    void batchRename(const juce::StringArray& inputFileNames, juce::StringArray& renamedFileNames, const std::string& prefix, const std::string& insert, int insertIndex, const std::string& suffix, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex,
        const std::string& regexPattern,
        const std::string& regexReplacement);
    void resetOriginalNames(const juce::StringArray& inputFileNames); // Resets the mapping to the original names
    void updateRenamedFileNames(const juce::StringArray& renamedFileNames);

    const juce::StringArray& Exporter::getRenamedFileNames() const { return newRenamedFileNames;}
    void exportMonoAudio(const AudioBuffer<float>& buffer, double sampleRate);
    void exportStereoAudio(const AudioBuffer<float>& buffer, double sampleRate);
    void exportMultiChannelAudio(const AudioBuffer<float>& buffer, double sampleRate);

    void saveLastExportFolder(const juce::String& path);
    juce::String getLastExportFolder() const;
    
    void saveRenameSettings(const std::string& prefix,
        const std::string& insert,
        int insertIndex,
        const std::string& suffix,
        const std::string& find,
        const std::string& replace,
        int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex,
        const std::string& regexPattern,
        const std::string& regexReplacement);
    std::unordered_map<std::string, juce::String> loadRenameSettings() const;
    void resetRenameSettings();


    std::string outputDirPath = "C:\\ProcessedAudio";

private:

    std::string extension;
    juce::String outputFileName;
    std::string addSuffix;
    std::string addPrefix;
    juce::StringArray newRenamedFileNames;
    std::unordered_map<std::string, std::string> originalFileNameMap; // Maps renamed -> original names
    
    //static const std::string outputDirPath ; // Output Directory Path
    WavAudioFormat wavFormat;
    //VSTPluginComponent vstPluginComponent;
};

#endif // EXPORTER_H
