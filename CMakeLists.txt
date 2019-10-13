cmake_minimum_required(VERSION 3.2.0)

project(create-img)


if(MSVC)

add_definitions(/std:c++17 /permissive-)

else()

add_definitions(-std=c++17)

endif(MSVC)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(libpng)
find_package(MINI_TEST)

include_directories( ${CMAKE_SOURCE_DIR})

if(LIB_PNG_FOUND AND MINI_TEST_FOUND)

add_executable(circle_img circle_img/main.cpp)

target_include_directories("circle_img" PRIVATE ${LIB_PNG_INCLUDE_DIR} ${MINI_TEST_INCLUDE_DIR})
target_link_libraries("circle_img" ${LIB_PNG_LIBRARY})


add_executable(clip clip/main.cpp)

target_include_directories("clip" PRIVATE ${LIB_PNG_INCLUDE_DIR} ${MINI_TEST_INCLUDE_DIR})
target_link_libraries("clip" ${LIB_PNG_LIBRARY})

endif()