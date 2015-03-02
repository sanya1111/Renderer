set(dependdi test.cpp)
add_executable(main_test ${dependdi})
add_dependencies(main_test result)
target_link_libraries(main_test ${gtkmm-3.0_LIBRARIES} result )
