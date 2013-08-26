project( CMWV )
#SET( CMAKE_BUILD_TYPE Debug)
SET( CMAKE_BUILD_TYPE Release)

# Set a default build type for single-configuration
# CMake generators if no build type is set.
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Select flags.
SET(CMAKE_CXX_FLAGS "-Wall  -g -O2 -frounding-math") #-O2
#SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
#SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
#SET(CMAKE_CXX_FLAGS_DEBUG  "-Wl,-Bstatic  -Wl,-Bdynamic ")


cmake_minimum_required(VERSION 2.4.5)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )
endif()

find_package(CGAL COMPONENTS Qt4)
include(${CGAL_USE_FILE})


#SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})
find_package(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIR})
set(LIBS ${LIBS} ${GDAL_LIBRARY})


find_package(Qt4 REQUIRED) # find and setup Qt3 for this project
#add_definitions(${QT_DEFINITIONS} )
include(${QT_USE_FILE})
SET( QT_USE_QT3SUPPORT TRUE )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
# tell cmake where to search for Qt/KDE headers:

SET(project_UI ui/UIVoronoi.ui ui/simplify.ui)
QT4_WRAP_UI( project_UI_HDRS ${project_UI} )
MESSAGE( STATUS "UI header:         " "${project_UI_HDRS}" )



set(Ui_Header tests/UIVoronoi.h ) #include/SimplifyWindow.h
QT4_WRAP_CPP(Ui_HEADERS_MOC ${Ui_Header})

set(CMWV_HEADERS ${Ui_Header}
include/geometryreader.h
include/mwv.h
include/mwv_base.h
include/cmwv_ps.h
include/grid.h
include/cmwv_definitions.h
#include/awv.h
include/mwv_grid.h
) #include/SimplifyWindow.h include/voronoiBaseWindow.h

set(CMWV_SRCS
tests/UIVoronoi.cpp
src/geometryreader.cpp
src/mwv.cpp
src/mwv_base.cpp
src/cmwv_ps.cpp
#src/awv.cpp
src/mwv_grid.cpp
src/grid.cpp
) #src/SimplifyWindow.cpp src/voronoiBaseWindow.cpp


# Make ui files from the designer to header files
#QT_WRAP_UI(CMWV project_UI_HDRS project_UI_SRCS ${project_UI})
#QT_WRAP_CPP(CMWV project_UI_MOC ${Ui_Implem} )

set(BOOST_LIBS thread date_time system)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package( Boost COMPONENTS ${BOOST_LIBS} REQUIRED )
find_package(Threads REQUIRED)

if(Boost_FOUND)
MESSAGE( STATUS "BOOST_LIBS:         " "${BOOST_LIBS}" )
MESSAGE( STATUS "Boost_INCLUDE_DIRS:         " "${Boost_INCLUDE_DIRS}" )
MESSAGE( STATUS "Boost_LIBRARIES:         " "${Boost_LIBRARIES}" )
endif()


include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include) # ${QT_INCLUDES})
include_directories(${QT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})


#creating the lib
set(SRCS ${CMWV_SRCS} ${CMWV_HEADERS} ${Ui_HEADERS_MOC} ${project_UI_MOC} ${project_UI_SRCS} ${project_UI_HDRS})
set(LIBS ${LIBS} ${QT_LIBRARIES}  ${CGAL_LIBRARIES}  ${CGAL_3RD_PARTY_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_library(cmwvoronoi ${SRCS})
target_link_libraries(cmwvoronoi ${LIBS})

add_executable(mainui tests/main.cpp)
add_executable(main_tests tests/main_tests.cpp)

target_link_libraries(mainui ${LIBS} cmwvoronoi)
target_link_libraries(main_tests ${LIBS} cmwvoronoi)
#add_executable(poly examples/polygon.cpp)
#target_link_libraries(poly ${LIBS} ${QT_LIBRARIES}  ${CGAL_LIBRARIES}  ${CGAL_3RD_PARTY_LIBRARIES})
