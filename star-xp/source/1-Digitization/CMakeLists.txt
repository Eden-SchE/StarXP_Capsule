#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(starXPproj_Digitization)

#----------------------------------------------------------------------------
# Specify the C++ standard 
#
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
set(CMAKE_BUILD_TYPE Debug)

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
#
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${ROOTSYS}/include)
include_directories(${YAML_CPP_INCLUDE_DIRS})

MESSAGE( ${PROJECT_SOURCE_DIR} )
include_directories(${PROJECT_SOURCE_DIR}/RootIO/include)
include_directories(${PROJECT_SOURCE_DIR}/TimeControl/include)
include_directories(${PROJECT_SOURCE_DIR}/EventControl/include)
include_directories(${PROJECT_SOURCE_DIR}/TopMetal/include)
include_directories(${PROJECT_SOURCE_DIR}/Filter/include)

# Add common libirary
include_directories(${CMAKE_SOURCE_DIR}/source/CommonLib/SimEvent)

file(GLOB_RECURSE sources "${PROJECT_SOURCE_DIR}/*.cc")
file(GLOB_RECURSE headers "${PROJECT_SOURCE_DIR}/*.hh")

#----------------------------------------------------------------------------
# Add the executable, and link it to the ROOT libraries
#
add_executable(Digitize Digitize.cc ${sources} ${headers})
target_link_libraries(Digitize SimEventDict ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build My. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.

file(GLOB_RECURSE Calib_list RELATIVE ${PROJECT_SOURCE_DIR} "./CalibFiles/*")
foreach(Calib ${Calib_list})
  MESSAGE( ${Calib} )
	configure_file(
		${PROJECT_SOURCE_DIR}/${Calib}
		${PROJECT_BINARY_DIR}/${Calib}
		COPYONLY
		)
endforeach() 

set(SCRIPTS_AND_CARD
DigiCard.yml
DigiScan.C
DigiMapScan.C
SimplePlot.C
)

foreach(script ${SCRIPTS_AND_CARD})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${script}
    ${PROJECT_BINARY_DIR}/${script}
    COPYONLY
  )
endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(starXPproj_Digitization DEPENDS Digitize)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#set(CMAKE_INSTALL_PREFIX ../StarXP-install) 
#install(TARGETS starXP DESTINATION bin)