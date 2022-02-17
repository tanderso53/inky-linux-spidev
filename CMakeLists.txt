cmake_minimum_required(VERSION 3.21)

project(inkyuserspace
  VERSION 1.0.0)

include(GNUInstallDirs)

add_subdirectory(lib/pimoroni-inky-driver)

##########################
# DOCUMENTATION OVERIDES #
##########################

if(NOT DEFINED INKY_BUILD_DOCS)

  set(INKY_BUILD_DOCS false)

endif()

set(DOXYGEN_GENERATE_LATEX YES)

set(DOXYGEN_OUTPUT_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/doc)

set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)

set(DOXYGEN_GENERATE_TREEVIEW YES)

set(DOXYGEN_FULL_SIDEBAR YES)

#######################################
# INKY LINUX SPIDEV LIBRARY INTERFACE #
#######################################

# Build Static library

add_library(inkyuserspace-static STATIC)

target_sources(inkyuserspace-static PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/inky-spidev.c)

target_include_directories(inkyuserspace-static PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(inkyuserspace-static PUBLIC
  pimoroni-inky-driver)

set_target_properties(inkyuserspace-static PROPERTIES
  PUBLIC_HEADER ${CMAKE_CURRENT_LIST_DIR}/include/inky-spidev.h
  OUTPUT_NAME ${PROJECT_NAME})

# Build Dynamic Library

add_library(inkyuserspace-shared SHARED)

target_sources(inkyuserspace-shared PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/inky-spidev.c)

target_include_directories(inkyuserspace-shared PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(inkyuserspace-shared PUBLIC
  pimoroni-inky-driver)

set_target_properties(inkyuserspace-shared PROPERTIES
  OUTPUT_NAME ${PROJECT_NAME})

# Generate documentation if doxygen was found

if(INKY_BUILD_DOCS)

  find_package(Doxygen
    REQUIRED dot
    OPTIONAL_COMPONENTS mscgen dia)

  doxygen_add_docs(builddocs ALL
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate API documentation")

  # Install docs

  install(DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}/html/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/html)

endif()

########################
# LIBRARY INSTALLATION #
########################

# Install header filies

install(TARGETS inkyuserspace-static
  PUBLIC_HEADER
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  PRIVATE_HEADER
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  ARCHIVE)

install(TARGETS inkyuserspace-shared
  LIBRARY)

install(TARGETS pimoroni-inky-driver
  PUBLIC_HEADER
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  PRIVATE_HEADER
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Install share files

install(FILES LICENSE
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
