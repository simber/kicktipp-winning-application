set( PROJECT_NAME kwa_gui )

set( INCLUDE_FILES
	kwa_gui.h
)

set( SOURCE_FILES
	mainwindow.cpp
	kwa_gui.cpp
)

set( MOC_HEADERS
	mainwindow.h
)

set( UI_FILES
	ui/mainwindow.ui
)

QT5_WRAP_CPP(MOC_GENERATED ${MOC_HEADERS})
QT5_WRAP_UI(UI_GENERATED ${UI_FILES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#enable_precompiled_headers(kwa_core_pch.h SOURCE_FILES)

add_library( ${PROJECT_NAME}
    STATIC
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
	${MOC_HEADERS}
	${UI_GENERATED}
	${MOC_GENERATED}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER lib)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} PUBLIC kwa_core)