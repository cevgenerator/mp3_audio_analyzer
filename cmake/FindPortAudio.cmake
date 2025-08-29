# cmake/FindPORTAUDIO.cmake
# Custom CMake module to find PortAudio library and headers

# Find the directory containing portaudio.h
find_path(PORTAUDIO_INCLUDE_DIR portaudio.h)

# Find the PortAudio library file
find_library(PORTAUDIO_LIBRARY NAMES portaudio)

# Use standard CMake macro to handle 'found' logic
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PortAudio
    REQUIRED_VARS PORTAUDIO_LIBRARY PORTAUDIO_INCLUDE_DIR)

# If found, set variables for include directories and libraries
if(PortAudio_FOUND)
    set(PortAudio_LIBRARIES ${PORTAUDIO_LIBRARY})
    set(PortAudio_INCLUDE_DIRS ${PORTAUDIO_INCLUDE_DIR})
endif()
