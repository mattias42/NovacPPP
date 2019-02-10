# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

set(PppLib_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include CACHE STRING "PPPLib headers")

# Add the DOAS-fit files from DOASIS
add_subdirectory(${PppLib_INCLUDE_DIRS}/PPPLib/SpectralEvaluation/Fit)

# Add the common Evaluation classes
add_subdirectory(${PppLib_INCLUDE_DIRS}/PPPLib/SpectralEvaluation/Evaluation)

# Add the common Spectrum property classes
add_subdirectory(${PppLib_INCLUDE_DIRS}/PPPLib/SpectralEvaluation/Spectra)

# Add the common Flux classes
add_subdirectory(${PppLib_INCLUDE_DIRS}/PPPLib/SpectralEvaluation/Flux)


set(PPPLIB_SPECTRA_HEADERS
    ${PppLib_INCLUDE_DIRS}/PPPLib/Spectra/ScanFileHandler.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/Spectra/Spectrum.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/Spectra/SpectrumInfo.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/Spectra/SpectrumIO.h
)

set(PPPLIB_SPECTRA_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/ScanFileHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/Spectrum.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/SpectrumInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/SpectrumIO.cpp
)

set(PPPLIB_SPECTRUM_FORMAT_HEADERS
    ${PppLib_INCLUDE_DIRS}/PPPLib/SpectrumFormat/MKPack.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/SpectrumFormat/STDFile.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/SpectrumFormat/TXTFile.h
)

set(PPPLIB_SPECTRUM_FORMAT_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/SpectrumFormat/MKPack.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SpectrumFormat/STDFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SpectrumFormat/TXTFile.cpp
)

# Add the different components
add_library(PPPLib
    ${PppLib_INCLUDE_DIRS}/PPPLib/CArray.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CCriticalSection.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CFileUtils.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CFtpUtils.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CList.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CSingleLock.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CStdioFile.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CString.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CStringTokenizer.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/Measurement.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/PPPLib.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/SpectrometerModel.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/ThreadUtils.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/VolcanoInfo.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/SpectralEvaluation/Utils.h
    ${PPPLIB_SPECTRA_HEADERS}
    ${PPPLIB_SPECTRUM_FORMAT_HEADERS}
    ${SPECTRUM_FIT_HEADERS}
    ${SPECTRUM_EVALUATION_HEADERS}
    ${SPECTRUM_CLASS_HEADERS}
    ${SPECTRUM_FLUX_HEADERS}

    ${CMAKE_CURRENT_LIST_DIR}/src/CFileUtils.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/src/CFtpUtils.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/src/CStdioFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/CString.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/CStringTokenizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/VolcanoInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SpectrometerModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/ScanFileHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/Spectrum.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/SpectrumInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Spectra/SpectrumIO.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SpectrumFormat/MKPack.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SpectrumFormat/STDFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SpectrumFormat/TXTFile.cpp
    ${PppLib_INCLUDE_DIRS}/PPPLib/SpectralEvaluation/Utils.cpp
    ${PPPLIB_SPECTRA_SOURCES}
    ${PPPLIB_SPECTRUM_FORMAT_SOURCES}
    ${SPECTRUM_EVALUATION_SOURCES}
    ${SPECTRUM_CLASS_SOURCES}
    ${SPECTRUM_FLUX_SOURCES}
)

target_include_directories (PPPLib PUBLIC ${PppLib_INCLUDE_DIRS})

IF(WIN32)
    target_compile_options(PPPLib PRIVATE /W4 /WX /sdl)
    target_compile_definitions(PPPLib PRIVATE _CRT_SECURE_NO_WARNINGS)
ELSE()
    target_compile_options(PPPLib PRIVATE -Wall -std=c++14)
ENDIF()

source_group("Header Files\\Spectra"                FILES ${PPPLIB_SPECTRA_HEADERS} ${SPECTRUM_CLASS_HEADERS})
source_group("Header Files\\SpectrumFormat"         FILES ${PPPLIB_SPECTRUM_FORMAT_HEADERS})
source_group("Header Files\\SpectrumEvaluation"     FILES ${SPECTRUM_FIT_HEADERS})
source_group("Header Files\\Evaluation"             FILES ${SPECTRUM_EVALUATION_HEADERS})
source_group("Header Files\\Flux"                   FILES ${SPECTRUM_FLUX_HEADERS})



source_group("Source Files\\Spectra"                FILES ${PPPLIB_SPECTRA_SOURCES} ${SPECTRUM_CLASS_SOURCES})
source_group("Source Files\\SpectrumFormat"         FILES ${PPPLIB_SPECTRUM_FORMAT_SOURCES})
source_group("Source Files\\SpectrumEvaluation"     FILES ${SPECTRUM_FIT_SOURCES})
source_group("Source Files\\Evaluation"             FILES ${SPECTRUM_EVALUATION_SOURCES})
source_group("Source Files\\Flux"                   FILES ${SPECTRUM_FLUX_SOURCES})
