cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set( PROJECT_NAME kwa_gui )

set( MOC_INPUT_FILES
	main_window.h
)

set( SOURCE_FILES
	main_window.cpp
	kwa_gui.cpp
)

set( UI_FILES
	main_window.ui
)

find_package(Qt5Widgets 5.5 REQUIRED)

qt5_wrap_cpp(_MOC_FILES ${MOC_INPUT_FILES})
qt5_wrap_ui (_UI_INCLUDE_FILES ${UI_FILES}) 

add_executable( ${PROJECT_NAME}
	${INCLUDE_FILES}
    ${SOURCE_FILES}
    ${MOC_INPUT_FILES}
    ${_MOC_FILES}
	${UI_FILES}
	${_UI_INCLUDE_FILES}
)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries( ${PROJECT_NAME} PRIVATE
    kwa_core
    Qt5::Widgets
)

if (WIN32)
	get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND windeployqt.exe $<TARGET_FILE:${PROJECT_NAME}>
		WORKING_DIRECTORY ${QT5_WINDEPLOYQT_EXECUTABLE})
endif(WIN32)
