cmake_minimum_required(VERSION 3.10)

# RapidXml
project(RapidXml)
add_library(RapidXml INTERFACE) 
set(RAPIDXML_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/AmaterasuDemo/Vendor/RapidXml")
target_include_directories(RapidXml INTERFACE "${RAPIDXML_DIRECTORY}")
