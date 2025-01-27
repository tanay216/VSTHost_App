#pragma once
#include <iostream>
#include <JuceHeader.h>
#include "VSTPluginHost.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "AudioFileManager.h"
#include "VSTPluginComponent.h"
#include "Exporter.h"
#include "WAAPIManager.h"

//class WwiseTreeItem;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class WwiseTreeItem;

class MainComponent : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Component,
    public juce::ListBox



{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //Wwise API ==============================================================================
    void updateWwiseStatus();

    void updateWwiseTree();

    void addChildItems(WwiseTreeItem* parentItem, const std::vector<WwiseEventNode>& children);

    void triggerWwiseEvent(const std::string& eventName);
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
    Exporter exporterComponent;
    WAAPIManager waapiManager;
   

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
    
    juce::TextButton deleteButton{ "X" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton exportAudioButton{ "Export Processed Audio" };
    juce::TextButton refreshPluginDetailsButton{ "Refresh Plugin Details" };
    juce::TextButton unloadPluginButton{ "X Remove Plugin" };
    juce::TextButton ShowEditorButton{ "Show Plugin UI" };
    juce::TextButton refreshIODevicesListButton{ "Refresh IO Devices list" };
    juce::TextButton refreshFileTreeButton{ "Refresh file Tree" };

    // Audio File Tree Components
    juce::TreeView audioFileTree{ "Audio File Tree" };

    // Wwise API /UI Tree View Components
    juce::Label wwiseStatusLabel;
    juce::TreeView wwiseTree{"Wwise Tree"};



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


class ExportAudioComponent : public juce::Component,
    public juce::Button::Listener
{
public:

    ExportAudioComponent(juce::Array<juce::AudioBuffer<float>>& audioBuffers,
        juce::StringArray& audioFileNames,
        VSTPluginComponent& vstPluginComponent,
        std::unordered_map<std::string, bool>& bypassStates, Exporter& exporter) : audioBuffers(audioBuffers),
        audioFileNames(audioFileNames),
        bypassStates(bypassStates),
        vstPluginComponent(vstPluginComponent),
        exporter(exporter)
    {
        // Load last export path from settings
        juce::String lastExportPath = exporter.getLastExportFolder();
        if (!lastExportPath.isEmpty())
        {
            fileNameLabel.setText("Output Folder: " + lastExportPath, juce::dontSendNotification);
            exporter.outputDirPath = lastExportPath.toStdString();
        }

        std::cout << "Last Export Folder Loaded: " << lastExportPath << std::endl;

        // Load rename settings
        auto renameSettings = exporter.loadRenameSettings();

        // Apply settings to UI fields
        prefixPatternInput.setText(renameSettings["prefix"]);
        insertPatternInput.setText(renameSettings["insert"]);
        indexEditor.setText(renameSettings["insertIndex"]);
        suffixPatternInput.setText(renameSettings["suffix"]);
        findPatternInput.setText(renameSettings["find"]);
        replacePatternInput.setText(renameSettings["replace"]);
        trimFromBeginningInput.setText(renameSettings["trimStart"]);
        trimFromEndInput.setText(renameSettings["trimEnd"]);
        rangeFromInput.setText(renameSettings["rangeStart"]);
        rangeToInput.setText(renameSettings["rangeEnd"]);
        regexPatternInput.setText(renameSettings["regexPattern"]);
        regexReplacementInput.setText(renameSettings["regexReplacement"]);

        // Automatically set toggles based on whether text exists in inputs
        prefixToggle.setToggleState(!renameSettings["prefix"].isEmpty(), juce::dontSendNotification);
        insertToggle.setToggleState(!renameSettings["insert"].isEmpty(), juce::dontSendNotification);
        suffixToggle.setToggleState(!renameSettings["suffix"].isEmpty(), juce::dontSendNotification);
        findReplaceToggle.setToggleState(!renameSettings["find"].isEmpty() || !renameSettings["replace"].isEmpty(), juce::dontSendNotification);
        trimFromBeginningToggle.setToggleState(renameSettings["trimStart"] != "0", juce::dontSendNotification);
        trimFromEndToggle.setToggleState(renameSettings["trimEnd"] != "0", juce::dontSendNotification);
        rangeToggle.setToggleState(renameSettings["rangeStart"] != "0" || renameSettings["rangeEnd"] != "0", juce::dontSendNotification);
        regexToggle.setToggleState(!renameSettings["regexPattern"].isEmpty(), juce::dontSendNotification);

        // Enable text fields based on toggle states
        prefixPatternInput.setEnabled(prefixToggle.getToggleState());
        insertPatternInput.setEnabled(insertToggle.getToggleState());
        indexEditor.setEnabled(insertToggle.getToggleState());
        suffixPatternInput.setEnabled(suffixToggle.getToggleState());
        findPatternInput.setEnabled(findReplaceToggle.getToggleState());
        replacePatternInput.setEnabled(findReplaceToggle.getToggleState());
        trimFromBeginningInput.setEnabled(trimFromBeginningToggle.getToggleState());
        trimFromEndInput.setEnabled(trimFromEndToggle.getToggleState());
        rangeFromInput.setEnabled(rangeToggle.getToggleState());
        rangeToInput.setEnabled(rangeToggle.getToggleState());
        regexPatternInput.setEnabled(regexToggle.getToggleState());
        regexReplacementInput.setEnabled(regexToggle.getToggleState());

        // Add reset button
        addAndMakeVisible(resetRenameButton);
        resetRenameButton.setButtonText("Reset Rename Settings");
        resetRenameButton.addListener(this);

        // Add and configure the "Browse" button
        addAndMakeVisible(browseButton);
        browseButton.setButtonText("Browse");
        browseButton.addListener(this);

        // Add and configure the "Export" button
        addAndMakeVisible(exportButton);
        exportButton.setButtonText("Export");
        exportButton.addListener(this);

        // Add and configure the "Rename" button
        addAndMakeVisible(renameButton);
        renameButton.setButtonText("Rename");
        renameButton.addListener(this);

        // Add the filename label
        addAndMakeVisible(fileNameLabel);
        fileNameLabel.setText("Output Folder: " + lastExportPath, juce::dontSendNotification);

        //==================== Prefix ========================

        // Prefix Toggle
        addAndMakeVisible(prefixToggle);
        prefixToggle.setButtonText("Prefix");
        prefixToggle.onClick = [this] { prefixPatternInput.setEnabled(prefixToggle.getToggleState()); };

        // Add label for renaming input
        addAndMakeVisible(prefixPatternLabel);
        prefixPatternLabel.setText("suffix Pattern:", juce::dontSendNotification);

        // Add text editor for renaming input
        addAndMakeVisible(prefixPatternInput);
        prefixPatternInput.setTextToShowWhenEmpty("Prefix", juce::Colours::grey);
        prefixPatternInput.setEnabled(false);



        //==================== Insert At Index ========================

        // Insert Toggle
        addAndMakeVisible(insertToggle);
        insertToggle.setButtonText("Insert");
        insertToggle.onClick = [this] {
            insertPatternInput.setEnabled(insertToggle.getToggleState());
            indexEditor.setEnabled(insertToggle.getToggleState());
            };


        addAndMakeVisible(insertPatternInput);
        insertPatternInput.setTextToShowWhenEmpty("Insert", juce::Colours::grey);
        insertPatternInput.setEnabled(false);

        addAndMakeVisible(indexEditor);
        indexEditor.setTextToShowWhenEmpty("At Index", juce::Colours::grey);
        indexEditor.setEnabled(false);

        //==================== Suffix ========================

        // Suffix Toggle
        addAndMakeVisible(suffixToggle);
        suffixToggle.setButtonText("Suffix");
        suffixToggle.onClick = [this] { suffixPatternInput.setEnabled(suffixToggle.getToggleState()); };

        // Add label for suffix renaming input
        addAndMakeVisible(suffixPatternLabel);
        suffixPatternLabel.setText("suffix Pattern:", juce::dontSendNotification);


        // Add text editor for suffix renaming input
        addAndMakeVisible(suffixPatternInput);
        suffixPatternInput.setTextToShowWhenEmpty("Suffix", juce::Colours::grey);
        suffixPatternInput.setEnabled(false);

        //==================== Find & Replace ========================

        // FindReplace Toggle
        addAndMakeVisible(findReplaceToggle);
        findReplaceToggle.setButtonText("Find & Replace");
        findReplaceToggle.onClick = [this] { findPatternInput.setEnabled(findReplaceToggle.getToggleState()), replacePatternInput.setEnabled(findReplaceToggle.getToggleState()); };

        // Add Label for Find
        addAndMakeVisible(findPatternLabel);
        findPatternLabel.setText("Find:", juce::dontSendNotification);

        // Add input for Find
        addAndMakeVisible(findPatternInput);
        findPatternInput.setTextToShowWhenEmpty("Find", juce::Colours::grey);
        findPatternInput.setEnabled(false);

        // Add Label for Replace
        addAndMakeVisible(replacePatternLabel);
        replacePatternLabel.setText("Replace:", juce::dontSendNotification);

        // Add input for Replace
        addAndMakeVisible(replacePatternInput);
        replacePatternInput.setTextToShowWhenEmpty("Replace", juce::Colours::grey);
        replacePatternInput.setEnabled(false);



        //==================== Find & Replace ========================

        // Trim from Beginning
        addAndMakeVisible(trimFromBeginningToggle);
        trimFromBeginningToggle.setButtonText("Trim From Beginning");
        trimFromBeginningToggle.onClick = [this] { trimFromBeginningInput.setEnabled(trimFromBeginningToggle.getToggleState()); };
        addAndMakeVisible(trimFromBeginningInput);
        trimFromBeginningInput.setTextToShowWhenEmpty("Characters", juce::Colours::grey);
        trimFromBeginningInput.setEnabled(false);

        // Trim from End
        addAndMakeVisible(trimFromEndToggle);
        trimFromEndToggle.setButtonText("Trim From End");
        trimFromEndToggle.onClick = [this] { trimFromEndInput.setEnabled(trimFromEndToggle.getToggleState()); };
        addAndMakeVisible(trimFromEndInput);
        trimFromEndInput.setTextToShowWhenEmpty("Characters", juce::Colours::grey);
        trimFromEndInput.setEnabled(false);

        // Range Trim
        addAndMakeVisible(rangeToggle);
        rangeToggle.setButtonText("Trim Range");
        rangeToggle.onClick = [this] {
            rangeFromInput.setEnabled(rangeToggle.getToggleState());
            rangeToInput.setEnabled(rangeToggle.getToggleState());
            };
        addAndMakeVisible(rangeFromInput);
        rangeFromInput.setTextToShowWhenEmpty("From", juce::Colours::grey);
        rangeFromInput.setEnabled(false);
        addAndMakeVisible(rangeToInput);
        rangeToInput.setTextToShowWhenEmpty("To", juce::Colours::grey);
        rangeToInput.setEnabled(false);

        //==================== Regular Expression ========================

        // Regular Expression Rename
        addAndMakeVisible(regexToggle);
        regexToggle.setButtonText("Regex Rename");
        regexToggle.onClick = [this] { regexPatternInput.setEnabled(regexToggle.getToggleState()); regexReplacementInput.setEnabled(regexToggle.getToggleState()); };
        addAndMakeVisible(regexPatternInput);
        regexPatternInput.setTextToShowWhenEmpty("Regex Pattern", juce::Colours::grey);
        regexPatternInput.setEnabled(false);
        addAndMakeVisible(regexReplacementInput);
        regexReplacementInput.setTextToShowWhenEmpty("Replacement", juce::Colours::grey);
        regexReplacementInput.setEnabled(false);
    }



    void handleExportAudio()
    {
        if (audioBuffers.size() != exporter.getRenamedFileNames().size())
        {
            std::cout << "Mismatch between buffers and renamed file names." << std::endl;
            return;
        }

        auto renamedFileNames = exporter.getRenamedFileNames();

        for (int i = 0; i < audioBuffers.size(); ++i)
        {
            const auto& renamedFileName = renamedFileNames[i].toStdString();
            juce::AudioBuffer<float>& buffer = audioBuffers[i];

            if (bypassStates.find(renamedFileName) != bypassStates.end() && bypassStates[renamedFileName])
            {
                std::cout << "Skipping export for bypassed file: " << renamedFileName << std::endl;
                continue;
            }

            if (buffer.getNumSamples() == 0)
            {
                std::cout << "Audio Buffer is empty for: " << renamedFileName << "." << std::endl;
                return;
            }

            std::cout << "Exporting: " << renamedFileName << std::endl;
            // Get the dynamic insert values
            const std::string insert = insertPatternInput.getText().toStdString();
            const int insertIndex = indexEditor.getText().getIntValue();
            const std::string find = findPatternInput.getText().toStdString();
            const std::string replace = replacePatternInput.getText().toStdString();
            int trimFromBeginningIndex = trimFromBeginningInput.getText().getIntValue();
            int trimFromEndIndex = trimFromEndInput.getText().getIntValue();
            int rangeFromIndex = rangeFromInput.getText().getIntValue();
            int rangeToIndex = rangeToInput.getText().getIntValue();
            const std::string& regexPattern = regexPatternInput.getText().toStdString();
            const std::string& regexReplacement = regexReplacementInput.getText().toStdString();

            // Validate insertIndex
            if (insertIndex < 0)
            {
                std::cerr << "Invalid insert index: " << insertIndex << std::endl;
                return;
            }
            vstPluginComponent.processAudioWithPlugin(buffer, renamedFileName, insert, insertIndex, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexPattern);
        }

        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon, "Export", "Audio exported successfully!");
    }




    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        resetRenameButton.setBounds(bounds.removeFromTop(30).removeFromLeft(100));

        auto topRow = bounds.removeFromTop(30);
        auto secondRow = bounds.removeFromTop(30);
        auto thirdRow = bounds.removeFromTop(30);
        auto fourthRow = bounds.removeFromTop(30);
        auto fifthRow = bounds.removeFromTop(30);

        fileNameLabel.setBounds(bounds.removeFromTop(30));


        prefixToggle.setBounds(topRow.removeFromLeft(100));
        prefixPatternInput.setBounds(topRow.removeFromLeft(120).withWidth(200));

        insertToggle.setBounds(secondRow.removeFromLeft(100));
        insertPatternInput.setBounds(secondRow.removeFromLeft(120));
        indexEditor.setBounds(secondRow.removeFromLeft(140));

        suffixToggle.setBounds(thirdRow.removeFromLeft(100));
        suffixPatternInput.setBounds(thirdRow.removeFromLeft(120).withWidth(200));

        findReplaceToggle.setBounds(fourthRow.removeFromLeft(100));
        // findPatternLabel.setBounds(fifthRow.removeFromTop(30).removeFromLeft(120));
        findPatternInput.setBounds(fifthRow.removeFromLeft(120));
        // replacePatternLabel.setBounds(sixthRow.removeFromTop(30).removeFromLeft(120));
        replacePatternInput.setBounds(fifthRow.removeFromLeft(140));


        auto row = bounds.removeFromTop(30);

        trimFromBeginningToggle.setBounds(row.removeFromLeft(150));
        trimFromBeginningInput.setBounds(row);
        row = bounds.removeFromTop(30);

        trimFromEndToggle.setBounds(row.removeFromLeft(150));
        trimFromEndInput.setBounds(row);
        row = bounds.removeFromTop(30);

        rangeToggle.setBounds(row.removeFromLeft(150));
        rangeFromInput.setBounds(row.removeFromLeft(70));
        rangeToInput.setBounds(row);
        row = bounds.removeFromTop(30);

        regexToggle.setBounds(row.removeFromLeft(150));
        regexPatternInput.setBounds(row.removeFromLeft(150));
        regexReplacementInput.setBounds(row);




        renameButton.setBounds(bounds.removeFromTop(30).removeFromLeft(100));
        browseButton.setBounds(bounds.removeFromTop(30).removeFromLeft(100));
        exportButton.setBounds(bounds.removeFromTop(30).removeFromLeft(100));
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black); // Set the background color to black
    }

    void buttonClicked(juce::Button* button) override
    {

        if (button == &browseButton)
        {
            std::cout << "Current Export Folder: " << exporter.getLastExportFolder() << std::endl;
            juce::String lastExportPath = exporter.getLastExportFolder();
            juce::File initialDirectory = juce::File(lastExportPath.isEmpty() ? juce::File::getSpecialLocation(juce::File::userDocumentsDirectory) : lastExportPath);
            // Open a file chooser for selecting a directory
            fileChooser = std::make_unique<juce::FileChooser>(
                "Choose an output directory", initialDirectory, "*");

            fileChooser->launchAsync(juce::FileBrowserComponent::canSelectDirectories,
                [this](const juce::FileChooser& chooser)
                {

                    auto selectedDirectory = chooser.getResult();
                    if (selectedDirectory.exists() && selectedDirectory.isDirectory())
                    {
                        // Set the output directory path
                        exporter.outputDirPath = selectedDirectory.getFullPathName().toStdString();
                        fileNameLabel.setText("Output Directory: " + selectedDirectory.getFullPathName(),
                            juce::dontSendNotification);

                        // Output the directory path for verification
                        std::cout << "Output directory set to: " << exporter.outputDirPath << std::endl;
                        exporter.saveLastExportFolder(exporter.outputDirPath);
                    }
                });
        }

        else if (button == &resetRenameButton)
        {
            exporter.resetRenameSettings();

            prefixPatternInput.setText("");
            suffixPatternInput.setText("");
            insertPatternInput.setText("");
            indexEditor.setText("0");
            findPatternInput.setText("");
            replacePatternInput.setText("");
            trimFromBeginningInput.setText("0");
            trimFromEndInput.setText("0");
            rangeFromInput.setText("0");
            rangeToInput.setText("0");
            regexPatternInput.setText("");
            regexReplacementInput.setText("");

            std::cout << "Rename settings reset to default." << std::endl;
        }

        else if (button == &exportButton)
        {
            if (!audioBuffers.isEmpty())

            {
                handleExportAudio();
                // Perform export logic here
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon, "Export", "Audio exported successfully!");
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon, "Export", "Please select a valid output file.");
            }



        }

        else if (button == &renameButton)
        {
            //exporter.resetOriginalNames(audioFileNames);
            performBatchRename(prefixPatternInput.getText(), suffixPatternInput.getText());
        }

    }


    void ExportAudioComponent::performBatchRename(const juce::String& prefix, const juce::String& suffix)
    {
        juce::StringArray renamedFileNames;
        exporter.resetOriginalNames(audioFileNames);

        // Get the dynamic values
        const juce::String insert = insertPatternInput.getText();
        const int insertIndex = indexEditor.getText().getIntValue();
        const juce::String find = findPatternInput.getText();
        const juce::String replace = replacePatternInput.getText();
        int trimFromBeginningIndex = trimFromBeginningInput.getText().getIntValue();
        int trimFromEndIndex = trimFromEndInput.getText().getIntValue();
        int rangeFromIndex = rangeFromInput.getText().getIntValue();
        int rangeToIndex = rangeToInput.getText().getIntValue();
        const std::string& regexPattern = regexPatternInput.getText().toStdString();
        const std::string& regexReplacement = regexReplacementInput.getText().toStdString();

        exporter.saveRenameSettings(prefix.toStdString(), insert.toStdString(), insertIndex, suffix.toStdString(),
            find.toStdString(), replace.toStdString(), trimFromBeginningIndex, trimFromEndIndex,
            rangeFromIndex, rangeToIndex, regexPattern, regexReplacement);
        std::cout << "Saved Rename Settings" << std::endl;

        // Validation
        if (insertIndex < 0)
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon, "Invalid Index", "Please enter a non-negative index.");
            return;
        }

        // Perform batch renaming
        exporter.batchRename(audioFileNames, renamedFileNames, prefix.toStdString(), insert.toStdString(), insertIndex, suffix.toStdString(), find.toStdString(), replace.toStdString(), trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement);

        // Update exporter with renamed file names
        exporter.updateRenamedFileNames(renamedFileNames);
    }






