# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_GEOMETRY_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/GeometryCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/GeometryResult.h
    ${CMAKE_CURRENT_LIST_DIR}/PlumeDataBase.h
    ${CMAKE_CURRENT_LIST_DIR}/PlumeHeight.h
    PARENT_SCOPE)
    
    
set(NPP_GEOMETRY_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/GeometryCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GeometryResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PlumeDataBase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PlumeHeight.cpp
    PARENT_SCOPE)

    