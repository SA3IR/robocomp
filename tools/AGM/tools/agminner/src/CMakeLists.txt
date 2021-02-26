
cmake_minimum_required(VERSION 2.6)
PROJECT(  agminner )

 
IF ( "$ENV{ROBOCOMP}" STREQUAL "")
  MESSAGE(WARNING "ROBOCOMP variable not set. Using the default value: /opt/robocomp")
  SET (ENV{ROBOCOMP} "/opt/robocomp/")
ENDIF ( "$ENV{ROBOCOMP}" STREQUAL "")

IF(NOT EXISTS $ENV{ROBOCOMP}/cmake)
  MESSAGE(FATAL_ERROR "Couldn't find RoboComp in $ENV{ROBOCOMP}! Exiting...")
ENDIF(NOT EXISTS $ENV{ROBOCOMP}/cmake)

INCLUDE( $ENV{ROBOCOMP}/cmake/robocomp.cmake )
INCLUDE ( CMakeListsSpecific.txt)

# Sources set
SET ( SOURCES
  ${SOURCES}
  $ENV{ROBOCOMP}/classes/rapplication/rapplication.cpp
  $ENV{ROBOCOMP}/classes/qlog/qlog.cpp
  main.cpp
  genericmonitor.cpp
  specificmonitor.cpp
  commonbehaviorI.cpp
  genericworker.cpp
  specificworker.cpp
)


#ROBOCOMP
ROBOCOMP_INITIALIZE( $ENV{ROBOCOMP}/ )
ROBOCOMP_WRAP_ICE( CommonBehavior AGMWorldModel Planning AGMExecutive AGMWorldModel Planning )
SET (EXECUTABLE_OUTPUT_PATH ${RC_COMPONENT_DEVEL_PATH}/bin)

ADD_DEFINITIONS( -Wall  -DQT_XML_LIB -DQT_DLL -DQT_GUI_LIB -DQT_CORE_LIB )
# FIND_PACKAGE( Qt5 )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTOPENGL TRUE )
SET( CMAKE_AUTOMOC TRUE )
INCLUDE( $ENV{ROBOCOMP}/cmake/modules/qt.cmake )

QT4_WRAP_UI( UI_HEADERS mainUI.ui )

if (POLICY CMP0071)
  # We do not require the old behaviour. It is only set to old, to prevent accidential use of
  # the new behavour. If the new behaviour becomes important, cmake_minimum_required has to be
  # set to "3.10".
  cmake_policy(SET CMP0071 OLD) # Policy introduced in CMake version 3.10
endif()

# Specify construction and link process
ADD_EXECUTABLE( agminner ${SOURCES} ${MOC_SOURCES} ${RC_SOURCES} ${UI_HEADERS} )
TARGET_LINK_LIBRARIES( agminner ${QT_LIBRARIES} ${LIBS} ${STATIC_LIBS} ${SPECIFIC_LIBS} ${Ice_LIBRARIES})
INSTALL(FILES ${EXECUTABLE_OUTPUT_PATH}/agminner DESTINATION ${RC_COMPONENT_INSTALL_PATH}/bin/ PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
