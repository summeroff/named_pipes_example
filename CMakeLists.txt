cmake_minimum_required(VERSION 3.1)
option(OVERLAPPED_MODE "Option description" OFF)
option(PRINT_POCKET_INFO "Option description" OFF)
project(SBAPP)

if(OVERLAPPED_MODE)
  message(STATUS "OVERLAPPED_MODE")
  add_definitions(-DOVERLAPPED_MODE=1)
  set(OVERLAPPED_MODE ON)
endif()

if(PRINT_POCKET_INFO)
  message(STATUS "PRINT_POCKET_INFO")
  add_definitions(-DPRINT_POCKET_INFO=1)
  set(PRINT_POCKET_INFO ON)
endif()

set(BUILD_VC_TYPE "${CMAKE_BUILD_TYPE}")
add_definitions(-DUNICODE -D_UNICODE)
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD_REQUIRED 14)

file(GLOB_RECURSE SBAPPSERVER_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "SBAPPServer/*.cpp")
file(GLOB_RECURSE SBAPPCLIENT_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "SBAPPClient/*.cpp")
file(GLOB_RECURSE SBAPPLIB_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "SBAPPLib/*.cpp")

include_directories("SBAPPLib/")  

add_library(SBAPPLib SHARED ${SBAPPLIB_SOURCES} )
target_compile_definitions(SBAPPLib PRIVATE PIPELIBRARY_EXPORTS = 1)

#include_directories( ${CMAKE_SOURCE_DIR}/lame-3.100/include/  )
#target_link_libraries(cinemo_converter ${LIB_PTHREAD})

add_executable(SBAPPServer ${SBAPPSERVER_SOURCES} )
target_link_libraries(SBAPPServer SBAPPLib)

add_executable(SBAPPClient ${SBAPPCLIENT_SOURCES} )
target_link_libraries(SBAPPClient SBAPPLib)
