# Create executable target
add_executable(ClientApp)

# Grab all source files and headers
file(GLOB_RECURSE CLIENT_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE CLIENT_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Add to target
target_sources(ClientApp PRIVATE ${CLIENT_SRCS} ${CLIENT_HDRS})

# Include this project's include folder
target_include_directories(ClientApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link against your static library
target_link_libraries(ClientApp PRIVATE SocketsLibrary)

# Organize Visual Studio solution explorer
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${CLIENT_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${CLIENT_HDRS})
