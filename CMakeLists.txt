cmake_minimum_required(VERSION 3.21)
project(marty_cbp CXX)

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(ixwebsocket CONFIG REQUIRED)
set(IXWEBSOCKET_LIBS ixwebsocket::ixwebsocket)
if (WIN32)
    set(IXWEBSOCKET_LIBS ${IXWEBSOCKET_LIBS} bcrypt)
endif()

set(MODULE_ROOT "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE sources "${MODULE_ROOT}/*.cpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${sources})

file(GLOB_RECURSE headers "${MODULE_ROOT}/*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${headers})

add_library(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} PUBLIC ${IXWEBSOCKET_LIBS})
add_library(marty::cbp ALIAS ${PROJECT_NAME})

