cmake_minimum_required(VERSION 3.15.0)

### Import vcpkg start
if (${CMAKE_GENERATOR_PLATFORM} STREQUAL "x64")
  set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
elseif (${CMAKE_GENERATOR_PLATFORM} STREQUAL "win32")
  set(VCPKG_TARGET_TRIPLET "x86-windows-static" CACHE STRING "")
endif()

include("E:/vcpkg/scripts/buildsystems/vcpkg.cmake")
# set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "") # This will not work in subdirectory
### Import vcpkg end

# C++17
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Unicode
add_definitions(
  -DUNICODE
  -D_UNICODE
)

project(PortableFirefox VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(RapidJSON REQUIRED CONFIG)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${RAPIDJSON_INCLUDE_DIRS})

aux_source_directory(. DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS} app.rc)

target_link_libraries(${PROJECT_NAME} PRIVATE
  # ${Boost_LIBRARIES}
  Boost::filesystem
  rapidjson
)

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

