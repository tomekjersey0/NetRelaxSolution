cmake_minimum_required(VERSION 3.15)
project(NetRelaxSolution)

# This sets the default to static library if you want it consistent
set(BUILD_SHARED_LIBS OFF)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add subdirectories
add_subdirectory(SocketsLibrary)
add_subdirectory(Server)
add_subdirectory(Client)