include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY
      https://v4.gh-proxy.org/https://github.com/google/googletest.git
    GIT_TAG
      v1.18.0
)
FetchContent_MakeAvailable(googletest)

add_executable(tests test_main.cpp)
target_link_libraries(tests PRIVATE GTest::gtest_main)
target_link_libraries(tests PRIVATE bgl::core)

include(GoogleTest)
gtest_discover_tests(tests)
