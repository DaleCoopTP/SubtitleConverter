cmake_minimum_required(VERSION 3.29)
project(se_cpp_prog_subtitles_DaleCoopTP)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(se_cpp_prog_subtitles_DaleCoopTP
        src/main.cpp
        include/SAMI.h
        include/SRT.h
        include/SSA.h
        include/Structures.h
        include/SubtitleFactory.h
        src/SAMI.cpp
        src/SRT.cpp
        src/SSA.cpp
        include/DynamicArray.h
        include/TTML.h
        src/TTML.cpp
        src/SRT.cpp
)

add_executable(unit_tests
        test/tests.cpp
        include/SAMI.h
        include/SRT.h
        include/SSA.h
        include/TTML.h
        include/Structures.h
        include/SubtitleFactory.h
        include/DynamicArray.h
        src/SAMI.cpp
        src/SRT.cpp
        src/SSA.cpp
        src/TTML.cpp
)


target_link_libraries(unit_tests
        gtest
        gtest_main
        pthread
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
