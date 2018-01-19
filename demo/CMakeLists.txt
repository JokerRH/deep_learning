cmake_minimum_required( VERSION 2.6 )
project( demo_linux )

find_package( OpenCV 3.3.0 REQUIRED )

add_compile_options( -std=c++11 )
add_compile_options( -Wall )

set( libs opencv_core opencv_highgui opencv_videoio opencv_objdetect opencv_imgproc opencv_imgcodecs X11 )

OPTION( WITH_CAFFE "Enables Caffe" OFF )
OPTION( WITH_FREDNET "Enables Frednet" ON )

if( WITH_CAFFE )
	message( STATUS "Using Caffe" )
	add_definitions( -DWITH_CAFFE )
	add_definitions( -DCPU_ONLY )
	set( CMAKE_CXX_STANDARD 11 )
	set( CMAKE_CXX_STANDARD_REQUIRED ON )
	set( CMAKE_CXX_EXTENSIONS OFF )
	set( libs ${libs} boost_system gflags glog caffe )

	set( WITH_FREDNET OFF )
endif( WITH_CAFFE )

if( WITH_FREDNET )
	message( STATUS "Using Frednet" )
	add_definitions( -DWITH_FREDNET )
	set( CMAKE_CXX_STANDARD 14 )
	set( CMAKE_CXX_STANDARD_REQUIRED ON )
	set( CMAKE_CXX_EXTENSIONS OFF )
	add_subdirectory( ${CMAKE_SOURCE_DIR}/Frednet/ )
	set( libs ${libs} frednet )

	set( WITH_CAFFE OFF )
endif( WITH_FREDNET )

add_subdirectory( ${CMAKE_SOURCE_DIR}/Camera/ )
add_subdirectory( ${CMAKE_SOURCE_DIR}/Render/ )

file( GLOB sources *.h *.c *.cxx *.cpp *.hxx )
add_executable( demo_linux ${sources} )
target_link_libraries( demo_linux render )
target_link_libraries( demo_linux camera )
target_link_libraries( demo_linux ${libs} )
