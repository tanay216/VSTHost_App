#include "MainComponent.h"
#define NUM_VOICE_BUFFERS 4 


// Constructors ===============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
   // you add any child components.

    waapiManager.connectToWAAPI();
    // OpenSharedMemory();
    // waapiManager.InitializeSoundEngine();
     //waapiManager.RegisterGameObjects();
    // metadataReceiver.startListening();

    formatManager.registerBasicFormats();


    juce::Component::addAndMakeVisible(viewport);
    viewport.setViewedComponent(&contentComponent, false);

    juce::Component::addAndMakeVisible(scanPluginButton);
    scanPluginButton.triggerClick();
    juce::Component::addAndMakeVisible(loadAudioButton);
    juce::Component::addAndMakeVisible(playButton);
    juce::Component::addAndMakeVisible(stopButton);
    juce::Component::addAndMakeVisible(pluginListDropdown);
    juce::Component::addAndMakeVisible(exportAudioButton);
    juce::Component::addAndMakeVisible(refreshPluginDetailsButton);
    juce::Component::addAndMakeVisible(unloadPluginButton);
    juce::Component::addAndMakeVisible(ShowEditorButton);
    juce::Component::addAndMakeVisible(audioFileTree);
    juce::Component::addAndMakeVisible(refreshIODevicesListButton);
    juce::Component::addAndMakeVisible(refreshFileTreeButton);
    refreshFileTreeButton.triggerClick();

    juce::Component::addAndMakeVisible(inputDeviceDropdown);
    inputDeviceDropdown.onChange = [this]() { changeAudioDevice(true); }; // Handle input selection


    juce::Component::addAndMakeVisible(outputDeviceDropdown);
    outputDeviceDropdown.onChange = [this]() { changeAudioDevice(false); }; // Handle output selection

    // Attach as a listener for audio device changes

    populateAudioDeviceDropdowns();

    // Wwise Status Label
    juce::Component::addAndMakeVisible(wwiseStatusLabel);
    wwiseStatusLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    wwiseStatusLabel.setJustificationType(juce::Justification::centred);
    wwiseStatusLabel.setBounds(10, 10, 300, 30); // Adjust size/position
    updateWwiseStatus();

    // updateWwiseTree();

     //// Wwise Tree View
     //juce::Component::addAndMakeVisible(wwiseTree);
     //updateWwiseTree();

    audioFileTree.setColour(juce::TreeView::backgroundColourId, juce::Colours::antiquewhite);
    audioFileTree.setDefaultOpenness(true);
    std::string rootName = "Root";
    auto rootItem = std::make_unique<AudioFileTreeItem>(rootName, nullptr, nullptr, this);
    auto wwiseRootItem = std::make_unique<WwiseTreeItem>(WwiseEventNode{ "Wwise Events", "", {} }, nullptr, this);
    wwiseTree.setDefaultOpenness(false);
    audioFileTree.setRootItem(rootItem.release());
    audioFileTree.setInterceptsMouseClicks(true, true); // Enable clicks globally
    audioFileTree.setWantsKeyboardFocus(true);



    // Configure the multichannel configuration dropdown
    juce::Component::addAndMakeVisible(channelConfigDropdown);
    channelConfigDropdown.addItem("Stereo (2.0)", 1);
    channelConfigDropdown.addItem("Surround (5.1)", 2);
    channelConfigDropdown.addItem("Surround (7.1)", 3);
    channelConfigDropdown.setSelectedId(1); // Default to Stereo
    channelConfigDropdown.onChange = [this]()
        {

            vstPluginComponent.handleMultichannelConfiguration(channelConfigDropdown.getSelectedId());

            vstPluginComponent.getLayoutDescription(channelConfigDropdown.getSelectedId());

        };

    // Audio file list setup
   // audioFileListBox.setModel(this); // This component will handle the interaction
   // audioFileListBox.setMultipleSelectionEnabled(false); // Disable multiple selections
   // juce::Component::addAndMakeVisible(audioFileListBox);
    juce::Component::setSize(800, 600);

    scanPluginButton.setButtonText("Scan Plugins");
    scanPluginButton.addListener(this);
    loadAudioButton.addListener(this);
    pluginListDropdown.onChange = [this]()
        {
            int selectedIndex = pluginListDropdown.getSelectedId() - 1; // Adjust for 1-based indexing
            if (selectedIndex >= 0 && selectedIndex < vstPluginComponent.pluginList.getNumTypes())
            {
                vstPluginComponent.loadPlugin(selectedIndex);
            }
        };
    playButton.addListener(this);
    stopButton.addListener(this);
    exportAudioButton.addListener(this);
    refreshPluginDetailsButton.addListener(this);
    unloadPluginButton.addListener(this);
    ShowEditorButton.addListener(this);
    refreshIODevicesListButton.addListener(this);
    refreshFileTreeButton.addListener(this);



    // Initialise for System Audio Driver
    vstPluginComponent.initialiseAudio();


    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    //shutdownAudio();
 //   waapiManager.ShutdownSoundEngine();
   // metadataReceiver.stopListening();
}




