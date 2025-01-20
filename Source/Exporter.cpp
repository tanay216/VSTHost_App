#include "VSTPluginComponent.h"
#include "Exporter.h"


Exporter::Exporter() {
	Exporter::addSuffix = "";
	Exporter::addPrefix = "";
}
Exporter::~Exporter() {}


//void Exporter::exportFileName(const std::string& currentFileName,
//    const std::string& prefix,
//    const std::string& insert,
//    int insertIndex,
//    const std::string& suffix,
//    const std::string& find,
//    const std::string& replace,
//    int trimFromBeginningIndex,
//    int trimFromEndIndex,
//    int rangeFromIndex,
//    int rangeToIndex,
//    const std::string& regexPattern,
//    const std::string& regexReplacement)
//{
//    std::string baseName = currentFileName;
//
//    auto it = originalFileNameMap.find(currentFileName);
//    if (it != originalFileNameMap.end())
//    {
//        baseName = it->second; // Get original name
//    }
//
//    File audioFile(baseName);
//    std::string nameWithoutExtension = audioFile.getFileNameWithoutExtension().toStdString();
//    extension = audioFile.getFileExtension().toStdString();
//
//    // Perform Find & Replace
//    if (!find.empty())
//    {
//        size_t pos = 0;
//        while ((pos = nameWithoutExtension.find(find, pos)) != std::string::npos)
//        {
//            nameWithoutExtension.replace(pos, find.length(), replace);
//            pos += replace.length();
//        }
//    }
//
//    // Trim from Beginning
//    if (trimFromBeginningIndex > 0 && trimFromBeginningIndex <= static_cast<int>(nameWithoutExtension.size()))
//    {
//        nameWithoutExtension = nameWithoutExtension.substr(trimFromBeginningIndex);
//    }
//
//    // Trim from End
//    if (trimFromEndIndex > 0 && trimFromEndIndex <= static_cast<int>(nameWithoutExtension.size()))
//    {
//        nameWithoutExtension = nameWithoutExtension.substr(0, nameWithoutExtension.size() - trimFromEndIndex);
//    }
//
//    // Trim by Range
//    if (rangeFromIndex >= 0 && rangeToIndex > rangeFromIndex && rangeToIndex <= static_cast<int>(nameWithoutExtension.size()))
//    {
//        nameWithoutExtension.erase(rangeFromIndex, rangeToIndex - rangeFromIndex);
//    }
//
//    // Insert at index (only if not already inserted)
//    if (!insert.empty() && nameWithoutExtension.find(insert) == std::string::npos)
//    {
//        if (insertIndex >= 0 && insertIndex <= static_cast<int>(nameWithoutExtension.size()))
//        {
//            nameWithoutExtension.insert(insertIndex, insert);
//        }
//    }
//
//    outputFileName = prefix + nameWithoutExtension + suffix + extension;
//
//    std::cout << "Renamed: " << baseName << " -> " << outputFileName << std::endl;
//}


void Exporter::exportFileName(const std::string& currentFileName,
    const std::string& prefix,
    const std::string& insert,
    int insertIndex,
    const std::string& suffix,
    const std::string& find,
    const std::string& replace,
    int trimFromBeginningIndex,
    int trimFromEndIndex,
    int rangeFromIndex,
    int rangeToIndex,
    const std::string& regexPattern,
    const std::string& regexReplacement)
{
    std::string baseName = currentFileName;

    auto it = originalFileNameMap.find(currentFileName);
    if (it != originalFileNameMap.end())
    {
        baseName = it->second; // Get original name
    }

    File audioFile(baseName);
    std::string nameWithoutExtension = audioFile.getFileNameWithoutExtension().toStdString();
    extension = audioFile.getFileExtension().toStdString();

    // Apply Regular Expression Replacement
    if (!regexPattern.empty())
    {
        try
        {
            std::regex re(regexPattern);
            nameWithoutExtension = std::regex_replace(nameWithoutExtension, re, regexReplacement);
        }
        catch (const std::regex_error& e)
        {
            std::cerr << "Regex Error: " << e.what() << std::endl;
        }
    }

    // Perform Find & Replace
    if (!find.empty())
    {
        size_t pos = 0;
        while ((pos = nameWithoutExtension.find(find, pos)) != std::string::npos)
        {
            nameWithoutExtension.replace(pos, find.length(), replace);
            pos += replace.length();
        }
    }

    // Trim from Beginning
    if (trimFromBeginningIndex > 0 && trimFromBeginningIndex <= static_cast<int>(nameWithoutExtension.size()))
    {
        nameWithoutExtension = nameWithoutExtension.substr(trimFromBeginningIndex);
    }

    // Trim from End
    if (trimFromEndIndex > 0 && trimFromEndIndex <= static_cast<int>(nameWithoutExtension.size()))
    {
        nameWithoutExtension = nameWithoutExtension.substr(0, nameWithoutExtension.size() - trimFromEndIndex);
    }

    // Trim by Range
    if (rangeFromIndex >= 0 && rangeToIndex > rangeFromIndex && rangeToIndex <= static_cast<int>(nameWithoutExtension.size()))
    {
        nameWithoutExtension.erase(rangeFromIndex, rangeToIndex - rangeFromIndex);
    }

    // Insert at index (only if not already inserted)
    if (!insert.empty() && nameWithoutExtension.find(insert) == std::string::npos)
    {
        if (insertIndex >= 0 && insertIndex <= static_cast<int>(nameWithoutExtension.size()))
        {
            nameWithoutExtension.insert(insertIndex, insert);
        }
    }

    outputFileName = prefix + nameWithoutExtension + suffix + extension;

    std::cout << "Renamed: " << baseName << " -> " << outputFileName << std::endl;
}



