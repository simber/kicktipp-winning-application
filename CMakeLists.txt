cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(kwa VERSION 0.1 LANGUAGES CXX)

option(KWA_WITH_GUI "Build gui." ON)

add_subdirectory(src/third_party/gsl)
add_subdirectory(src/kwa_core)
if(KWA_WITH_GUI)
  add_subdirectory(src/kwa_gui)
endif()