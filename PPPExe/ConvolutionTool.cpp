#include <SpectralEvaluation/Spectra/ReferenceSpectrumConvolution.h>
#include <SpectralEvaluation/Evaluation/CrossSectionData.h>
#include <SpectralEvaluation/File/File.h>
#include <SpectralEvaluation/Utils.h>
#include <rapidxml.hpp>
#include <iostream>

struct ConvolutionToolSettings
{
    struct ConvolutionInputPair
    {
        std::string highResolutionCrossSection = "";
        std::string destinationFile = "";
        bool highPassFilter = false; //<- Set to true if the reference should be scaled to ppmm and high-pass-filtered
    };

    struct SpectrometerSettings
    {
        std::string slitFunction = "";
        std::string wavelengthCalibration = "";

        std::vector<ConvolutionInputPair> data;
    };

    std::vector<SpectrometerSettings> spectrometerSettings;
};

std::vector<char> ReadEntireFile(FILE* fileStream)
{
    std::vector<char> vBuffer;
    vBuffer.reserve(16384);

    unsigned char buffer[16384];
    while (!feof(fileStream))
    {
        const size_t bytesRead = fread(buffer, 1, 16384, fileStream);
        for(size_t ii = 0; ii < bytesRead; ++ii)
            vBuffer.push_back(buffer[ii]);
    }

    vBuffer.push_back(0); // end-of-string

    return vBuffer;
}

void ParseReference(rapidxml::xml_node<> *node, ConvolutionToolSettings::ConvolutionInputPair& data)
{
    int elementsRead = 0;
    
    auto refNode = node->first_node();
    while (refNode != nullptr)
    {
        if (EqualsIgnoringCase(refNode->name(), "CrossSection"))
        {
            data.highResolutionCrossSection = refNode->value();
            ++elementsRead;
        }
        else if (EqualsIgnoringCase(refNode->name(), "Output"))
        {
            data.destinationFile = refNode->value();
            ++elementsRead;
        }
        else if (EqualsIgnoringCase(refNode->name(), "HighPassFilter"))
        {
            data.highPassFilter = EqualsIgnoringCase(refNode->value(), "true");
            ++elementsRead;
        }

        refNode = refNode->next_sibling();
    }

    if (elementsRead < 2 || elementsRead > 3)
    {
        throw std::invalid_argument("Failed to parse 'Reference' section, invalid number of 'CrossSection' and 'Output' pairs.");
    }
}

void ParseSettings(std::vector<char>& configurationXml, ConvolutionToolSettings& settings)
{
    rapidxml::xml_document<> doc;    // character type defaults to char
    doc.parse<0>(configurationXml.data());  // 0 means default parse flags

    rapidxml::xml_node<> *node = doc.first_node("ReferenceConvolution");
    if (nullptr == node)
    {
        throw std::invalid_argument("Failed to parse configuration file, expected a 'ReferenceConvolution' section.");
    }

    rapidxml::xml_node<> *spec = node->first_node("Spectrometer");
    if (nullptr == spec)
    {
        throw std::invalid_argument(" Failed to parse configuration file, expected the 'ReferenceConvolution' section to contain a 'Spectrometer' section.");
   }

    // Parse the spectrometer(s)
    while (spec != nullptr)
    {
        ConvolutionToolSettings::SpectrometerSettings specSettings;

        auto specNode = spec->first_node();
        while(specNode != nullptr)
        {
            if (EqualsIgnoringCase(specNode->name(), "SlitFunction"))
            {
                specSettings.slitFunction = specNode->value();
            }
            else if (EqualsIgnoringCase(specNode->name(), "WavlengthCalibration"))
            {
                specSettings.wavelengthCalibration = specNode->value();
            }
            else if (EqualsIgnoringCase(specNode->name(), "Reference"))
            {
                ConvolutionToolSettings::ConvolutionInputPair pair;
                ParseReference(specNode, pair);
                specSettings.data.push_back(pair);
            }

            specNode = specNode->next_sibling();
        }

        settings.spectrometerSettings.push_back(specSettings);
        spec = spec->next_sibling("Spectrometer");
    }
}

int main(int argc, char* argv[])
{
    if (2 != argc || strlen(argv[1]) < 4)
    {
        std::cout << "This is the Novac Reference Convolution tool, pass a configuration file (in xml) as parameter." << std::endl;
        return 0;
    }

    // Read the configuration
    const std::string configurationFileName(argv[1]);
    FILE* configFile = fopen(configurationFileName.c_str(), "r");
    if (configFile == nullptr)
    {
        std::cout << " Failed to read configuration file: '" << configurationFileName << "'" << std::endl;
        return 0;
    }

    std::vector<char> configurationXml = ReadEntireFile(configFile);
    fclose(configFile);
    configFile = nullptr;

    // Parse the data
    ConvolutionToolSettings settings;
    try
    {
        ParseSettings(configurationXml, settings);
    }
    catch (std::exception& ex)
    {
        std::cout << "Failed to parse the configuration file: " << ex.what() << std::endl;
        return 0;
    }

    // Do the convolutions
    for (const ConvolutionToolSettings::SpectrometerSettings& spectrometer : settings.spectrometerSettings)
    {
        for(const ConvolutionToolSettings::ConvolutionInputPair& data : spectrometer.data)
        {
            Evaluation::CCrossSectionData result;
            if (Evaluation::ConvolveReference(spectrometer.wavelengthCalibration, spectrometer.slitFunction, data.highResolutionCrossSection, result))
            {
                if (data.highPassFilter)
                {
                    HighPassFilter(result);
                }

                FileIo::SaveCrossSectionFile(data.destinationFile, result);
            }
        }
    }
}
