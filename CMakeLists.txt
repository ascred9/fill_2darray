# CMakeLists.txt for g2esoft
# @author Taikan Suehara (Kyushu University)
cmake_minimum_required(VERSION 3.1)

project( threading_test ) 
set(exec_name ${PROJECT_NAME}.bin)

# root inclusion
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT 6.0)
find_package(Threads)

# global include directories
include_directories( "include" )
include_directories( ${ROOT_INCLUDE_DIRS} )

set( CMAKE_CXX_STANDARD 17 )
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
add_definitions( ${ROOT_DEFINITIONS} )

aux_source_directory( ./src sources)
add_executable( ${exec_name} test.cpp ${sources})
target_link_libraries( ${exec_name} ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} Threads::Threads)

install( TARGETS ${exec_name} DESTINATION ${CMAKE_SOURCE_DIR}/bin )
