/*
  ==============================================================================

    VSTPluginHost.cpp
    Created: 16 Dec 2024 12:38:06pm
    Author:  Admin

  ==============================================================================
*/

#include "VSTPluginComponent.h"
#include "AudioFileManager.h"

VSTPluginComponent::VSTPluginComponent() {
    audioDeviceManager = std::make_unique<AudioDeviceManager>();
}

void VSTPluginComponent::initialiseAudio()
{

    audioDeviceManager->initialise(0, 2, nullptr, true);
    audioDeviceManager->addAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(&transportSource);
    refereshAudioIODetails();
   

}

void VSTPluginComponent::refereshAudioIODetails() 
{
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



void VSTPluginComponent::scanPlugins(const String& pluginFilePath)
{

    std::cout << "Starting scan for plugin: " << pluginFilePath << std::endl;

    // Use the file path instead of a directory
    FileSearchPath searchPath(pluginFilePath);
    VST3PluginFormat vst3Format;
    // KnownPluginList pluginList;

     // Instantiate PluginDirectoryScanner and pass the specific plugin format
    std::cout << "Initializing PluginDirectoryScanner..." << std::endl;
    PluginDirectoryScanner scanner(pluginList, vst3Format, searchPath, true, File());

    String pluginName;
    bool foundPlugins = false;

    // Scan through the directory and print the file found (should be just one plugin)
    while (scanner.scanNextFile(true, pluginName))
    {
        // std::cout << "Found file: " << pluginName << std::endl;
        std::cout << "----------------" << std::endl;
        foundPlugins = true; // If any files are found, set foundPlugins to true
    }

    if (!foundPlugins)
    {
        std::cout << "No plugins were found in the specified file: " << pluginFilePath << std::endl;
    }

    // Check if the pluginList has plugins and print them
    std::cout << "Scanning complete." << std::endl;
    std::cout << "================== Found " << pluginList.getNumTypes() << " plugins. ================== " << std::endl;

    if (pluginList.getNumTypes() == 0)
    {
        std::cout << "No plugins were detected or loaded." << std::endl;
        return;
    }

    // Display a list of detected plugins
    std::cout << "Detected plugins:" << std::endl;
    for (int i = 0; i < pluginList.getNumTypes(); ++i)
    {
        const PluginDescription* description = pluginList.getType(i);
        std::cout << "[" << i << "] " << description->name << " (By: " << description->manufacturerName
            << ", Category: " << description->category << ")" << std::endl;
    }
}

void VSTPluginComponent::loadPlugin(int pluginIndex)
{
    AudioPluginFormatManager formatManager;
    formatManager.addDefaultFormats();

    const auto* selectedPlugin = pluginList.getType(pluginIndex);  // Use auto to deduce the type

    if (selectedPlugin == nullptr)
    {
        std::cerr << "Invalid plugin selected!" << std::endl;
        return;
    }

    std::cout << "----------------" << std::endl;
    std::cout << "You selected: " << selectedPlugin->name << std::endl;

    // Initialize the plugin and audio device
    AudioDeviceManager audioDeviceManager;
    AudioDeviceManager::AudioDeviceSetup deviceSetup;

    audioDeviceManager.initialise(0, 2, nullptr, true);
    audioDeviceManager.getAudioDeviceSetup(deviceSetup);

   // double sampleRate = 44100.0;
    //int blockSize = 441;

    deviceSetup.sampleRate = sampleRate;
    deviceSetup.bufferSize = blockSize;
    audioDeviceManager.setAudioDeviceSetup(deviceSetup, true);

    String errorMessage;
    pluginInstance = formatManager.createPluginInstance(*selectedPlugin, sampleRate, blockSize, errorMessage);
   // handleBusChange(pluginInstance.get(), audioBuffer);

    if (pluginInstance)
    {
        std::cout << "Plugin loaded successfully!" << std::endl;

        // Display I/O Configuration
        int numInputChannels = pluginInstance->getTotalNumInputChannels();
        int numOutputChannels = pluginInstance->getTotalNumOutputChannels();
        std::cout << "UID: " << selectedPlugin->uniqueId << std::endl;
        std::cout << "name: " << selectedPlugin->name << std::endl;
        std::cout << "Manufacturer: " << selectedPlugin->manufacturerName << std::endl;
        std::cout << "category: " << selectedPlugin->category << std::endl;
        std::cout << "Input Channels: " << numInputChannels << std::endl;
        std::cout << "Output Channels: " << numOutputChannels << std::endl;

        // Display Parameters
        int numParameters = pluginInstance->getNumParameters();
        for (int paramIndex = 0; paramIndex < numParameters; ++paramIndex)
        {
            String parameterName = pluginInstance->getParameterName(paramIndex, 30);
            float defaultValue = pluginInstance->getParameterDefaultValue(paramIndex);
            std::cout << "  [" << paramIndex << "] " << parameterName
                << " (Default Value: " << defaultValue << ")" << std::endl;
        }
        pluginInstance->prepareToPlay(sampleRate, blockSize);
    }
    else
    {
        std::cerr << "Failed to load plugin instance for: " << selectedPlugin->name << std::endl;
        std::cerr << "Error: " << errorMessage << std::endl;
    }

    this->pluginInstance = std::move(pluginInstance);
    this->selectedPlugin = selectedPlugin;  // Store the selected plugin for later use
    std::cout << "------------------------" << std::endl;
    std::cout << "------------------------" << std::endl;
    // Query I/O Capabilities
    std::cout << "Supported Bus Layouts:" << std::endl;
    for (int busIdx = 0; busIdx < pluginInstance->getBusCount(true); ++busIdx)
    {
        auto layout = pluginInstance->getBusesLayout().getChannelSet(true, busIdx);
        std::cout << "Input Bus " << busIdx << ": " << layout.getDescription() << std::endl;
    }
    for (int busIdx = 0; busIdx < pluginInstance->getBusCount(false); ++busIdx)
    {
        auto layout = pluginInstance->getBusesLayout().getChannelSet(false, busIdx);
        std::cout << "Output Bus " << busIdx << ": " << layout.getDescription() << std::endl;
    }
}

//void VSTPluginComponent::handleBusChange(AudioProcessor* pluginInstance, AudioBuffer<float> audioBuffer)
//{
//    std::cout << "=======================" << std::endl;
//    std::cout << "handleBusChange called" << std::endl;
//    // Query the current layout
//    auto currentLayout = pluginInstance->getBusesLayout();
//   // int inputLayoutNumber = currentLayout.getChannelSet(true, 0).size();
//
//    std::cout << "Current Plugin Input Layout: " << currentLayout.getChannelSet(true, 0).getDescription() << std::endl;
//    std::cout << "Current Plugin Output Layout: " << currentLayout.getChannelSet(false, 0).getDescription() << std::endl;
//   
//   AudioProcessor::BusesLayout newLayout;
//    
//    newLayout.inputBuses.clear();
//    newLayout.outputBuses.clear();
//    
//    newLayout.inputBuses.add(AudioChannelSet::create7point1());
//    newLayout.outputBuses.add(AudioChannelSet::create7point1());
//    pluginInstance->setBusesLayout(newLayout);
//   // pluginInstance->getBusCount(true);
//
//    int NewInputChannels = newLayout.getMainInputChannels();
//    int NewOutputChannels = newLayout.getMainInputChannels();
//   // std::cout << "New Main Input Channels: " << NewInputChannels << std::endl;
//   //std::cout << "New Main Output Channels: " << NewOutputChannels << std::endl;
//
// 
//    if (pluginInstance->setBusesLayout(newLayout))
//    {
//        std::cout << "Successfully changed to 7.1 layout" << std::endl;
//        std::cout << "Current Plugin Input Layout: " << pluginInstance->getBusesLayout().getChannelSet(true, 0).getDescription() << std::endl;
//        std::cout << "Current Plugin Output Layout: " << pluginInstance->getBusesLayout().getChannelSet(false, 0).getDescription() << std::endl;
//        
//       auto numInputChannels = pluginInstance->getTotalNumInputChannels();
//       auto numOutputChannels = pluginInstance->getTotalNumOutputChannels();
//
//       // audioBuffer.setSize(std::max(numInputChannels, numOutputChannels), blockSize);
//
//        //DBG("Buffer resized for " + String(numInputChannels) + " input channels and " +
//            //String(numOutputChannels) + " output channels.");
//       // std::cout << "Buffer resized for " << String(numInputChannels) << "in channels and, "<< String(numOutputChannels)<<"output channnels." << std::endl;
//    }
//    else
//    {
//        //DBG("Failed to change bus layout.");
//        std::cout << "Failed to change bus layput" << std::endl;
//        auto layouts = pluginInstance->getBusesLayout();
//        std::cout << "Current input layout: " << layouts.getChannelSet(true, 0).getDescription() << std::endl;
//        std::cout << "Current output layout: " << layouts.getChannelSet(false, 0).getDescription() << std::endl;
//    }
//}

void VSTPluginComponent::refreshPlugin(int pluginIndex, AudioProcessor* pluginInstance)
{
    std::cout<<"refreshing plugin" << std::endl;
    auto currentLayout = pluginInstance->getBusesLayout();
    
    // int inputLayoutNumber = currentLayout.getChannelSet(true, 0).size();

    std::cout << "Current input Layout: " << currentLayout.getChannelSet(true, 0).getDescription() << std::endl;
    if (selectedPlugin == nullptr)
    {
        std::cerr << "Invalid plugin selected!" << std::endl;
        return;
    }

    std::cout << "----------------" << std::endl;
    std::cout << "You selected: " << selectedPlugin->name << std::endl;

    String errorMessage;

    if (pluginInstance)
    {
        std::cout << "Plugin loaded successfully!" << std::endl;

        // Display I/O Configuration
        int MainNumInputChannels = pluginInstance->getMainBusNumInputChannels();
        int MainNumOutputChannels = pluginInstance->getMainBusNumOutputChannels();
        
        
       // int numOutputChannels = pluginInstance->();
        std::cout << "UID: " << selectedPlugin->uniqueId << std::endl;
        std::cout << "name: " << selectedPlugin->name << std::endl;
        std::cout << "Manufacturer: " << selectedPlugin->manufacturerName << std::endl;
        std::cout << "category: " << selectedPlugin->category << std::endl;
        std::cout << "---------------" << std::endl;
        
        // Query Supported Bus Layouts
        auto currentLayout = pluginInstance->getBusesLayout();
        
        std::cout << "Current Layout Number of Channesls " << currentLayout.getNumChannels(true, 0)<< std::endl;
        std::cout << "Main Bus Input Chanenls: " << currentLayout.getMainInputChannels() << std::endl;
        std::cout << "Main Bus Output Channels: " << currentLayout.getMainOutputChannels() << std::endl;


        auto layout = pluginInstance->getBusesLayout();

        for (int busIdx = 0; busIdx < pluginInstance->getBusCount(true); ++busIdx)
        {
            inputBus = layout.getChannelSet(true, busIdx);
            std::cout << "Input Bus " << String(busIdx) << ": " << inputBus.getDescription() << std::endl;
        }

        for (int busIdx = 0; busIdx < pluginInstance->getBusCount(false); ++busIdx)
        {
            auto& outputBus = layout.getChannelSet(false, busIdx);
            std::cout << "Output Bus " << String(busIdx) << ": " << outputBus.getDescription() << std::endl;
        }
    }
    else
    {
        std::cerr << "No plugin instance " << std::endl;
        std::cerr << "Error: " << errorMessage << std::endl;
    }
    std::cout << "------------------------" << std::endl;
    debugBusAndChannelInfo(pluginInstance);


}
void VSTPluginComponent::debugBusAndChannelInfo(AudioProcessor* pluginInstance)
{
    if (!pluginInstance)
    {
        std::cerr << "Plugin instance is null!" << std::endl;
        return;
    }

    std::cout << "=====================" << std::endl;
    std::cout << "Debugging Plugin Buses and Channels..." << std::endl;
    std::cout << "Supports Multi-Channel: " << pluginInstance->supportsDoublePrecisionProcessing() << std::endl;

    // Total number of buses
    int inputBusCount = pluginInstance->getBusCount(true);
    int outputBusCount = pluginInstance->getBusCount(false);
    

    std::cout << "Input Buses: " << inputBusCount << std::endl;
    std::cout << "Output Buses: " << outputBusCount << std::endl;

    // Loop through all input buses
    for (int busIdx = 0; busIdx < inputBusCount; ++busIdx)
    {
        auto* inputBus = pluginInstance->getBus(true, busIdx);
        if (inputBus)
        {
            auto layout = inputBus->getCurrentLayout();
            std::cout << "Input Bus " << busIdx << " - Layout: " << layout.getDescription() << std::endl;

            for (int ch = 0; ch < layout.size(); ++ch)
            {
                auto channelIndex = inputBus->getChannelIndexInProcessBlockBuffer(ch);

                std::cout << "  [Input Bus " << busIdx << "] Channel " << ch
                    << ": " << layout.getTypeOfChannel(ch)
                    << " (Process Block Index: " << channelIndex << ")" << std::endl;
            }
        }
    }

    // Loop through all output buses
    for (int busIdx = 0; busIdx < outputBusCount; ++busIdx)
    {
        auto* outputBus = pluginInstance->getBus(false, busIdx);
        if (outputBus)
        {
            auto layout = outputBus->getCurrentLayout();
            std::cout << "Output Bus " << busIdx << " - Layout: " << layout.getDescription() << std::endl;

            for (int ch = 0; ch < layout.size(); ++ch)
            {
                auto channelIndex = outputBus->getChannelIndexInProcessBlockBuffer(ch);

                std::cout << "  [Output Bus " << busIdx << "] Channel " << ch
                    << ": " << layout.getTypeOfChannel(ch)
                    << " (Process Block Index: " << channelIndex << ")" << std::endl;
            }
        }
    }

    std::cout << "=====================" << std::endl;
}

void VSTPluginComponent::processAudioWithPlugin(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
    int trimFromEndIndex,
    int rangeFromIndex,
    int rangeToIndex,
    const std::string& regexPattern,
    const std::string& regexReplacement) {
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
        processMonoAudio(audioBuffer, loadedAudioFileNames, insert, insertIndex, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement); // Function to handle mono audio processing
    }
    else if (audioBuffer.getNumChannels() == 2)
    {
        isMono = false;
        processStereoAudio(audioBuffer, loadedAudioFileNames, insert, insertIndex, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement); // Function to handle stereo audio processing
    }
    else if (audioBuffer.getNumChannels() > 2)
    {
        processMultiChannelAudio(audioBuffer, loadedAudioFileNames, insert, insertIndex, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement); // Function to handle multi-channel audio processing
    }
}

