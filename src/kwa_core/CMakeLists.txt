INCLUDE_DIRECTORIES( ../../extern/gsl/include)
SET( PROJECT_NAME kwa_core )

SET( INCLUDE_FILES
	kwa_core.h
	kwa_core_pch.h
	kwa_common.h
	team_register.h
	stats_provider.h
	match_estimator.h
	calculations.h
)

SET( SOURCE_FILES
	team_register.cpp
	stats_provider.cpp
	match_estimator.cpp
	calculations.cpp
	kwa_core.cpp
)

enable_precompiled_headers(kwa_core_pch.h SOURCE_FILES)

add_library( ${PROJECT_NAME}
    STATIC
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER kwa)