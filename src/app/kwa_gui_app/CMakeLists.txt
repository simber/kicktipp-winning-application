set( PROJECT_NAME kwa_gui_app )

set( INCLUDE_FILES
)

set( SOURCE_FILES
	main.cpp
)

#enable_precompiled_headers(kwa_core_pch.h SOURCE_FILES)


add_executable( ${PROJECT_NAME}
	${INCLUDE_FILES}
    ${SOURCE_FILES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER app)

target_link_libraries( ${PROJECT_NAME}
    kwa_core
	kwa_gui
)