void Exporter::batchRename(const juce::StringArray& inputFileNames, juce::StringArray& renamedFileNames, const std::string& prefix, const std::string& insert, int insertIndex, const std::string& suffix, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
    int trimFromEndIndex,
    int rangeFromIndex,
    int rangeToIndex,
    const std::string& regexPattern,
    const std::string& regexReplacement)
{
    for (const auto& currentFileName : inputFileNames)
    {
        exportFileName(currentFileName.toStdString(), prefix, insert, insertIndex, suffix, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement);
        renamedFileNames.add(outputFileName);
    }
}


void Exporter::resetOriginalNames(const juce::StringArray& inputFileNames)
{
    originalFileNameMap.clear(); // Reset all mappings
    for (const auto& fileName : inputFileNames)
    {
        originalFileNameMap[fileName.toStdString()] = fileName.toStdString(); // Map each file to its original name
    }
}

void Exporter::updateRenamedFileNames(const juce::StringArray& renamedFileNames)
{
     newRenamedFileNames = renamedFileNames;
   // this->renamedFileNames = renamedFileNames; // Store renamed file names
}

void Exporter::exportAudioToFile(const AudioBuffer<float>& buffer, double sampleRate, const std::string& originalFileName, const std::string& insert, int insertIndex, const std::string& find, const std::string& replace, int trimFromBeginningIndex,
    int trimFromEndIndex,
    int rangeFromIndex,
    int rangeToIndex,
    const std::string& regexPattern,
    const std::string& regexReplacement)
{
    std::cout << "Exporting processed audio to file..." << std::endl;
    //std::cout << "buffer channels: "<< buffer.getNumChannels() << "" << std::endl;

    exportFileName(originalFileName, addPrefix, insert, insertIndex ,addSuffix, find, replace, trimFromBeginningIndex, trimFromEndIndex, rangeFromIndex, rangeToIndex, regexPattern, regexReplacement);

    
    if (buffer.getNumChannels() == 1) {
        std::cout << "ExportMonoAudio function called" << std::endl;
        exportMonoAudio(buffer, sampleRate);
        
    }
    else if (buffer.getNumChannels() == 2) {
        std::cout << "ExportStereoAudio function called" << std::endl;
        exportStereoAudio(buffer, sampleRate);
        
    }
    else if (buffer.getNumChannels() > 2) {
        std::cout << "ExportMultiChannelAudio function called" << std::endl;
        exportMultiChannelAudio(buffer, sampleRate);

    }
}

void Exporter::exportMonoAudio(const AudioBuffer<float>& buffer, double sampleRate) {

    std::cout << "Exporting mono audio to file..." << std::endl;

    // Ensure mono by creating a temporary buffer with only one channel
   // AudioBuffer<float> buffer(1, buffer.getNumSamples());
    //buffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples()); // Copy from the left channel

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
    // Release the ownership of the monoOutputStream to the writer
    monoOutputStream.release();

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
void Exporter::exportStereoAudio(const AudioBuffer<float>& buffer, double sampleRate) {

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

void Exporter::exportMultiChannelAudio(const AudioBuffer<float>& buffer, double sampleRate) {

    std::cout << "=======================================" << std::endl;
    std::cout << "Exporting Multi Channel audio to file..." << std::endl;
    std::cout << "Multi Channel Audio Channels"<< buffer.getNumChannels() << std::endl;

    std::cout << "buffer sample size: " << buffer.getNumSamples() << std::endl;
    std::cout << "buffer RMS for channel 1: " << buffer.getRMSLevel(1, 0, buffer.getNumSamples()) << std::endl;
    std::cout << "buffer RMS for channel 3: " << buffer.getRMSLevel(3, 0, buffer.getNumSamples()) << std::endl;

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
    std::unique_ptr<FileOutputStream> MultiOutputStream(outputFile.createOutputStream());
    if (!MultiOutputStream)
    {
        std::cerr << "Failed to create output stream for " << outputFileName << std::endl;
        return;
    }
    // Set up the WAV format writer

    std::unique_ptr<AudioFormatWriter> writer(wavFormat.createWriterFor(MultiOutputStream.get(),
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
    MultiOutputStream.release();

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

void Exporter::saveLastExportFolder(const juce::String& path)
{
    SettingsManager::getInstance().saveSetting("lastExportFolder", path);
}

juce::String Exporter::getLastExportFolder() const
{
    return SettingsManager::getInstance().getSetting("lastExportFolder", "");
}

void Exporter::saveRenameSettings(const juce::String& settings)
{
    SettingsManager::getInstance().saveSetting("lastRenameSettings", settings);
}

juce::String Exporter::getLastRenameSettings() const
{
    return SettingsManager::getInstance().getSetting("lastRenameSettings", "");
}




