set(TARGET_NAME shanoirqr)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_MODULE_PATH}
    )
find_package(docopt REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(QuaZip REQUIRED)


## #############################################################################
## List Sources.
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
## exe definition.
## #############################################################################

add_executable(${TARGET_NAME}
    ${${TARGET_NAME}_CFILES}
    )

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME} PUBLIC
    ${${TARGET_NAME}_INCLUDE_DIRS}
    ${docopt_INCLUDE_DIRS}
    ${yaml-cpp_INCLUDE_DIRS}
    )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Xml
    QtShanoirDAO
    docopt
    yaml-cpp
    )

## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME})
