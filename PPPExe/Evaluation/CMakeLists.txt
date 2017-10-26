# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_EVALUATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/BasicMath.h
    ${CMAKE_CURRENT_LIST_DIR}/ColumnCorrection.h
    ${CMAKE_CURRENT_LIST_DIR}/CrossSectionData.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation.h
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationResult.h
    ${CMAKE_CURRENT_LIST_DIR}/ExtendedScanResult.h
    ${CMAKE_CURRENT_LIST_DIR}/FitWindow.h
    ${CMAKE_CURRENT_LIST_DIR}/MessageLog.h
    ${CMAKE_CURRENT_LIST_DIR}/PostEvaluationController.h
    ${CMAKE_CURRENT_LIST_DIR}/ReferenceFile.h
    ${CMAKE_CURRENT_LIST_DIR}/ReferenceFitResult.h
    ${CMAKE_CURRENT_LIST_DIR}/ScanEvaluation.h
    ${CMAKE_CURRENT_LIST_DIR}/ScanResult.h
    PARENT_SCOPE)
    
    
set(NPP_EVALUATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/BasicMath.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ColumnCorrection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CrossSectionData.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ExtendedScanResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FitWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MessageLog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostEvaluationController.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ReferenceFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ReferenceFitResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ScanEvaluation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ScanResult.cpp
    PARENT_SCOPE)

    