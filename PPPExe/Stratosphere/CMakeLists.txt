# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)


set(NPP_STRATOSPHERE_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/StratosphereCalculator.h
    PARENT_SCOPE)
    
    
set(NPP_STRATOSPHERE_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/StratosphereCalculator.cpp
    PARENT_SCOPE)

    