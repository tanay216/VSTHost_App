#pragma once
#include <windows.h>
#include <string>

class SharedMemoryHandler
{
public:
    SharedMemoryHandler();
    ~SharedMemoryHandler();

    bool Initialize(const std::string& memName, int sampleRate);
    void WriteAudioData(float* audioData, int numFrames, int numChannels);
    void Shutdown();

private:
    HANDLE hMapFile;
    float* pBuffer;
    size_t bufferSize;
};
