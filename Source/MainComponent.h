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

    void getAudioBuffersList();


    const std::string getLoadedAudiFileNames();


    /*int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) ;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;*/

private:
    //==============================================================================
    bool pluginLoaded = false;
    double sampleRate = 44100.0;

    juce::AudioDeviceManager audioDeviceManager;
    VSTPluginHost pluginHost;
    AudioFileManager audioFileManager;
    VSTPluginComponent vstPluginComponent;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Array<juce::File> loadedFiles; // Array to store loaded audio files

    juce::AudioBuffer<float> audioBuffer;
    std::string loadedAudioFileNames;
    juce::StringArray audioFileNames; // Array to store loaded audio files
    std::unique_ptr<juce::AudioProcessorEditor> pluginEditor; // Plugin editor
    juce::Array<juce::AudioBuffer<float>> audioBuffers;
    std::vector<std::unique_ptr<juce::AudioFormatReaderSource>> readerSources;
    std::set<juce::String> loadedFilePaths;
    std::unordered_set<std::string> loadedTreeItems;


    juce::AudioTransportSource transportSource;
    juce::HashMap<std::string, std::unique_ptr<juce::AudioTransportSource>> audioTransportSources;
    juce::AudioSourcePlayer audioSourcePlayer;
    //juce::AudioFormatReaderSource* readerSource = nullptr;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioProcessorPlayer pluginPlayer;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::Viewport> pluginEditorViewport;

    juce::Viewport viewport;                     // Viewport for scrolling
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


    juce::ComboBox pluginListDropdown;
    juce::ComboBox channelConfigDropdown;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

//class AudioFileTreeItem : public juce::TreeViewItem,
//						  public juce::Component,
//                            public juce::Button::Listener
//{
//public:
//    explicit AudioFileTreeItem(std::string& fileName, std::function<void()> onPlayCallback) : fileName(fileName), onPlayCallback(std::move(onPlayCallback)) {
//        
//       
//       
//    }
//    bool mightContainSubItems() override { return false; }
//    void paintItem(juce::Graphics& g, int width, int height) override
//    {
//        g.fillAll(juce::Colours::lightgrey);
//        g.setColour(juce::Colours::black);
//        g.drawText(fileName, 2, 0, width - 4, height, juce::Justification::centredLeft);
//
//    }
//    void resized() override
//    {
//      
//    }
//
//    void itemClicked(const juce::MouseEvent& e) override
//    {
//        // Play the audio file when clicked
//        //std::cout << "Item Clicked " << std::endl;
//
//        std::cout << "Playing audio file: " << fileName << std::endl;
//        if (onPlayCallback) {
//            onPlayCallback(); // Trigger playback callback
//        }
//       
//
//    }
//
//
//private:
//    std::string fileName;
//    std::function<void()> onPlayCallback;
//
//}; // VEry old

