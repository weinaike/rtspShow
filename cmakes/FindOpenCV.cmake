# Try to find libraries for OpenCV
# Once done this will define
#
#  OpenCV_FOUND - system has OpenCV
#  OpenCV_INCLUDE_DIRS - the OpenCV include directories
#  OpenCV_LIBRARIES - link these to use OpenCV

find_path(OpenCV_INCLUDE_DIR
    NAMES opencv2/core/core.hpp
    PATHS /usr/local/include /usr/include
)

find_library(OpenCV_LIB
    NAMES opencv_core
    PATHS /usr/local/lib /usr/lib
)

set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIR})
set(OpenCV_LIBRARIES ${OpenCV_LIB})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenCV DEFAULT_MSG OpenCV_INCLUDE_DIR OpenCV_LIBRARIES)