cmake_minimum_required(VERSION 3.10)

# glad
project(glad)
set(GLAD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glad")
add_library(glad "${GLAD_DIRECTORY}/include/glad/glad.h" "${GLAD_DIRECTORY}/include/KHR/khrplatform.h" "${GLAD_DIRECTORY}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIRECTORY}/include")

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glfw")

# imgui
project(imgui)
set(IMGUI_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui")
file(GLOB IMGUI_HEADERS "${IMGUI_DIRECTORY}/*.h")
file(GLOB IMGUI_SOURCES "${IMGUI_DIRECTORY}/*.cpp")
add_library(imgui "${IMGUI_HEADERS}" "${IMGUI_SOURCES}" "${IMGUI_DIRECTORY}/backends/imgui_impl_glfw.cpp" "${IMGUI_DIRECTORY}/backends/imgui_impl_opengl3.cpp")

target_include_directories(imgui PUBLIC "${IMGUI_DIRECTORY}" "${IMGUI_DIRECTORY}/examples")
target_link_libraries(imgui glfw)

# Khaos Systems, Amaterasu
project(Amaterasu)

file(GLOB_RECURSE AMATERASU_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")
file(GLOB_RECURSE AMATERASU_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
add_library(Amaterasu STATIC "${AMATERASU_HEADERS}" "${AMATERASU_SOURCES}")

set_property(TARGET Amaterasu PROPERTY CXX_STANDARD 20)

target_include_directories(Amaterasu PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_link_libraries(Amaterasu glad glfw imgui opengl32 Dwmapi)
