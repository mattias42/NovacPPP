# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_FLUX_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/FluxCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/FluxResult.h
    ${CMAKE_CURRENT_LIST_DIR}/FluxStatistics.h
    PARENT_SCOPE)
    
    
set(NPP_FLUX_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/FluxCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FluxResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FluxStatistics.cpp
    PARENT_SCOPE)

    