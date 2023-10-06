#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

#----------------------------------------------------------------------------
# Get git commit ID
SET(ENV{GIT_DIR} ${PROJECT_SOURCE_DIR}/.git)
EXECUTE_PROCESS(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE VERSION_GIT_HEAD_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_VARIABLE GET_GIT_VERSION_FAILED
)
IF(GET_GIT_VERSION_FAILED)
    MESSAGE(FATAL_ERROR ${GET_GIT_VERSION_FAILED})
ELSE(GET_GIT_VERSION_FAILED)
    MESSAGE("-- Current Git Commit ID: ${VERSION_GIT_HEAD_VERSION}")
ENDIF(GET_GIT_VERSION_FAILED)
add_definitions(-DVERSION_GIT_HEAD_VERSION=\"${VERSION_GIT_HEAD_VERSION}\")

#----------------------------------------------------------------------------
# Setup ROOT include directories and compile definitions
#
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)

# check ROOT version
if(ROOT_FOUND)
  STRING(REGEX MATCH "6.*" VERSION6MATCH ${ROOT_VERSION})
  if(NOT VERSION6MATCH)
    message(FATAL_ERROR "--> ERROR: ROOT 6 is required but not found")
  endif()
endif()

#----------------------------------------------------------------------------
# Find Yaml-cpp
#
find_package(yaml-cpp REQUIRED)

set(CMAKE_INSTALL_PREFIX ../StarXP-install) 

add_subdirectory(source/CommonLib)
add_subdirectory(source/0-Simulation)
add_subdirectory(source/1-Digitization)
add_subdirectory(source/1.6-QuickAna)
#add_subdirectory(source/2-Reconstruction)
#add_subdirectory(source/3-Analysis)
#add_subdirectory(source/4-EventDisplay)
