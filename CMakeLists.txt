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
find_package(freetype)
find_package(STB_IMG)

include_directories( "${CMAKE_SOURCE_DIR}/include")

if(LIB_PNG_FOUND AND MINI_TEST_FOUND)

	add_executable(circle_img circle_img/main.cpp)

	target_include_directories("circle_img" PRIVATE ${LIB_PNG_INCLUDE_DIR} ${MINI_TEST_INCLUDE_DIR})
	target_link_libraries("circle_img" ${LIB_PNG_LIBRARY})


	add_executable(clip clip/main.cpp)

	target_include_directories("clip" PRIVATE ${LIB_PNG_INCLUDE_DIR} ${MINI_TEST_INCLUDE_DIR})
	target_link_libraries("clip" ${LIB_PNG_LIBRARY})

	if(STB_IMG_FOUND)
		add_executable(atlas atlas/main.cpp)

		target_include_directories("atlas" PRIVATE ${LIB_PNG_INCLUDE_DIR} ${MINI_TEST_INCLUDE_DIR} ${STB_IMG_INCLUDE_DIR})
		target_link_libraries("atlas" ${LIB_PNG_LIBRARY})
	endif()

endif()

if(FREE_TYPE_FOUND AND MINI_TEST_FOUND)

	add_executable(test_ft2 test_ft2/main.cpp src/comm.cpp)

	target_include_directories("test_ft2" PRIVATE  ${MINI_TEST_INCLUDE_DIR}  ${FREE_TYPE_INCLUDE_DIR})
	target_link_libraries("test_ft2" ${FREE_TYPE_LIBRARY})

	add_executable(ft3 ft3/main.cpp src/comm.cpp)

	target_include_directories("ft3" PRIVATE  ${MINI_TEST_INCLUDE_DIR}  ${FREE_TYPE_INCLUDE_DIR})
	target_link_libraries("ft3" ${FREE_TYPE_LIBRARY})

	add_executable(clock clock/main.cpp src/comm.cpp)

	target_include_directories("clock" PRIVATE  ${MINI_TEST_INCLUDE_DIR}  ${FREE_TYPE_INCLUDE_DIR})
	target_link_libraries("clock" ${FREE_TYPE_LIBRARY})

endif()