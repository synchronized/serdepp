file(GLOB benchmarksourcelist *.cpp)

foreach(file_full_name ${benchmarksourcelist})
    get_filename_component(file_name ${file_full_name} NAME_WLE)
    set(TARGET_NAME "bench_${file_name}")
    message("TARGET_NAME: ${TARGET_NAME}")

    add_executable(${TARGET_NAME})
    add_executable(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})
    target_sources(${TARGET_NAME} PRIVATE ${file_full_name})
    target_include_directories(${TARGET_NAME} PRIVATE benchmark)
    target_link_libraries(${TARGET_NAME} PRIVATE serdepp::serdepp)
    target_link_libraries(${TARGET_NAME} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
    target_link_libraries(${TARGET_NAME} PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(${TARGET_NAME} PRIVATE RapidJSON)
    target_link_libraries(${TARGET_NAME} PRIVATE toml11::toml11)
    target_link_libraries(${TARGET_NAME} PRIVATE yaml-cpp::yaml-cpp)
    set_target_properties(${TARGET_NAME} PROPERTIES 
                            FOLDER "Serdepp/benchmark")
    set(TARGET_NAME "")

endforeach()

