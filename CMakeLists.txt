cmake_minimum_required(VERSION 3.1)

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
  message(STATUS "Setting default vcpkg target triplet to $ENV{VCPKG_DEFAULT_TRIPLET}")
  set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_DEFAULT_TRIPLET})
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  message(STATUS "VCPKG found: $ENV{VCPKG_ROOT}")
  message(STATUS "Using VCPKG integration")
  message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
elseif(DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
  if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(STATUS "Toolchain uses VCPKG integration")
    message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
  endif()
else()
  message(WARNING "vcpkg not found, toolchain not defined, build might fail")
endif()

project(datalogger)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

if(UNIX AND NOT APPLE)
  find_package(LibRt REQUIRED)
endif()
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(FLTK    REQUIRED)
find_package(OpenGL  REQUIRED)
find_package(Boost   COMPONENTS chrono date_time filesystem iostreams regex serialization system thread REQUIRED)

if(UNIX AND NOT APPLE)
  find_package(X11   REQUIRED)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Install prefix" FORCE)
endif()

set(INSTALL_BIN_DIR      "bin"                     CACHE PATH "Path where exe and dll will be installed")
set(INSTALL_LIB_DIR      "lib"                     CACHE PATH "Path where lib will be installed")
set(INSTALL_INCLUDE_DIR  "include"                 CACHE PATH "Path where headers will be installed")
set(INSTALL_CMAKE_DIR    "share/${PROJECT_NAME}"   CACHE PATH "Path where cmake configs will be installed")


add_library(datalog
  ${CMAKE_CURRENT_LIST_DIR}/src/data_tools.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/datalogger.h
  ${CMAKE_CURRENT_LIST_DIR}/src/draw.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/form.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/Frame.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/serial_tools.h
  ${CMAKE_CURRENT_LIST_DIR}/src/serial_tools.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/sharedmem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/swap_tools.hpp
)

target_include_directories(datalog
  PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${FLTK_INCLUDE_DIR}
)

target_compile_definitions(datalog
  PUBLIC
    -DBOOST_BIND_GLOBAL_PLACEHOLDERS
    -D_CRT_SECURE_NO_WARNINGS
)

target_link_libraries(datalog
  PUBLIC
    OpenGL::GL
    OpenGL::GLU
    ${FLTK_LIBRARIES}
    Boost::boost
    Boost::chrono
    Boost::date_time
    Boost::filesystem
    Boost::iostreams
    Boost::regex
    Boost::serialization
    Boost::system
    Boost::thread
    Threads::Threads
)

if(UNIX AND NOT APPLE)
  target_link_libraries(datalog
  PUBLIC
    X11::X11
    X11::Xcursor
    X11::Xext
    X11::Xfixes
    X11::Xft
    X11::Xinerama
    LIBRT::LIBRT )
endif()

if(UNIX)
  target_link_libraries(datalog PUBLIC ${CMAKE_DL_LIBS} )
endif()

add_executable(serial_viewer
  ${CMAKE_CURRENT_LIST_DIR}/src/serial_viewer.cpp
)

add_executable(serial_reader
  ${CMAKE_CURRENT_LIST_DIR}/src/serial_reader.cpp
)

add_executable(file_converter
  ${CMAKE_CURRENT_LIST_DIR}/src/file_converter.cpp
)

target_include_directories(serial_viewer
  PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${FLTK_INCLUDE_DIR}
)

target_include_directories(serial_reader
  PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${FLTK_INCLUDE_DIR}
)

target_include_directories(file_converter
  PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${FLTK_INCLUDE_DIR}
)

target_link_libraries(serial_viewer PRIVATE datalog)
target_link_libraries(serial_reader PRIVATE datalog)
target_link_libraries(file_converter PRIVATE datalog)

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/doc")
  set(DOXYGEN_GENERATE_LATEX "YES")
  set(DOXYGEN_WARN_LOGFILE "${CMAKE_CURRENT_LIST_DIR}/doc/warnings.txt")
  set(DOXYGEN_LATEX_HEADER "${CMAKE_CURRENT_LIST_DIR}/doc/doxy_header.tex")
  set(DOXYGEN_LATEX_HEADER_URL "https://raw.githubusercontent.com/physycom/templates/master/texmf/tex/generic/doxy_header.tex")
  if(NOT EXISTS ${DOXYGEN_LATEX_HEADER})
    message(STATUS "Retrieving Doxygen header")
    file(DOWNLOAD ${DOXYGEN_LATEX_HEADER_URL} ${DOXYGEN_LATEX_HEADER} SHOW_PROGRESS)
  endif()
  set(DOXYGEN_REPEAT_BRIEF "NO")
  set(DOXYGEN_SHOW_INCLUDE_FILES "NO")
  set(DOXYGEN_USE_MATHJAX "YES")
  set(DOXYGEN_MATHJAX_FORMAT "SVG") # or HTML-CSS or NativeMML
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")

  file(GLOB md_sources "${CMAKE_CURRENT_LIST_DIR}/*.md")
  file(GLOB c_sources  "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
  file(GLOB h_sources  "${CMAKE_CURRENT_LIST_DIR}/src/*.h")

  doxygen_add_docs(doxy
    ${md_sources} ${c_sources} ${h_sources}
    ALL
    COMMENT "Generating documentation with Doxygen"
  )

  if(WIN32)
    set (MAKE_PROGRAM ${CMAKE_CURRENT_LIST_DIR}/doc/latex/make.bat)
  else()
    find_program (MAKE_PROGRAM NAMES make)
  endif()
  find_program (LATEX_PROGRAM NAMES latex)

  if(MAKE_PROGRAM AND LATEX_PROGRAM)
    message(STATUS "Make and LaTeX have been found, you can build LaTeX documentation by running CMake with target latex!")
    add_custom_target(latex ALL
                      COMMAND ${MAKE_PROGRAM}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/doc/latex
                      COMMENT "Generating latex documentation"
                      VERBATIM)
    add_dependencies(latex doxy)
  else()
    message(STATUS "Make or LaTeX have not been found, you have to build the LaTeX documentation by yourself using the Doxygen-generated Makefile")
  endif()
endif()

install(TARGETS datalog EXPORT DataLoggerTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT dev
)

install(EXPORT DataLoggerTargets
  FILE DataLoggerConfig.cmake
  NAMESPACE DataLogger::
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

install(TARGETS serial_viewer serial_reader file_converter DESTINATION "${INSTALL_BIN_DIR}")