void VSTPluginComponent::processMonoAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert,int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
    int trimFromEndIndex,
    int rangeFromIndex,
    int rangeToIndex,
    const std::string& regexPattern,
    const std::string& regexReplacement)
{
    std::cout << "Processing mono audio..." << std::endl;
    AudioFileManager audioFileManager;

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
    AudioBuffer<float> monoBuffer(pluginOutputChannels, audioBuffer.getNumSamples());
    //monoOutputBuffer.setSize(pluginOutputChannels, audioBuffer.getNumSamples());
    monoBuffer.clear();

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
            monoBuffer.copyFrom(0, pos, blockBuffer, 0, 0, numSamples);
            
        }
        
    }
   // duration = static_cast<double>(monoBuffer.getNumSamples()) / sampleRate;
    std::cout << "monoBuffer Channels: " << monoBuffer.getNumChannels() << std::endl;
    monoOutputBuffer.setSize(1, monoBuffer.getNumSamples());
    monoOutputBuffer.copyFrom(0, 0, monoBuffer, 0, 0, monoBuffer.getNumSamples()); // Copy from the left channel
    std::cout << "monoOutputBuffer Channels: " << monoOutputBuffer.getNumChannels() << std::endl;
    std::cout << "Mono processing complete." << std::endl;
    // Export the output buffer
    std::cout << "Audio Exporting Started." << std::endl;
    exporter.exportAudioToFile(monoOutputBuffer, sampleRate, loadedAudioFileNames, insert, insertIndex, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement );
   
}

