cmake_minimum_required(VERSION 3.21...3.28)
project(MySDLApp VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

# Option to use system-installed SDL3
option(USE_SYSTEM_SDL3 "Use system-installed SDL3 if available" ON)

if(USE_SYSTEM_SDL3)
  find_package(SDL3 3.0.0 QUIET)
endif()

if(NOT SDL3_FOUND)  
  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG preview-3.1.3
  )
  
  FetchContent_MakeAvailable(SDL3)
endif()

FetchContent_Declare(
  box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d.git
  GIT_TAG v3.0.0
)

FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.3
)

FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PRIVATE SDL3::SDL3)

add_executable(MySDLApp WIN32 main.cpp)

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set_property(TARGET MySDLApp PROPERTY LINK_DEPENDS ${PROJECT_SOURCE_DIR}/shell.html)
  target_link_options(MySDLApp PRIVATE "SHELL:--shell-file ${PROJECT_SOURCE_DIR}/shell.html")
  target_link_options(MySDLApp PRIVATE "SHELL:-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2")
  target_link_options(MySDLApp PRIVATE "SHELL:-sENVIRONMENT=web")
endif()

target_link_libraries(MySDLApp PRIVATE SDL3::SDL3 box2d imgui)

if(WIN32)
  # copy DLLs to output directory
  add_custom_command(TARGET MySDLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:MySDLApp>
      $<TARGET_FILE_DIR:MySDLApp>
    COMMAND_EXPAND_LISTS
  )
endif()
