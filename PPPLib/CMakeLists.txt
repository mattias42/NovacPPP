# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

set(PppLib_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include CACHE STRING "PPPLib headers")

# Define the different groups of files
add_subdirectory(src/MFC)

# Add the different components
add_library(PPPLib
    ${NPPLIB_MFC_HEADERS} 
    ${NPPLIB_MFC_SOURCES}

    ${PppLib_INCLUDE_DIRS}/PPPLib/Measurement.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/PPPLib.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/ThreadUtils.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/VolcanoInfo.h

    ${CMAKE_CURRENT_LIST_DIR}/src/VolcanoInfo.cpp
)

target_include_directories (PPPLib PUBLIC ${PppLib_INCLUDE_DIRS} ${SPECTRALEVAUATION_INCLUDE_DIRS})
target_link_libraries(PPPLib PUBLIC NovacSpectralEvaluation)

IF(WIN32)
    target_compile_options(PPPLib PRIVATE /W4 /WX /sdl)
    target_compile_definitions(PPPLib PRIVATE _CRT_SECURE_NO_WARNINGS)
ELSE()
    target_compile_options(PPPLib PRIVATE -Wall -std=c++14)
ENDIF()

IF(WIN32)
    #Organize the files into folders, for ease of use (Windows and Visual Studio only)
    source_group("Header Files\\MFC"            FILES ${NPPLIB_MFC_HEADERS})

    source_group("Source Files\\MFC"            FILES ${NPPLIB_MFC_SOURCES})
endif()

