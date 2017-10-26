# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_COMMON_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/ASCII.h
    ${CMAKE_CURRENT_LIST_DIR}/Common.h
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationConfigurationParser.h
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationLogFileHandler.h
    ${CMAKE_CURRENT_LIST_DIR}/GPSData.h
    ${CMAKE_CURRENT_LIST_DIR}/ProcessingFileReader.h
    ${CMAKE_CURRENT_LIST_DIR}/SpectrometerModel.h
    ${CMAKE_CURRENT_LIST_DIR}/Version.h
    ${CMAKE_CURRENT_LIST_DIR}/XMLFileReader.h
    PARENT_SCOPE)
    
set(NPP_COMMON_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/Common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationConfigurationParser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationLogFileHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GPSData.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ProcessingFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SpectrometerModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Version.cpp
    ${CMAKE_CURRENT_LIST_DIR}/XMLFileReader.cpp
    PARENT_SCOPE)

    
set(NPP_COMMON_SPECTRA_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/ScanFileHandler.h
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/Spectrum.h
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/SpectrumInfo.h
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/SpectrumIO.h
    PARENT_SCOPE)
    
set(NPP_COMMON_SPECTRA_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/ScanFileHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/Spectrum.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/SpectrumInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Spectra/SpectrumIO.cpp
    PARENT_SCOPE)



set(NPP_COMMON_SPECTRUM_FORMAT_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/SpectrumFormat/MKPack.h
    ${CMAKE_CURRENT_LIST_DIR}/SpectrumFormat/STDFile.h
    ${CMAKE_CURRENT_LIST_DIR}/SpectrumFormat/TXTFile.h
    PARENT_SCOPE
)

set(NPP_COMMON_SPECTRUM_FORMAT_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/SpectrumFormat/MKPack.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SpectrumFormat/STDFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SpectrumFormat/TXTFile.cpp
    PARENT_SCOPE
)
