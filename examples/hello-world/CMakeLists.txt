cmake_minimum_required(VERSION 3.21)

project(inky-hello-world
  VERSION 1.0.0
  LANGUAGES C)

add_executable(inky-hello-world
  ${CMAKE_CURRENT_LIST_DIR}/src/hello-world.c)

target_link_libraries(inky-hello-world PRIVATE
  inkyuserspace-static)

if(DEFINED INKY_SPIDEV_AS_SUBMODULE)

  target_compile_definitions(inky-hello-world PRIVATE
    INKY_SPIDEV_AS_SUBMODULE=1)

endif()
