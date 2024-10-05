cmake_minimum_required(VERSION 3.14...3.28)
project(MySDLApp VERSION 1.0 LANGUAGES CXX)

# Include the FetchContent module
include(FetchContent)

# Fetch SDL3 from the specified Git repository and tag
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG preview-3.1.3
)

FetchContent_MakeAvailable(SDL3)

# Add the executable
add_executable(MySDLApp main.cpp)

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# Link SDL3 to your application
target_link_libraries(MySDLApp PRIVATE SDL3::SDL3)