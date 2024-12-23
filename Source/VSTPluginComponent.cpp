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

//void VSTPluginComponent::loadPlugin(int pluginIndex)
//{
//
//    AudioPluginFormatManager formatManager;
//    formatManager.addDefaultFormats();
//    int selectedIndex = -1;
//    while (true)
//    {
//        std::cout << "----------------" << std::endl;
//        std::cout << "Enter the index of the plugin to load (0-" << pluginList.getNumTypes() - 1 << "): ";
//
//        std::cin >> selectedIndex;
//
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
//
//    selectedPlugin = pluginList.getType(selectedIndex);
//    std::cout << "----------------" << std::endl;
//    std::cout << "You selected: " << selectedPlugin->name << std::endl;
//
//
//    // Load the selected plugin
//
//    String errorMessage;
//
//
//    AudioDeviceManager audioDeviceManager;
//    AudioDeviceManager::AudioDeviceSetup deviceSetup;
//
//    audioDeviceManager.initialise(0, 2, nullptr, true);
//    audioDeviceManager.getAudioDeviceSetup(deviceSetup);
//
//    // Default sample rate and buffer size
//    double sampleRate = 44100.0;
//    int blockSize = 1024;
//
//    deviceSetup.sampleRate = sampleRate;
//    deviceSetup.bufferSize = blockSize;
//    audioDeviceManager.setAudioDeviceSetup(deviceSetup, true);
//
//    std::cout << "Using Sample Rate: " << sampleRate << ", Buffer Size: " << blockSize << std::endl;
//    pluginInstance = formatManager.createPluginInstance(*selectedPlugin, sampleRate, blockSize, errorMessage);
//    // std::cout << "Sample Rate: " << sampleRate<< std::endl;
//    // std::cout << "Buffer Size: " << blockSize<< std::endl;
//
//    if (pluginInstance)
//    {
//        std::cout << "------------------------" << std::endl;
//        std::cout << "------------------------" << std::endl;
//        std::cout << "Plugin loaded successfully!" << std::endl;
//        std::cout << "------------------------" << std::endl;
//        std::cout << "------------------------" << std::endl;
//
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
//
//
//        std::cout << "------------------------" << std::endl;
//        std::cout << "Audio I/O Configuration: " << std::endl;
//        std::cout << "  Input Channels: " << numInputChannels << std::endl;
//        std::cout << "  Output Channels: " << numOutputChannels << std::endl;
//        std::cout << "------------------------" << std::endl;
//        std::cout << "------------------------" << std::endl;
//
//        // Display Parameters
//        std::cout << "Key Parameters: " << std::endl;
//
//        const int maxParametersToDisplay = 200;;
//        int numParameters = pluginInstance->getNumParameters();
//        int displayedParameters = 0;
//
//        for (int paramIndex = 0; paramIndex < numParameters; ++paramIndex)
//        {
//            String parameterName = pluginInstance->getParameterName(paramIndex, 30);
//            float defaultValue = pluginInstance->getParameterDefaultValue(paramIndex);
//            /*std::cout << "  [" << paramIndex << "] " << parameterName
//                << " (Default Value: " << defaultValue << ")" << std::endl;*/
//                // Display parameters based on relevance
//            if (parameterName.containsIgnoreCase("Volume") ||
//                parameterName.containsIgnoreCase("Gain") ||
//                parameterName.containsIgnoreCase("Pan") ||
//                displayedParameters < maxParametersToDisplay)
//            {
//                std::cout << "  [" << paramIndex << "] " << parameterName
//                    << " (Default Value: " << defaultValue << ")" << std::endl;
//                ++displayedParameters;
//            }
//
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
//
//    this->pluginInstance = std::move(pluginInstance); // Assign to class member
//    this->selectedPlugin = selectedPlugin;           // Assign to class member
//    std::cout << "------------------------" << std::endl;
//}

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
}

void VSTPluginComponent::handleBusChange(AudioProcessor* pluginInstance, AudioBuffer<float> audioBuffer)
{
    // Query the current layout
    auto currentLayout = pluginInstance->getBusesLayout();
    int inputLayoutNumber = currentLayout.getChannelSet(true, 0).size();

    std::cout << "Current input Layout: " << currentLayout.getChannelSet(true, 0).getDescription() << std::endl;
   // DBG("Current Input Layout: " + currentLayout.getChannelSet(true, 0).getDescription());
    //DBG("Current Output Layout: " + currentLayout.getChannelSet(false, 0).getDescription());

    // Dynamically set a new layout (e.g., 7.1 surround)
    AudioProcessor::BusesLayout newLayout;

    
    /*if (inputLayoutNumber >= 5 && inputLayoutNumber < 7) {

        newLayout.inputBuses.add(AudioChannelSet::create7point1());
        newLayout.outputBuses.add(AudioChannelSet::create7point1());
    }
    if (inputLayoutNumber >= 7 && inputLayoutNumber <= 10) {

        newLayout.inputBuses.add(AudioChannelSet::create7point1());
        newLayout.outputBuses.add(AudioChannelSet::create7point1());
    }*/
    newLayout.inputBuses.add(AudioChannelSet::create7point1());
    newLayout.outputBuses.add(AudioChannelSet::create7point1());

 
    if (pluginInstance->setBusesLayout(newLayout))
    {
        std::cout << "Successfully changed to 7.1 layout" << std::endl;

        // Resize buffers
        auto numInputChannels = pluginInstance->getTotalNumInputChannels();
        auto numOutputChannels = pluginInstance->getTotalNumOutputChannels();

        audioBuffer.setSize(std::max(numInputChannels, numOutputChannels), blockSize);

        //DBG("Buffer resized for " + String(numInputChannels) + " input channels and " +
            //String(numOutputChannels) + " output channels.");
        std::cout << "Buffer resized for " << String(numInputChannels) << "in channels and, "<< String(numOutputChannels)<<"output channnels." << std::endl;
    }
    else
    {
        //DBG("Failed to change bus layout.");
        std::cout << "Failed to change bus layput" << std::endl;
    }
}

