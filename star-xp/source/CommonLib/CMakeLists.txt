#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(starXPproj_CommonLib)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available Vis drivers by default
# You can set WITH_GEANT4_VIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
#option(WITH_GEANT4_VIS "Build example with Geant4 Vis drivers" ON)
#if(WITH_GEANT4_VIS)
#  find_package(Geant4 REQUIRED gdml ui_all vis_all)
#else()
#  find_package(Geant4 REQUIRED gdml)
#endif()

#----------------------------------------------------------------------------
##note that for MacOS, no RPATH is allowed for install
##a warning will be given when set INSTALL_RPATH

# for mac
#set(CMAKE_MACOSX_RPATH 1)

# the RPATH to be used when building  
#set(CMAKE_BUILD_RPATH 
#   "./"
#   "/Users/chad/Work/src/programs/geant/geant4.10.03.p03-install/lib/"   
#   "/anaconda3/lib")
#   "/Users/liuqian/Applications/Qt5.6.0/5.6/clang_64/lib")
   
#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
#include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
## Setup ROOT include directories and compile definitions
##
#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
#find_package(ROOT REQUIRED)
#
## check ROOT version
#if(ROOT_FOUND)
#  STRING(REGEX MATCH "6.*" VERSION6MATCH ${ROOT_VERSION})
#  if(NOT VERSION6MATCH)
#    message(FATAL_ERROR "--> ERROR: ROOT 6 is required but not found")
#  endif()
#endif()

include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Find QT (required package)   
#
#find_package(Qt5Widgets)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
#include_directories(${Geant4_INCLUDE_DIR})
include_directories(${ROOTSYS}/include)

MESSAGE( ${PROJECT_SOURCE_DIR} )
include_directories(${PROJECT_SOURCE_DIR}/SimEvent)

#----------------------------------------------------------------------------
# Generate dictionaries, add ROOT libraries properties
ROOT_GENERATE_DICTIONARY(SimEvent ${PROJECT_SOURCE_DIR}/SimEvent/SimEvent.h LINKDEF ${PROJECT_SOURCE_DIR}/SimEvent/Linkdef.h)
#REFLEX_GENERATE_DICTIONARY(SimEvent ${PROJECT_SOURCE_DIR}/SimEvent/SimEvent.h SELECTION ${PROJECT_SOURCE_DIR}/SimEvent/xml/selection.xml)
add_library(SimEventDict SHARED SimEvent.cxx)
set(libsuffix .so)
set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})
set_target_properties(SimEventDict PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
target_link_libraries(SimEventDict ${ROOT_LIBRARIES})



#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#set(CMAKE_INSTALL_PREFIX ../StarXP-install) 
#install(TARGETS starXP DESTINATION bin)