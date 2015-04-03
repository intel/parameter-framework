SET(CTEST_CUSTOM_MEMCHECK_IGNORE
  ${CTEST_CUSTOM_MEMCHECK_IGNORE}
  # Python generates too many valgrind errors,
  # runing python based tests would be long and useless.
  fix_point_parameter
  functional-test
  )
