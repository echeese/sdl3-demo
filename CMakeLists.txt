cmake_minimum_required(VERSION 3.21...3.28)
project(MySDLApp VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG preview-3.1.3
)

FetchContent_MakeAvailable(SDL3)

add_executable(MySDLApp WIN32 main.cpp)

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_link_options(MySDLApp PRIVATE "SHELL:--shell-file ${PROJECT_SOURCE_DIR}/shell.html")
endif()

target_link_libraries(MySDLApp PRIVATE SDL3::SDL3)

if(WIN32)
  # copy DLLs to output directory
  add_custom_command(TARGET MySDLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:MySDLApp>
      $<TARGET_FILE_DIR:MySDLApp>
    COMMAND_EXPAND_LISTS
  )
endif()
