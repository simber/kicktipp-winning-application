INCLUDE_DIRECTORIES( ../../extern/gsl/include)
INCLUDE_DIRECTORIES(${GMOCK_INCLUDE_DIRS})

SET( PROJECT_NAME kwa_core_test )

SET( INCLUDE_FILES
	kwa_core_test.h
)

SET( SOURCE_FILES
	kwa_core_test.cpp
	stats_provider_test.cpp
	match_estimator_test.cpp
)

#enable_precompiled_headers(kwa_core_pch.h SOURCE_FILES)

ADD_EXECUTABLE( ${PROJECT_NAME}
	${INCLUDE_FILES}
    ${SOURCE_FILES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER tests)

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
    kwa_core
    pthread
    GMock
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})