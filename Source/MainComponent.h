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
class MainComponent  : public juce::AudioAppComponent,
                        public juce::Button::Listener, 
                        public juce::Component,
                        public juce::ListBox
                       
                        
                       
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

    juce::AudioProcessorEditor* getPluginEditor();
   // void MainComponent::handleChannelConfigurationChange(int configId);
    void closePluginEditor();


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
    juce::TextButton scanPluginButton { "Scan Plugins" };
    juce::TextButton loadAudioButton { "Load Audio File" };
   // juce::ListBox audioFileListBox{ "Audio File List" }; // ListBox for displaying audio files
    juce::TreeView audioFileTree { "Audio File Tree" };
    juce::TextButton deleteButton{ "X" };
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton exportAudioButton { "Export Processed Audio" };
    juce::TextButton refreshPluginDetailsButton { "Refresh Plugin Details" };
    juce::TextButton unloadPluginButton{ "X Remove Plugin" };
    juce::TextButton ShowEditorButton { "Show Plugin UI" };
    

    juce::ComboBox pluginListDropdown;
    juce::ComboBox channelConfigDropdown;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

class AudioFileTreeItem : public juce::TreeViewItem
{
public:

    

    explicit AudioFileTreeItem(std::string& fileName) : fileName(fileName), deleteButton("X") {}
    

    bool mightContainSubItems() override { return false; }
    void paintItem(juce::Graphics& g, int width, int height) override
    {
        g.setColour(juce::Colours::black);
        g.drawText(fileName, 2, 0, width - 4, height, juce::Justification::centredLeft);

    }

    

private:
    std::string fileName;
    juce::TextButton deleteButton;

};