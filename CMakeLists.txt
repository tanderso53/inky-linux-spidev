cmake_minimum_required(VERSION 3.21)

project(inkyuserspace
  VERSION 1.0.0)

include(GNUInstallDirs)

add_subdirectory(lib/pimoroni-inky-driver)

string(CONCAT LIB_STATIC_TGT
  ${PROJECT_NAME} "-static")
string(CONCAT LIB_SHARED_TGT
  ${PROJECT_NAME} "-shared")

string(CONCAT LIB_STATIC_OUTPUT
  ${CMAKE_STATIC_LIBRARY_PREFIX}
  ${PROJECT_NAME}
  ${CMAKE_STATIC_LIBRARY_SUFFIX})

string(CONCAT LIB_SHARED_OUTPUT
  ${CMAKE_SHARED_LIBRARY_PREFIX}
  ${PROJECT_NAME}
  ${CMAKE_SHARED_LIBRARY_SUFFIX})

##########################
# DOCUMENTATION OVERIDES #
##########################

set(INKY_BUILD_DOCS false)

set(DOXYGEN_GENERATE_LATEX YES)

set(DOXYGEN_OUTPUT_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/doc)

#######################################
# INKY LINUX SPIDEV LIBRARY INTERFACE #
#######################################

# Build Static library

add_library(inkyuserspace-static STATIC)

target_sources(inkyuserspace-static PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/inky-spidev.c)

target_include_directories(inkyuserspace-static PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(inkyuserspace-static PUBLIC
  pimoroni-inky-driver)

set_target_properties(inkyuserspace-static PROPERTIES
  PUBLIC_HEADER ${CMAKE_CURRENT_LIST_DIR}/include/inky-spidev.h)

# Build Dynamic Library

add_library(inkyuserspace-shared SHARED)

target_sources(inkyuserspace-shared PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/inky-spidev.c)

target_include_directories(inkyuserspace-shared PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(inkyuserspace-shared PUBLIC
  pimoroni-inky-driver)

# Generate documentation if doxygen was found

find_package(Doxygen
  REQUIRED dot
  OPTIONAL_COMPONENTS mscgen dia)

if(DOXYGEN_FOUND AND INKY_BUILD_DOCS)

  doxygen_add_docs(doxygen
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate API documentation")

endif()

########################
# LIBRARY INSTALLATION #
########################

install(TARGETS inkyuserspace-static
  PUBLIC_HEADER
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  PRIVATE_HEADER
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  ARCHIVE)

install(TARGETS inkyuserspace-shared
  LIBRARY)
