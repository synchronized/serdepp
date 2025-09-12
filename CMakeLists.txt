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


find_package(magic_enum REQUIRED)
find_package(nameof REQUIRED)
find_package(Catch2 REQUIRED)
find_package(benchmark REQUIRED)
find_package(RapidJSON)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json)
find_package(toml11)
find_package(yaml-cpp)

add_subdirectory(3rd)

option(SERDEPP_BUILD_TESTING "Build the unit tests of serdepp" OFF)
option(SERDEPP_BUILD_EXAMPLES "Build the examples of serdepp" OFF)
option(SERDEPP_BUILD_BENCHMARKS "Enable this to build the benchmarks" OFF)

if (PROJECT_IS_TOP_LEVEL)
    set(SERDEPP_BUILD_TESTING ON)
    set(SERDEPP_BUILD_EXAMPLES ON)
    set(SERDEPP_BUILD_BENCHMARKS ON)
endif()

set(target_name serdepp)
message("target_name: ${target_name}")
add_library(${target_name} STATIC)
add_library(${PROJECT_NAME}::${target_name} ALIAS ${target_name})
target_include_directories(${target_name} PUBLIC include)
target_sources(${target_name} PRIVATE src/to_static.cpp)
target_link_libraries(${target_name} PRIVATE magic_enum::magic_enum)
target_link_libraries(${target_name} PRIVATE nameof::nameof)
target_link_libraries(${target_name} PUBLIC fmt::fmt-header-only)
set(target_name "")



if(SERDEPP_BUILD_TESTING)

    file(GLOB testsourcelist tests/*.cpp)
    set(target_name unittest)
    message("target_name: ${target_name}")
    add_executable(${target_name})
    add_executable(${PROJECT_NAME}::${target_name} ALIAS ${target_name})
    target_sources(${target_name} PRIVATE ${testsourcelist})
    target_include_directories(${target_name} PUBLIC tests)
    target_link_libraries(${target_name} PRIVATE serdepp)
    target_link_libraries(${target_name} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
    target_link_libraries(${target_name} PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(${target_name} PRIVATE RapidJSON rapidjson)
    target_link_libraries(${target_name} PRIVATE toml11::toml11)
    target_link_libraries(${target_name} PRIVATE yaml-cpp::yaml-cpp)
    set(target_name "")

endif()

if(SERDEPP_BUILD_EXAMPLES)

    #rttr provides CMake targets:
    # this is heuristically generated, and may not be correct
    #find_package(rttr CONFIG REQUIRED)
    #target_link_libraries(main PRIVATE RTTR::Core)
    file(GLOB examplesourcelist examples/*.cpp)

    foreach(file_full_name ${examplesourcelist})
        get_filename_component(file_name ${file_full_name} NAME_WLE)
        set(target_name "example_${file_name}")
        message("target_name: ${target_name}")
        add_executable(${target_name})
        add_executable(${PROJECT_NAME}::${target_name} ALIAS ${target_name})
        target_sources(${target_name} PRIVATE ${file_full_name})
        target_include_directories(${target_name} PUBLIC examples)
        target_link_libraries(${target_name} PRIVATE serdepp)
        target_link_libraries(${target_name} PRIVATE nlohmann_json::nlohmann_json)
        target_link_libraries(${target_name} PRIVATE RapidJSON rapidjson)
        target_link_libraries(${target_name} PRIVATE toml11::toml11)
        target_link_libraries(${target_name} PRIVATE yaml-cpp::yaml-cpp)
        target_link_libraries(${target_name} PRIVATE fmt::fmt)
        target_link_libraries(${target_name} PRIVATE RTTR::Core)
        if(MSVC)
            target_compile_options(${target_name} PRIVATE "/bigobj")
        endif()

        # 将MySharedLib的DLL复制到MyApp的输出目录
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:RTTR::Core>"           # 源：DLL完整路径
            "$<TARGET_FILE_DIR:${target_name}>"   # 目标：exe所在目录
            COMMENT "Copying RTTR::Core DLL to executable directory"
        )

        #file(GLOB source_list examples/${file_name}/*.cpp)
        #if(source_list)
        #    target_sources(${target_name} PRIVATE ${source_list})
        #endif()

        set(target_name "")

    endforeach()

endif()

if(SERDEPP_BUILD_BENCHMARKS)

    file(GLOB benchmarksourcelist benchmark/*.cpp)

    foreach(file_full_name ${benchmarksourcelist})
        get_filename_component(file_name ${file_full_name} NAME_WLE)
        set(target_name "bench_${file_name}")
        message("target_name: ${target_name}")

        add_executable(${target_name})
        add_executable(${PROJECT_NAME}::${target_name} ALIAS ${target_name})
        target_sources(${target_name} PRIVATE ${file_full_name})
        target_include_directories(${target_name} PRIVATE benchmark)
        target_link_libraries(${target_name} PRIVATE serdepp)
        #target_link_libraries(${target_name} PRIVATE benchmark::benchmark benchmark::benchmark_main)
        target_link_libraries(${target_name} PRIVATE benchmark::benchmark)
        target_link_libraries(${target_name} PRIVATE nlohmann_json::nlohmann_json)
        target_link_libraries(${target_name} PRIVATE RapidJSON rapidjson)
        target_link_libraries(${target_name} PRIVATE toml11::toml11)
        target_link_libraries(${target_name} PRIVATE yaml-cpp::yaml-cpp)
        set(target_name "")

    endforeach()

endif()
