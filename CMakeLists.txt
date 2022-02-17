cmake_minimum_required(VERSION 3.21)

project(inkyuserspace
  VERSION 1.0.0)

include(GNUInstallDirs)

add_subdirectory(lib/pimoroni-inky-driver)

##########################
# DOCUMENTATION OVERIDES #
##########################

if(NOT DEFINED INKY_BUILD_DOCS)

  set(INKY_BUILD_DOCS true)

endif()

set(DOXYGEN_OUTPUT_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/doc)

set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)

set(DOXYGEN_GENERATE_TREEVIEW YES)

set(DOXYGEN_FULL_SIDEBAR YES)

set(DOXYGEN_USE_MDFILE_AS_MAINPAGE
  ${PROJECT_SOURCE_DIR}/README.md)

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

#############################
# LIBRARY API DOCUMENTATION #
#############################

find_package(Doxygen
  REQUIRED dot
  OPTIONAL_COMPONENTS mscgen dia)

if(INKY_BUILD_DOCS AND DOXYGEN_FOUND)

  find_package(LATEX COMPONENTS PDFLATEX MAKEINDEX)

  if(NOT DEFINED INKY_BUILD_DOCS_PDF)

    set(INKY_BUILD_DOCS_PDF true)

  endif()

  if(LATEX_FOUND AND INKY_BUILD_DOCS_PDF)

    set(DOXYGEN_GENERATE_LATEX YES)

  else()

    message(NOTICE "PDF Documentation will not be build")

  endif()

  doxygen_add_docs(builddocs ALL
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate API documentation")

  # Install docs

  install(DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}/html/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/html)

  # If LATEX available, build and install a PDF as well

  if(LATEX_FOUND AND INKY_BUILD_DOCS_PDF)

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdf
      COMMAND make -C ${DOXYGEN_OUTPUT_DIRECTORY}/latex
      LATEX_CMD=${PDFLATEX_COMPILER}
      COMMAND cp ${DOXYGEN_OUTPUT_DIRECTORY}/latex/refman.pdf
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdf
      DEPENDS builddocs)

    add_custom_target(builddocs-pdf ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdf
      COMMENT "Building PDF Documentation")

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdf
      TYPE DOC)

  endif()

else()

  message(NOTICE "Documentation will not be build")

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
