set(TARGET_NAME DAO)

find_package(QuaZip REQUIRED)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

## #############################################################################
## include directories.
## #############################################################################

list_header_directories_to_include(${TARGET_NAME}
    ${${TARGET_NAME}_HEADERS}
    )

## #############################################################################
## Lib definition.
## #############################################################################

add_library(${TARGET_NAME} SHARED
    ${${TARGET_NAME}_CFILES}
    )

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME} PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${QuaZip_INCLUDE_DIRS}
    )

set(${TARGET_NAME}_INCLUDE_DIRS
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    PARENT_SCOPE
    )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Xml
    quazip
    )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME})
