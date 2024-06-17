cmake_minimum_required(VERSION 3.5)

project(startertool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_WIN32_EXECUTABLE ON) # Don't open a console window for Windows apps on Relase mode.
	endif()
endif (WIN32)

set(USE_QT6 ON)
set(LIBSHV_WITH_WEBSOCKETS OFF CACHE BOOL "")
set(LIBSHV_WITH_BROKER OFF CACHE BOOL "")
set(LIBSHV_WITH_VISU OFF CACHE BOOL "")
set(BUILD_TESTING OFF CACHE BOOL "")
set(BUILD_SHV_UTILS OFF CACHE BOOL "")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets )
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Svg SerialPort Network Multimedia LinguistTools)

set(USE_QT6 ON)
set(LIBSHV_WITH_WEBSOCKETS OFF CACHE BOOL "")
set(LIBSHV_WITH_BROKER OFF CACHE BOOL "")
set(LIBSHV_WITH_VISU OFF CACHE BOOL "")
set(BUILD_TESTING OFF CACHE BOOL "")
set(BUILD_SHV_UTILS OFF CACHE BOOL "")

add_subdirectory(3rdparty/necrolog)
add_subdirectory(3rdparty/libshv)

qt_add_executable(${PROJECT_NAME}
	MANUAL_FINALIZATION
	src/application.cpp
	src/appclioptions.cpp
	src/main.cpp
	src/mainwindow.cpp
	src/startlistwidget.cpp
	src/startlistitemdelegate.cpp
	src/rpcsqlresultmodel.cpp
	src/runwidget.cpp
	src/startlisttableview.cpp
	src/startlistmodel.cpp
	src/si.cpp

	src/logindialogwidget.cpp
	src/unconfirmedchangesdialogwidget.cpp
	src/settingsdialogwidget.cpp

	src/settingspage.cpp
	src/classfiltersettingspage.cpp
	src/uisettingspage.cpp
	src/stagesettingspage.cpp
	src/serialportsettingspage.cpp
	src/serialport.h src/serialport.cpp

	startertool.qrc

	android/src/org/quickbox/startertool/SerialPort.java
	android/src/org/quickbox/startertool/CustomProber.java

)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Svg Qt::Multimedia libshviotqt)

set_target_properties(${PROJECT_NAME} PROPERTIES
	${BUNDLE_ID_OPTION}
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

if(ANDROID)
	set_property(TARGET ${PROJECT_NAME} PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
	BUNDLE DESTINATION .
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(${PROJECT_NAME})
endif()
