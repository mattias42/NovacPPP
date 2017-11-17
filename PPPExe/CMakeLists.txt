# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

# Define the different groups of files
add_subdirectory(Common)
add_subdirectory(Communication)
add_subdirectory(Configuration)
add_subdirectory(Evaluation)
add_subdirectory(Fit)
add_subdirectory(Flux)
add_subdirectory(Geometry)
add_subdirectory(Meteorology)
add_subdirectory(Stratosphere)
add_subdirectory(WindMeasurement)


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
    ${CMAKE_CURRENT_LIST_DIR}/PlumeInScanProperty.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PlumeInScanProperty.h
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessing.h
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessingStatistics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessingStatistics.h
    ${CMAKE_CURRENT_LIST_DIR}/SetupFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SetupFileReader.h
    ${CMAKE_CURRENT_LIST_DIR}/StringTokenizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/StringTokenizer.h

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
    ${NPP_FIT_HEADERS}
    ${NPP_FIT_SOURCES}
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
    ${PocoNet_DIR}
    ${PocoFoundation_DIR})

target_link_libraries(NovacPPP PRIVATE PPPLib ${Poco_LIBRARIES})

IF(WIN32)
    # TODO: Also include /sdl and /WX
    target_compile_options(NovacPPP PUBLIC /W4 /WX /sdl)
ELSE()
    target_compile_options(NovacPPP PUBLIC -Wall -std=c++14)
ENDIF()


#Organize the files into folders, for ease of use
source_group("Header Files\\Common"                 FILES ${NPP_COMMON_HEADERS})
source_group("Header Files\\Common\\Spectra"        FILES ${NPP_COMMON_SPECTRA_HEADERS})
source_group("Header Files\\Common\\SpectrumFormat" FILES ${NPP_COMMON_SPECTRUM_FORMAT_HEADERS})
source_group("Header Files\\Communication"          FILES ${NPP_COMMUNICATION_HEADERS})
source_group("Header Files\\Configuration"          FILES ${NPP_CONFIGURATION_HEADERS})
source_group("Header Files\\Evaluation"             FILES ${NPP_EVALUATION_HEADERS})
source_group("Header Files\\Fit"                    FILES ${NPP_FIT_HEADERS})
source_group("Header Files\\Flux"                   FILES ${NPP_FLUX_HEADERS})
source_group("Header Files\\Geometry"               FILES ${NPP_GEOMETRY_HEADERS})
source_group("Header Files\\Meteorology"            FILES ${NPP_METEOROLOGY_HEADERS})
source_group("Header Files\\Stratosphere"           FILES ${NPP_STRATOSPHERE_HEADERS})
source_group("Header Files\\WindMeasurement"        FILES ${NPP_WINDMEASUREMENT_HEADERS})


source_group("Source Files\\Common"                 FILES ${NPP_COMMON_SOURCES})
source_group("Source Files\\Common\\Spectra"        FILES ${NPP_COMMON_SPECTRA_SOURCES})
source_group("Source Files\\Common\\SpectrumFormat" FILES ${NPP_COMMON_SPECTRUM_FORMAT_SOURCES})
source_group("Source Files\\Communication"          FILES ${NPP_COMMUNICATION_SOURCES})
source_group("Source Files\\Configuration"          FILES ${NPP_CONFIGURATION_SOURCES})
source_group("Source Files\\Evaluation"             FILES ${NPP_EVALUATION_SOURCES})
source_group("Source Files\\Fit"                    FILES ${NPP_FIT_SOURCES})
source_group("Source Files\\Flux"                   FILES ${NPP_FLUX_SOURCES})
source_group("Source Files\\Geometry"               FILES ${NPP_GEOMETRY_SOURCES})
source_group("Source Files\\Meteorology"            FILES ${NPP_METEOROLOGY_SOURCES})
source_group("Source Files\\Stratosphere"           FILES ${NPP_STRATOSPHERE_SOURCES})
source_group("Source Files\\WindMeasurement"        FILES ${NPP_WINDMEASUREMENT_SOURCES})


