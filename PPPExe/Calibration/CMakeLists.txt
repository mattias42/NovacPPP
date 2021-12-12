# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_CALIBRATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/PostCalibration.h
    ${CMAKE_CURRENT_LIST_DIR}/PostCalibrationStatistics.h
    PARENT_SCOPE)
    
    
set(NPP_CALIBRATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/PostCalibration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostCalibrationStatistics.cpp
    PARENT_SCOPE)

    