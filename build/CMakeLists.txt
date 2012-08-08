# TerraView Voronoi cmake project.
# (c) Paulo Renato M. Oliveira, 2011
# ----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/plugins)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/plugins)  

set (PROJ_TVPLGS_DIR "${TVPLGS_DIR}/Voronoi") # change this if terralib is somewhere else

set ( PROJ_NAME "Voronoi" )

project ( ${PROJ_NAME} )
INCLUDE( ${QT_USE_FILE} )

if( (NOT DEFINED TE_DIRS) OR  (NOT DEFINED TE_LIBS) ) 
	find_package ( terralib REQUIRED )
endif()

include_directories (
	${TE_DIRS}
	${TV_DIRS}
	${PROJ_TVPLGS_DIR}/include
	${QT_INCLUDE_DIRS}
	"${CMAKE_CURRENT_BINARY_DIR}"
)

find_package(CGAL REQUIRED)
include(${CGAL_USE_FILE})
find_package(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIR})

SET(CMAKE_CXX_FLAGS "-O2")

FILE ( GLOB SRCS ${PROJ_TVPLGS_DIR}/src/*.cpp )
FILE ( GLOB HEADERS ${PROJ_TVPLGS_DIR}/include/*.h )

#Qt files
#Requires Qt multi-threaded and QT3 support
set(QT_MT_REQUIRED, TRUE)
SET( QT_USE_QT3SUPPORT TRUE )
find_package ( Qt4 COMPONENTS REQUIRED QtCore QtGui qt3support )

set (
	TO_MOC_FILES
	${PROJ_TVPLGS_DIR}/include/VoronoiWindow.h
	${PROJ_TVPLGS_DIR}/include/voronoidiagramplugin.h
	${PROJ_TVPLGS_DIR}/include/UIVoronoi.h
)

# Adding resource
set( Qt4_RCCS ${PROJ_TVPLGS_DIR}/voronoi.qrc )
QT4_ADD_RESOURCES( QT4_RCC_SRCS ${Qt4_RCCS})

file( GLOB UI_FILES ${PROJ_TVPLGS_DIR}/ui/*.ui )

# Qt4 pre-processing files
QT4_WRAP_UI( UI_HDRS ${UI_FILES} )
TV_QT4_WRAP_CPP( MOC_SRCS ${TO_MOC_FILES} )

source_group( "Generated UI Files" FILES ${UI_HDRS})
source_group( "Generated MOC Files" FILES ${MOC_SRCS})
source_group( "Forms" FILES ${UI_FILES})

# Translate
list( APPEND TSLIST "voronoi_pt" "voronoi_sp" )
set (PRO_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.pro )
foreach( TS ${TSLIST})
	set ( TSFILES "${TSFILES} ${PROJ_TVPLGS_DIR}/${TS}.ts" )
endforeach()

generatePRO( ${PRO_FILENAME} "${HEADERS}" "${SRCS}" "${UI_FILES}" "${TSFILES}" )
generateTS( ${PRO_FILENAME} )
foreach( TS ${TSLIST})
	set ( QMFILE "${PROJ_TVPLGS_DIR}/${TS}.qm" )
	set ( TSFILE "${PROJ_TVPLGS_DIR}/${TS}.ts" )
	generateQM( ${TSFILE} ${QMFILE} )
	FILE ( COPY ${QMFILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()
# -----------------------------------

add_definitions ( ${QT_DEFINITIONS} -DQT_THREAD_SUPPORT -DQT3_SUPPORT)

add_library ( ${PROJ_NAME} SHARED ${SRCS} ${HEADERS} ${MOC_SRCS} ${UI_HDRS} ${QT4_RCC_SRCS} ${GDAL_LIBRARY})  

target_link_libraries ( ${PROJ_NAME} terralib terraViewCore )