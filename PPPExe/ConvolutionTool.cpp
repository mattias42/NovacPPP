#include <SpectralEvaluation/Spectra/ReferenceSpectrumConvolution.h>
#include <SpectralEvaluation/Evaluation/CrossSectionData.h>
#include <SpectralEvaluation/File/File.h>
#include <SpectralEvaluation/StringUtils.h>
#include <rapidxml.hpp>
#include <iostream>

struct ConvolutionToolSettings
{
    enum class SLIT_FUNCTION_TYPE
    {
        FILE,
        GAUSSIAN
    };

    struct ConvolutionSettings
    {
        // The high resolved cross section to convolve
        std::string highResolutionCrossSectionFile = "";

        // The destination, where the result should be saved
        std::string destinationFile = "";

        // The slit function type
        SLIT_FUNCTION_TYPE type = SLIT_FUNCTION_TYPE::FILE;

        /** Parameters for the slit-function, interpretation depends on 'type'.
            GAUSSIAN: params[0] is sigma.
         */
        double params[4] = { 0.0, 0.0, 0.0, 0.0 };

        // Set to true if the reference should be scaled to ppmm and high-pass-filtered
        bool highPassFilter = false;

        // The type of wavelength conversion to perform, default is None.
        WavelengthConversion wavelengthConversion = WavelengthConversion::None;
    };

    // Instrument settings for the convolution
    struct SpectrometerSettings
    {
        std::string slitFunctionFile = "";
        std::string wavelengthCalibrationFile = "";

        // The data for a specific reference
        std::vector<ConvolutionSettings> data;
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
        for (size_t ii = 0; ii < bytesRead; ++ii)
            vBuffer.push_back(buffer[ii]);
    }

    vBuffer.push_back(0); // end-of-string

    return vBuffer;
}

void ParseWavelengthConversion(rapidxml::xml_node<> *refNode, ConvolutionToolSettings::ConvolutionSettings& data)
{
    if (strlen(refNode->value()) == 0)
    {
        throw std::invalid_argument("Failed to parse wavelength conversion, only accepted values are 'None' and 'VacToAir'.");
    }

    if (EqualsIgnoringCase(refNode->value(), "None"))
    {
        data.wavelengthConversion = WavelengthConversion::None;
    }
    else if(EqualsIgnoringCase(refNode->value(), "VacToAir"))
    {
        data.wavelengthConversion = WavelengthConversion::VacuumToAir;
    }
    else
    {
        throw std::invalid_argument("Failed to parse wavelength conversion, only accepted values are 'None' and 'VacToAir'.");
    }
}

void ParseSlitFunction(rapidxml::xml_node<> *refNode, ConvolutionToolSettings::ConvolutionSettings& data)
{
    if (strlen(refNode->value()) > 0)
    {
        if (EqualsIgnoringCase(refNode->value(), "File"))
        {
            data.type = ConvolutionToolSettings::SLIT_FUNCTION_TYPE::FILE;
            return;
        }
    }
    else
    {
        auto child = refNode->first_node();
        if (nullptr != child)
        {
            if (EqualsIgnoringCase(child->name(), "Gaussian"))
            {
                data.type = ConvolutionToolSettings::SLIT_FUNCTION_TYPE::GAUSSIAN;

                auto attr = child->first_attribute();
                while (attr != nullptr)
                {
                    if (EqualsIgnoringCase(attr->name(), "Sigma"))
                    {
                        data.params[0] = std::atof(attr->value());
                    }
                    else if (EqualsIgnoringCase(attr->name(), "fwhm"))
                    {
                        data.params[0] = std::atof(attr->value()) / ( 2.0 * std::sqrt(2.0 * std::log(2.0)));
                    }
                    attr = attr->next_attribute();
                }

                if (fabs(data.params[0]) < 1e-7)
                {
                    throw std::invalid_argument("Cannot setup a gaussian with zero width.");
                }
                return;
            }
        }
    }

    throw std::invalid_argument("Invalid type of slit function encountered, can only handle 'File' or 'Gaussian'.");
}

void ParseReference(rapidxml::xml_node<> *node, ConvolutionToolSettings::ConvolutionSettings& data)
{
    int elementsRead = 0;

    auto refNode = node->first_node();
    while (refNode != nullptr)
    {
        if (EqualsIgnoringCase(refNode->name(), "CrossSection"))
        {
            data.highResolutionCrossSectionFile = refNode->value();
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
        else if (EqualsIgnoringCase(refNode->name(), "SlitFunctionType"))
        {
            ParseSlitFunction(refNode, data);
        }
        else if (EqualsIgnoringCase(refNode->name(), "Conversion"))
        {
            ParseWavelengthConversion(refNode, data);
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
        while (specNode != nullptr)
        {
            if (EqualsIgnoringCase(specNode->name(), "SlitFunction"))
            {
                specSettings.slitFunctionFile = specNode->value();
            }
            else if (EqualsIgnoringCase(specNode->name(), "WavlengthCalibration"))
            {
                specSettings.wavelengthCalibrationFile = specNode->value();
            }
            else if (EqualsIgnoringCase(specNode->name(), "Reference"))
            {
                ConvolutionToolSettings::ConvolutionSettings pair;
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
        for (const ConvolutionToolSettings::ConvolutionSettings& data : spectrometer.data)
        {
            Evaluation::CCrossSectionData result;
            std::cout << "Convolving reference: '" << data.destinationFile << "'... " << std::flush;

            bool success = true;

            if (data.type == ConvolutionToolSettings::SLIT_FUNCTION_TYPE::FILE)
            {
                success = Evaluation::ConvolveReference(spectrometer.wavelengthCalibrationFile, spectrometer.slitFunctionFile, data.highResolutionCrossSectionFile, result, data.wavelengthConversion);
            }
            else if (data.type == ConvolutionToolSettings::SLIT_FUNCTION_TYPE::GAUSSIAN)
            {
                success = Evaluation::ConvolveReferenceGaussian(spectrometer.wavelengthCalibrationFile, data.params[0], data.highResolutionCrossSectionFile, result, data.wavelengthConversion);
            }

            if (success)
            {
                if (data.highPassFilter)
                {
                    HighPassFilter(result);
                }

                std::cout << " done. Saving file to disk." << std::endl;
                FileIo::SaveCrossSectionFile(data.destinationFile, result);
            }
            else
            {
                std::cout << " Failed to convolve reference, could not create output file " << data.destinationFile << std::endl;
            }
        }
    }
}
