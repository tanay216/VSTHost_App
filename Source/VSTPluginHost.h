/*
  ==============================================================================

    VSTPluginHost.h
    Created: 17 Dec 2024 9:55:25pm
    Author:  Admin

  ==============================================================================
*/

#pragma once
#pragma once

#include "VSTPluginHost.h"
#include <iostream>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>

using namespace juce;

class VSTPluginHost
{
public:
    VSTPluginHost();
    ~VSTPluginHost();

    void initialiseAudio();
    void playProcessedAudio(AudioBuffer<float>& buffer);
   // void scanPlugins(const juce::String& pluginFilePath);
    //File scanAudioFiles(const String& audioDirectory);
    //void loadAudioFile(const juce::File& audioFile);
    void processAudioWithPlugin();
    void processMonoAudio();
    void processStereoAudio();
    void exportFileName(const std::string& originalFileName);
    void exportAudioToFile(const AudioBuffer<float>& buffer, double sampleRate, const std::string& originalFileName);
    void exportStereoAudio(const AudioBuffer<float>& buffer, double sampleRate);
    void exportMonoAudio(const AudioBuffer<float>& buffer, double sampleRate);
   // void playAudio();
    //void stopAudio();
    int getPluginCount() const { return pluginList.getNumTypes(); }
    juce::String getPluginName(int index) const
    {
        if (index >= 0 && index < pluginList.getNumTypes())
            return pluginList.getType(index)->name;
        return "Invalid Index";
    }

private:
    std::unique_ptr<AudioDeviceManager> audioDeviceManager;
    AudioSourcePlayer audioSourcePlayer;
    AudioTransportSource transportSource;
    AudioFormatReaderSource* readerSource = nullptr;
    std::unique_ptr<AudioPluginInstance> pluginInstance;
    const PluginDescription* selectedPlugin = nullptr;
    AudioBuffer<float> audioBuffer;
    bool isMono = false;
    KnownPluginList pluginList;
    int selectedIndex = -1; // User input for selected plugin index
    std::string loadedAudioFileNames;
    std::string extension;
    std::string outputFileName;
    AudioBuffer<float> monoOutputBuffer;
    AudioBuffer<float> stereoOutputBuffer;
    WavAudioFormat wavFormat;
    double sampleRate = 44100.0;
    int blockSize = 512; // Processing block size
    double duration;
    std::string outputDirPath = "C:/ProcessedAudio"; // Output Directory Path

    //VSTPluginHost() = default;
};
