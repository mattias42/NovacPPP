# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_EVALUATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/ExtendedScanResult.h
    ${CMAKE_CURRENT_LIST_DIR}/PostEvaluationController.h
    ${CMAKE_CURRENT_LIST_DIR}/ScanEvaluation.h
    ${CMAKE_CURRENT_LIST_DIR}/ScanResult.h
    PARENT_SCOPE)
    
    
set(NPP_EVALUATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/PostEvaluationController.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ScanEvaluation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ScanResult.cpp
    PARENT_SCOPE)

    