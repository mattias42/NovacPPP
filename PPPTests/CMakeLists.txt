# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

set(PppTests_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)

# Add the different components
add_executable(PPPTests
	${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UnitTest_CArray.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UnitTest_CFileUtils.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UnitTest_CFtpUtils.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UnitTest_CList.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UnitTest_CString.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UnitTest_CStringTokenizer.cpp)

target_link_libraries(PPPTests PRIVATE PPPLib)
    
target_include_directories(PPPTests PRIVATE ${PppTests_INCLUDE_DIRS} ${PppLib_INCLUDE_DIRS})

target_compile_options(PPPTests PUBLIC /W4 /WX /sdl)
