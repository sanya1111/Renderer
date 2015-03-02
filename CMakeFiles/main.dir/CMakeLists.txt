macro(add_library lib)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(${lib} REQUIRED ${lib})
    include_directories(${${lib}_INCLUDE_DIRS})
    link_directories(${${lib}_LIBRARY_DIRS})
    add_definitions(${${lib}_CFLAGS_OTHER})    
endmacro(add_library)

cmake_minimum_required(VERSION 2.8)  
project(Renderer)

set(SRC_DIR src)
set(TEST_DIR test)
include_directories(src)
link_directories(src)

add_library(gtkmm-3.0)
add_library(glibmm-2.4)



add_subdirectory(${TEST_DIR} )
