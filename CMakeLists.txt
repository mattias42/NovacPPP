# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

# Output the executables to the '/bin' directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../poco/lib/cmake/Poco")

project (NovacPPP)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ------------------- Dependencies -------------------

find_package(Poco REQUIRED COMPONENTS Foundation Net XML Util 
    PATHS "${CMAKE_CURRENT_LIST_DIR}/../External/poco/install" "${CMAKE_CURRENT_LIST_DIR}/../poco" "${CMAKE_CURRENT_LIST_DIR}/../poco/install")

set(RAPID_XML_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/rapidxml/)

FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

    
# ------------------- Components -------------------

# Add the different components
add_subdirectory(SpectralEvaluation)
add_subdirectory(PPPLib)
add_subdirectory(PPPExe)
add_subdirectory(PPPTests)
 