void VSTPluginComponent::processStereoAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert,int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
    int trimFromEndIndex,
    int rangeFromIndex,
    int rangeToIndex,
    const std::string& regexPattern,
    const std::string& regexReplacement)
{
    std::cout << "Processing stereo audio..." << std::endl;
    AudioFileManager audioFileManager;

    if (!pluginInstance)
    {
        std::cerr << "No plugin instance available for processing!" << std::endl;
        return;
    }

    // Ensure stereo buffer
    if (audioBuffer.getNumChannels() != 2)
    {
        std::cerr << "Input is not stereo!" << std::endl;
        std::cout << "current number of channels: " << audioBuffer.getNumChannels() << std::endl;
        return;
    }

    int pluginInputChannels = pluginInstance->getTotalNumInputChannels();
    int pluginOutputChannels = pluginInstance->getTotalNumOutputChannels();

    // Create an output buffer
    stereoOutputBuffer.setSize(pluginOutputChannels, audioBuffer.getNumSamples());
    stereoOutputBuffer.clear();

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

    std::cout << "Stereo processing complete." << std::endl;
    // Export the output buffer
    std::cout << "Audio Exporting Started." << std::endl;
    exporter.exportAudioToFile(stereoOutputBuffer, sampleRate, loadedAudioFileNames, insert, insertIndex, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement);

}



