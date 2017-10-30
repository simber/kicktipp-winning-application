set( PROJECT_NAME kwa_core_test )

set( INCLUDE_FILES
	kwa_core_test.h
)

set( SOURCE_FILES
	kwa_core_test.cpp
	stats_provider_test.cpp
	match_estimator_test.cpp
)

#enable_precompiled_headers(kwa_core_pch.h SOURCE_FILES)

add_executable( ${PROJECT_NAME}
	${INCLUDE_FILES}
    ${SOURCE_FILES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER tests)

target_link_libraries(${PROJECT_NAME} PUBLIC kwa_core)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::main)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})