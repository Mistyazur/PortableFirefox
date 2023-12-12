cmake_minimum_required(VERSION 3.15.0)

add_definitions(
  -DUNICODE
  -D_UNICODE
)

message("================================ Import vcpkg ================================")
set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_PREFIX_PATH ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})

if (${CMAKE_GENERATOR_PLATFORM} STREQUAL "x64")
  set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
elseif (${CMAKE_GENERATOR_PLATFORM} STREQUAL "win32")
  set(VCPKG_TARGET_TRIPLET "x86-windows-static" CACHE STRING "")
endif()
message("================================ ${VCPKG_TARGET_TRIPLET} ================================")


project(PortableFirefox VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS filesystem json)

include_directories(${Boost_INCLUDE_DIRS})

aux_source_directory(. DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS} app.rc)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${Boost_LIBRARIES}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

