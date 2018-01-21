# - Try to find NDI
# Once done, this will define
#
#  NDI_FOUND - system has NDI
#  NDI_INCLUDE_DIRS - the NDI include directories
#  NDI_LIBRARIES - link these to use NDI
#  NDI_VERSION - detected version of NDI
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits

	find_library( NDI_LIBRARY Processing.NDI.Lib.x64.lib
	  HINTS
	  $ENV{NDI_DIR}
	)

elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	# 32 bits

	find_library( NDI_LIBRARY optimised Processing.NDI.Lib.x86.lib
	  HINTS
	  $ENV{NDI_DIR}
	)

endif()

find_path( NDI_INCLUDE_DIR Processing.NDI.Lib.h
  HINTS
  $ENV{NDI_DIR}
)

set( NDI_LIBRARIES optimised ${NDI_LIBRARY} )

include( FindPackageHandleStandardArgs)

find_package_handle_standard_args( NDI REQUIRED_VARS NDI_LIBRARIES NDI_INCLUDE_DIR )

mark_as_advanced( NDI_LIBRARY )
