# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

# Output the executables to the '/bin' directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../poco/lib/cmake/Poco")

project (NovacPPP)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Poco REQUIRED COMPONENTS Foundation Net XML Util PATHS "${CMAKE_CURRENT_LIST_DIR}/../poco/install" "${CMAKE_CURRENT_LIST_DIR}/../poco")

# Add the different components
add_subdirectory(SpectralEvaluation)
add_subdirectory(PPPLib)
add_subdirectory(PPPExe)
add_subdirectory(PPPTests)
 