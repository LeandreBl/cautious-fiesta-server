cmake_minimum_required (VERSION 3.7)

project("cautious-server" CXX)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE SRCS "src/*.cpp")

set(HDRS_DIRS "include/common" "include/tcp-server" "include/ingame")

set(CMAKE_CXX_FLAGS_DEBUG "-O3 -g3 -Wall -Wextra -Weffc++ -Wshadow -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wsign-conversion -Wlogical-op -Wuseless-cast -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -g3 -Wextra -O3 -fsanitize=address")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

add_subdirectory(cautious-fiesta-common)

include_directories(
	${Boost_INCLUDE_DIR}
	"cautious-fiesta-common/include"
	"cautious-fiesta-common/sfml-scene/include"
	"cautious-fiesta-common/discord-rpc/include"
)

link_directories(${Boost_LIBRARY_DIR})
add_executable("cautious-server" ${SRCS})
target_include_directories("cautious-server" PRIVATE ${HDRS_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries("cautious-server" ${CONAN_LIBS} ${Boost_LIBRARIES} cautious-fiesta-common asan stdc++fs sfml-scene discord-rpc sfml-graphics sfml-system sfml-network sfml-audio stdc++fs)
