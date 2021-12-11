##########################################################################
## This is the main project of Novac Post Processing Program (NovacPPP) ##
##########################################################################

cmake_minimum_required (VERSION 3.6)

# Define the different groups of files
add_subdirectory(Calibration)
add_subdirectory(Common)
add_subdirectory(Communication)
add_subdirectory(Configuration)
add_subdirectory(Evaluation)
add_subdirectory(Filesystem)
add_subdirectory(Flux)
add_subdirectory(Geometry)
add_subdirectory(Meteorology)
add_subdirectory(Stratosphere)
add_subdirectory(WindMeasurement)

## --------- Creating NovacPPP ---------

# Add the different components
add_executable(NovacPPP
    ${CMAKE_CURRENT_LIST_DIR}/ContinuationOfProcessing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ContinuationOfProcessing.h
    ${CMAKE_CURRENT_LIST_DIR}/FileInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FileInfo.h
    ${CMAKE_CURRENT_LIST_DIR}/MeteorologySource.h
    ${CMAKE_CURRENT_LIST_DIR}/Molecule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Molecule.h
    ${CMAKE_CURRENT_LIST_DIR}/NovacPPP.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ObservatoryInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ObservatoryInfo.h
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessing.h
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessingStatistics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessingStatistics.h
    ${CMAKE_CURRENT_LIST_DIR}/SetupFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SetupFileReader.h
    ${CMAKE_CURRENT_LIST_DIR}/StringTokenizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/StringTokenizer.h

    ${NPP_CALIBRATION_HEADERS} 
    ${NPP_CALIBRATION_SOURCES}
    ${NPP_COMMON_HEADERS} 
    ${NPP_COMMON_SOURCES}
    ${NPP_COMMON_SPECTRA_HEADERS}
    ${NPP_COMMON_SPECTRA_SOURCES}
    ${NPP_COMMON_SPECTRUM_FORMAT_HEADERS}
    ${NPP_COMMON_SPECTRUM_FORMAT_SOURCES}
    ${NPP_COMMUNICATION_HEADERS}
    ${NPP_COMMUNICATION_SOURCES}
    ${NPP_CONFIGURATION_HEADERS}
    ${NPP_CONFIGURATION_SOURCES}
    ${NPP_EVALUATION_HEADERS}
    ${NPP_EVALUATION_SOURCES}
    ${NPP_FILESYSTEM_HEADERS}
    ${NPP_FILESYSTEM_SOURCES}
    ${NPP_FLUX_HEADERS}
    ${NPP_FLUX_SOURCES}
    ${NPP_GEOMETRY_HEADERS}
    ${NPP_GEOMETRY_SOURCES}
    ${NPP_METEOROLOGY_HEADERS}
    ${NPP_METEOROLOGY_SOURCES}
    ${NPP_STRATOSPHERE_HEADERS}
    ${NPP_STRATOSPHERE_SOURCES}
    ${NPP_WINDMEASUREMENT_HEADERS}
    ${NPP_WINDMEASUREMENT_SOURCES}
)

target_include_directories(NovacPPP PRIVATE 
    ${PppLib_INCLUDE_DIRS}
    ${SPECTRALEVAUATION_INCLUDE_DIRS}
    ${PocoNet_DIR}
    ${PocoFoundation_DIR})

target_link_libraries(NovacPPP PRIVATE PPPLib ${Poco_LIBRARIES})

IF(WIN32)
    # TODO: Also include /WX
    target_compile_options(NovacPPP PUBLIC /W4 /sdl)
    target_compile_definitions(NovacPPP PUBLIC _CRT_SECURE_NO_WARNINGS)
ELSE()
    target_compile_options(NovacPPP PUBLIC -Wall -std=c++14)
ENDIF()


