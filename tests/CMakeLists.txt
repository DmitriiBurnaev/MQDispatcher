add_executable(mqdispatcher-tests mqdispatcher_tests.cpp)
target_link_libraries(mqdispatcher-tests
    Boost::unit_test_framework
    MQDispatcher
)
target_compile_definitions(mqdispatcher-tests
    PRIVATE
      BOOST_TEST_DYN_LINK
  )
add_test(mqdispatcher-tests mqdispatcher-tests)

add_executable(mqdispatcher-stress-tests mqdispatcher_stress_tests.cpp)
target_link_libraries(mqdispatcher-stress-tests
    MQDispatcher
)
add_test(mqdispatcher-stress-tests mqdispatcher-stress-tests)