cmake_minimum_required(VERSION 3.2)

cmake_policy(SET CMP0048 NEW)
set(CMAKE_MACOSX_RPATH OFF)

project(QtShanoir VERSION 1.0.0)

## #############################################################################
## Additionals modules
## #############################################################################

set(${PROJECT_NAME}_CMAKE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
  )

set(CMAKE_MODULE_PATH
  ${${PROJECT_NAME}_CMAKE_DIR}
  ${CMAKE_MODULE_PATH}
  )

include(list_source_files)
include(list_header_directories_to_include)
include(set_lib_install_rules)
include(set_exe_install_rules)
include(set_lib_properties_variables)

## #############################################################################
## Find packages.
## #############################################################################

# Find the Qt library
find_package(Qt5 REQUIRED COMPONENTS
  Core
  Gui
  Network
  Widgets
  Xml
  )

## #############################################################################
## Qt Automoc
## #############################################################################

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
#   Find includes in corresponding build (and source) directories
#   (nedded for generated files)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


## #############################################################################
## enable c++ 11
## #############################################################################

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## #############################################################################
## Configure libs
## #############################################################################

add_subdirectory(DAO)
add_subdirectory(GUI)

################################################################################
# Configure files
################################################################################

set_lib_properties_variables(QtShanoirDAO QtShanoirGUI)

configure_file(${CMAKE_SOURCE_DIR}/CMake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    @ONLY IMMEDIATE
)

## #############################################################################
## Configure the demo
## #############################################################################

option(BUILD_DEMO "Build example application" ON)

if(BUILD_DEMO)
  add_subdirectory(Demo)
endif(BUILD_DEMO)

option(BUILD_COMMAND "Build command line toolkit" ON)

if(BUILD_COMMAND)
  add_subdirectory(Command)
endif()
