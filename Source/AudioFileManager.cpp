#include "AudioFileManager.h"
#include "VSTPluginHost.h"

AudioFileManager::AudioFileManager()
{
    // Register basic formats like WAV, AIFF, and MP3
    formatManager.registerBasicFormats();
}

AudioFileManager::~AudioFileManager() {}

std::vector<File> AudioFileManager::scanAudioFiles(const String& audioDirectory)
{
    std::cout << "Scanning for audio files in: " << audioDirectory << std::endl;

    File directory(audioDirectory);
    if (!directory.isDirectory())
    {
        std::cerr << "Error: Invalid directory!" << std::endl;
        return {};
    }

    Array<File> audioFiles = directory.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
    if (audioFiles.isEmpty())
    {
        std::cerr << "No audio files found." << std::endl;
        return {};
    }

    std::cout << "Available audio files:" << std::endl;
    for (int i = 0; i < audioFiles.size(); ++i)
    {
        std::cout << "[" << i << "] " << audioFiles[i].getFileName() << std::endl;
    }
    std::cout << "------------------------" << std::endl;


    Array<int> selectedIndexes; // Array to store selected indexes
    int selectedFileIndex = -1;

    while (true)
    {
        std::cout << "Enter the index of the audio file to load (0-" << audioFiles.size() - 1 << "), -2 to select all, or -1 to finish: ";
        std::cin >> selectedFileIndex;

        if (selectedFileIndex <= -2) {
            for (int i = 0; i < audioFiles.size(); ++i) {
                selectedIndexes.add(i);

            }
            break;
        }

        if (selectedFileIndex == -1)
        {
            break; // Exit loop when user is done selecting files
        }

        if (selectedFileIndex >= 0 && selectedFileIndex < audioFiles.size())
        {
            selectedIndexes.add(selectedFileIndex);
        }
        else
        {
            std::cout << "Invalid index. Try again." << std::endl;
        }
    }

    // Now load all selected files
    for (int i = 0; i < selectedIndexes.size(); ++i)
    {
        loadAudioFile(audioFiles[selectedIndexes[i]]);

    }

}

AudioBuffer<float> AudioFileManager::loadAudioFile(const File& audioFile)
{
    AudioFormatManager audioFormatManager;
    audioFormatManager.registerBasicFormats(); // Register built-in audio formats.

    std::unique_ptr<AudioFormatReader> reader(audioFormatManager.createReaderFor(audioFile));
    if (!reader)
    {
        std::cerr << "Error: Unsupported or Corrupted audio file!" << std::endl;
        return {};
    }
    

    audioBuffer.clear();
    audioBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
    reader->read(&audioBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
    float durationInSeconds = static_cast<float>(reader->lengthInSamples) / reader->sampleRate;


    std::cout << "Loaded audio file: " << audioFile.getFileName() << std::endl;
    std::cout << "Sample Rate: " << reader->sampleRate << ", Channels (Mono/Stereo): " << reader->numChannels
        << ", Length: " << reader->lengthInSamples << " samples, Duration: " << durationInSeconds << " seconds" << std::endl;
    fileSampleRate = reader->sampleRate;

    loadedAudioFileNames = audioFile.getFileName().toStdString(); // Store the name of the loaded audio file

    if (audioBuffer.getNumSamples() == 0)
    {
        std::cerr << "No valid audio file loaded to process!" << std::endl;
        return {};
    }
    std::cout << "Loaded" << std::endl;

    return audioBuffer;

}

//juce::AudioBuffer<float>& AudioFileManager::getAudioBuffer() {
//    return audioBuffer;  // Return the audio buffer
//}

