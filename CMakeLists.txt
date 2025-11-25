cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
if(MSVC)
    add_compile_options(/bigobj)
endif()

project(serdepp VERSION 0.1.4 LANGUAGES C CXX)

#find_cppkg(Catch2 3.5.0  MODULE Catch2::Catch2WithMain TYPE lib OPTIONAL OFF)
#find_cppkg(RapidJSON 1.1.1  MODULE rapidjson TYPE lib OPTIONAL OFF)
#find_cppkg(benchmark 1.5.2  MODULE benchmark::benchmark TYPE lib OPTIONAL OFF)
#find_cppkg(fmt 10.2.0  MODULE fmt::fmt-header-only TYPE lib OPTIONAL OFF)
#find_cppkg(magic_enum 0.9.5  MODULE magic_enum::magic_enum TYPE lib)
#find_cppkg(nameof 0.10.3  MODULE nameof::nameof TYPE lib)
#find_cppkg(nlohmann_json 3.11.3  MODULE nlohmann_json::nlohmann_json TYPE lib OPTIONAL OFF)
#find_cppkg(toml11 3.7.1  MODULE toml11::toml11 TYPE lib OPTIONAL OFF)
#find_cppkg(yaml-cpp 0.6.3  MODULE yaml-cpp TYPE lib OPTIONAL OFF)


add_subdirectory(3rd)

option(SERDEPP_BUILD_TESTING "Build the unit tests of serdepp" OFF)
option(SERDEPP_BUILD_EXAMPLES "Build the examples of serdepp" OFF)
option(SERDEPP_BUILD_BENCHMARKS "Enable this to build the benchmarks" OFF)

if (PROJECT_IS_TOP_LEVEL)
    set(SERDEPP_BUILD_TESTING ON)
    set(SERDEPP_BUILD_EXAMPLES ON)
    set(SERDEPP_BUILD_BENCHMARKS ON)
endif()

set(TARGET_NAME serdepp)
message("TARGET_NAME: ${TARGET_NAME}")
add_library(${TARGET_NAME} STATIC)
add_library(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})
target_include_directories(${TARGET_NAME} PUBLIC include)
target_sources(${TARGET_NAME} PRIVATE src/to_static.cpp)
target_link_libraries(${TARGET_NAME} PUBLIC magic_enum::magic_enum)
target_link_libraries(${TARGET_NAME} PUBLIC nameof::nameof)
target_link_libraries(${TARGET_NAME} PUBLIC fmt::fmt-header-only)
target_link_libraries(${TARGET_NAME} PUBLIC RTTR::Core)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Core")
set(TARGET_NAME "")



if(SERDEPP_BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(SERDEPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SERDEPP_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