IF(WIN32)
    #Organize the files into folders, for ease of use (Windows and Visual Studio only)
    source_group("Header Files\\Calibration"            FILES ${NPP_CALIBRATION_HEADERS})
    source_group("Header Files\\Common"                 FILES ${NPP_COMMON_HEADERS})
    source_group("Header Files\\Communication"          FILES ${NPP_COMMUNICATION_HEADERS})
    source_group("Header Files\\Configuration"          FILES ${NPP_CONFIGURATION_HEADERS})
    source_group("Header Files\\Evaluation"             FILES ${NPP_EVALUATION_HEADERS})
    source_group("Header Files\\Filesystem"             FILES ${NPP_FILESYSTEM_HEADERS})
    source_group("Header Files\\Flux"                   FILES ${NPP_FLUX_HEADERS})
    source_group("Header Files\\Geometry"               FILES ${NPP_GEOMETRY_HEADERS})
    source_group("Header Files\\Meteorology"            FILES ${NPP_METEOROLOGY_HEADERS})
    source_group("Header Files\\Stratosphere"           FILES ${NPP_STRATOSPHERE_HEADERS})
    source_group("Header Files\\WindMeasurement"        FILES ${NPP_WINDMEASUREMENT_HEADERS})

    source_group("Source Files\\Calibration"            FILES ${NPP_CALIBRATION_SOURCES})
    source_group("Source Files\\Common"                 FILES ${NPP_COMMON_SOURCES})
    source_group("Source Files\\Communication"          FILES ${NPP_COMMUNICATION_SOURCES})
    source_group("Source Files\\Configuration"          FILES ${NPP_CONFIGURATION_SOURCES})
    source_group("Source Files\\Evaluation"             FILES ${NPP_EVALUATION_SOURCES})
    source_group("Source Files\\Flux"                   FILES ${NPP_FLUX_SOURCES})
    source_group("Source Files\\Filesystem"             FILES ${NPP_FILESYSTEM_SOURCES})
    source_group("Source Files\\Geometry"               FILES ${NPP_GEOMETRY_SOURCES})
    source_group("Source Files\\Meteorology"            FILES ${NPP_METEOROLOGY_SOURCES})
    source_group("Source Files\\Stratosphere"           FILES ${NPP_STRATOSPHERE_SOURCES})
    source_group("Source Files\\WindMeasurement"        FILES ${NPP_WINDMEASUREMENT_SOURCES})
endif()


## --------- Creating The Reference Convolution Tool ---------

add_executable(ConvolutionTool
    ${CMAKE_CURRENT_LIST_DIR}/ConvolutionTool.cpp
)

target_include_directories(ConvolutionTool PRIVATE 
    ${SPECTRALEVAUATION_INCLUDE_DIRS}
    ${RAPID_XML_INCLUDE_DIR}
)

target_link_libraries(ConvolutionTool PRIVATE NovacSpectralEvaluation)

IF(WIN32)
    target_compile_options(ConvolutionTool PUBLIC /W4 /WX /sdl)
    target_compile_definitions(ConvolutionTool PUBLIC _CRT_SECURE_NO_WARNINGS)
ELSE()
    target_compile_options(ConvolutionTool PUBLIC -Wall -std=c++14)
ENDIF()

## --------- Creating The Auto Calibration Tool ---------

add_executable(CalibrationTool
    ${CMAKE_CURRENT_LIST_DIR}/CalibrationTool.cpp
)

target_include_directories(CalibrationTool PRIVATE 
    ${SPECTRALEVAUATION_INCLUDE_DIRS}
    ${RAPID_XML_INCLUDE_DIR}
)

target_link_libraries(CalibrationTool PRIVATE NovacSpectralEvaluation)

IF(WIN32)
    target_compile_options(CalibrationTool PUBLIC /W4 /WX /sdl)
    target_compile_definitions(CalibrationTool PUBLIC _CRT_SECURE_NO_WARNINGS)
ELSE()
    target_compile_options(CalibrationTool PUBLIC -Wall -std=c++14)
ENDIF()
