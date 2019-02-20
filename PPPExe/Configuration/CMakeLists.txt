# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_CONFIGURATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/DarkCorrectionConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/InstrumentConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/InstrumentLocation.h
    ${CMAKE_CURRENT_LIST_DIR}/LocationConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/NovacPPPConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/UserConfiguration.h
    PARENT_SCOPE)
    
    
set(NPP_CONFIGURATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/DarkCorrectionConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/EvaluationConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/InstrumentConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/InstrumentLocation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/LocationConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NovacPPPConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UserConfiguration.cpp
    PARENT_SCOPE)
