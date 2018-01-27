# - Try to find PortMidi
# Once done, this will define
#
#  PortMidi_FOUND - system has PortMidi
#  PortMidi_INCLUDE_DIRS - the PortMidi include directories
#  PortMidi_LIBRARIES - link these to use PortMidi
#  PortMidi_VERSION - detected version of PortMidi
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

find_library( VIDEOCAPTURE_LIBRARY videocapture )

find_path( VIDEOCAPTURE_INCLUDE_DIR videocapture/Base.h )

set( VIDEOCAPTURE_LIBRARIES ${VIDEOCAPTURE_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args( VideoCapture REQUIRED_VARS VIDEOCAPTURE_LIBRARIES VIDEOCAPTURE_INCLUDE_DIR )

mark_as_advanced( VIDEOCAPTURE_LIBRARY )

