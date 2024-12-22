/*
  ==============================================================================

    VSTPluginHost.cpp
    Created: 17 Dec 2024 9:55:25pm
    Author:  Admin

  ==============================================================================
*/

#include "VSTPluginHost.h"

    VSTPluginHost::VSTPluginHost()
    {
        duration = 0;
        audioDeviceManager = std::make_unique<AudioDeviceManager>();
    }

    VSTPluginHost::~VSTPluginHost() {

    }
    void VSTPluginHost::initialiseAudio()
    {
        audioDeviceManager->initialise(0, 2, nullptr, true);
        audioDeviceManager->addAudioCallback(&audioSourcePlayer);
        audioSourcePlayer.setSource(&transportSource);
        auto* device = audioDeviceManager->getCurrentAudioDevice();
        sampleRate = device->getCurrentSampleRate();
        blockSize = device->getCurrentBufferSizeSamples();
        std::cout << "===============================" << std::endl;
        std::cout << "Audio Device Initialised:" << std::endl;
        std::cout << "  Device Name: " << device->getName() << std::endl;
        std::cout << "  Type: " << device->getTypeName() << std::endl;
        std::cout << "  Sample Rate: " << sampleRate << std::endl;
        std::cout << "  Block Size: " << blockSize << " samples" << std::endl;
        std::cout << "  Input Channels: " << device->getActiveInputChannels().toString(2) << std::endl;
        std::cout << "  Output Channels: " << device->getActiveOutputChannels().toString(2) << std::endl;
        std::cout << "===============================" << std::endl;

    }

    void VSTPluginHost::playProcessedAudio(AudioBuffer<float>& buffer)
    {
        // AudioTransportSource transportSource;
        transportSource.prepareToPlay(buffer.getNumSamples(), sampleRate);
        // Use the buffer as input for the transport source
        transportSource.setSource(new MemoryAudioSource(buffer, false, false));
        // Play audio
        transportSource.start();
        std::cout << "Playing processed audio..." << std::endl;
        std::this_thread::sleep_for(std::chrono::duration<double>(duration));
        transportSource.stop();
        std::cout << "Playing Finished" << std::endl;
    }


    //void VSTPluginHost::scanPlugins(const String& pluginFilePath)
    //{
    //    DBG("Starting scan for plugin: " + pluginFilePath);

    //    // Use the file path instead of a directory
    //    FileSearchPath searchPath(pluginFilePath);
    //    // Create an instance of AudioPluginFormatManager to hold the plugin formats
    //    AudioPluginFormatManager formatManager;

    //    // Add VST3 plugin format to the format manager
    //    formatManager.addDefaultFormats();  // This will add VST3PluginFormat along with others like VSTPluginFormat

    //    // Instead of passing the formatManager, pass a specific format (e.g., VST3PluginFormat)
    //    VST3PluginFormat vst3Format; // You can replace VST3PluginFormat with other formats like VSTPluginFormat if needed

    //    // Instantiate PluginDirectoryScanner and pass the specific plugin format
    //    std::cout << "Initializing PluginDirectoryScanner..." << std::endl;
    //    PluginDirectoryScanner scanner(pluginList, vst3Format, searchPath, true, File());

    //    String pluginName;
    //    bool foundPlugins = false;

    //    // Scan through the directory and print the file found (should be just one plugin)
    //    while (scanner.scanNextFile(true, pluginName))
    //    {
    //        // std::cout << "Found file: " << pluginName << std::endl;
    //        std::cout << "----------------" << std::endl;
    //        foundPlugins = true; // If any files are found, set foundPlugins to true
    //    }

    //    if (!foundPlugins)
    //    {
    //        std::cout << "No plugins were found in the specified file: " << pluginFilePath << std::endl;
    //    }

    //    // Check if the pluginList has plugins and print them
    //    std::cout << "Scanning complete." << std::endl;
    //    std::cout << "================== Found " << pluginList.getNumTypes() << " plugins. ================== " << std::endl;

    //    if (pluginList.getNumTypes() == 0)
    //    {
    //        std::cout << "No plugins were detected or loaded." << std::endl;
    //        return;
    //    }

    //    // Display a list of detected plugins
    //    std::cout << "Detected plugins:" << std::endl;
    //    for (int i = 0; i < pluginList.getNumTypes(); ++i)
    //    {
    //        const PluginDescription* description = pluginList.getType(i);
    //        std::cout << "[" << i << "] " << description->name << " (By: " << description->manufacturerName
    //            << ", Category: " << description->category << ")" << std::endl;
    //    }


    //    while (true)
    //    {
    //        std::cout << "----------------" << std::endl;
    //        std::cout << "Enter the index of the plugin to load (0-" << pluginList.getNumTypes() - 1 << "): ";
    //        std::cin >> selectedIndex;

    //        if (std::cin.fail() || selectedIndex < 0 || selectedIndex >= pluginList.getNumTypes())
    //        {
    //            std::cin.clear(); // Clear error flag
    //            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
    //            std::cout << "Invalid index. Please try again." << std::endl;
    //        }
    //        else
    //        {
    //            break;
    //        }
    //    }

    //    selectedPlugin = pluginList.getType(selectedIndex);
    //    std::cout << "----------------" << std::endl;
    //    std::cout << "You selected: " << selectedPlugin->name << std::endl;

    //    // Load the selected plugin

    //    String errorMessage;
    //    AudioDeviceManager audioDeviceManager;
    //    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    //    audioDeviceManager.initialise(0, 2, nullptr, true);
    //    audioDeviceManager.getAudioDeviceSetup(deviceSetup);

    //    // Default sample rate and buffer size
    //    double sampleRate = 44100.0;
    //    int blockSize = 1024;

    //    deviceSetup.sampleRate = sampleRate;
    //    deviceSetup.bufferSize = blockSize;
    //    audioDeviceManager.setAudioDeviceSetup(deviceSetup, true);

    //    std::cout << "Using Sample Rate: " << sampleRate << ", Buffer Size: " << blockSize << std::endl;
    //    pluginInstance = formatManager.createPluginInstance(*selectedPlugin, sampleRate, blockSize, errorMessage);

    //    if (pluginInstance)
    //    {
    //        std::cout << "------------------------" << std::endl;
    //        std::cout << "------------------------" << std::endl;
    //        std::cout << "Plugin loaded successfully!" << std::endl;
    //        std::cout << "------------------------" << std::endl;
    //        std::cout << "------------------------" << std::endl;

    //        // Display I/O Configuration
    //        int numInputChannels = pluginInstance->getTotalNumInputChannels();
    //        int numOutputChannels = pluginInstance->getTotalNumOutputChannels();
    //        const PluginDescription* description = pluginList.getType(selectedIndex);
    //        std::cout << "UID: " << description->uniqueId << std::endl;
    //        std::cout << "Name: " << description->name << std::endl;
    //        std::cout << "Manufacturer: " << description->manufacturerName << std::endl;
    //        std::cout << "Category: " << description->category << std::endl;
    //        std::cout << "Type: " << description->pluginFormatName << std::endl;
    //        std::cout << "File Path: " << description->fileOrIdentifier << std::endl;
    //        std::cout << "------------------------" << std::endl;
    //        std::cout << "Audio I/O Configuration: " << std::endl;
    //        std::cout << "  Input Channels: " << numInputChannels << std::endl;
    //        std::cout << "  Output Channels: " << numOutputChannels << std::endl;
    //        std::cout << "------------------------" << std::endl;
    //        std::cout << "------------------------" << std::endl;

    //        // Display Parameters
    //        std::cout << "Key Parameters: " << std::endl;

    //        const int maxParametersToDisplay = 200;;
    //        int numParameters = pluginInstance->getNumParameters();
    //        int displayedParameters = 0;

    //        for (int paramIndex = 0; paramIndex < numParameters; ++paramIndex)
    //        {
    //            String parameterName = pluginInstance->getParameterName(paramIndex, 30);
    //            float defaultValue = pluginInstance->getParameterDefaultValue(paramIndex);
    //            if (parameterName.containsIgnoreCase("Volume") ||
    //                parameterName.containsIgnoreCase("Gain") ||
    //                parameterName.containsIgnoreCase("Pan") ||
    //                displayedParameters < maxParametersToDisplay)
    //            {
    //                std::cout << "  [" << paramIndex << "] " << parameterName
    //                    << " (Default Value: " << defaultValue << ")" << std::endl;
    //                ++displayedParameters;
    //            }

    //        }
    //        if (displayedParameters == 0)
    //        {
    //            std::cout << "  No key parameters detected." << std::endl;
    //        }
    //    }
    //    else
    //    {
    //        std::cerr << "Failed to load plugin instance for: " << selectedPlugin->name << std::endl;
    //        std::cerr << "Error: " << errorMessage << std::endl;
    //    }

    //    this->pluginInstance = std::move(pluginInstance); // Assign to class member
    //    this->selectedPlugin = selectedPlugin;           // Assign to class member
    //    std::cout << "------------------------" << std::endl;
    //}

    //File VSTPluginHost::scanAudioFiles(const String& audioDirectory)
    //{
    //    std::cout << "Scanning for audio files in: " << audioDirectory << std::endl;

    //    File directory(audioDirectory);
    //    if (!directory.isDirectory())
    //    {
    //        std::cerr << "Error: Invalid directory!" << std::endl;
    //        return {};
    //    }

    //    Array<File> audioFiles = directory.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
    //    if (audioFiles.isEmpty())
    //    {
    //        std::cerr << "No audio files found." << std::endl;
    //        return {};
    //    }

    //    std::cout << "Available audio files:" << std::endl;
    //    for (int i = 0; i < audioFiles.size(); ++i)
    //    {
    //        std::cout << "[" << i << "] " << audioFiles[i].getFileName() << std::endl;
    //    }
    //    std::cout << "------------------------" << std::endl;


    //    Array<int> selectedIndexes; // Array to store selected indexes
    //    int selectedFileIndex = -1;

    //    while (true)
    //    {
    //        std::cout << "Enter the index of the audio file to load (0-" << audioFiles.size() - 1 << "), -2 to select all, or -1 to finish: ";
    //        std::cin >> selectedFileIndex;

    //        if (selectedFileIndex <= -2) {
    //            for (int i = 0; i < audioFiles.size(); ++i) {
    //                selectedIndexes.add(i);

    //            }
    //            break;
    //        }

    //        if (selectedFileIndex == -1)
    //        {
    //            break; // Exit loop when user is done selecting files
    //        }

    //        if (selectedFileIndex >= 0 && selectedFileIndex < audioFiles.size())
    //        {
    //            selectedIndexes.add(selectedFileIndex);
    //        }
    //        else
    //        {
    //            std::cout << "Invalid index. Try again." << std::endl;
    //        }
    //    }

    //    // Now load all selected files
    //    for (int i = 0; i < selectedIndexes.size(); ++i)
    //    {
    //        loadAudioFile(audioFiles[selectedIndexes[i]]);
    //    }
    //}

    //void VSTPluginHost::loadAudioFile(const File& audioFile)
    //{
    //    AudioFormatManager audioFormatManager;
    //    audioFormatManager.registerBasicFormats(); // Register built-in audio formats.

    //    std::unique_ptr<AudioFormatReader> reader(audioFormatManager.createReaderFor(audioFile));
    //    if (!reader)
    //    {
    //        std::cerr << "Error: Unsupported or Corrupted audio file!" << std::endl;
    //        return;
    //    }

    //    audioBuffer.clear();
    //    audioBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
    //    reader->read(&audioBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
    //    float durationInSeconds = static_cast<float>(reader->lengthInSamples) / reader->sampleRate;


    //    std::cout << "Loaded audio file: " << audioFile.getFileName() << std::endl;
    //    std::cout << "Sample Rate: " << reader->sampleRate << ", Channels (Mono/Stereo): " << reader->numChannels
    //        << ", Length: " << reader->lengthInSamples << " samples, Duration: " << durationInSeconds << " seconds" << std::endl;


    //    loadedAudioFileNames = audioFile.getFileName().toStdString(); // Store the name of the loaded audio file

    //    if (audioBuffer.getNumSamples() == 0)
    //    {
    //        std::cerr << "No valid audio file loaded to process!" << std::endl;
    //        return;
    //    }
    //    processAudioWithPlugin();
    //}

    void VSTPluginHost::processAudioWithPlugin()
    {
        std::cout << "Processing:" << std::endl;
        if (!selectedPlugin || !pluginInstance)
        {
            std::cerr << "No plugin loaded to process!" << std::endl;
            return;
        }

        if (audioBuffer.getNumSamples() == 0)
        {
            std::cerr << "Audio buffer is empty!" << std::endl;
            return;
        }

        if (!pluginInstance)
        {
            std::cerr << "Error creating plugin instance: " << std::endl;
            return;
        }


        std::cout << "Channels at the start of processing: " << audioBuffer.getNumChannels() << std::endl;
        // Separate mono and stereo processing
        if (audioBuffer.getNumChannels() == 1)
        {
            isMono = true;
            processMonoAudio(); // Function to handle mono audio processing
        }
        else if (audioBuffer.getNumChannels() == 2)
        {
            isMono = false;
            processStereoAudio(); // Function to handle stereo audio processing
        }
    }

    void VSTPluginHost::processMonoAudio()
    {
        std::cout << "Processing mono audio..." << std::endl;

        if (!pluginInstance)
        {
            std::cerr << "No plugin instance available for processing!" << std::endl;
            return;
        }

        // Ensure mono buffer
        if (audioBuffer.getNumChannels() != 1)
        {
            std::cerr << "Input is not mono!" << std::endl;
            return;
        }

        int pluginInputChannels = pluginInstance->getTotalNumInputChannels();
        int pluginOutputChannels = pluginInstance->getTotalNumOutputChannels();

        if (pluginInputChannels > 1)
        {
            AudioBuffer<float> stereoBuffer(pluginInputChannels, audioBuffer.getNumSamples());
            stereoBuffer.clear();

            // Duplicate the mono channel into the stereo channels
            stereoBuffer.copyFrom(0, 0, audioBuffer, 0, 0, audioBuffer.getNumSamples());
            stereoBuffer.copyFrom(1, 0, audioBuffer, 0, 0, audioBuffer.getNumSamples());
            audioBuffer = std::move(stereoBuffer);
        }

        // Create an output buffer
        monoOutputBuffer.setSize(pluginOutputChannels, audioBuffer.getNumSamples());
        monoOutputBuffer.clear();

        // Prepare plugin instance for playback
        pluginInstance->prepareToPlay(44100.0, audioBuffer.getNumSamples());

        // Create a MidiBuffer (no MIDI in this case)
        MidiBuffer midiBuffer;

        // Process the audio buffer in blocks

        for (int pos = 0; pos < audioBuffer.getNumSamples(); pos += blockSize)
        {
            int numSamples = std::min(blockSize, audioBuffer.getNumSamples() - pos);
            AudioBuffer<float> blockBuffer(audioBuffer.getArrayOfWritePointers(), audioBuffer.getNumChannels(), pos, numSamples);
            pluginInstance->processBlock(blockBuffer, midiBuffer);

            // Add processed samples to the output buffer
            for (int channel = 0; channel < std::min(pluginOutputChannels, audioBuffer.getNumChannels()); ++channel)
            {
                monoOutputBuffer.copyFrom(0, pos, blockBuffer, 0, 0, numSamples);
            }
        }
        duration = static_cast<double>(monoOutputBuffer.getNumSamples()) / sampleRate;
        std::cout << "Mono processing complete." << std::endl;

        //playProcessedAudio(monoOutputBuffer);
        //std::cout << "press Y to export the audio: ";
        //char userInput;
        //std::cin >> userInput;

        //if (userInput == 'y' || userInput == 'Y')
        //{
        //    // Export the output buffer
        //    exportAudioToFile(monoOutputBuffer, sampleRate, loadedAudioFileNames);
        //    std::cout << "Audio exported successfully." << std::endl;
        //}

    }

    void VSTPluginHost::processStereoAudio()
    {
        std::cout << "Processing stereo audio..." << std::endl;

        if (!pluginInstance)
        {
            std::cerr << "No plugin instance available for processing!" << std::endl;
            return;
        }

        // Ensure stereo buffer
        if (audioBuffer.getNumChannels() != 2)
        {
            std::cerr << "Input is not stereo!" << std::endl;
            return;
        }

        int pluginInputChannels = pluginInstance->getTotalNumInputChannels();
        int pluginOutputChannels = pluginInstance->getTotalNumOutputChannels();

        // Create an output buffer
        stereoOutputBuffer.setSize(pluginOutputChannels, audioBuffer.getNumSamples());
        stereoOutputBuffer.clear();

        // Prepare plugin instance for playback
        pluginInstance->prepareToPlay(sampleRate, audioBuffer.getNumSamples());

        // Create a MidiBuffer (no MIDI in this case)
        MidiBuffer midiBuffer;

        // Process the audio buffer in blocks

        for (int pos = 0; pos < audioBuffer.getNumSamples(); pos += blockSize)
        {
            int numSamples = std::min(blockSize, audioBuffer.getNumSamples() - pos);
            AudioBuffer<float> blockBuffer(audioBuffer.getArrayOfWritePointers(), audioBuffer.getNumChannels(), pos, numSamples);
            pluginInstance->processBlock(blockBuffer, midiBuffer);

            // Add processed samples to the output buffer
            for (int channel = 0; channel < std::min(pluginOutputChannels, audioBuffer.getNumChannels()); ++channel)
            {
                stereoOutputBuffer.copyFrom(channel, pos, blockBuffer, channel, 0, numSamples);
            }
        }
        duration = static_cast<double>(stereoOutputBuffer.getNumSamples()) / sampleRate;

        std::cout << "Stereo processing complete." << std::endl;

        //playProcessedAudio(stereoOutputBuffer);
        //std::cout << "press Y to export the audio: ";
        //char userInput;
        //std::cin >> userInput;

        //if (userInput == 'y' || userInput == 'Y')
        //{
        //    // Export the output buffer
        //    exportAudioToFile(stereoOutputBuffer, sampleRate, loadedAudioFileNames);
        //    std::cout << "Audio exported successfully." << std::endl;
        //}

    }
    void VSTPluginHost::exportFileName(const std::string& originalFileName) {

        File audioFile(originalFileName); // Assume originalFileName is the full path or name with extension
        extension = audioFile.getFileExtension().toStdString();
        outputFileName = audioFile.getFileNameWithoutExtension().toStdString() + "_processed" + extension; // Add the suffix and extension
    }

    void VSTPluginHost::exportAudioToFile(const AudioBuffer<float>& buffer, double sampleRate, const std::string& originalFileName)
    {
        std::cout << "Exporting processed audio to file..." << std::endl;
        exportFileName(originalFileName);

        if (buffer == monoOutputBuffer) {
            exportMonoAudio(buffer, sampleRate);
        }
        if (buffer == stereoOutputBuffer) {
            exportStereoAudio(buffer, sampleRate);
        }
    }

    void VSTPluginHost::exportStereoAudio(const AudioBuffer<float>& buffer, double sampleRate) 
    {

        std::cout << "Exporting stereo audio to file..." << std::endl;

        File outputDir(outputDirPath);; // Specify the output file path
        File outputFile(outputDir.getChildFile(outputFileName));  // Use the new filename

        // Ensure the output directory exists
        if (!outputDir.exists())
        {
            if (!outputDir.createDirectory())
            {
                std::cerr << "Failed to create output directory: " << outputDir.getFullPathName() << std::endl;
                return;
            }
        }
        if (outputFile.exists())
        {
            outputFile.deleteFile();

        }
        // Prepare an output stream
        std::unique_ptr<FileOutputStream> stereoOutputStream(outputFile.createOutputStream());
        if (!stereoOutputStream)
        {
            std::cerr << "Failed to create output stream for " << outputFileName << std::endl;
            return;
        }
        // Set up the WAV format writer

        std::unique_ptr<AudioFormatWriter> writer(wavFormat.createWriterFor(stereoOutputStream.get(),
            44100, // Sample rate
            buffer.getNumChannels(), // Number of channels
            16, // Bits per sample
            {}, // Metadata
            0)); // Use the default quality
        if (!writer)
        {
            std::cerr << "Failed to create WAV writer for " << outputFileName << std::endl;
            return;
        }
        // Release the ownership of the stereoOutputStream to the writer
        stereoOutputStream.release();

        // Write the buffer to the file
        if (!writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples()))
        {
            std::cerr << "Failed to write audio buffer to " << outputFileName << std::endl;
        }
        else
        {
            std::cout << "Successfully exported audio to " << outputFileName << std::endl;
        }
    }

    void VSTPluginHost::exportMonoAudio(const AudioBuffer<float>& buffer, double sampleRate) 
    {
        std::cout << "Exporting mono audio to file..." << std::endl;

        // Ensure mono by creating a temporary buffer with only one channel
        AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
        monoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples()); // Copy from the left channel

        File outputDir(outputDirPath);; // Specify the output file path
        File outputFile(outputDir.getChildFile(outputFileName));  // Use the new filename

        // Ensure the output directory exists
        if (!outputDir.exists())
        {
            if (!outputDir.createDirectory())
            {
                std::cerr << "Failed to create output directory: " << outputDir.getFullPathName() << std::endl;
                return;
            }
        }
        if (outputFile.exists())
        {
            outputFile.deleteFile();
        }
        // Prepare an output stream
        std::unique_ptr<FileOutputStream> monoOutputStream(outputFile.createOutputStream());
        if (!monoOutputStream)
        {
            std::cerr << "Failed to create output stream for " << outputFileName << std::endl;
            return;
        }
        // Set up the WAV format writer
        WavAudioFormat wavFormat;
        std::unique_ptr<AudioFormatWriter> writer(wavFormat.createWriterFor(monoOutputStream.get(),
            44100, // Sample rate
            1, // Number of channels
            16, // Bits per sample
            {}, // Metadata
            0)); // Use the default quality
        if (!writer)
        {
            std::cerr << "Failed to create WAV writer for " << outputFileName << std::endl;
            return;
        }
        monoOutputStream.release(); // Release the ownership of the monoOutputStream to the writer

        // Write the buffer to the file
        if (!writer->writeFromAudioSampleBuffer(monoBuffer, 0, buffer.getNumSamples()))
        {
            std::cerr << "Failed to write audio buffer to " << outputFileName << std::endl;
        }
        else
        {
            std::cout << "Successfully exported audio to " << outputFileName << std::endl;
        }
    }

    static VSTPluginHost& getInstance()
    {
        static VSTPluginHost instance;
        return instance;
    }

//
//
//int main(int argc, char* argv[])
//{
//
//    if (argc < 2)
//    {
//        std::cerr << "Usage: vst_scanner <plugin_file_path>" << std::endl;
//        return 1;
//    }
//
//    String pluginFilePath(argv[1]);
//    String audioDirectory = "C:/AudioFiles";  // Fixed directory path for audio files
//    VSTPluginHost::getInstance().initialiseAudio();
//    VSTPluginHost::getInstance().scanPlugins(pluginFilePath); // Scan for VST plugins
//    VSTPluginHost::getInstance().scanAudioFiles(audioDirectory); // Store the selected audio file
//    return 0;
//}