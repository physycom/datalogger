cmake_minimum_required (VERSION 2.8)
project (datalogger)
set (CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Boost COMPONENTS chrono system filesystem regex serialization date_time thread REQUIRED)

if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  set(FLTK_SKIP_FLUID TRUE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME ON)
endif()
set(Boost_USE_MULTITHREADED ON)  

include_directories(${Boost_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${PROJECT_BINARY_DIR}) 
add_library(common ${CMAKE_SOURCE_DIR}/src/sharedmem.cpp ${CMAKE_SOURCE_DIR}/src/draw.cpp ${CMAKE_SOURCE_DIR}/src/form.cpp ${CMAKE_SOURCE_DIR}/src/Frame.cpp ${CMAKE_SOURCE_DIR}/src/serial_tools.cpp)
add_executable(serial_viewer ${CMAKE_SOURCE_DIR}/src/serial_viewer.cpp)
add_executable(serial_reader ${CMAKE_SOURCE_DIR}/src/serial_reader.cpp)
add_executable(file_converter ${CMAKE_SOURCE_DIR}/src/file_converter.cpp )
target_link_libraries(serial_viewer common ${FLTK_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(serial_reader common ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(file_converter common ${Boost_LIBRARIES} Threads::Threads)
install (TARGETS serial_viewer serial_reader file_converter 
	DESTINATION ${CMAKE_SOURCE_DIR}/bin)


