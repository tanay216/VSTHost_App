#ifndef EXPORTER_H
#define EXPORTER_H

#include <juce_audio_formats/juce_audio_formats.h>
#include <string>
#include "VSTPluginComponent.h"

using namespace juce;

class Exporter {
public:

    Exporter();
    ~Exporter();
    void exportAudioToFile(const AudioBuffer<float>& buffer, double sampleRate, const std::string& originalFileName);
    void exportFileName(const std::string& originalFileName, const std::string& prefix, const std::string& suffix);
    void batchRename(const juce::StringArray& inputFileNames, juce::StringArray& renamedFileNames, const std::string& prefix, const std::string& suffix);
    void resetOriginalNames(const juce::StringArray& inputFileNames); // Resets the mapping to the original names
    void updateRenamedFileNames(const juce::StringArray& renamedFileNames);

    const juce::StringArray& Exporter::getRenamedFileNames() const { return newRenamedFileNames;}
    void exportMonoAudio(const AudioBuffer<float>& buffer, double sampleRate);
    void exportStereoAudio(const AudioBuffer<float>& buffer, double sampleRate);
    void exportMultiChannelAudio(const AudioBuffer<float>& buffer, double sampleRate);

    std::string outputDirPath = "C:\\ProcessedAudio";

private:

    std::string extension;
    std::string outputFileName;
    std::string addSuffix;
    std::string addPrefix;
    juce::StringArray newRenamedFileNames;
    std::unordered_map<std::string, std::string> originalFileNameMap; // Maps renamed -> original names
    
    //static const std::string outputDirPath ; // Output Directory Path
    WavAudioFormat wavFormat;
    //VSTPluginComponent vstPluginComponent;
};

#endif // EXPORTER_H
