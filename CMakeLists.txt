cmake_minimum_required(VERSION 3.2.2)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.15.4.tar.gz"
	SHA1 ac8969dd8c2bc849376006d607bda4052c93aff5
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/precompiled_headers.cmake)

project(kwa)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setup output directories
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
   ${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single directory for all dynamic libraries"
   )
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
   ${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single directory for all executables."
   )
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
   ${PROJECT_BINARY_DIR}/lib
   CACHE PATH
   "Single directory for all static libraries."
   )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup build options
option( KWA_BUILD_TESTS "Build unit tests." ON )


# Add extern dependencies
find_package(Threads)
hunter_add_package(Qt)
find_package(Qt5Widgets REQUIRED)

if(KWA_BUILD_TESTS)
    hunter_add_package(GTest)
	find_package(GTest CONFIG REQUIRED)
endif(KWA_BUILD_TESTS)  

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lib)
# Add projects
add_subdirectory( src/lib/kwa_core )
add_subdirectory( src/lib/kwa_gui )
add_subdirectory( src/app/kwa_gui_app )

if(KWA_BUILD_TESTS)
    add_subdirectory( src/test/kwa_core_test )
ENDIF(KWA_BUILD_TESTS) 
