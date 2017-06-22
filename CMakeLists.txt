cmake_minimum_required (VERSION 2.8)
project (datalogger)
set (CMAKE_CXX_STANDARD 11)

### Set output directories on a per-configuration base
# Single configuration
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/bin )
# Multi configuration
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
  if( OUTPUTCONFIG STREQUAL "RELEASE" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} $ENV{HOME}/bin )
  else()
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG} )
  endif()
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if(MSVC)
  set(CompilerFlags
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
  )
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD")
  set(FLTK_SKIP_FLUID TRUE)
  set(Boost_USE_STATIC_LIBS ON)
  #set(Boost_USE_STATIC_RUNTIME ON)
endif()

set(Boost_USE_MULTITHREADED ON)  
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS chrono system filesystem regex serialization date_time thread REQUIRED)


include_directories(${Boost_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS}) 
add_library(common 
    ${CMAKE_SOURCE_DIR}/src/sharedmem.cpp 
    ${CMAKE_SOURCE_DIR}/src/draw.cpp 
    ${CMAKE_SOURCE_DIR}/src/form.cpp 
    ${CMAKE_SOURCE_DIR}/src/Frame.cpp 
    ${CMAKE_SOURCE_DIR}/src/serial_tools.cpp
)

add_executable(serial_viewer ${CMAKE_SOURCE_DIR}/src/serial_viewer.cpp)
add_executable(serial_reader ${CMAKE_SOURCE_DIR}/src/serial_reader.cpp)
add_executable(file_converter ${CMAKE_SOURCE_DIR}/src/file_converter.cpp)

target_link_libraries(serial_viewer 
    common 
    ${FLTK_LIBRARIES} 
    ${OPENGL_gl_LIBRARY} 
    ${OPENGL_glu_LIBRARY} 
    ${Boost_LIBRARIES} 
    Threads::Threads
)

target_link_libraries(serial_reader common ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(file_converter common ${Boost_LIBRARIES} Threads::Threads)