// Wwise Tree View ==============================================================================


void MainComponent::updateWwiseStatus()
{
    std::string statusText = "Wwise Connected: " + waapiManager.getWwiseProjectName() +
        " (" + waapiManager.getWwiseVersion() + " - " + waapiManager.getWwisePlatform() + ")";
    wwiseStatusLabel.setText(statusText, juce::dontSendNotification);
}

void MainComponent::updateWwiseTree() {
    std::cout << "Updating Wwise Tree..." << std::endl;

    if (audioFileTree.getRootItem() == nullptr) {
        std::string rootName = "Root";
        auto rootItem = std::make_unique<AudioFileTreeItem>(rootName, nullptr, nullptr, this);
        audioFileTree.setRootItem(rootItem.release());
    }

    auto* rootItem = audioFileTree.getRootItem();
    if (!rootItem) {
        std::cerr << "ERROR: Root item is still null!" << std::endl;
        return;
    }

    // Fetch Wwise event hierarchy
    auto eventFolderMap = waapiManager.getWwiseEventsTree();

    // Track added paths to avoid duplication
    std::set<std::string> addedPaths;

    // Add each event as a root item directly
    for (const auto& [folderPath, folderNode] : eventFolderMap) {
        for (const auto& event : folderNode.children) {
            // Only add the event if it's not already added
            if (addedPaths.find(event.path) == addedPaths.end()) {

                auto removeCallback = [this, rootItem](WwiseTreeItem* item)
                    {
                        if (rootItem)
                        {
                            for (int i = 0; i < rootItem->getNumSubItems(); ++i)
                            {
                                if (rootItem->getSubItem(i) == item)
                                {
                                    rootItem->removeSubItem(i, true);
                                    break;
                                }
                            }
                        }
                    };

                auto* eventItem = new WwiseTreeItem(event, removeCallback, this);

                rootItem->addSubItem(eventItem);
                addedPaths.insert(event.path);

                // Retrieve and add nested hierarchy (containers & SFX)
                auto descendants = waapiManager.GetEventDescendants(event.name, event.path);
                addChildItems(eventItem, descendants, addedPaths);

                // DEBUG: Store and print details for verification
                std::cout << "Added Event: " << event.name << "| Path: " << event.path << std::endl;
                for (const auto& descendant : descendants) {
                    std::cout << "  Descendant: " << descendant.name << " Path: " << descendant.path
                        << " GUID: " << descendant.guid << " Type: " << descendant.type << std::endl;
                }
            }
        }
    }

    audioFileTree.getRootItem()->treeHasChanged();
    audioFileTree.repaint();
}

void MainComponent::addChildItems(WwiseTreeItem* parentItem, const std::vector<WwiseEventNode>& children, std::set<std::string>& addedPaths) {
    for (const auto& childNode : children) {
        // Skip adding node if we've already added it based on its path
        if (addedPaths.find(childNode.path) != addedPaths.end()) {
            continue; // Node already added, skip it
        }

        // Mark the path as added
        addedPaths.insert(childNode.path);

        // Create and add the child node
        auto* childItem = new WwiseTreeItem(childNode, nullptr, this);
        parentItem->addSubItem(childItem);

        // Debug output to trace adding nodes
        std::cout << "Adding child node: " << childNode.name << " to parent: " << parentItem->getUniqueName() << std::endl;

        // Add only children of the current node (no recursive duplication)
        if (!childNode.children.empty()) {
            addChildItems(childItem, childNode.children, addedPaths);  // This call will only add sub-items once per node
        }
    }
}

