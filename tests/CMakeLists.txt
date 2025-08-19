Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)

FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE TESTS_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(${PROJECT_NAME}.tests ${TESTS_SRC})
target_include_directories(${PROJECT_NAME}.tests PRIVATE include)
target_link_libraries(${PROJECT_NAME}.tests ${PROJECT_NAME}LIB Catch2::Catch2WithMain)
