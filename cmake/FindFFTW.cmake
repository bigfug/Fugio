# Module used for finding the FFTW3 libraries (float, double, long double)
#
# Usage:
#   find_package(FFTW [REQUIRED] [QUIET] )
#     
# This module will define the following variables:
#   FFTW_FOUND         - TRUE if working install of FFTW3 was found
#   FFTW_LIBRARIES     - FFTW libraries (All found)
#   FFTW_INCLUDE_DIRS  - FFTW include directories
#   FFTW_LIB           - Path to default fftw3 library
#   FFTW_F_LIB         - Path to float fftw3 library
#   FFTW_L_LIB         - Path to long double fftw3 library
#
# The following variables will be checked by the function
#   FFTW_ROOT          - Set this to the path of the FFTW3 installation to search
#                        this exclusively. Can be either CMake or environment var
#

# This file is part of libLiFFT.
#
# libLiFFT is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# libLiFFT is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with libLiFFT.  If not, see <www.gnu.org/licenses/>.

if(NOT FFTW_ROOT AND (DEFINED ENV{FFTW_ROOT}))
    set(FFTW_ROOT $ENV{FFTW_ROOT})
endif()

if(FFTW_ROOT)
    find_library(FFTW_LIB   NAMES "fftw3"  PATHS ${FFTW_ROOT} PATH_SUFFIXES "lib" "lib64")
    find_library(FFTW_F_LIB NAMES "fftw3f" PATHS ${FFTW_ROOT} PATH_SUFFIXES "lib" "lib64")
    find_library(FFTW_L_LIB NAMES "fftw3l" PATHS ${FFTW_ROOT} PATH_SUFFIXES "lib" "lib64")
    find_path(FFTW_INCLUDE_DIRS NAMES "fftw3.h" PATHS ${FFTW_ROOT} PATH_SUFFIXES "include")
else()
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(PKG_FFTW QUIET "fftw3")
        find_library(FFTW_LIB   NAMES "fftw3"  PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR})
        find_library(FFTW_F_LIB NAMES "fftw3f" PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR})
        find_library(FFTW_L_LIB NAMES "fftw3l" PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR})
        find_path(FFTW_INCLUDE_DIRS NAMES "fftw3.h" PATHS ${PKG_FFTW_INCLUDE_DIRS} ${INCLUDE_INSTALL_DIR})
    endif()
endif()

set(FFTW_LIBRARIES ${FFTW_LIB} ${FFTW_F_LIB})

if(FFTW_L_LIB)
  set(FFTW_LIBRARIES ${FFTW_LIBRARIES} ${FFTW_L_LIB})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW DEFAULT_MSG FFTW_INCLUDE_DIRS FFTW_LIBRARIES)

mark_as_advanced(FFTW_INCLUDE_DIRS FFTW_LIBRARIES FFTW_LIB FFTW_F_LIB FFTW_L_LIB)
