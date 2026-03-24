cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(SOURCE main.cpp)
set(TARGET osgapp)
set(OSG_VERSION "osg161")
set(OSG_ROOT "/eknabe1/GIT/esmini_ps/externals/osg" CACHE STRING "path to osg folder, e.g. /tmp/esmini/externals/osg")
set(CMAKE_CXX_STANDARD 14)

project(${TARGET})

if (APPLE)
    set ( OSG_DIR "${OSG_ROOT}/mac" )
    set ( OSG_LIBRARIES
      osgdb_serializers_osgsim
      osgdb_serializers_osg
      osgdb_osg
      osgdb_jpeg
      osgdb_png
      osgAnimation
      osgText
      osgSim
      osgViewer
      osgUtil
      osgDB
      osgGA
      osg
      OpenThreads
      jpeg
      "-framework OpenGL"
      "-framework Cocoa"
      dl
      z
    )
elseif (UNIX)
    SET(CMAKE_CXX_FLAGS "${CXX_STD_FLAG} -pthread -fPIC -Wl,-strip-all")
    set ( OSG_DIR "${OSG_ROOT}/linux" )
    set ( OSG_LIBRARIES
      optimized osgdb_serializers_osgsim debug osgdb_serializers_osgsimd
      optimized osgdb_serializers_osg debug osgdb_serializers_osgd
      optimized osgdb_osg debug osgdb_osgd
      optimized osgdb_jpeg debug osgdb_jpegd
      optimized osgdb_png debug osgdb_pngd
      optimized osgAnimation debug osgAnimationd
      optimized osgSim debug osgSimd
      optimized osgText debug osgTextd
      optimized osgViewer debug osgViewerd
      optimized osgUtil debug osgUtild
      optimized osgDB debug osgDBd
      optimized osgGA debug osgGAd
      optimized osg debug osgd
      optimized OpenThreads debug OpenThreadsd
      optimized jpeg debug jpegd

      GL
      X11
      Xrandr
      dl
      z
      Xinerama
      fontconfig
#      fbxsdk-mt
    )

elseif (WIN32)
    set ( OSG_DIR "${OSG_ROOT}/v10" )
    set ( OSG_LIBRARIES
      opengl32.lib
      optimized zlibstatic.lib debug zlibstaticd.lib
      optimized jpeg.lib debug jpegd.lib
#      optimized libpng16_static.lib debug libpng16_staticd.lib
      optimized ${OSG_VERSION}-osg.lib debug ${OSG_VERSION}-osgd.lib
      optimized ${OSG_VERSION}-osgAnimation.lib debug ${OSG_VERSION}-osgAnimationd.lib
      optimized ${OSG_VERSION}-osgDB.lib debug ${OSG_VERSION}-osgDBd.lib
      optimized ${OSG_VERSION}-osgGA.lib debug ${OSG_VERSION}-osgGAd.lib
      optimized ${OSG_VERSION}-osgViewer.lib debug ${OSG_VERSION}-osgViewerd.lib
      optimized ${OSG_VERSION}-osgText.lib debug ${OSG_VERSION}-osgTextd.lib
      optimized ${OSG_VERSION}-osgUtil.lib debug ${OSG_VERSION}-osgUtild.lib
      optimized ${OSG_VERSION}-osgSim.lib debug ${OSG_VERSION}-osgSimd.lib
      optimized ot21-OpenThreads.lib debug ot21-OpenThreadsd.lib
      optimized osgdb_serializers_osgsim.lib debug osgdb_serializers_osgsimd.lib
      optimized osgdb_serializers_osg.lib debug osgdb_serializers_osgd.lib
      optimized osgdb_osg.lib debug osgdb_osgd.lib
#      optimized osgdb_fbx.lib debug osgdb_fbxd.lib
      optimized osgdb_jpeg.lib debug osgdb_jpegd.lib
#      optimized osgdb_png.lib debug osgdb_png.lib
    )
else ()
    message ("Unsupported Windows configuration")
endif ()

set ( OSG_INCLUDE_DIR
	"${OSG_DIR}/build/include"
	"${OSG_DIR}/include"
)
set ( OSG_LIBRARIES_PATH
	"${OSG_DIR}/lib"
	"${OSG_DIR}/lib/osgPlugins-3.6.5"
)

include_directories(${OSG_INCLUDE_DIR})
link_directories(${OSG_LIBRARIES_PATH})

add_executable(${TARGET} ${SOURCE})
add_definitions(-DOSG_LIBRARY_STATIC)

target_link_libraries(${TARGET} ${OSG_LIBRARIES})
