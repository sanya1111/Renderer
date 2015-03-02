cmake_minimum_required(VERSION 2.8)  
project(Renderer)

macro(load_library lib)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(${lib} REQUIRED ${lib})
    include_directories(${${lib}_INCLUDE_DIRS})
    link_directories(${${lib}_LIBRARY_DIRS})
    add_definitions(${${lib}_CFLAGS_OTHER})    
endmacro(add_library)


macro(add_smth prefix li suffix)
    set(newo)
    foreach(cur ${${li}})
        list(APPEND newo ${prefix}${cur}${suffix})
    endforeach(cur)   
    set(${li} ${newo})
    unset(newo)
endmacro(add_smth)

macro(list_to_string li sep)
    set(newo "")
    foreach(cur ${${li}})
        set(newo ${newo}${cur}${sep})
    endforeach(cur)   
    set(${li} ${newo})    
    unset(newo)
endmacro(list_to_string)

set(CMAKE_CXX_FLAGS  "-g")
set(SRC_DIR src)
set(TEST_DIR test)
set(INCLUDE_DIR include)
set(LIB_DIR lib)
include_directories(${INCLUDE_DIR})


load_library(gtkmm-3.0)
load_library(glibmm-2.4)



add_subdirectory(${TEST_DIR} )
add_subdirectory(${LIB_DIR} )
