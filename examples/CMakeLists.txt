
#rttr provides CMake targets:
# this is heuristically generated, and may not be correct
#find_package(rttr CONFIG REQUIRED)
#target_link_libraries(main PRIVATE RTTR::Core)
file(GLOB examplesourcelist *.cpp)

foreach(file_full_name ${examplesourcelist})
    get_filename_component(file_name ${file_full_name} NAME_WLE)
    set(TARGET_NAME "example_${file_name}")
    message("TARGET_NAME: ${TARGET_NAME}")
    add_executable(${TARGET_NAME})
    add_executable(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})
    target_sources(${TARGET_NAME} PRIVATE ${file_full_name})
    target_include_directories(${TARGET_NAME} PUBLIC examples)
    target_link_libraries(${TARGET_NAME} PRIVATE serdepp::serdepp)
    target_link_libraries(${TARGET_NAME} PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(${TARGET_NAME} PRIVATE RapidJSON)
    target_link_libraries(${TARGET_NAME} PRIVATE toml11::toml11)
    target_link_libraries(${TARGET_NAME} PRIVATE yaml-cpp::yaml-cpp)
    target_link_libraries(${TARGET_NAME} PRIVATE fmt::fmt-header-only)
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Examples")

    if(MSVC)
        #target_compile_options(${TARGET_NAME} PRIVATE "/bigobj")
    endif()

    # 将MySharedLib的DLL复制到MyApp的输出目录
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:RTTR::Core>"           # 源：DLL完整路径
        "$<TARGET_FILE_DIR:${TARGET_NAME}>"   # 目标：exe所在目录
        COMMENT "Copying RTTR::Core DLL to executable directory"
    )

    #file(GLOB source_list examples/${file_name}/*.cpp)
    #if(source_list)
    #    target_sources(${TARGET_NAME} PRIVATE ${source_list})
    #endif()

    set(TARGET_NAME "")

endforeach()