void MainComponent::triggerWwiseEvent(const std::string& ObjectID)
{
    std::cout << "Requesting WAAPI to trigger event: " << ObjectID << std::endl;
    waapiManager.postWwiseEvent(ObjectID);
}






// Shared Memory ==============================================================================


//==============================================================================
void MainComponent::populateAudioDeviceDropdowns()
{
    std::cout << "Populating audio device dropdowns..." << std::endl;

    // Get available device types
    auto* deviceManager = vstPluginComponent.getDeviceManager();
    const juce::OwnedArray<juce::AudioIODeviceType>& availableDevices = deviceManager->getAvailableDeviceTypes();

    // Clear dropdowns
    inputDeviceDropdown.clear();
    outputDeviceDropdown.clear();

    int inputIndex = 1, outputIndex = 1;

    // Temporary containers for unique device names
    juce::StringArray uniqueInputDevices;
    juce::StringArray uniqueOutputDevices;

    for (auto* type : availableDevices)
    {
        type->scanForDevices(); // Scan for devices

        // Input Devices
        auto inputDevices = type->getDeviceNames(true);
        for (auto& name : inputDevices)
        {
            if (!uniqueInputDevices.contains(name))  // Only add unique names
            {
                uniqueInputDevices.add(name);
                inputDeviceDropdown.addItem(name, inputIndex++);
            }
        }

        // Output Devices
        auto outputDevices = type->getDeviceNames(false);
        for (auto& name : outputDevices)
        {
            if (!uniqueOutputDevices.contains(name))  // Only add unique names
            {
                uniqueOutputDevices.add(name);
                outputDeviceDropdown.addItem(name, outputIndex++);
            }
        }
    }

    // Set default selections based on the current device
    auto* currentDevice = deviceManager->getCurrentAudioDevice();
    if (currentDevice != nullptr)
    {
        inputDeviceDropdown.setText(currentDevice->getName(), juce::dontSendNotification);
        outputDeviceDropdown.setText(currentDevice->getName(), juce::dontSendNotification);
    }

    // Force dropdown UI update
    inputDeviceDropdown.repaint();
    outputDeviceDropdown.repaint();
}


void MainComponent::changeAudioDevice(bool isInput)
{
    auto selectedInput = inputDeviceDropdown.getText();
    auto selectedOutput = outputDeviceDropdown.getText();

    // Access device manager
    auto* deviceManager = vstPluginComponent.getDeviceManager();

    // Setup audio configuration
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager->getAudioDeviceSetup(setup);

    if (isInput)
    {
        setup.inputDeviceName = selectedInput;
    }
    else
    {
        setup.outputDeviceName = selectedOutput;
    }

    // Apply changes
    juce::String error = deviceManager->setAudioDeviceSetup(setup, true);

    if (!error.isEmpty())
    {
        std::cerr << "Failed to switch audio device: " << error << std::endl;
    }
    else
    {
        std::cout << "Switched to Device: " << (isInput ? selectedInput : selectedOutput).toStdString() << std::endl;
    }

    // Reinitialize audio
   // vstPluginComponent.initialiseAudio();
    vstPluginComponent.refereshAudioIODetails();
}


//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}


