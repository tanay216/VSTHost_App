/*
  ==============================================================================

    SharedMemoryManager.h
    Created: 6 Feb 2025 9:55:25pm
    Author:  Admin

  ==============================================================================
*/

#pragma once
// SharedMemoryManager.h (Used by both projects)
#include <windows.h>  // For Windows shared memory
#include <JuceHeader.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <string>
#include <AK/SoundEngine/Common/AkCommonDefs.h>

struct SharedAudioBuffer {
    volatile bool updated;
    volatile bool processing;
    AkPlayingID playingID;
    AkUInt32 numChannels;
    AkUInt32 numSamples;
    float samples[8][1024]; // [frames][channels]
};

class SharedMemoryManager {
public:


    SharedMemoryManager() {
        // Windows shared memory setup
        hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "WwiseVSTBridge_AudioData");
        if (hMapFile) {
            buffer = static_cast<SharedAudioBuffer*>(MapViewOfFile(
                hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedAudioBuffer)));
        }
    }

    ~SharedMemoryManager() {
        if (buffer) UnmapViewOfFile(buffer);
        if (hMapFile) CloseHandle(hMapFile);
    }

    bool readAvailable() {
        return buffer && buffer->updated;
    }


    void readAudio(juce::AudioBuffer<float>& outputBuffer) {
        if (!buffer || !buffer->updated) {
            // outputBuffer.clear();
            return;
        }

        int numChannels = juce::jmin(outputBuffer.getNumChannels(),
            static_cast<int>(buffer->numChannels));
        int numSamples = outputBuffer.getNumSamples();

        std::cout << "------" << std::endl;
        std::cout << "[VST Host] Reading " << numSamples << " samples from shared memory." << std::endl;

        if (numSamples > outputBuffer.getNumSamples()) {
            std::cerr << "[VST Host] WARNING: Shared memory samples exceed buffer size! Clamping." << std::endl;
            numSamples = outputBuffer.getNumSamples();
        }

        else {
            std::cout << "[VST Host] Shared memory samples match buffer size." << std::endl;
        }
        for (int ch = 0; ch < numChannels; ++ch) {
            outputBuffer.copyFrom(ch, 0, buffer->samples[ch], numSamples);
        }

        buffer->updated = false;  // Reset the flag
        buffer->processing = true;
    }

    void writeProcessedAudio(const juce::AudioBuffer<float>& processedBuffer) {
        if (!buffer) return;

        int numChannels = juce::jmin(processedBuffer.getNumChannels(), static_cast<int>(buffer->numChannels));
        // int numSamples = processedBuffer.getNumSamples();
        int numSamples = juce::jmin(processedBuffer.getNumSamples(), static_cast<int>(buffer->numSamples));
        std::cout << "[VST Host] Writing " << numSamples << " samples to shared memory." << std::endl;
        std::cout << " - First Sample (Channel 0): " << processedBuffer.getReadPointer(0)[0] << std::endl;
        std::cout << "=========================================" << std::endl;

        for (int ch = 0; ch < numChannels; ++ch) {
            memcpy(buffer->samples[ch], processedBuffer.getReadPointer(ch), numSamples * sizeof(float));
        }

        buffer->updated = true;
    }

    void markProcessed() {
        if (buffer) {
            buffer->updated = false; // Reset flag after reading
        }
    }

    AkPlayingID getCurrentPlayingID() const {
        return buffer ? buffer->playingID : 0;
    }

    int getNumSamples() {
        return buffer ? buffer->numSamples : 0;
    }

    int getNumChannels() {
        return buffer ? buffer->numChannels : 0;
    }
private:
    HANDLE hMapFile;
    SharedAudioBuffer* buffer;
};