private:
    juce::TextButton resetRenameButton{ "Reset Rename" };
    juce::TextButton browseButton{ "Browse" };
    juce::TextButton exportButton{ "Export" };
    juce::Label fileNameLabel;
    juce::TextButton renameButton{ "Rename" };

    juce::ToggleButton prefixToggle, insertToggle, suffixToggle, findReplaceToggle, trimFromBeginningToggle, trimFromEndToggle, rangeToggle;

    juce::Label prefixPatternLabel{ "Prefix Label", "Prefix" };
    juce::TextEditor prefixPatternInput;

    juce::Label insertPatternLabel{ "Insert Label", "Insert" };
    juce::TextEditor insertPatternInput, indexEditor;

    juce::Label suffixPatternLabel{ "Suffix Label", "Suffix" };
    juce::TextEditor suffixPatternInput;

    juce::Label findPatternLabel{ "Find Label", "Find" };
    juce::TextEditor findPatternInput;

    juce::Label replacePatternLabel{ "Replace Label", "Replace" };
    juce::TextEditor replacePatternInput, trimFromBeginningInput, trimFromEndInput, rangeFromInput, rangeToInput;

    juce::ToggleButton regexToggle;
    juce::TextEditor regexPatternInput, regexReplacementInput;

    


    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File outputFile;
    Exporter& exporter;

    juce::Array<juce::AudioBuffer<float>>& audioBuffers;
    juce::StringArray& audioFileNames;
    std::unordered_map<std::string, bool>& bypassStates;
    VSTPluginComponent& vstPluginComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExportAudioComponent)
};


