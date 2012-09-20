project( CMWV )
SET( CMAKE_BUILD_TYPE Debug)

# Set a default build type for single-configuration
# CMake generators if no build type is set.
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Select flags.
SET(CMAKE_CXX_FLAGS "-Wall -frounding-math -O2 -g")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
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
# tell cmake where to search for Qt/KDE headers:

#SET(project_UI ui/batchDialog.ui )

#set(Ui_Implem include/batchcor.h)

set(CMWV_HEADERS include/cmwv.h include/geometryreader.h include/cmwv_definitions.h include/mwv.h include/mwv_base.h)
set(CMWV_SRCS tests/main.cpp src/cmwv.cpp src/geometryreader.cpp src/mwv.cpp src/mwv_base.cpp)


# Make ui files from the designer to header files
#QT_WRAP_UI(CMWV project_UI_HDRS project_UI_SRCS ${project_UI})
#QT_WRAP_CPP(CMWV project_UI_MOC ${Ui_Implem} )


include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include) # ${QT_INCLUDES})
include_directories(${QT_INCLUDE_DIR})

#add_executable(testes src/teste.cpp)
#target_link_libraries(testes ${LIBS} ${QT_LIBRARIES}  ${CGAL_LIBRARIES}  ${CGAL_3RD_PARTY_LIBRARIES})

add_executable(CMWV ${CMWV_SRCS} ${CMWV_HEADERS}  ${project_UI_MOC} ${project_UI_SRCS})
target_link_libraries(CMWV ${LIBS} ${QT_LIBRARIES}  ${CGAL_LIBRARIES}  ${CGAL_3RD_PARTY_LIBRARIES})
#add_executable(poly examples/polygon.cpp)
#target_link_libraries(poly ${LIBS} ${QT_LIBRARIES}  ${CGAL_LIBRARIES}  ${CGAL_3RD_PARTY_LIBRARIES})
