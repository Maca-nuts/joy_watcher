cmake_minimum_required(VERSION 3.8)
project(joy_watcher)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(joy_watcher_node src/joy_watcher_node.cpp)
target_compile_features(joy_watcher_node PUBLIC cxx_std_17)
ament_target_dependencies(joy_watcher_node rclcpp sensor_msgs)
target_link_libraries(joy_watcher_node Qt5::Widgets)

install(TARGETS joy_watcher_node DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