//void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
//    if (readerSource.get() != nullptr) {
//        // 1. Get audio from transportSource (existing files)
//        transportSource.getNextAudioBlock(bufferToFill);
//
//
//        if (vstPluginComponent.pluginInstance != nullptr) {
//            try {
//                juce::MidiBuffer midiBuffer;
//                auto busLayout = vstPluginComponent.pluginInstance->getBusesLayout();
//                auto selectedFileName = audioFileNames[selectedFileIndex].toStdString();
//                auto isBypassed = bypassStates.find(selectedFileName) != bypassStates.end() && bypassStates[selectedFileName];
//
//                if (!isBypassed) {
//
//                    juce::ScopedLock lock(audioLock);
//
//                    if (!sharedMemoryReader.readAvailable()) {
//                        bufferToFill.clearActiveBufferRegion();
//                        std::cout << "[VST Host] No audio available." << std::endl;
//                        return;
//                    }
//
//                    sharedMemoryReader.readAudio(*bufferToFill.buffer);
//                    std::cout<<"[VST Host] Reading audio from shared memory." << std::endl;
//                    std::cout<<" - Channels: " <<juce::String(bufferToFill.buffer->getNumChannels())  << std::endl;
//                    std::cout<<" - Samples: " <<juce::String(bufferToFill.buffer->getNumSamples())  << std::endl;
//                   
//                    vstPluginComponent.pluginInstance->processBlock(*bufferToFill.buffer, midiBuffer);
//
//                    if (!pluginLoaded) {
//                        pluginLoaded = true;
//                        std::cout << "Processing. [Audio Stream is Active...]" << std::endl;
//                    }
//
//                    sharedMemoryReader.writeProcessedAudio(*bufferToFill.buffer);
//                    sharedMemoryReader.markProcessed();
//
//
//                }
//            }
//            catch (const std::exception& e) {
//                std::cerr << "Error processing audio block: " << e.what() << std::endl;
//            }
//        }
//    }
//    else {
//        bufferToFill.clearActiveBufferRegion();
//    }
//}


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {


    transportSource.getNextAudioBlock(bufferToFill);


    if (vstPluginComponent.pluginInstance != nullptr) {
        try {
            juce::MidiBuffer midiBuffer;
            auto busLayout = vstPluginComponent.pluginInstance->getBusesLayout();
            auto selectedFileName = audioFileNames[selectedFileIndex].toStdString();
            auto isBypassed = bypassStates.find(selectedFileName) != bypassStates.end() && bypassStates[selectedFileName];

            if (!isBypassed) {

                juce::ScopedLock lock(audioLock);


                if (!sharedMemoryReader.readAvailable()) {
                    bufferToFill.clearActiveBufferRegion();
                    //  std::cout << "[VST Host] No audio available." << std::endl;
                    return;
                }

                int numSamplesFromMemory = sharedMemoryReader.getNumSamples(); // Get Wwise’s buffer size
                std::cout << "[VST Host] Shared memory buffer size from wwise: " << numSamplesFromMemory << " samples." << std::endl;
                if (numSamplesFromMemory != bufferToFill.buffer->getNumSamples()) {
                    std::cerr << "[VST Host] WARNING: Mismatch in buffer sizes. Resizing to " << numSamplesFromMemory << " samples." << std::endl;
                    bufferToFill.buffer->setSize(bufferToFill.buffer->getNumChannels(), numSamplesFromMemory, false, false, true);
                }

                sharedMemoryReader.readAudio(*bufferToFill.buffer);
                auto startTime = juce::Time::getMillisecondCounterHiRes();

                std::cout << "[VST Host] Reading audio from shared memory." << std::endl;
                std::cout << " - Channels: " << juce::String(bufferToFill.buffer->getNumChannels()) << std::endl;
                std::cout << " - Samples: " << juce::String(bufferToFill.buffer->getNumSamples()) << std::endl;

                vstPluginComponent.pluginInstance->processBlock(*bufferToFill.buffer, midiBuffer);
                auto endTime = juce::Time::getMillisecondCounterHiRes();
                double latencyMs = endTime - startTime;
                std::cout << "[VST Host] Processing latency: " << latencyMs << " ms" << std::endl;

                if (!pluginLoaded) {
                    pluginLoaded = true;
                    std::cout << "Processing. [Audio Stream is Active...]" << std::endl;
                }

                sharedMemoryReader.writeProcessedAudio(*bufferToFill.buffer);
                sharedMemoryReader.markProcessed();


            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error processing audio block: " << e.what() << std::endl;
        }
    }

}




void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

//==============================================================================

void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("VST Host GUI App", juce::Component::getLocalBounds(), juce::Justification::centredTop);


}



