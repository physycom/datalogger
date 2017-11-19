cmake_minimum_required (VERSION 3.1)
project (datalogger)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if(VCPKG_CHAINLOAD_TOOLCHAIN_FILE)
  include("${VCPKG_CHAINLOAD_TOOLCHAIN_FILE}")
endif()

set(Boost_USE_MULTITHREADED ON)  
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(UNIX AND NOT APPLE)
  find_package(LibRt REQUIRED)
endif()
find_package(Threads REQUIRED)
find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS chrono system filesystem regex serialization date_time thread REQUIRED)


include_directories(${Boost_INCLUDE_DIRS} ${FLTK_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS}) 

add_library(common 
    ${CMAKE_SOURCE_DIR}/src/sharedmem.cpp 
    ${CMAKE_SOURCE_DIR}/src/draw.cpp 
    ${CMAKE_SOURCE_DIR}/src/form.cpp 
    ${CMAKE_SOURCE_DIR}/src/Frame.cpp 
    ${CMAKE_SOURCE_DIR}/src/serial_tools.cpp
)

set (COMMON_FILES  
    ${CMAKE_SOURCE_DIR}/src/sharedmem.cpp 
    ${CMAKE_SOURCE_DIR}/src/draw.cpp 
    ${CMAKE_SOURCE_DIR}/src/form.cpp 
    ${CMAKE_SOURCE_DIR}/src/Frame.cpp 
    ${CMAKE_SOURCE_DIR}/src/serial_tools.cpp
)

add_executable(serial_viewer 
  ${CMAKE_SOURCE_DIR}/src/serial_viewer.cpp
  ${COMMON_FILES}
)

add_executable(serial_reader 
  ${CMAKE_SOURCE_DIR}/src/serial_reader.cpp
  ${COMMON_FILES}
)

add_executable(file_converter 
  ${CMAKE_SOURCE_DIR}/src/file_converter.cpp
  ${COMMON_FILES}
)

target_link_libraries(serial_viewer 
#    common 
    ${OPENGL_glu_LIBRARY} 
    ${OPENGL_gl_LIBRARY} 
    ${FLTK_LIBRARIES} 
    ${Boost_LIBRARIES} 
    Threads::Threads
)

target_link_libraries(serial_reader 
#    common 
    ${OPENGL_glu_LIBRARY} 
    ${OPENGL_gl_LIBRARY} 
    ${FLTK_LIBRARIES} 
    ${Boost_LIBRARIES} 
    Threads::Threads
)

target_link_libraries(file_converter 
#    common 
    ${OPENGL_glu_LIBRARY} 
    ${OPENGL_gl_LIBRARY} 
    ${FLTK_LIBRARIES} 
    ${Boost_LIBRARIES} 
    Threads::Threads
)

if(UNIX AND NOT APPLE)
target_link_libraries(serial_viewer            LIBRT::LIBRT )
target_link_libraries(serial_reader            LIBRT::LIBRT )
target_link_libraries(file_converter           LIBRT::LIBRT )
endif()

if(UNIX)
target_link_libraries(serial_viewer            ${CMAKE_DL_LIBS} )
target_link_libraries(serial_reader            ${CMAKE_DL_LIBS} )
target_link_libraries(file_converter           ${CMAKE_DL_LIBS} )
endif()