//class WwiseTreeItem : public juce::TreeViewItem
//                    
//                        
//{
//public:
//    WwiseTreeItem(const WwiseEventNode& node) : eventNode(node) {}
//
//    bool mightContainSubItems() override { return !eventNode.children.empty(); }
//    void paintItem(juce::Graphics& g, int width, int height) override
//    {
//        g.drawText(eventNode.name, 4, 0, width - 4, height, juce::Justification::centredLeft);
//       // std::cout << "Wwise Tree Item (paint item): " << eventNode.name << std::endl;
//    }
//    
//
//    void itemOpennessChanged(bool isNowOpen) override
//    {
//        if (isNowOpen && getNumSubItems() == 0)
//        {
//            for (const auto& child : eventNode.children)
//            {
//                addSubItem(new WwiseTreeItem(child));
//            }
//        }
//    }
//
//private:
//    WwiseEventNode eventNode;
//};

class WwiseTreeItem : public juce::TreeViewItem
{
public:
    WwiseTreeItem(const WwiseEventNode& node, MainComponent* mainComponent)
        : eventNode(node), mainComponent(mainComponent) {
    }

    bool mightContainSubItems() override { return !eventNode.children.empty(); }
   

    void paintItem(juce::Graphics& g, int width, int height) override
    {
        g.setColour(juce::Colours::lightgrey);
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::black);
        g.drawText(eventNode.name, 4, 0, width - 4, height, juce::Justification::centredLeft);
    }

    

    // Custom UI component (bypass + remove buttons)
    std::unique_ptr<juce::Component> createItemComponent() override
    {
        return std::make_unique<ItemComponent>(
            eventNode.name,
            [this]() { std::cout << "Playing Wwise Event: " << eventNode.name << std::endl; },
            [this]() { if (onRemoveCallback) onRemoveCallback(this); },
            mainComponent);
    }

    void setRemoveCallback(std::function<void(WwiseTreeItem*)> callback)
    {
        onRemoveCallback = std::move(callback);
    }

   
    void itemOpennessChanged(bool isNowOpen) override
    {
        if (isNowOpen && getNumSubItems() == 0)
        {
            for (const auto& child : eventNode.children)
            {
                addSubItem(new WwiseTreeItem(child, mainComponent));
            }
        }
    }


