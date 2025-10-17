add_executable(${PROJECT_NAME}.sample ${CMAKE_SOURCE_DIR}/samples/sample.cpp)
target_link_libraries(${PROJECT_NAME}.sample ${PROJECT_NAME}LIB)
target_include_directories(${PROJECT_NAME}.sample PRIVATE ${CMAKE_SOURCE_DIR}/include)

include(FetchContent)