void MainComponent::resized()
{
    // Main FlexBox for the entire layout
    juce::FlexBox mainFlexBox;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::row; // Left-to-right layout

    // Left column: Audio file tree
    juce::FlexBox leftColumn;
    leftColumn.items.add(juce::FlexItem(audioFileTree).withMinWidth(300).withFlex(0));
    leftColumn.items.add(juce::FlexItem(wwiseTree).withMinWidth(300).withFlex(0));
    // wwiseTree.setBounds(10, 50, 300, getHeight() - 60);
    mainFlexBox.items.add(juce::FlexItem(leftColumn).withMinWidth(300).withFlex(0));


    // Middle column: Buttons and plugin UI
    juce::FlexBox middleColumn;
    middleColumn.flexDirection = juce::FlexBox::Direction::column; // Top-to-bottom layout
    // Wwise Status Label (added at the top)
    middleColumn.items.add(juce::FlexItem(wwiseStatusLabel).withFlex(0).withHeight(30).withMargin(juce::FlexItem::Margin(15, 0, 5, 0)));

    // Button row (Top section of the middle column)
    juce::FlexBox buttonRow;
    buttonRow.flexDirection = juce::FlexBox::Direction::row; // Horizontal button layout
    buttonRow.flexWrap = juce::FlexBox::Wrap::wrap;        // No wrapping

    // Add buttons to the buttonRow with fixed sizes
    buttonRow.items.add(juce::FlexItem(scanPluginButton).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(loadAudioButton).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(pluginListDropdown).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(exportAudioButton).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(refreshPluginDetailsButton).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(unloadPluginButton).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(ShowEditorButton).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(inputDeviceDropdown).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(outputDeviceDropdown).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(refreshIODevicesListButton).withFlex(1).withWidth(120).withHeight(40));
    buttonRow.items.add(juce::FlexItem(refreshFileTreeButton).withFlex(1).withWidth(120).withHeight(40));

    // Add the buttonRow to the middleColumn
    middleColumn.items.add(juce::FlexItem(buttonRow).withFlex(0)); // Fixed height for buttons

    // Plugin Editor (takes up the remaining space)
    if (pluginEditor != nullptr)
    {
        middleColumn.items.add(juce::FlexItem(*pluginEditor).withFlex(1)); // Plugin UI expands to fill
    }

    // Add the middleColumn to the mainFlexBox
    mainFlexBox.items.add(juce::FlexItem(middleColumn).withFlex(1)); // Middle column takes up remaining space

    // Perform the layout
    mainFlexBox.performLayout(juce::Component::getLocalBounds());
}




