
find_library( VST3SDK_LIBRARY base_stdc++ 
  HINTS
  $ENV{VST3SDK_DIR}
)

find_path( VST3SDK_INCLUDE_DIR public.sdk/source/common/memorystream.h
  HINTS
  $ENV{VST3SDK_DIR}
)

set( VST3SDK_LIBRARIES ${VST3SDK_LIBRARY})

include( FindPackageHandleStandardArgs )

find_package_handle_standard_args( VST3SDK REQUIRED_VARS VST3SDK_LIBRARIES VST3SDK_INCLUDE_DIR )

mark_as_advanced( VST3SDK_LIBRARY )

