# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_METEOROLOGY_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/WindDataBase.h
    ${CMAKE_CURRENT_LIST_DIR}/WindField.h
    ${CMAKE_CURRENT_LIST_DIR}/XMLWindFileReader.h
    PARENT_SCOPE)
    
    
set(NPP_METEOROLOGY_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/WindDataBase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WindField.cpp
    ${CMAKE_CURRENT_LIST_DIR}/XMLWindFileReader.cpp
    PARENT_SCOPE)

    