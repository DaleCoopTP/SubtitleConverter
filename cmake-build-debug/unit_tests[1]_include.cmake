if(EXISTS "/Users/lucifer/Desktop/SubtitleConverter/cmake-build-debug/unit_tests[1]_tests.cmake")
  include("/Users/lucifer/Desktop/SubtitleConverter/cmake-build-debug/unit_tests[1]_tests.cmake")
else()
  add_test(unit_tests_NOT_BUILT unit_tests_NOT_BUILT)
endif()