void VSTPluginComponent::processMultiChannelAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
    int trimFromEndIndex,
    int rangeFromIndex,
    int rangeToIndex,
    const std::string& regexPattern,
    const std::string& regexReplacement)
{
    std::cerr << "================================" << std::endl;
    if (!pluginInstance)
    {
        std::cerr << "No plugin instance available for processing!" << std::endl;
        return;
    }

    auto currentLayout = pluginInstance->getBusesLayout();
    std::cout << "Multi-channel audio processing started. Current layout: " << currentLayout.getChannelSet(true, 0).getDescription() << ", num channels: " << currentLayout.getNumChannels(true, 0) << std::endl;
    std::cout << "audioBuffer sampleSize: " << audioBuffer.getNumSamples() << std::endl;
    std::cout << "audioBuffer RMS for channel 1: " << audioBuffer.getRMSLevel(1, 0, audioBuffer.getNumSamples()) << std::endl;
    std::cout << "audioBuffer RMS for channel 3: " << audioBuffer.getRMSLevel(3, 0, audioBuffer.getNumSamples()) << std::endl;
    int numInputChannels = audioBuffer.getNumChannels();
    int numPluginInputChannels = pluginInstance->getMainBusNumInputChannels();
    int numPluginOutputChannels = pluginInstance->getMainBusNumOutputChannels();

    if (numInputChannels > numPluginInputChannels)
    {
        std::cerr << "Warning: Input file has more channels than the plugin supports! Some channels may not be processed." << std::endl;
    }

    // Create a buffer for plugin processing
    juce::AudioBuffer<float> pluginInputBuffer(numPluginInputChannels, audioBuffer.getNumSamples());
    

    // Clear buffers
    pluginInputBuffer.clear();
   

	// Create a MidiBuffer (no MIDI in this case)
	MidiBuffer midiBuffer;

    // Map input buffer channels to plugin input channels
    for (int busIdx = 0; busIdx < pluginInstance->getBusCount(true); ++busIdx)
    {
        auto* inputBus = pluginInstance->getBus(true, busIdx);
        std::cout << "Bus " << busIdx << ": " << inputBus->getCurrentLayout().getDescription() << std::endl;
        std::cout << "Bus Size " << inputBus->getCurrentLayout().size() << std::endl;
        if (!inputBus)
            continue;

       // auto layout = inputBus->getCurrentLayout();
        for (int ch = 0; ch < inputBus->getCurrentLayout().size(); ++ch)
        {
            int processBlockIndex = inputBus->getChannelIndexInProcessBlockBuffer(ch);
            std::cout << "Input Process Block Index: " << processBlockIndex << std::endl;
            pluginInputBuffer.copyFrom(processBlockIndex, 0, audioBuffer, ch, 0, audioBuffer.getNumSamples());
            std::cout << "pluginInputBuffer sample size: " << pluginInputBuffer.getNumSamples() << std::endl;
            std::cout << "pluginInputBuffer RMS for channel "<< ch << ": " << pluginInputBuffer.getRMSLevel(ch, 0, pluginInputBuffer.getNumSamples()) << std::endl;
          /*  if (processBlockIndex < numPluginInputChannels && ch < numInputChannels)
            {
                
                
            }*/
        }
        std::cout << "pluginInputBuffer RMS for channel 1: " << pluginInputBuffer.getRMSLevel(1, 0, pluginInputBuffer.getNumSamples()) << std::endl;
        std::cout << "pluginInputBuffer RMS for channel 3: " << pluginInputBuffer.getRMSLevel(3, 0, pluginInputBuffer.getNumSamples()) << std::endl;
        
    }

    pluginInstance->processBlock(pluginInputBuffer, midiBuffer);
    for (int ch = 0; ch < pluginInputBuffer.getNumChannels(); ++ch) {
        std::cout << "Post-processBlock RMS for channel " << ch << ": "
            << pluginInputBuffer.getRMSLevel(ch, 0, pluginInputBuffer.getNumSamples()) << std::endl;
    }


    AudioBuffer<float> multioutputBuffer;
    // Map plugin output channels back to the output buffer
    for (int busIdx = 0; busIdx < pluginInstance->getBusCount(false); ++busIdx)
    {
        auto* outputBus = pluginInstance->getBus(false, busIdx);
        std::cout << "Output Bus " << busIdx << ": " << outputBus->getCurrentLayout().getDescription() << std::endl;
        if (!outputBus)
            continue;
        multioutputBuffer = AudioBuffer<float>(outputBus->getCurrentLayout().size(), audioBuffer.getNumSamples());

        //auto layout = outputBus->getCurrentLayout();
        for (int ch = 0; ch < outputBus->getCurrentLayout().size(); ++ch)
        {
            int processBlockIndex = outputBus->getChannelIndexInProcessBlockBuffer(ch);
            std::cout << "Output Process Block Index: " << processBlockIndex << std::endl;
            if (processBlockIndex < numPluginOutputChannels)
            {
                multioutputBuffer.copyFrom(ch, 0, pluginInputBuffer, processBlockIndex, 0, pluginInputBuffer.getNumSamples());
                std::cout<<"multioutputBuffer sample size: " << multioutputBuffer.getNumSamples() << std::endl;
                
            }
        }
        std::cout << "multioutputBuffer RMS for channel 1: " << multioutputBuffer.getRMSLevel(1, 0, multioutputBuffer.getNumSamples()) << std::endl;
        std::cout << "multioutputBuffer RMS for channel 3: " << multioutputBuffer.getRMSLevel(3, 0, multioutputBuffer.getNumSamples()) << std::endl;
    }

    // Export the processed audio
    exporter.exportAudioToFile(multioutputBuffer, sampleRate, loadedAudioFileNames, insert, insertIndex, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement);
}


