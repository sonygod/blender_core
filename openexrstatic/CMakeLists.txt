cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(openexrlib LANGUAGES CXX)

include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

# Add openexr as an external project
ExternalProject_Add(
    openexr
    URL https://github.com/AcademySoftwareFoundation/openexr/archive/refs/tags/v3.1.1.tar.gz
    TIMEOUT 10
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>/openexr
        -DOPENEXR_BUILD_SHARED_LIBS=OFF
)

# Get the install location of openexr
ExternalProject_Get_Property(openexr INSTALL_DIR)

# Collect all source files into the variable SOURCES
file(GLOB SOURCES src/*.cpp)

# Create a static library target
add_library(openexrlib STATIC ${SOURCES})

target_include_directories(openexrlib PUBLIC ${INSTALL_DIR}/openexr/include)
target_link_libraries(openexrlib PUBLIC ${INSTALL_DIR}/openexr/lib)

# Ensure our static library is built after openexr
add_dependencies(openexrlib openexr)
