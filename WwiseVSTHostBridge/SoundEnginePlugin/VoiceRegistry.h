#pragma once

// VoiceRegistry.h - Thread-safe voice registry
#pragma once
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <map>
#include <mutex>
#include <iostream>

struct VoiceData {
    AkPlayingID playingID;
    AkGameObjectID gameObjID;
    AkAudioBuffer* buffer;
    std::string objectName;
    std::string filePath;
};

class VoiceRegistry {
public:
    static VoiceRegistry& Instance() {
        static VoiceRegistry instance;
        return instance;
    }

    void RegisterVoice(AkPlayingID id, AkGameObjectID gameObj, const char* name) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_voices[id] = { id, gameObj, nullptr, name ? name : "", "" };
        std::cout << "Registered voice: " << name << std::endl;
    }


    /* static void UpdateVoiceRegistry(AkPlayingID playingID, AkAudioBuffer* buffer) {
         std::lock_guard<std::mutex> lock(m_mutex);
         m_voices[playingID] = buffer;
     }*/

    void UpdateBuffer(AkPlayingID id, AkAudioBuffer* buffer) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_voices.find(id);
        if (it != m_voices.end()) {
            it->second.buffer = buffer;
        }
        std::cout << "Updated buffer for voice: " << id << std::endl;
    }

    void UnregisterVoice(AkPlayingID id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_voices.erase(id);
        std::cout << "Unregistered voice: " << id << std::endl;
    }

    std::map<AkPlayingID, VoiceData> GetCurrentVoices() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_voices;
    }

private:
    /*  std::mutex m_mutex;
      std::map<AkPlayingID, VoiceData> m_voices;*/
    std::mutex m_mutex;
    // add unordered map
    std::map<AkPlayingID, VoiceData> m_voices;

};