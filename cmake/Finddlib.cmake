
find_library( DLIB_LIBRARY dlib )

find_path( DLIB_INCLUDE_DIR dlib/algs.h )

set( DLIB_LIBRARIES ${DLIB_LIBRARY})

include( FindPackageHandleStandardArgs )

find_package_handle_standard_args( dlib REQUIRED_VARS DLIB_LIBRARIES DLIB_INCLUDE_DIR)

mark_as_advanced( DLIB_LIBRARY )
