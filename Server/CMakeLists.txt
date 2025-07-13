add_executable(ServerApp)

# Separate header and source files
file(GLOB_RECURSE SERVER_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE SERVER_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

target_sources(ServerApp PRIVATE ${SERVER_SRCS} ${SERVER_HDRS})

# Include directory
target_include_directories(ServerApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link libraries
target_link_libraries(ServerApp PRIVATE SocketsLibrary)

# Source groups for VS
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SERVER_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${SERVER_HDRS})
