# cmake/FindMPG123.cmake
# Custom CMake module to find mpg123 library and headers

# Find the directory containing mpg123.h
find_path(MPG123_INCLUDE_DIR mpg123.h)

# Find the mpg123 library file
find_library(MPG123_LIBRARY NAMES mpg123)

# Use standard CMake macro to handle 'found' logic
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPG123
    REQUIRED_VARS MPG123_LIBRARY MPG123_INCLUDE_DIR)

# If found, set variables for include directories and libraries
if(MPG123_FOUND)
    set(MPG123_LIBRARIES ${MPG123_LIBRARY})
    set(MPG123_INCLUDE_DIRS ${MPG123_INCLUDE_DIR})
endif()
