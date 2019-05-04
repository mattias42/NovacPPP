# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_FILESYSTEM_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/Filesystem.h
    PARENT_SCOPE)
    
    
set(NPP_FILESYSTEM_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/Filesystem.cpp
    PARENT_SCOPE)

    