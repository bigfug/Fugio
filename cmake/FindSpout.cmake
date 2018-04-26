
find_library( SPOUT_LIBRARY Spout )

find_path( SPOUT_INCLUDE_DIR Spout.h )

set( SPOUT_LIBRARIES ${SPOUT_LIBRARY} )

include( FindPackageHandleStandardArgs )

find_package_handle_standard_args( SPOUT REQUIRED_VARS SPOUT_LIBRARIES SPOUT_INCLUDE_DIR )

mark_as_advanced( SPOUT_LIBRARY )

