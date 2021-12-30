# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPPLIB_CALIBRATION_HEADERS
    ${PppLib_INCLUDE_DIRS}/PPPLib/Calibration/PostCalibration.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/Calibration/PostCalibrationStatistics.h
    PARENT_SCOPE)    
    
set(NPPLIB_CALIBRATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/PostCalibration.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/PostCalibrationStatistics.cpp
    PARENT_SCOPE)

    