cmake_minimum_required(VERSION 3.18.4)

# set(WITH_QE_SHVAPI OFF CACHE BOOL "Build SHV API service with QuickEvent")

project(startertool LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_SHARED_LIBRARY_PREFIX "") # we don't want CMake to prepend "lib" to our libraries, we prefer adding that ourselves

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic ${CMAKE_CXX_FLAGS}")
endif()

if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_WIN32_EXECUTABLE ON) # Don't open a console window for Windows apps on Relase mode.
	endif()
endif (WIN32)

set(USE_QT6 ON)
set(LIBSHV_WITH_BROKER OFF CACHE BOOL "")
set(LIBSHV_WITH_VISU OFF CACHE BOOL "")
set(BUILD_TESTING OFF CACHE BOOL "")
set(BUILD_SHV_UTILS OFF CACHE BOOL "")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Svg SerialPort Network LinguistTools)

if(Qt6_FOUND)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)

	add_subdirectory(3rdparty/necrolog)
	# add_subdirectory(3rdparty/quickbox/libqf/libqfcore)
	# add_subdirectory(3rdparty/quickbox/libquickevent)
	# add_subdirectory(3rdparty/quickbox/libsiut)
	add_subdirectory(3rdparty/libshv)
else()
	message(FATAL_ERROR "Qt not found")
endif()


add_executable(startertool
    src/application.cpp
    src/appclioptions.cpp
    src/main.cpp
    src/mainwindow.cpp
    startertool.qrc
    )

target_link_libraries(startertool PUBLIC Qt::Widgets libshviotqt)

install(TARGETS startertool)