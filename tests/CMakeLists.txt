

file(GLOB testsourcelist *.cpp)
set(TARGET_NAME unittest)
message("TARGET_NAME: ${TARGET_NAME}")
add_executable(${TARGET_NAME})
add_executable(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE ${testsourcelist})
target_include_directories(${TARGET_NAME} PUBLIC tests)
target_link_libraries(${TARGET_NAME} PRIVATE serdepp::serdepp)
target_link_libraries(${TARGET_NAME} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(${TARGET_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${TARGET_NAME} PRIVATE RapidJSON)
target_link_libraries(${TARGET_NAME} PRIVATE toml11::toml11)
target_link_libraries(${TARGET_NAME} PRIVATE yaml-cpp::yaml-cpp)
set_target_properties(${TARGET_NAME} PROPERTIES 
                        FOLDER "Serdepp/Testing")

add_test(NAME ${TARGET_NAME}_test
         COMMAND ${TARGET_NAME}
         WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>"
)

# 为测试添加标签
set_tests_properties(${TARGET_NAME}_test PROPERTIES
    LABELS "serdepp_unit;catch2"
    TIMEOUT 120
)
set(TARGET_NAME "")
