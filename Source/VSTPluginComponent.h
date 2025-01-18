/*
  ==============================================================================

    VSTPluginHost.h
    Created: 16 Dec 2024 12:38:06pm
    Author:  Admin

  ==============================================================================
*/

#pragma once
#ifndef VSTPLUGINHOST_H
#define VSTPLUGINHOST_H

#include <iostream>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <memory>
#include <string>
#include "AudioFileManager.h"
#include "Exporter.h"



using namespace juce;

class VSTPluginComponent {


private:
    
    std::unique_ptr<AudioDeviceManager> audioDeviceManager;
    Exporter exporter;
    juce::AudioProcessor* pluginProcessor;
    AudioSourcePlayer audioSourcePlayer;
    AudioTransportSource transportSource;

 
    
   // AudioBuffer<float> audioBuffer;
    AudioBuffer<float> monoOutputBuffer;
    AudioBuffer<float> stereoOutputBuffer;
    juce::AudioChannelSet inputBus;
    

    
    double sampleRate = 44100.0;
    int blockSize = 512;
    const PluginDescription* selectedPlugin = nullptr;
    bool isMono = false;
    AudioBuffer<float> pluginBuffer;

public:

    VSTPluginComponent();

    void initialiseAudio();
    void refereshAudioIODetails();
     juce::AudioDeviceManager* getDeviceManager() const
    {
        return audioDeviceManager.get();
    }


    std::unique_ptr<AudioPluginInstance> pluginInstance;
    void scanPlugins(const juce::String& pluginFilePath);
    
    KnownPluginList pluginList;
    void loadPlugin(int pluginIndex);
   // void handleBusChange(AudioProcessor* pluginInstance, AudioBuffer<float> audioBuffer);
    void refreshPlugin(int pluginIndex, AudioProcessor* pluginInstance);
    void debugBusAndChannelInfo(AudioProcessor* pluginInstance);
    void processAudioWithPlugin(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex);
    
   // void insertAtIndex(std::string modifiedFileName);
    void processMonoAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex);
    void processStereoAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex);
    void processMultiChannelAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
        int trimFromEndIndex,
        int rangeFromIndex,
        int rangeToIndex);
    const AudioBuffer<float>& getMonoOutputBuffer() const;  // Getter for monoOutputBuffer
    const AudioBuffer<float>& getStereoOutputBuffer() const; // Getter for stereoOutputBuffer
    void VSTPluginComponent::handleMultichannelConfiguration(int configId);
   /* void VSTPluginComponent::resizeAudioBuffer(const AudioProcessor::BusesLayout& layout);*/
    String VSTPluginComponent::getLayoutDescription(int configId);
    

    ~VSTPluginComponent();

    static VSTPluginComponent& getInstance();
};

#endif // VSTPLUGINHOST_H

