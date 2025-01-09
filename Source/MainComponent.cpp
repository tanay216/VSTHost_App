#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    formatManager.registerBasicFormats();

    juce::Component::addAndMakeVisible(viewport);
    viewport.setViewedComponent(&contentComponent, false);

    juce::Component::addAndMakeVisible(scanPluginButton);
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
    
    juce::Component::addAndMakeVisible(inputDeviceDropdown);
    inputDeviceDropdown.onChange = [this]() { changeAudioDevice(true); }; // Handle input selection

    
    juce::Component::addAndMakeVisible(outputDeviceDropdown);
    outputDeviceDropdown.onChange = [this]() { changeAudioDevice(false); }; // Handle output selection

    // Attach as a listener for audio device changes
   
    populateAudioDeviceDropdowns();

    audioFileTree.setColour(juce::TreeView::backgroundColourId, juce::Colours::antiquewhite);
    audioFileTree.setDefaultOpenness(true);
    std::string rootName = "Root";
    auto rootItem = std::make_unique<AudioFileTreeItem>(rootName, nullptr, nullptr, this);
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
}
//==============================================================================

//void MainComponent::populateAudioDeviceDropdowns()
//{
//    std::cout << "Populating audio device dropdowns..." << std::endl;
//    // Get available device types
//    auto* deviceManager = vstPluginComponent.getDeviceManager();
//    const juce::OwnedArray<juce::AudioIODeviceType>& availableDevices = deviceManager->getAvailableDeviceTypes();
//
//    // Clear dropdowns
//    inputDeviceDropdown.clear();
//    outputDeviceDropdown.clear();
//
//    int inputIndex = 1, outputIndex = 1;
//
//    for (auto* type : availableDevices)
//    {
//        type->scanForDevices(); // Scan for devices
//
//        // Input Devices
//        auto inputDevices = type->getDeviceNames(true);
//        for (auto& name : inputDevices)
//        {
//            inputDeviceDropdown.addItem(name, inputIndex++);
//        }
//
//        // Output Devices
//        auto outputDevices = type->getDeviceNames(false);
//        for (auto& name : outputDevices)
//        {
//            outputDeviceDropdown.addItem(name, outputIndex++);
//        }
//    }
//
//    // Set default selections based on the current device
//    auto* currentDevice = deviceManager->getCurrentAudioDevice();
//    if (currentDevice != nullptr)
//    {
//        inputDeviceDropdown.setText(currentDevice->getName(), juce::dontSendNotification);
//        outputDeviceDropdown.setText(currentDevice->getName(), juce::dontSendNotification);
//    }
//    
//    // Force dropdown UI update
//    inputDeviceDropdown.repaint();
//    outputDeviceDropdown.repaint();
//}

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

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() != nullptr)
    {
        transportSource.getNextAudioBlock(bufferToFill);
        if (vstPluginComponent.pluginInstance != nullptr)
        {
            try {
                juce::MidiBuffer midiBuffer;

                //std::cout << "Buffer Channels: " << bufferToFill.buffer->getNumChannels() << std::endl;

                auto busLayout = vstPluginComponent.pluginInstance->getBusesLayout();
                auto selectedFileName = audioFileNames[selectedFileIndex].toStdString();
                auto isBypassed = bypassStates.find(selectedFileName) != bypassStates.end() && bypassStates[selectedFileName];

              // std::cout << "Bypass state for " << selectedFileName << ": " << isBypassed << std::endl; // Debugging
                if (isBypassed)
                {
                    // If bypassed, skip processing and just output the raw audio data
                    std::cout << "Bypassed: Playing raw audio for " << selectedFileName << std::endl;
                   // bufferToFill.clearActiveBufferRegion();
                }
                else
                {
                    vstPluginComponent.pluginInstance->processBlock(*bufferToFill.buffer, midiBuffer);

                    if (!pluginLoaded)
                    {
                        pluginLoaded = true;
                        std::cout << "Processing. [Audio Stream is Active...]" << std::endl;
                    }

                }
                
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error processing audio block: " << e.what() << std::endl;
            }

        }

        else
        {
            if (pluginLoaded)
            {
                pluginLoaded = false;
                std::cout << "Plugin not loaded. [Audio Stream is Active...]" << std::endl;

            }
        }
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
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



/* Original code*/

//void MainComponent::resized()
//{
//    // This is called when the MainContentComponent is resized.
//
//    int buttonHeight = 30; // Custom height for buttons
//    int buttonWidth = 200; // Custom width for buttons
//    auto area = juce::Component::getLocalBounds().reduced(10);
//    auto treeViewArea = area.removeFromRight(300).withHeight(300).withWidth(300);
//
//    audioFileTree.setBounds(treeViewArea);
//    channelConfigDropdown.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    scanPluginButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    loadAudioButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    pluginListDropdown.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    playButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    stopButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    exportAudioButton.setBounds(area.removeFromTop(60).withWidth(buttonWidth).withX(10));
//    refreshPluginDetailsButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(area.getWidth() - 210));
//    ShowEditorButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    unloadPluginButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(area.getWidth() - 210));
//    inputDeviceDropdown.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    outputDeviceDropdown.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//    refreshIODevicesListButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
//
//    // audioFileListBox.setBounds(area.removeFromTop(200).withWidth(200).withX(area.getWidth() - 210)); // Position the list to the top right
//
//
//    if (pluginEditor != nullptr && pluginEditor->isVisible())
//    {
//        // Adjust height and position as needed
//        pluginEditor->setBounds(10, area.getY() + 30, area.getWidth(), 200);
//
//        pluginEditor->setBounds(0, 0, pluginEditor->getWidth(), pluginEditor->getHeight()); // Adjust bounds
//        pluginEditorViewport->setViewedComponent(pluginEditor.get(), true); // Refresh viewport
//    }
//
//
//}

/* new code*/


void MainComponent::resized()
{
    // Main FlexBox for the entire layout
    juce::FlexBox mainFlexBox;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::row; // Left-to-right layout

    // Left column: Audio file tree
    juce::FlexBox leftColumn;
    leftColumn.items.add(juce::FlexItem(audioFileTree).withMinWidth(200).withFlex(0));
    mainFlexBox.items.add(juce::FlexItem(leftColumn).withMinWidth(200).withFlex(0));

    // Middle column: Buttons and plugin UI
    juce::FlexBox middleColumn;
    middleColumn.flexDirection = juce::FlexBox::Direction::column; // Top-to-bottom layout

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
        fileChooser = std::make_unique<juce::FileChooser>("Select an audio file", juce::File(), "*.wav");
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
                        // const auto& file = selectedFiles[i];
                        
                        if (!audioFileNames.contains(file.getFileName()))
                        {
                            // Display the name of the file
                            std::cout << "["  << "] " << file.getFileName() << std::endl;
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
                                auto audioBufferIndex = audioBuffers.size()-1;
                                // auto& fileName = audioFileNames[i].toStdString();
                                auto fileName =file.getFileName().toStdString();
                                auto playCallback = [this, fileName]()
                                {
                                    int index = audioFileNames.indexOf(juce::String(fileName));
                                    selectedFileIndex = index;
                                    std::cout<< "Selected file index in playback: " << selectedFileIndex << std::endl;
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
    //else if (button == &exportAudioButton)
    //{
    //    if (readerSource != nullptr)
    //    {
    //        if (audioFileNames.size() != audioBuffers.size())
    //        {
    //            std::cout << "Audio buffers do not match the number of loaded files." << std::endl;
    //            return;
    //        }

    //        // Iterate through all the loaded audio files
    //        for (int i = 0; i < audioFileNames.size(); ++i)
    //        {

    //            // std::cout << "Main Audio buffer size: " << audioBuffer.getNumSamples() << std::endl;
    //            // std::cout << "Main Audio buffer Channels: " << audioBuffer.getNumChannels() << std::endl               

    //            std::cout << "Exporting audio file..." << std::endl;
    //            std::string currentAudioFileName = audioFileNames[i].toStdString();
    //            juce::AudioBuffer<float>& currentAudioBuffer = audioBuffers[i];

    //            auto isBypassed = bypassStates.find(currentAudioFileName) != bypassStates.end() && bypassStates[currentAudioFileName];

    //            if (isBypassed)
    //            {
    //                // Skip processing and log bypassed files
    //                std::cout << "Skipping export for bypassed file: " << currentAudioFileName << std::endl;
    //                continue; // Skip this file and move to the next one
    //            }



    //            if (currentAudioBuffer.getNumSamples() == 0)
    //            {
    //                std::cout << "Audio Buffer is empty for: " << currentAudioFileName << "." << std::endl;
    //                return;
    //            }

    //            std::cout << "Exporting: " << currentAudioFileName << std::endl;

    //            // Process each audio file
    //            vstPluginComponent.processAudioWithPlugin(currentAudioBuffer, currentAudioFileName);
    //        }

    //    }
    //    else
    //    {
    //        std::cout << "readerSource is empty." << std::endl;
    //    }
    //}

    else if (button == &exportAudioButton)
    {
        juce::DialogWindow::LaunchOptions options;

        // Create and show the ExportAudioComponent
        auto exportAudioComponent = std::make_unique<ExportAudioComponent>();
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

