add_subdirectory(rttr)

set(SERDEPP_3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT TARGET Catch2::Catch2)
    set(CATCH_INCLUDE_DIR ${SERDEPP_3RD_PARTY_DIR}/rttr/3rd_party/catch-v3.11.0)
    add_library(Catch2 INTERFACE)
    add_library(Catch2::Catch2 ALIAS Catch2)
    set_target_properties(Catch2 PROPERTIES
        FOLDER "Serdepp/3rdParty/Catch2"
        )
    target_include_directories(Catch2 INTERFACE ${CATCH_INCLUDE_DIR})
    add_library(Catch2WithMain STATIC ${CATCH_INCLUDE_DIR}/catch2/catch_amalgamated.cpp)
    add_library(Catch2::Catch2WithMain ALIAS Catch2WithMain)
    set_target_properties(Catch2WithMain PROPERTIES
        FOLDER "Serdepp/3rdParty/Catch2"
        )
endif()

if(NOT TARGET magic_enum::magic_enum)
    set(MAGIC_ENUM_ROOT_DIR "${SERDEPP_3RD_PARTY_DIR}/magic_enum-v0.9.7")
    add_library(magic_enum INTERFACE)
    add_library(magic_enum::magic_enum ALIAS magic_enum)
    target_include_directories(magic_enum INTERFACE ${MAGIC_ENUM_ROOT_DIR}/include)
endif()

if(NOT TARGET fmt::fmt-header-only)
    set(FMT_ROOT_DIR "${SERDEPP_3RD_PARTY_DIR}/fmt-v12.1.0")
    add_library(fmt-header-only INTERFACE)
    add_library(fmt::fmt-header-only ALIAS fmt-header-only)
    target_include_directories(fmt-header-only INTERFACE ${FMT_ROOT_DIR}/include)

    if (NOT MSVC)
    # Unicode is always supported on compilers other than MSVC.
    elseif (FMT_UNICODE)
        # Unicode support requires compiling with /utf-8.
        target_compile_options(fmt-header-only INTERFACE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/utf-8>)
    endif ()

    target_compile_definitions(fmt-header-only INTERFACE FMT_HEADER_ONLY=1)
    target_compile_features(fmt-header-only INTERFACE cxx_std_11)
endif()

if(NOT TARGET nameof::nameof)
    set(NAMEOF_ROOT_DIR "${SERDEPP_3RD_PARTY_DIR}/nameof-v0.10.4")
    add_library(nameof INTERFACE)
    add_library(nameof::nameof ALIAS nameof)
    target_include_directories(nameof INTERFACE ${NAMEOF_ROOT_DIR}/include)
endif()

if(NOT TARGET nlohmann_json::nlohmann_json)
    set(NLOHMANN_JSON_ROOT_DIR "${SERDEPP_3RD_PARTY_DIR}/nlohmann_json-v3.12.0")
    add_library(nlohmann_json INTERFACE)
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
    target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_ROOT_DIR}/include)
endif()

if(NOT TARGET RapidJSON::RapidJSON)
    set(RAPIDJSON_ROOT_DIR "${SERDEPP_3RD_PARTY_DIR}/rapidjson-v1.1.0")
    add_library(RapidJSON INTERFACE)
    add_library(RapidJSON::RapidJSON ALIAS RapidJSON)
    target_include_directories(RapidJSON INTERFACE ${RAPIDJSON_ROOT_DIR}/include)
endif()

if(NOT TARGET toml11::toml11)
    set(TOML11_ROOT_DIR "${SERDEPP_3RD_PARTY_DIR}/toml11-v4.4.0")
    add_library(toml11 INTERFACE)
    add_library(toml11::toml11 ALIAS toml11)
    target_include_directories(toml11 INTERFACE ${TOML11_ROOT_DIR})
endif()

if(NOT TARGET yaml-cpp::yaml-cpp)
    set(YAML_CPP_ROOT_DIR "${SERDEPP_3RD_PARTY_DIR}/yaml-cpp-v0.8.0")
    #add_subdirectory(${YAML_CPP_ROOT_DIR})

    set(contrib-pattern "${YAML_CPP_ROOT_DIR}/src/contrib/*.cpp")
    set(src-pattern "${YAML_CPP_ROOT_DIR}/src/*.cpp")
    set(YAML_CPP_STATIC_DEFINE true)
    if (CMAKE_VERSION VERSION_GREATER 3.12)
        list(INSERT contrib-pattern 0 CONFIGURE_DEPENDS)
        list(INSERT src-pattern 0 CONFIGURE_DEPENDS)
    endif()

    file(GLOB yaml-cpp-contrib-sources ${contrib-pattern})
    file(GLOB yaml-cpp-sources ${src-pattern})

    add_library(yaml-cpp STATIC)
    add_library(yaml-cpp::yaml-cpp ALIAS yaml-cpp)
    target_include_directories(yaml-cpp
        PUBLIC ${YAML_CPP_ROOT_DIR}/include
        PRIVATE ${YAML_CPP_ROOT_DIR}/src
    )
    target_sources(yaml-cpp PRIVATE 
        $<$<BOOL:${YAML_CPP_BUILD_CONTRIB}>:${yaml-cpp-contrib-sources}>)
    target_sources(yaml-cpp PRIVATE 
        ${yaml-cpp-sources})
    set_target_properties(yaml-cpp
        PROPERTIES
        CXX_STANDARD 11)
    target_compile_definitions(yaml-cpp PUBLIC YAML_CPP_STATIC_DEFINE)
    set_target_properties(yaml-cpp PROPERTIES
        FOLDER "Serdepp/3rdParty/yaml-cpp"
        )
endif()
