#pragma once
#pragma once

#include <windows.h>  // For Windows shared memory
#include <AK/SoundEngine/Common/AkTypes.h>
#include <string>
#include <AK/SoundEngine/Common/AkCommonDefs.h>

struct SharedAudioBuffer {
    volatile bool updated;
    AkPlayingID playingID;
    AkUInt32 numChannels;
    AkUInt32 numSamples;
    float samples[8][1024]; // [frames][channels]
};

class SharedMemoryManager {
public:
    SharedMemoryManager() {
        // Windows example
        hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
            sizeof(SharedAudioBuffer), L"WwiseVSTBridge_AudioData"
        );
        buffer = (SharedAudioBuffer*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    }

    bool readAvailable() { return buffer && buffer->updated; }

    const float* getProcessedSamples(AkUInt32 channel) {
        if (!buffer || channel >= buffer->numChannels) return nullptr;
        return buffer->samples[channel];
    }

    void markProcessed() {
        if (buffer) {
            buffer->updated = false; // Reset flag after reading
        }
    }

    void WriteBuffer(AkAudioBuffer* wwiseBuffer, AkPlayingID id) {
        buffer->playingID = id;
        buffer->numChannels = wwiseBuffer->NumChannels();
        buffer->numSamples = wwiseBuffer->uValidFrames;

        for (AkUInt32 ch = 0; ch < buffer->numChannels; ch++) {
            const AkReal32* src = wwiseBuffer->GetChannel(ch);
            memcpy(buffer->samples[ch], src, buffer->numSamples * sizeof(float));
        }
        buffer->updated = true;
    }

private:
    HANDLE hMapFile;
    SharedAudioBuffer* buffer;
};