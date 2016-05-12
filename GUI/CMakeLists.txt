set (TARGET_NAME QtShanoirGUI)

find_package(QuaZip REQUIRED)

## #############################################################################
## List Sources.
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    Resources
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

add_library(${TARGET_NAME}
    ${${TARGET_NAME}_CFILES}
    ${${TARGET_NAME}_QRC}
    ${${TARGET_NAME}_UI}
    )

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME} PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
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
    QtShanoirDAO
    )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME})