void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &scanPluginButton)
    {
        // Scan plugins and populate dropdown
        DBG(" Scanning for plugins");
        DBG(" ===============================");
        vstPluginComponent.scanPlugins("C:\\Program Files\\Common Files\\VST3");
        pluginListDropdown.clear();
        for (int i = 0; i < vstPluginComponent.pluginList.getNumTypes(); ++i)
        {
            const auto* description = vstPluginComponent.pluginList.getType(i);
            pluginListDropdown.addItem(description->name, i + 1); // ComboBox uses 1-based indexing
        }

    }

    else if (button == &loadAudioButton)
    {
        std::cout << "Loading audio" << std::endl;
        std::cout << "====================" << std::endl;
        // Print the current last saved path every time button is clicked
        std::cout << "Current path: " << audioFileManager.getLastAudioLoadPath() << std::endl;

        juce::String lastPath = audioFileManager.getLastAudioLoadPath();
        juce::File initialFolder = lastPath.isNotEmpty() ? juce::File(lastPath) : juce::File();
        fileChooser = std::make_unique<juce::FileChooser>("Select an audio file", initialFolder, "*.wav");
        // Launch the file chooser asynchronously
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& chooser)
            {
                auto selectedFiles = chooser.getResults(); // Get all selected files
                //audioFileNames.clear();
                // audioBuffers.clear();
                if (!selectedFiles.isEmpty())
                {
                    std::cout << "Selected files:" << std::endl;

                    for (const auto& file : selectedFiles)
                    {
                        audioFileManager.saveLastAudioLoadPath(file.getParentDirectory().getFullPathName());
                        std::cout << "Output directory saved to: " << audioFileManager.getLastAudioLoadPath().toStdString() << std::endl;
                        // const auto& file = selectedFiles[i];

                        if (!audioFileNames.contains(file.getFileName()))
                        {
                            // Display the name of the file
                            std::cout << "[" << "] " << file.getFileName() << std::endl;
                            loadedAudioFileNames = file.getFileName().toStdString();
                            // Load the selected file using AudioFileManager
                            audioFileManager.loadAudioFile(file);
                            audioFileNames.add(file.getFileName().toStdString());
                            juce::AudioBuffer<float> fileBuffer;
                            fileBuffer.makeCopyOf(audioFileManager.getAudioBuffer());
                            // Store the buffer for this file in the audioBuffers Array
                            audioBuffers.add(fileBuffer);
                            getAudioBuffersList();
                            auto* rootItem = audioFileTree.getRootItem();
                            if (rootItem != nullptr)
                            {
                                auto audioBufferIndex = audioBuffers.size() - 1;
                                // auto& fileName = audioFileNames[i].toStdString();
                                auto fileName = file.getFileName().toStdString();
                                auto playCallback = [this, fileName]()
                                    {
                                        int index = audioFileNames.indexOf(juce::String(fileName));
                                        selectedFileIndex = index;
                                        std::cout << "Selected file index in playback: " << selectedFileIndex << std::endl;
                                        if (index >= 0 && index < audioBuffers.size())
                                        {
                                            auto& buffer = audioBuffers[index];

                                            if (buffer.getNumChannels() == 1) // Mono file
                                            {
                                                // Create a stereo buffer for playback
                                                juce::AudioBuffer<float> stereoBuffer(2, buffer.getNumSamples());
                                                stereoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples()); // Left
                                                stereoBuffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples()); // Right

                                                // Play the stereo buffer
                                                transportSource.setSource(new juce::MemoryAudioSource(stereoBuffer, true));
                                            }
                                            else // Already stereo or multichannel
                                            {
                                                transportSource.setSource(new juce::MemoryAudioSource(buffer, true));
                                            }

                                            // Start playback
                                            transportSource.start();
                                        }
                                        else
                                        {
                                            std::cout << "Invalid buffer index for playback: " << fileName << std::endl;
                                        }
                                    };
                                auto removeCallback = [this, rootItem, fileName](AudioFileTreeItem* item)
                                    {
                                        // Find the dynamic index of the buffer based on filename
                                        int index = audioFileNames.indexOf(juce::String(fileName));
                                        if (index >= 0) // Ensure the file exists
                                        {
                                            std::cout << "Removing item at index: " << index << std::endl;
                                            audioBuffers.remove(index);
                                            audioFileNames.remove(index);
                                            getAudioBuffersList(); // Refresh the list
                                        }
                                        else
                                        {
                                            std::cout << "File not found in list: " << fileName << std::endl;
                                        }


                                        if (item != nullptr && rootItem != nullptr)
                                        {
                                            for (int i = 0; i < rootItem->getNumSubItems(); ++i)
                                            {
                                                if (rootItem->getSubItem(i) == item)
                                                {
                                                    rootItem->removeSubItem(i, true);
                                                    break;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            std::cout << "Item or rootItem is null" << std::endl;
                                        }

                                    };
                                auto item = std::make_unique<AudioFileTreeItem>(fileName, playCallback, removeCallback, this);
                                rootItem->addSubItem(item.release());

                            }
                            else
                            {
                                std::cout << "Root item for TreeView not initialized!" << std::endl;
                            }

                        }
                        else
                        {
                            std::cout << "Skipping duplicate file: " << file.getFileName() << std::endl;
                            getAudioBuffersList();
                            continue;
                        }
                        auto* reader = formatManager.createReaderFor(file);
                        if (reader != nullptr)
                        {
                            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));

                        }
                    }

                    audioFileTree.getRootItem()->treeHasChanged();
                    audioFileTree.repaint();
                    std::cout << "All selected files loaded." << std::endl;
                }
                else
                {
                    for (int i = 0; i < selectedFiles.size(); ++i)
                    {
                        const auto& file = selectedFiles[i];
                        // Display the name of the file
                        std::cout << "No file selected or invalid file." << file.getFileName() << std::endl;
                        std::cout << "[" << i << "] " << file.getFileName() << std::endl;
                    }
                }
            });
    }


    else if (button == &playButton)
    {
        /* if (readerSource != nullptr && !transportSource.isPlaying())
         {
             transportSource.setPosition(0.0);
             transportSource.start();
         }*/
    }
    else if (button == &stopButton)
    {
        //transportSource.stop();
    }


    else if (button == &exportAudioButton)
    {
        juce::DialogWindow::LaunchOptions options;

        // Create and show the ExportAudioComponent
       // auto exportAudioComponent = std::make_unique<ExportAudioComponent>(*this);
        auto exportAudioComponent = std::make_unique<ExportAudioComponent>(
            audioBuffers, audioFileNames, vstPluginComponent, bypassStates, exporterComponent);
        exportAudioComponent->setSize(500, 500); // Width: 400, Height: 300
        options.content.setOwned(exportAudioComponent.release());
        options.dialogTitle = "Export Processed Audio";
        options.dialogBackgroundColour = juce::Colours::white;
        options.resizable = false;
        options.useNativeTitleBar = true;
        options.launchAsync();




    }
    else if (button == &refreshPluginDetailsButton)
    {
        int selectedIndex = pluginListDropdown.getSelectedId() - 1; // Adjust for 1-based indexing
        if (selectedIndex >= 0 && selectedIndex < vstPluginComponent.pluginList.getNumTypes())
        {
            //  vstPluginComponent.handleBusChange(vstPluginComponent.pluginInstance.get(), audioBuffer);
            vstPluginComponent.refreshPlugin(selectedIndex, vstPluginComponent.pluginInstance.get());
        }
    }

    else if (button == &unloadPluginButton)
    {
        if (vstPluginComponent.pluginInstance != nullptr)
        {
            closePluginEditor(); // close plugin editor ui
            vstPluginComponent.pluginInstance.reset();
            std::cout << "Plugin unloaded" << std::endl;
        }
    }
    else if (button == &ShowEditorButton)
    {
        if (vstPluginComponent.pluginInstance != nullptr)
        {
            if (vstPluginComponent.pluginInstance->hasEditor())
            {
                if (pluginEditor == nullptr)
                {
                    pluginEditor.reset(vstPluginComponent.pluginInstance->createEditorIfNeeded());
                    //juce::Component::addAndMakeVisible(pluginEditor.get());
                    //pluginEditor->setBounds(10, 250, 780, 300); // Adjust as needed

                    // Add the viewport
                    pluginEditorViewport = std::make_unique<juce::Viewport>();
                    juce::Component::addAndMakeVisible(pluginEditorViewport.get());

                    pluginEditorViewport->setViewedComponent(pluginEditor.get(), true); // Attach the editor to the viewport
                    pluginEditorViewport->setScrollBarsShown(true, true); // Enable scrollbars
                    pluginEditorViewport->setBounds(10, 250, 780, 300);  // Set initial viewport bounds
                }
                else
                {
                    pluginEditor->setVisible(!pluginEditor->isVisible());
                }
            }
            else
            {
                std::cout << "Plugin does not have an editor." << std::endl;
            }
        }
        else
        {
            std::cout << "No plugin loaded." << std::endl;
        }
    }

    else if (button == &refreshIODevicesListButton)
    {
        std::cout << "Refreshing audio device list..." << std::endl;
        populateAudioDeviceDropdowns();
    }

    else if (button == &refreshFileTreeButton) {

        // clear the audio file tree
        audioFileTree.getRootItem()->clearSubItems();
        updateWwiseTree();
        audioFileTree.getRootItem()->treeHasChanged();
        audioFileTree.repaint();

    }


}



