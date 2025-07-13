add_executable(ServerApp)

# Source files
file(GLOB_RECURSE SERVER_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# App’s own headers
file(GLOB_RECURSE SERVER_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

target_sources(ServerApp PRIVATE ${SERVER_SRCS} ${SERVER_HDRS})

# Add the app’s own include dir (PRIVATE) — so the app can see its own headers
target_include_directories(ServerApp
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link to the sockets library, which already exposes its API via PUBLIC
target_link_libraries(ServerApp PRIVATE SocketsLibrary)

# VS source groups (nice to keep for IDE structure)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SERVER_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${SERVER_HDRS})