void VSTPluginComponent::refreshPlugin(int pluginIndex)
{
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

        //// List All Buses and Their Channel Sets
        //std::cout << "Supported Bus Layouts:" << std::endl;
        //for (int busIdx = 0; busIdx < pluginInstance->getBusCount(false); ++busIdx) // Output buses
        //{
        //    auto& bus = *pluginInstance->getBus(false, busIdx);
        //    std::cout << "  Output Bus " << busIdx << ": " << bus.getCurrentLayout().getDescription() << std::endl;
        //}
        //for (int busIdx = 0; busIdx < pluginInstance->getBusCount(true); ++busIdx) // Input buses
        //{
        //    auto& bus = *pluginInstance->getBus(true, busIdx);
        //    std::cout << "  Input Bus " << busIdx << ": " << bus.getCurrentLayout().getDescription() << std::endl;
        //}

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
}
void VSTPluginComponent::processAudioWithPlugin(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames) {
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
        processMonoAudio(audioBuffer, loadedAudioFileNames); // Function to handle mono audio processing
    }
    else if (audioBuffer.getNumChannels() == 2)
    {
        isMono = false;
        processStereoAudio(audioBuffer, loadedAudioFileNames); // Function to handle stereo audio processing
    }
    else if (audioBuffer.getNumChannels() > 2)
    {
        processMultiChannelAudio(audioBuffer, loadedAudioFileNames);
    }
}

void VSTPluginComponent::processMonoAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames)
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
    exporter.exportAudioToFile(monoOutputBuffer, sampleRate, loadedAudioFileNames);
   
}

void VSTPluginComponent::processStereoAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames)
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
    exporter.exportAudioToFile(stereoOutputBuffer, sampleRate, loadedAudioFileNames);

}

void VSTPluginComponent::processMultiChannelAudio(AudioBuffer<float>& audioBuffer, const std::string& loadedAudioFileNames)
{
    std::cout << "Processing multichannel audio..." << std::endl;

    if (!pluginInstance)

    {
        std::cerr << "No plugin instance available for processing!" << std::endl;
        return;
    }

    int inputChannels = audioBuffer.getNumChannels();
    int pluginInputChannels = pluginInstance->getTotalNumInputChannels();
    int pluginOutputChannels = pluginInstance->getTotalNumOutputChannels();

    if (pluginInputChannels < inputChannels) {
        std::cerr << "Plugin cannot handle all input channels. " << pluginInputChannels << " channels." << std::endl;
    }

    int channelsToProcess = std::min(inputChannels, pluginInputChannels);

    // Create an output buffer
    juce::AudioBuffer<float> multichannelOutputBuffer(pluginOutputChannels, audioBuffer.getNumSamples());
    multichannelOutputBuffer.clear();

    // Create a MidiBuffer (no MIDI in this case)
    MidiBuffer midiBuffer;

    // Process the audio buffer in blocks
    for (int pos = 0; pos < audioBuffer.getNumSamples(); pos += blockSize)
    {
        int numSamples = std::min(blockSize, audioBuffer.getNumSamples() - pos);
        juce::AudioBuffer<float> blockBuffer(audioBuffer.getArrayOfWritePointers(), inputChannels, pos, numSamples);

        // Map input channels to plugin input
        juce::AudioBuffer<float> pluginInputBuffer(pluginInputChannels, numSamples);
        pluginInputBuffer.clear();
        std::cout << "channelsToProcess: " << channelsToProcess << std::endl;
        for (int ch = 0; ch < channelsToProcess; ++ch) {
            pluginInputBuffer.copyFrom(ch, 0, blockBuffer, ch, 0, numSamples);
        }

        // Process the block
        pluginInstance->processBlock(pluginInputBuffer, midiBuffer);

        // Map plugin output back to multichannelOutputBuffer
        for (int ch = 0; ch < std::min(pluginOutputChannels, inputChannels); ++ch) {
            multichannelOutputBuffer.copyFrom(ch, pos, pluginInputBuffer, ch, 0, numSamples);
        }
    }

    std::cout << "Multichannel processing complete." << std::endl;

    // Export the output buffer
    //std::cout << "Audio Exporting Started." << std::endl;
    exporter.exportAudioToFile(multichannelOutputBuffer, sampleRate, loadedAudioFileNames);
}
const AudioBuffer<float>& VSTPluginComponent::getMonoOutputBuffer() const {
    return monoOutputBuffer;
}

const AudioBuffer<float>& VSTPluginComponent::getStereoOutputBuffer() const {
    return stereoOutputBuffer;
}


VSTPluginComponent::~VSTPluginComponent() {
    //audioDeviceManager->removeAudioCallback(this);
}

VSTPluginComponent& VSTPluginComponent::getInstance() {
    static VSTPluginComponent instance;
    return instance;
}
