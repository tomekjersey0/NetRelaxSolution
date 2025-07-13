add_library(SocketsLibrary STATIC)

# Gather all source files
file(GLOB_RECURSE SOCKETLIB_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Gather all header files
file(GLOB_RECURSE SOCKETLIB_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Add them to the target
target_sources(SocketsLibrary PRIVATE ${SOCKETLIB_SRCS} ${SOCKETLIB_HDRS})

# Include dirs
target_include_directories(SocketsLibrary PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Organize in VS folders properly
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOCKETLIB_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${SOCKETLIB_HDRS})