const AudioBuffer<float>& VSTPluginComponent::getMonoOutputBuffer() const {
    return monoOutputBuffer;
}

const AudioBuffer<float>& VSTPluginComponent::getStereoOutputBuffer() const {
    return stereoOutputBuffer;
}

void VSTPluginComponent::handleMultichannelConfiguration(int configId)
{
    AudioPluginFormatManager formatManager;
    formatManager.addDefaultFormats();

    if (pluginInstance == nullptr)
    {
        std::cerr << "No plugin instance loaded!" << std::endl;
        return;
    }

    // Define layout based on configuration ID
    AudioProcessor::BusesLayout newLayout;

    switch (configId)
    {
    case 1: // Stereo (2.0)
        newLayout.inputBuses.clear();
        newLayout.inputBuses.add(AudioChannelSet::mono());

        newLayout.outputBuses.clear();
        newLayout.outputBuses.add(AudioChannelSet::stereo());
        break;

    case 2: // Surround (5.1)
        newLayout.inputBuses.clear();
        newLayout.inputBuses.add(AudioChannelSet::create5point1());

        newLayout.outputBuses.clear();
        newLayout.outputBuses.add(AudioChannelSet::create5point1());
        break;

    case 3: // Surround (7.1)
        newLayout.inputBuses.clear();
        newLayout.inputBuses.add(AudioChannelSet::create7point1());

        newLayout.outputBuses.clear();
        newLayout.outputBuses.add(AudioChannelSet::create7point1());
        break;

    default:
        jassertfalse; // Invalid configuration ID
        break;
    }

    // Set the bus layout for the plugin
    if (pluginInstance->setBusesLayout(newLayout))
    {
        std::cout << "Successfully changed to the " << getLayoutDescription(configId) << " layout." << std::endl;
        pluginInstance->prepareToPlay(sampleRate, blockSize);
    }
    else
    {
        std::cerr << "Failed to change bus layout." << std::endl;
    }

    // After setting the new layout, handle the audio processing
  //  pluginInstance->updateHostDisplay();
   // pluginInstance->reset();
    
    
   pluginInstance->prepareToPlay(sampleRate, blockSize);
}

//void VSTPluginComponent::resizeAudioBuffer(const AudioProcessor::BusesLayout& layout, AudioBuffer<float>& audioBuffer)
//{
//    const int numInputChannels = layout.getNumChannels(true, 0); // First input bus
//    const int numOutputChannels = layout.getNumChannels(false, 0); // First output bus
//
//    audioBuffer.setSize(numInputChannels, numOutputChannels);
//    //audioBuffer.clear();
//    std::cout << "Resized buffer for " << numInputChannels << " input channels and "
//        << numOutputChannels << " output channels." << std::endl;
//}


String VSTPluginComponent::getLayoutDescription(int configId)
{
    switch (configId)
    {
    case 1: return "Stereo (2.0)";
    case 2: return "Surround (5.1)";
    case 3: return "Surround (7.1)";
    default: return "Unknown Layout";
    }
}



VSTPluginComponent::~VSTPluginComponent() {
    //audioDeviceManager->removeAudioCallback(this);
}

VSTPluginComponent& VSTPluginComponent::getInstance() {
    static VSTPluginComponent instance;
    return instance;
}