//class AudioFileTreeItem : public juce::TreeViewItem
//{
//public:
//    explicit AudioFileTreeItem(const std::string& fileName,
//        std::function<void()> onPlayCallback,
//        std::function<void()> onRemoveCallback)
//        : fileName(fileName),
//        onPlayCallback(std::move(onPlayCallback)),
//        onRemoveCallback(std::move(onRemoveCallback)) // Update this
//    {
//    }
//
//    bool mightContainSubItems() override { return false; }
//
//    void paintItem(juce::Graphics& g, int width, int height) override
//    {
//        g.fillAll(juce::Colours::lightgrey);
//        g.setColour(juce::Colours::black);
//        g.drawText(fileName, 2, 0, width - 30, height, juce::Justification::centredLeft);
//    }
//
//    
//
//    // Create a custom component for the item
//    std::unique_ptr<juce::Component> createItemComponent() override
//    {
//        // Return a custom component for the tree view item
//       // return std::make_unique<ItemComponent>(fileName, onPlayCallback, onRemoveCallback);
//
//        return std::make_unique<ItemComponent>(
//            fileName,
//            onPlayCallback,
//            [this]() { // Wrap lambda inside another lambda
//                if (onRemoveCallback)
//                    onRemoveCallback(this); // Pass 'this' correctly
//            });
//
//    }
//
//private:
//    std::string fileName;
//    std::function<void()> onPlayCallback;
//    std::function<void()> onRemoveCallback:
//
//    // Inner component for displaying the item
//    class ItemComponent : public juce::Component,
//        public juce::Button::Listener
//    {
//    public:
//        ItemComponent(const std::string& name,
//            std::function<void()> onPlay,
//            std::function<void()> onRemove)
//            :fileName(name), onPlayCallback(std::move(onPlay)),
//            onRemoveCallback(std::move(onRemove))
//        {
//            // Label for displaying the file name
//            //fileLabel.setText(name, juce::dontSendNotification);
//           // fileLabel.setJustificationType(juce::Justification::centredLeft);
//           // addAndMakeVisible(fileLabel);
//
//
//            addNewButtons(removeButton, "X");
//            addNewButtons(bypassButton, "Bypass");
//            addNewButtons(testButton, "test");
//
//            
//        }
//
//        void addNewButtons(juce::TextButton& button, const juce::String &ButtonText ) {
//
//            button.setButtonText(ButtonText);
//            button.addListener(this);
//            addAndMakeVisible(button);
//            
//        }
//
//       
//
//        void resized() override
//        {
//            auto bounds = getLocalBounds();
//            fileLabel.setBounds(bounds.removeFromLeft(bounds.getWidth() - 130)); // File label
//            bypassButton.setBounds(bounds.removeFromRight(50));
//            removeButton.setBounds(bounds.removeFromRight(30));
//            testButton.setBounds(bounds.removeFromRight(30));
//            
//                         
//        }
//
//        void buttonClicked(juce::Button* button) override
//        {
//            if (button == &removeButton)
//            {
//                if (onRemoveCallback)
//                    onRemoveCallback(); // Trigger remove callback
//            }
//            else if (button == &bypassButton)
//            {
//                std::cout << "Bypass button clicked" << std::endl;
//            }
//            else if (button == &testButton)
//            {
//                std::cout << "Test button clicked" << std::endl;
//            }
//            
//        }
//
//        void mouseDown(const juce::MouseEvent& e) override
//        {
//            // Play the audio file when clicked
//            //std::cout << "Item Clicked " << std::endl;
//
//            std::cout << "Playing audio file: " << fileName << std::endl;
//            if (onPlayCallback) {
//                onPlayCallback(); // Trigger playback callback
//            }
//
//
//        }
//
//    private:
//        juce::Label fileLabel;
//        std::string fileName;
//        juce::TextButton removeButton;
//        juce::TextButton bypassButton;
//        juce::TextButton testButton;
//        
//
//        std::function<void()> onPlayCallback;
//        std::function<void()> onRemoveCallback;
//    };
//}; // New solution



class AudioFileTreeItem : public juce::TreeViewItem
{
public:
    explicit AudioFileTreeItem(const std::string& fileName,
        std::function<void()> onPlayCallback,
        std::function<void(AudioFileTreeItem*)> onRemoveCallback)
        : fileName(fileName),
        onPlayCallback(std::move(onPlayCallback)),
        onRemoveCallback(std::move(onRemoveCallback)) // Update this to match the correct type
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
            });
    }

