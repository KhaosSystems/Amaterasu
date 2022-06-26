cmake_minimum_required(VERSION 3.10)

# RapidXml
project(RapidXml)
add_library(RapidXml INTERFACE) 
set(RAPIDXML_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/AmaterasuDemo/Vendor/RapidXml")
target_include_directories(RapidXml INTERFACE "${RAPIDXML_DIRECTORY}")

# Khaos Systems, AmaterasuDemo
project(AmaterasuDemo)

file(GLOB_RECURSE AMATERASU_DEMO_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/AmaterasuDemo/Source/*.h")
file(GLOB_RECURSE AMATERASU_DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/AmaterasuDemo/Source/*.cpp")
add_executable(AmaterasuDemo "${AMATERASU_DEMO_HEADERS}" "${AMATERASU_DEMO_SOURCES}")

set_property(TARGET AmaterasuDemo PROPERTY CXX_STANDARD 20)

target_include_directories(AmaterasuDemo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/AmaterasuDemo/Source")
target_link_libraries(AmaterasuDemo Amaterasu RapidXml)