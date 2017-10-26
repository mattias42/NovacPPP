# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

set(PppLib_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include CACHE STRING "PPPLib headers")

# Add the different components
add_library(PPPLib
	${PppLib_INCLUDE_DIRS}/PPPLib/CArray.h
	${PppLib_INCLUDE_DIRS}/PPPLib/CCriticalSection.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CFileUtils.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/CFtpUtils.h
	${PppLib_INCLUDE_DIRS}/PPPLib/CList.h
	${PppLib_INCLUDE_DIRS}/PPPLib/CSingleLock.h
	${PppLib_INCLUDE_DIRS}/PPPLib/CStdioFile.h
	${PppLib_INCLUDE_DIRS}/PPPLib/CString.h
	${PppLib_INCLUDE_DIRS}/PPPLib/CStringTokenizer.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/DateTime.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/Measurement.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/VolcanoInfo.h
    ${PppLib_INCLUDE_DIRS}/PPPLib/ThreadUtils.h

    ${CMAKE_CURRENT_LIST_DIR}/src/CFileUtils.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/src/CFtpUtils.cpp 
	${CMAKE_CURRENT_LIST_DIR}/src/CStdioFile.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/CString.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/CStringTokenizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/DateTime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/VolcanoInfo.cpp
    )

target_include_directories (PPPLib PUBLIC ${PppLib_INCLUDE_DIRS})

target_compile_options(PPPLib PRIVATE /W4 /WX /sdl)