private:
    std::string fileName;
    std::function<void()> onPlayCallback;
    std::function<void(AudioFileTreeItem*)> onRemoveCallback; // Corrected the type to match the lambda callback

    // Inner component for displaying the item
    class ItemComponent : public juce::Component,
        public juce::Button::Listener
    {
    public:
        ItemComponent(const std::string& name,
            std::function<void()> onPlay,
            std::function<void()> onRemove)
            : fileName(name), onPlayCallback(std::move(onPlay)),
            onRemoveCallback(std::move(onRemove))
        {
            addNewButtons(removeButton, "X");
            addNewButtons(bypassButton, "Bypass");
            addNewButtons(testButton, "test");
        }

        void addNewButtons(juce::TextButton& button, const juce::String& ButtonText)
        {
            button.setButtonText(ButtonText);
            button.addListener(this);
            addAndMakeVisible(button);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            fileLabel.setBounds(bounds.removeFromLeft(bounds.getWidth() - 130)); // File label
            bypassButton.setBounds(bounds.removeFromRight(50));
            removeButton.setBounds(bounds.removeFromRight(30));
            testButton.setBounds(bounds.removeFromRight(30));
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
            else if (button == &testButton)
            {
                std::cout << "Test button clicked" << std::endl;
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
        juce::Label fileLabel;
        std::string fileName;
        juce::TextButton removeButton;
        juce::TextButton bypassButton;
        juce::TextButton testButton;

        std::function<void()> onPlayCallback;
        std::function<void()> onRemoveCallback;
    };
};



//class AudioFileTreeItem : public juce::TreeViewItem
//{
//public:
//    explicit AudioFileTreeItem(const std::string& fileName,
//        std::function<void()> onPlayCallback,
//        std::function<void(AudioFileTreeItem*)> onRemoveCallback) // Correct callback type
//        : fileName(fileName),
//        onPlayCallback(std::move(onPlayCallback)),
//        onRemoveCallback(std::move(onRemoveCallback)) // Move semantics
//    {
//    }
//
//    bool mightContainSubItems() override { return false; }
//
//    void paintItem(juce::Graphics& g, int width, int height) override
//    {
//        g.fillAll(juce::Colours::lightgrey);
//        g.setColour(juce::Colours::black);
//        g.drawText(fileName, 2, 0, width - 30, height, juce::Justification::centredLeft);
//    }
//
//    // Create a custom component for the item
//    std::unique_ptr<juce::Component> createItemComponent() override
//    {
//        // Correct the lambda to pass 'this' correctly
//        return std::make_unique<ItemComponent>(
//            fileName,
//            onPlayCallback,
//            [this]() { // Wrap lambda inside another lambda
//                if (onRemoveCallback)
//                    onRemoveCallback(this); // Pass 'this' as the argument (AudioFileTreeItem*)
//            });
//    }
//
//private:
//    std::string fileName;
//    std::function<void()> onPlayCallback;
//    std::function<void(AudioFileTreeItem*)> onRemoveCallback; // Correct callback type
//
//    // Inner component for displaying the item
//    class ItemComponent : public juce::Component,
//        public juce::Button::Listener
//    {
//    public:
//        ItemComponent(const std::string& name,
//            std::function<void()> onPlay,
//            std::function<void()> onRemove)
//            : fileName(name),
//            onPlayCallback(std::move(onPlay)),
//            onRemoveCallback(std::move(onRemove))
//        {
//            // Create buttons
//            addNewButton(removeButton, "X");
//            addNewButton(bypassButton, "Bypass");
//            addNewButton(testButton, "Test");
//        }
//
//        void addNewButton(juce::TextButton& button, const juce::String& buttonText)
//        {
//            button.setButtonText(buttonText);
//            button.addListener(this);
//            addAndMakeVisible(button);
//        }
//
//        void resized() override
//        {
//            auto bounds = getLocalBounds();
//            bypassButton.setBounds(bounds.removeFromRight(50));
//            removeButton.setBounds(bounds.removeFromRight(30));
//            testButton.setBounds(bounds.removeFromRight(30));
//        }
//
//        void buttonClicked(juce::Button* button) override
//        {
//            if (button == &removeButton)
//            {
//                if (onRemoveCallback)
//                    onRemoveCallback(); // Trigger remove callback
//            }
//            else if (button == &bypassButton)
//            {
//                std::cout << "Bypass button clicked" << std::endl;
//            }
//            else if (button == &testButton)
//            {
//                std::cout << "Test button clicked" << std::endl;
//            }
//        }
//
//        void mouseDown(const juce::MouseEvent&) override
//        {
//            // Play the audio file when clicked
//            std::cout << "Playing audio file: " << fileName << std::endl;
//            if (onPlayCallback)
//                onPlayCallback(); // Trigger playback callback
//        }
//
//    private:
//        std::string fileName;
//        juce::TextButton removeButton;
//        juce::TextButton bypassButton;
//        juce::TextButton testButton;
//
//        std::function<void()> onPlayCallback;
//        std::function<void()> onRemoveCallback;
//    };
//};










