#pragma once
#include <iostream>
#include <JuceHeader.h>
#include "VSTPluginHost.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "AudioFileManager.h"
#include "VSTPluginComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Component,
    public juce::ListBox



{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
    //==============================================================================


    void populateAudioDeviceDropdowns();
    void changeAudioDevice(bool isInput);

   



    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================


    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

    juce::AudioProcessorEditor* getPluginEditor();

    // void MainComponent::handleChannelConfigurationChange(int configId);

    void closePluginEditor();


    const std::string getLoadedAudiFileNames();

    void setBypassState(const std::string& fileName, bool state)
    {
        bypassStates[fileName] = state;
        std::cout << "Bypass state updated for " << fileName << ": " << state << std::endl;
    }

    /* Getter Functions */

    void getAudioBuffersList();

   

    

private:
    //==============================================================================

    /* Variables */
    bool pluginLoaded = false;
    double sampleRate = 44100.0;

    /* Initialise */
    juce::AudioDeviceManager audioDeviceManager;
    VSTPluginHost pluginHost;
    AudioFileManager audioFileManager;
    VSTPluginComponent vstPluginComponent;
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    /* Audio & Files */
    std::unordered_map<std::string, bool> bypassStates; // Tracks bypass state for each file
    juce::AudioBuffer<float> audioBuffer;
    std::string loadedAudioFileNames;
    juce::StringArray audioFileNames; // Array to store loaded audio files names
    std::unique_ptr<juce::AudioProcessorEditor> pluginEditor; // Plugin editor
    juce::Array<juce::AudioBuffer<float>> audioBuffers;
    std::vector<std::unique_ptr<juce::AudioFormatReaderSource>> readerSources;
    int selectedFileIndex = -1;


    juce::AudioTransportSource transportSource;
    juce::HashMap<std::string, std::unique_ptr<juce::AudioTransportSource>> audioTransportSources;
    juce::AudioSourcePlayer audioSourcePlayer;
    //juce::AudioFormatReaderSource* readerSource = nullptr;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioProcessorPlayer pluginPlayer;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::Viewport> pluginEditorViewport;


    /* Buttons and UI elements*/

    juce::Viewport viewport;                     
    juce::Component contentComponent;            // Holds all buttons and UI elements
    juce::TextButton scanPluginButton{ "Scan Plugins" };
    juce::TextButton loadAudioButton{ "Load Audio File" };
    // juce::ListBox audioFileListBox{ "Audio File List" }; // ListBox for displaying audio files
    juce::TreeView audioFileTree{ "Audio File Tree" };
    juce::TextButton deleteButton{ "X" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton exportAudioButton{ "Export Processed Audio" };
    juce::TextButton refreshPluginDetailsButton{ "Refresh Plugin Details" };
    juce::TextButton unloadPluginButton{ "X Remove Plugin" };
    juce::TextButton ShowEditorButton{ "Show Plugin UI" };
    juce::TextButton refreshIODevicesListButton{ "Refresh IO Devices list" };



    juce::ComboBox pluginListDropdown;
    juce::ComboBox channelConfigDropdown;
    juce::ComboBox inputDeviceDropdown;  // Dropdown for input devices
    juce::ComboBox outputDeviceDropdown; // Dropdown for output devices

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};


class AudioFileTreeItem : public juce::TreeViewItem
{
public:
    explicit AudioFileTreeItem(const std::string& fileName,
        std::function<void()> onPlayCallback,
        std::function<void(AudioFileTreeItem*)> onRemoveCallback,
        MainComponent* mainComponent)
        : fileName(fileName),
        onPlayCallback(std::move(onPlayCallback)),
        onRemoveCallback(std::move(onRemoveCallback)),
        mainComponent(mainComponent)
    {
    }

    bool mightContainSubItems() override { return false; }

    void paintItem(juce::Graphics& g, int width, int height) override
    {
        g.fillAll(juce::Colours::lightgrey);
        g.setColour(juce::Colours::black);
        g.drawText(fileName, 2, 0, width - 30, height, juce::Justification::centredLeft);
    }

    // Create a custom component for the item
    std::unique_ptr<juce::Component> createItemComponent() override
    {
        // Return a custom component for the tree view item
        return std::make_unique<ItemComponent>(
            fileName,
            onPlayCallback,
            [this]() { // Wrap lambda inside another lambda
                if (onRemoveCallback)
                    onRemoveCallback(this); // Pass 'this' correctly
            },
            mainComponent);
    }

private:
    std::string fileName;
    std::function<void()> onPlayCallback;
    std::function<void(AudioFileTreeItem*)> onRemoveCallback;
    MainComponent* mainComponent; // Corrected the type to match the lambda callback

    // Inner component for displaying the item
    class ItemComponent : public juce::Component,
        public juce::Button::Listener
    {
    public:
        ItemComponent(const std::string& name,
            std::function<void()> onPlay,
            std::function<void()> onRemove,
            MainComponent* mainComponent)
            : fileName(name), onPlayCallback(std::move(onPlay)),
            onRemoveCallback(std::move(onRemove)),
			mainComponent(mainComponent)
        {
            addNewButtons(removeButton, "X");
          //  addNewButtons(bypassButton, "Bypass");
            addNewToggle(bypassToggleButton, "Bypass");
           
        }

        void addNewButtons(juce::TextButton& button, const juce::String& ButtonText)
        {
            button.setButtonText(ButtonText);
            button.addListener(this);
            addAndMakeVisible(button);
        }
        void addNewToggle(juce::ToggleButton& button, const juce::String& ButtonText)
        {
            button.setButtonText(ButtonText);
            button.addListener(this);
            addAndMakeVisible(button);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            fileLabel.setBounds(bounds.removeFromLeft(bounds.getWidth() - 130)); // File label
          //  bypassButton.setBounds(bounds.removeFromRight(50));
            bypassToggleButton.setBounds(bounds.removeFromRight(30));
            removeButton.setBounds(bounds.removeFromRight(30));
            
           
        }

        void buttonClicked(juce::Button* button) override
        {
            if (button == &removeButton)
            {
                if (onRemoveCallback)
                    onRemoveCallback(); // Trigger remove callback
            }
            else if (button == &bypassButton)
            {
                std::cout << "Bypass button clicked" << std::endl;
            }

            else if (button == &bypassToggleButton)
            {
               // std::cout << "Bypass Toggle clicked" << std::endl;

                bool isBypassed = bypassToggleButton.getToggleState();
              //  std::cout << "Bypass Toggle clicked for " << fileName << " : " << isBypassed << std::endl;

                
                if (mainComponent)
                {
                    mainComponent->setBypassState(fileName, isBypassed);
                }

            }
            

           
        }

        void mouseDown(const juce::MouseEvent& e) override
        {
            // Play the audio file when clicked
            std::cout << "Playing audio file: " << fileName << std::endl;
            if (onPlayCallback)
            {
                onPlayCallback(); // Trigger playback callback
            }
        }

    private:
        MainComponent* mainComponent;
        juce::Label fileLabel;
        std::string fileName;
        juce::TextButton removeButton;
        juce::TextButton bypassButton;
        juce::ToggleButton bypassToggleButton;
       

        std::function<void()> onPlayCallback;
        std::function<void()> onRemoveCallback;
    };
};












