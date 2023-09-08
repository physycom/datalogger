cmake_minimum_required(VERSION 3.16)

option(CMAKE_VERBOSE_MAKEFILE "Create verbose makefile" OFF)
option(USE_NSIS "Use NSIS to create installer" OFF)

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
  message(STATUS "Setting default vcpkg target triplet to $ENV{VCPKG_DEFAULT_TRIPLET}")
  set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_DEFAULT_TRIPLET})
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
  message(STATUS "VCPKG found: $ENV{VCPKG_ROOT}")
  message(STATUS "Using VCPKG integration")
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  if(VCPKG_MANIFEST_FEATURES)
    message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
  endif()
elseif(DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
  if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(STATUS "Toolchain uses VCPKG integration")
    if(VCPKG_MANIFEST_FEATURES)
      message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
    endif()
  endif()
else()
  message(WARNING "vcpkg not found, toolchain not defined, build might fail")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json VCPKG_JSON_STRING)
  string(JSON VERSION_STRING GET ${VCPKG_JSON_STRING} version)
else()
  set(VERSION_STRING "0.0.0")
endif()

string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})
list(LENGTH VERSION_LIST VERSION_LIST_LENGTH)
if(VERSION_LIST_LENGTH LESS 2)
  message(FATAL_ERROR "Tool needs at least major.minor version numbers to properly configure")
endif()
list(GET VERSION_LIST 0 TOOL_MAJOR_VERSION)
list(GET VERSION_LIST 1 TOOL_MINOR_VERSION)
if(VERSION_LIST_LENGTH GREATER 2)
  list(GET VERSION_LIST 2 TOOL_PATCH_VERSION)
else()
  set(TOOL_PATCH_VERSION 0)
endif()
if(VERSION_LIST_LENGTH GREATER 3)
  list(GET VERSION_LIST 3 TOOL_TWEAK_VERSION)
else()
  set(TOOL_TWEAK_VERSION 0)
endif()

set(TOOL_VERSION ${TOOL_MAJOR_VERSION}.${TOOL_MINOR_VERSION}.${TOOL_PATCH_VERSION}.${TOOL_TWEAK_VERSION})
message(STATUS "TOOL_VERSION: ${TOOL_VERSION}")

project(datalogger VERSION ${TOOL_VERSION})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(UNIX AND NOT APPLE)
  find_package(LibRt REQUIRED)
endif()
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(FLTK    CONFIG REQUIRED)
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

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/src/version.h"
)

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
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
)

target_compile_definitions(datalog
  PUBLIC
    -DBOOST_BIND_GLOBAL_PLACEHOLDERS
    -D_CRT_SECURE_NO_WARNINGS
)

target_link_libraries(datalog
  PUBLIC
    fltk_gl
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

install(TARGETS serial_viewer serial_reader file_converter
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)

install(
    PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    DESTINATION ${INSTALL_BIN_DIR}
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json VCPKG_JSON_STRING)
  string(JSON CPACK_PACKAGE_NAME GET ${VCPKG_JSON_STRING} name)
  string(JSON CPACK_PACKAGE_HOMEPAGE_URL GET ${VCPKG_JSON_STRING} homepage)
  string(JSON CPACK_PACKAGE_DESCRIPTION GET ${VCPKG_JSON_STRING} description)
  #set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)

  if(UNIX AND NOT APPLE)
    find_program(LSB_RELEASE_EXEC lsb_release)
    execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
        OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu")
      set(CPACK_GENERATOR "DEB")
    else()
      set(CPACK_GENERATOR "RPM")
    endif()
  elseif(APPLE)
    string(JSON CPACK_BUNDLE_NAME GET ${VCPKG_JSON_STRING} name)
    #set(CPACK_BUNDLE_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/src/${CMAKE_PROJECT_NAME}.ico")
  elseif(WIN32)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
    if(USE_NSIS)
      set(CPACK_GENERATOR "NSIS")
      string(JSON CPACK_NSIS_PACKAGE_NAME GET ${VCPKG_JSON_STRING} name)
      string(JSON CPACK_NSIS_DISPLAY_NAME GET ${VCPKG_JSON_STRING} name)
      set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
      #set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/${CMAKE_PROJECT_NAME}.ico")
      #set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/src/${CMAKE_PROJECT_NAME}.ico")
      set(CPACK_NSIS_MODIFY_PATH OFF) #disable extra page for adding to PATH, because it's broken on Win10+
    else()
      set(CPACK_GENERATOR "WIX")
      set(CPACK_WIX_UPGRADE_GUID "7C577FF4-3B90-45C1-A2E6-C74810C6DEA5") # IMPORTANT! It has to be unique for every project!!
    endif()
  endif()

  include(CPack)
endif()
