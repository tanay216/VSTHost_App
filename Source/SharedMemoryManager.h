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
    AkPlayingID playingID;
    AkUInt32 numChannels;
    AkUInt32 numSamples;
    float samples[][8]; // [frames][channels]
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
        if (!buffer || !buffer->updated) return;

        const int numChannels = juce::jmin(outputBuffer.getNumChannels(),
            static_cast<int>(buffer->numChannels));
        const int numSamples = juce::jmin(outputBuffer.getNumSamples(),
            static_cast<int>(buffer->numSamples));

        for (int ch = 0; ch < numChannels; ++ch) {
            outputBuffer.copyFrom(ch, 0, buffer->samples[ch], numSamples);
        }

        buffer->updated = false;  // Reset the flag
    }

    void writeProcessedAudio(const juce::AudioBuffer<float>& processedBuffer) {
        if (!buffer) return;

        int numChannels = juce::jmin(processedBuffer.getNumChannels(), static_cast<int>(buffer->numChannels));
        int numSamples = juce::jmin(processedBuffer.getNumSamples(), static_cast<int>(buffer->numSamples));

        for (int ch = 0; ch < numChannels; ++ch) {
            memcpy(buffer->samples[ch], processedBuffer.getReadPointer(ch), numSamples * sizeof(float));
        }

        buffer->updated = true;
    }

    AkPlayingID getCurrentPlayingID() const {
        return buffer ? buffer->playingID : 0;
    }
private:
    HANDLE hMapFile;
    SharedAudioBuffer* buffer;
};