private:
    WwiseEventNode eventNode;
    MainComponent* mainComponent;
    std::function<void(WwiseTreeItem*)> onRemoveCallback;

    class ItemComponent : public juce::Component, public juce::Button::Listener
    {
    public:
        ItemComponent(const std::string& name,
            std::function<void()> onPlay,
            std::function<void()> onRemove,
            MainComponent* mainComponent)
            : fileName(name), onPlayCallback(std::move(onPlay)), onRemoveCallback(std::move(onRemove)), mainComponent(mainComponent)
        {
            addNewButtons(removeButton, "X");
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
            bypassToggleButton.setBounds(bounds.removeFromRight(30));
            removeButton.setBounds(bounds.removeFromRight(30));
        }

        void buttonClicked(juce::Button* button) override
        {
            if (button == &removeButton)
            {
                if (onRemoveCallback) onRemoveCallback(); // Trigger remove callback
            }
            else if (button == &bypassToggleButton)
            {
                bool isBypassed = bypassToggleButton.getToggleState();
                if (mainComponent)
                {
                    mainComponent->setBypassState(fileName, isBypassed);
                    
                }
            }
        }

        void mouseDown(const juce::MouseEvent& e) override
        {
            if (mainComponent)
            {
                std::cout << "Triggering Wwise Event: " << fileName << std::endl;
                mainComponent->triggerWwiseEvent(fileName); // Call function in MainComponent
            }
        }

       
    private:
        std::string fileName;
        juce::TextButton removeButton;
        juce::ToggleButton bypassToggleButton;
        MainComponent* mainComponent;
        std::function<void()> onPlayCallback;
        std::function<void()> onRemoveCallback;
        
        
    };
};












