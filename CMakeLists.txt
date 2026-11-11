cmake_minimum_required(VERSION 3.16)
project(StudentRecordManagementSystem VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_executable(StudentRecordManager src/main.cpp)
target_link_libraries(StudentRecordManager PRIVATE Qt6::Widgets)
if(MSVC)
  target_compile_options(StudentRecordManager PRIVATE /W4)
else()
  target_compile_options(StudentRecordManager PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_custom_command(TARGET StudentRecordManager POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${CMAKE_CURRENT_SOURCE_DIR}/src/students.csv"
  "$<TARGET_FILE_DIR:StudentRecordManager>/students.csv")
