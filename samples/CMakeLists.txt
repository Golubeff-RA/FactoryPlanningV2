add_executable(${PROJECT_NAME}.sample ${CMAKE_SOURCE_DIR}/samples/sample.cpp)
target_link_libraries(${PROJECT_NAME}.sample ${PROJECT_NAME}LIB)
target_include_directories(${PROJECT_NAME}.sample PRIVATE ${CMAKE_SOURCE_DIR}/include)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann_json)
add_executable(Visualisation visual.cpp)
target_link_libraries(Visualisation PRIVATE nlohmann_json::nlohmann_json)