cmake_minimum_required(VERSION 2.8.11)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/precompiled_headers.cmake)

project(kwa)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setup output directories
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
   ${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single directory for all dynamic libraries"
   )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
   ${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single directory for all executables."
   )
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
   ${PROJECT_BINARY_DIR}/lib
   CACHE PATH
   "Single directory for all static libraries."
   )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup build options
OPTION( KWA_BUILD_TESTS "Build unit tests." ON )


# Add extern dependencies

add_definitions(-std=gnu++14)
if(KWA_BUILD_TESTS)
    enable_testing()
    ADD_SUBDIRECTORY( extern/cmake/gmock )
ENDIF(KWA_BUILD_TESTS)  

SET( GSL_INCLUDE_DIRS extern/gsl/include ) 

add_definitions(-Werror)
add_definitions(-Weverything)
add_definitions(-Wno-c++98-compat)
add_definitions(-Wno-c++98-compat-pedantic)
add_definitions(-Wno-old-style-cast)
add_definitions(-Wno-padded)
add_definitions(-Wno-system-headers)
add_definitions(-Wno-weak-vtables)

# Add projects
ADD_SUBDIRECTORY( src/kwa_core )
ADD_SUBDIRECTORY( src/kwa_gui )

if(KWA_BUILD_TESTS)
    add_definitions(-Wno-global-constructors)

    ADD_SUBDIRECTORY( src/kwa_core_test )
ENDIF(KWA_BUILD_TESTS) 