void MainComponent::getAudioBuffersList() {

    std::cout << "---------------------" << std::endl;
    std::cout << "Audio Buffers Content:" << std::endl;

    for (int i = 0; i < audioBuffers.size(); ++i)
    {


        if (audioBuffers[i].getNumSamples() != 0) // Ensure the buffer is valid
        {
            std::cout << "Name: " << audioFileNames[i].toStdString() << std::endl;
            std::cout << "  Buffer " << i << ":" << std::endl;
            std::cout << "  Channels: " << audioBuffers[i].getNumChannels() << std::endl;
            std::cout << "  Samples: " << audioBuffers[i].getNumSamples() << std::endl;


        }
        else
        {
            std::cout << "  Null buffer at index " << i << std::endl;
        }
    }
}



const std::string MainComponent::getLoadedAudiFileNames() {
    return loadedAudioFileNames;
}



// VST Plugin Editor Window ==============================================================================

juce::AudioProcessorEditor* MainComponent::getPluginEditor()
{
    if (vstPluginComponent.pluginInstance != nullptr)
    {
        if (vstPluginComponent.pluginInstance->hasEditor())
        {
            return vstPluginComponent.pluginInstance->createEditorIfNeeded();
        }
    }
    return nullptr;
}

void MainComponent::closePluginEditor()
{
    if (pluginEditor != nullptr)
    {
        pluginEditor->setVisible(false);
        pluginEditor.reset();
    }
}