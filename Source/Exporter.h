#ifndef EXPORTER_H
#define EXPORTER_H

#include <juce_audio_formats/juce_audio_formats.h>
#include <string>
#include "VSTPluginComponent.h"

using namespace juce;

class Exporter {
public:
    void exportAudioToFile(const AudioBuffer<float>& buffer, double sampleRate, const std::string& originalFileName);
    void exportFileName(const std::string& originalFileName);
    void exportMonoAudio(const AudioBuffer<float>& buffer, double sampleRate);
    void exportStereoAudio(const AudioBuffer<float>& buffer, double sampleRate);
    void exportMultiChannelAudio(const AudioBuffer<float>& buffer, double sampleRate);

private:

    std::string extension;
    std::string outputFileName;
    std::string outputDirPath = "C:\\ProcessedAudio";
    //static const std::string outputDirPath ; // Output Directory Path
    WavAudioFormat wavFormat;
    //VSTPluginComponent vstPluginComponent;
};

#endif // EXPORTER_H
