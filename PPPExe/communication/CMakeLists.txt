# Adding the Common helper files

cmake_minimum_required (VERSION 3.6)

set(NPP_COMMUNICATION_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/FTPServerConnection.h
    ${CMAKE_CURRENT_LIST_DIR}/FTPSocket.h
    PARENT_SCOPE)
    
    
set(NPP_COMMUNICATION_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/FTPServerConnection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FTPSocket.cpp
    PARENT_SCOPE)
