/*
  ==============================================================================

    AudioFileManager.h
    Created: 16 Dec 2024 3:14:53pm
    Author:  Admin

  ==============================================================================
*/

#pragma once
#ifndef AUDIOFILEMANAGER_H
#define AUDIOFILEMANAGER_H

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <iostream>  


using namespace juce;

class AudioFileManager
{
public:
    AudioFileManager();
    ~AudioFileManager();

    // Scans a directory for audio files and allows user selection
    std::vector<juce::File> scanAudioFiles(const juce::String& directoryPath);

    // Loads the selected audio file into a buffer
    juce::AudioBuffer<float> loadAudioFile(const juce::File& audioFile);
    const juce::AudioSampleBuffer& getAudioBuffer() const { return audioBuffer; }
    
    const std::string& AudioFileManager::getLoadedAudioFileNames() const {
        return loadedAudioFileNames;
    }
    
    AudioBuffer<float> audioBuffer;
    


private:
    juce::AudioFormatManager formatManager;
   // std::unique_ptr<AudioFormatReader> reader;
    std::string loadedAudioFileNames;
    double fileSampleRate = 0.0;
    
};

#endif // AUDIOFILEMANAGER_H

