# minimum cmake version supported
cmake_minimum_required (VERSION 2.6)

# monitor project
project ([{PROJECT_NAME}])

# c++ compilation flags used everywhere
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O2")

# local headers for parser and debugger
include_directories("${CMAKE_CURRENT_LIST_DIR}/parselib" "${CMAKE_CURRENT_LIST_DIR}")

# The sources to be used
file(GLOB SOURCE_FILES "*.cpp")

# Output
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/bin")

# Define the executable to be generated
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Add the project static libs at linking
set(THE_LIBS ${BASE_LIBS} token wallet "${CMAKE_CURRENT_LIST_DIR}/parselib/libparse[{PROJECT_NAME}].a")
target_link_libraries (${PROJECT_NAME} ${THE_LIBS} ncurses)
target_link_libraries(${TOOL_NAME} ${CURL_LIBRARIES})
