cmake_minimum_required(VERSION 3.10)

# Project
project(SpeedTest VERSION 1.0)

# Pthread flag
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find Thread (could be pthread or win-thread)
find_package(Threads REQUIRED)
link_libraries(
    Threads::Threads
    )

# Include header
include_directories(hello_client PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
# Executable
add_executable(client_side ${CMAKE_CURRENT_SOURCE_DIR}/client_side.cpp)
add_executable(server_side ${CMAKE_CURRENT_SOURCE_DIR}/server_side.cpp)
# Additional flags
add_compile_options(
  -Wall -Wextra
  )