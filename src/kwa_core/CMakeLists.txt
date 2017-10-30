cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set( MODULE_NAME kwa_core )
project(${MODULE_NAME})

set( INC_DIR include/${MODULE_NAME})

set( INCLUDE_FILES
    ${INC_DIR}/kwa_core.h
	${INC_DIR}/kwa_common.h
	${INC_DIR}/team_register.h
	${INC_DIR}/stats_provider.h
	${INC_DIR}/match_estimator.h
	src/calculations.h
)

set( SOURCE_FILES
	src/team_register.cpp
	src/stats_provider.cpp
	src/match_estimator.cpp
	src/calculations.cpp
	src/kwa_core.cpp
)

set( TEST_FILES
	test/kwa_core.t.cpp
    test/match_estimator.t.cpp
    test/stats_provider.t.cpp
)

add_library( ${MODULE_NAME}
	${INCLUDE_FILES}
    ${SOURCE_FILES}
)

target_include_directories(${MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE ${INC_DIR})
target_compile_features(${MODULE_NAME} PUBLIC cxx_std_14)
target_link_libraries(${MODULE_NAME} PUBLIC GSL)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(${MODULE_NAME}_test ${TEST_FILES})
    target_link_libraries(${MODULE_NAME}_test 
      PUBLIC 
        ${MODULE_NAME}
        GTest::GTest
    )
    GTEST_ADD_TESTS(${MODULE_NAME}_test "" AUTO)
endif()