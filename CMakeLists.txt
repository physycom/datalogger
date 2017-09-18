cmake_minimum_required (VERSION 2.8)
project (datalogger)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

### Set output directories on a per-configuration base
# Single configuration
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
# Multi configuration
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
  if( OUTPUTCONFIG STREQUAL "RELEASE" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin )
  else()
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG} )
  endif()
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)  
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(UNIX AND NOT APPLE)
  find_package(LibRt REQUIRED)
endif()
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
    ${CMAKE_DL_LIBS}
    ${FLTK_LIBRARIES} 
    ${OPENGL_gl_LIBRARY} 
    ${OPENGL_glu_LIBRARY} 
    ${Boost_LIBRARIES} 
    Threads::Threads
)


target_link_libraries(serial_reader common ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(file_converter common ${Boost_LIBRARIES} Threads::Threads)

if(UNIX AND NOT APPLE)
  target_link_libraries(serial_viewer            LIBRT::LIBRT )
  target_link_libraries(serial_reader            LIBRT::LIBRT )
  target_link_libraries(file_converter           LIBRT::LIBRT )
endif()

