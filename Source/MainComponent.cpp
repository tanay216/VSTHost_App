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
    

    
    // Initialise for System Audio Driver
    pluginHost.initialiseAudio();
    

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 8 : 0, 8); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(8, 8);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    //shutdownAudio();
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
            try{
            juce::MidiBuffer midiBuffer;

            //std::cout << "Buffer Channels: " << bufferToFill.buffer->getNumChannels() << std::endl;
            
            auto busLayout = vstPluginComponent.pluginInstance->getBusesLayout();
            
           /* std::cout << "====================" << std::endl;
            std::cout << "Input Channels: " << busLayout.getNumChannels(true, 0) << std::endl;
            std::cout << "Output Channels: " << busLayout.getNumChannels(true, 0) << std::endl;
            std::cout << "====================" << std::endl;*/
            vstPluginComponent.pluginInstance->processBlock(*bufferToFill.buffer, midiBuffer);
            
            if (!pluginLoaded)
            {
                pluginLoaded = true;
                std::cout << "Processing. [Audio Stream is Active...]" << std::endl;
            }
            }
            catch(const std::exception& e)
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

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.

    int buttonHeight = 30; // Custom height for buttons
    int buttonWidth = 200; // Custom width for buttons
    auto area = juce::Component::getLocalBounds().reduced(10);

    scanPluginButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
    loadAudioButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
    pluginListDropdown.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
    playButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
    stopButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
    exportAudioButton.setBounds(area.removeFromTop(60).withWidth(buttonWidth).withX(10));
    refreshPluginDetailsButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
    ShowEditorButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(10));
    unloadPluginButton.setBounds(area.removeFromTop(buttonHeight).withWidth(buttonWidth).withX(area.getWidth() - 210));
   // audioFileListBox.setBounds(area.removeFromTop(200).withWidth(200).withX(area.getWidth() - 210)); // Position the list to the top right
    
    if (pluginEditor != nullptr && pluginEditor->isVisible()) 
    {
        // Adjust height and position as needed
        pluginEditor->setBounds(10, area.getY() + 30, area.getWidth(), 200); 
        
        pluginEditor->setBounds(0, 0, pluginEditor->getWidth(), pluginEditor->getHeight()); // Adjust bounds
        pluginEditorViewport->setViewedComponent(pluginEditor.get(), true); // Refresh viewport
    }

    
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
                audioFileNames.clear();

                if (!selectedFiles.isEmpty())
                {
                    std::cout << "Selected files:" << std::endl;

                    // Iterate through selected files
                    for (int i = 0; i < selectedFiles.size(); ++i)
                    {
                        const auto& file = selectedFiles[i];

                        // Display the name of the file
                        std::cout << "[" << i << "] " << file.getFileName() << std::endl;
                        loadedAudioFileNames = file.getFileName().toStdString();

                        // Load the selected file using AudioFileManager
                        audioFileManager.loadAudioFile(file);
                        audioFileNames.add(file.getFullPathName());


                        auto* reader = formatManager.createReaderFor(file);
                        if (reader != nullptr)
                        {
                            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
                            transportSource.setSource(readerSource.get(),
                                0,                // buffer size (default)
                                nullptr,          // no time-stretching
                                reader->sampleRate);
                        }
                    }
                   // audioFileListBox.updateContent();

                    

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
        if (readerSource != nullptr && !transportSource.isPlaying())
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
    else if (button == &stopButton)
    {
        transportSource.stop();
    }
    else if (button == &exportAudioButton)
    {
		if (readerSource != nullptr)
		{
			
            audioBuffer.makeCopyOf( audioFileManager.getAudioBuffer());
            std::cout<< "Main Audio buffer size: " << audioBuffer.getNumSamples() << std::endl;
            std::cout<< "Main Audio buffer Channels: " << audioBuffer.getNumChannels() << std::endl;

            if (audioBuffer.getNumSamples() == 0)
            {
                std::cout << "No audio files loaded for processing." << std::endl;
                return;
            }

            std::cout << "Exporting audio file..." << std::endl;

            vstPluginComponent.processAudioWithPlugin(audioBuffer, loadedAudioFileNames);
			
		}
    }
    else if (button == &refreshPluginDetailsButton)
    {
        int selectedIndex = pluginListDropdown.getSelectedId() - 1; // Adjust for 1-based indexing
        if (selectedIndex >= 0 && selectedIndex < vstPluginComponent.pluginList.getNumTypes())
        {
            vstPluginComponent.handleBusChange(vstPluginComponent.pluginInstance.get(), audioBuffer);
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
            std::cout<< "No plugin loaded." << std::endl;
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

// Audio Files Listbox ==============================================================================

//int MainComponent::getNumRows() {
//    return audioFileNames.size(); // Return the number of loaded audio files
//}
//void MainComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
//    if (rowIsSelected) {
//        g.fillAll(juce::Colours::lightblue); // Highlight selected row
//    }
//    else {
//        g.fillAll(juce::Colours::white); // Normal row background
//    }
//}
//void MainComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
//    g.setColour(juce::Colours::black);
//    g.drawText(audioFileNames[rowNumber], 2, 0, width - 4, height, juce::Justification::centredLeft); // Draw the filename
//}
//
//void MainComponent::listBoxItemClicked(int row, const juce::MouseEvent& e) {
//    if (row >= 0 && row < audioFileNames.size()) {
//        const juce::String& fileName = audioFileNames[row];
//        juce::File audioFile(fileName); // Load the file
//
//        // Now load and play the selected audio file
//        auto* reader = formatManager.createReaderFor(audioFile);
//        if (reader != nullptr) {
//            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
//            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
//            transportSource.setPosition(0.0); // Start from the beginning
//            transportSource.start();
//        }
//    }
//}
