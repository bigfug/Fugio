
find_library( SNAPPY_LIBRARY snappy )

find_path( SNAPPY_INCLUDE_DIR snappy.h )

set( SNAPPY_LIBRARIES ${SNAPPY_LIBRARY})

include( FindPackageHandleStandardArgs )

find_package_handle_standard_args( SNAPPY REQUIRED_VARS SNAPPY_LIBRARIES SNAPPY_INCLUDE_DIR )

mark_as_advanced( SNAPPY_LIBRARY )

