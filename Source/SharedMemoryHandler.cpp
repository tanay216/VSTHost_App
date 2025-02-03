#include "SharedMemoryHandler.h"
#include <iostream>
#include <cstring>

SharedMemoryHandler::SharedMemoryHandler() : hMapFile(NULL), pBuffer(nullptr), bufferSize(0) {}

SharedMemoryHandler::~SharedMemoryHandler()
{
    Shutdown();
}

bool SharedMemoryHandler::Initialize(const std::string& memName, int sampleRate)
{
    bufferSize = sampleRate * 2 * sizeof(float);  // 2 seconds of audio
    hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufferSize, memName.c_str());
    if (!hMapFile)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        return false;
    }

    pBuffer = (float*)MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, bufferSize);
    if (!pBuffer)
    {
        std::cerr << "Failed to map shared memory." << std::endl;
        CloseHandle(hMapFile);
        return false;
    }

    return true;
}

void SharedMemoryHandler::WriteAudioData(float* audioData, int numFrames, int numChannels)
{
    if (!pBuffer) return;

    memcpy(pBuffer, audioData, numFrames * numChannels * sizeof(float));
}

void SharedMemoryHandler::Shutdown()
{
    if (pBuffer) UnmapViewOfFile(pBuffer);
    if (hMapFile) CloseHandle(hMapFile);
}
