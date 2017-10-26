# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)


set(NPP_WINDMEASUREMENT_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/WindSpeedCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/WindSpeedMeasSettings.h
    PARENT_SCOPE)
    
    
set(NPP_WINDMEASUREMENT_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/WindSpeedCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WindSpeedMeasSettings.cpp
    PARENT_SCOPE)

    