# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_COMMON_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/ASCII.h
    ${CMAKE_CURRENT_LIST_DIR}/Common.h
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationConfigurationParser.h
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationLogFileHandler.h
    ${CMAKE_CURRENT_LIST_DIR}/ProcessingFileReader.h
    ${CMAKE_CURRENT_LIST_DIR}/Version.h
    ${CMAKE_CURRENT_LIST_DIR}/XMLFileReader.h
    PARENT_SCOPE)
    
set(NPP_COMMON_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/Common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationConfigurationParser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationLogFileHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ProcessingFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Version.cpp
    ${CMAKE_CURRENT_LIST_DIR}/XMLFileReader.cpp
    PARENT_SCOPE)


