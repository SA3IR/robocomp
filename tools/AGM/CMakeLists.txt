cmake_minimum_required( VERSION 2.8)

#  Compiler options  #
#####################
set(CMAKE_CXX_FLAGS "-Wall -Wfatal-errors -pthread" )
INCLUDE_DIRECTORIES ( . libagm ${CMAKE_SOURCE_DIR}/libs/qmat/include ${CMAKE_SOURCE_DIR}/libs/innermodel/include ${CMAKE_BINARY_DIR}/libs/innermodel/include )


#  Optional features and options  #
###################################
option (PYTHON_BINDINGS_SUPPORT "Build python bindings" OFF)
option (ROBOCOMP_SUPPORT "Build with support for RoboComp" ON)
SET(ROBOCOMP_SUPPORT_VAL "1")

#  Set sources  #
#################

SET (AGM_CODE
  libagm/agm_misc_functions.cpp
  libagm/agm_model.cpp
  libagm/agm_modelEdge.cpp
  libagm/agm_modelPrinter.cpp
  libagm/agm_modelSymbols.cpp
  libagm/agm_modelDrawer.cpp
  libagm/agm_modelConverter.cpp
  libagm/agm_plan.cpp
  libagm/agm_inner.cpp
#   libagm/graphModelViewer.cpp
  $ENV{ROBOCOMP}/classes/rcdraw/rcdraw.cpp
)


SET (HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_modelDrawer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_modelEdge.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_modelException.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_model.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_modelPrinter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_modelSymbols.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_misc_functions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_modelConverter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_config.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_plan.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_inner.h
  ${CMAKE_CURRENT_SOURCE_DIR}/libagm/graphModelViewer.h
)
#  Show enabled optional features  #
####################################
MESSAGE(STATUS "\n\nOptional features:")

if (PYTHON_BINDINGS_SUPPORT)
	MESSAGE(STATUS "PYTHON BINDINGS:        YES")
else (PYTHON_BINDINGS_SUPPORT)
	MESSAGE(STATUS "PYTHON BINDINGS:        NO")
endif (PYTHON_BINDINGS_SUPPORT)



set(CMAKE_CXX_STANDARD 11)

#  Boost library  #
set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system thread python REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
# set(LIBS ${LIBS} ${Boost_INCLUDE_DIRS})

# Fix issue: Boost does not found any python symbol
# - find_package() "PythonLibs" is REQUIRED,
# - but should force EXACT version if boost is compiled against Python 2.7 but default CMake version found is Python 3.x
#find_package(PythonLibs REQUIRED)										# Links to default python libs
find_package(PythonLibs 2.7 EXACT REQUIRED)								# Links to python libs 2.7
#message (STATUS "---DEBUG--- Found Python: ${PYTHON_LIBRARIES}")		# Check python version found


#  Handle optional features  #
##############################

# Python bindings
if (PYTHON_BINDINGS_SUPPORT)
	# Add bindings source code file
	SET(SOURCES ${SOURCES} libagm/agm_python.cpp)
endif (PYTHON_BINDINGS_SUPPORT)

# (libpython)
if (PYTHON_BINDINGS_SUPPORT)
	find_package(PythonLibs REQUIRED)
	MESSAGE(STATUS "PYTHON BINDINGS: DIRS ${PYTHON_INCLUDE_DIRS}")
	MESSAGE(STATUS "PYTHON BINDINGS: LIBS ${PYTHON_LIBRARIES}")
	include_directories(${PYTHON_INCLUDE_DIRS})
	link_directories(${PYTHON_LIBRARIES})
endif (PYTHON_BINDINGS_SUPPORT)

# RoboComp support
MESSAGE(STATUS "ROBOCOMP PATH:        $ENV{ROBOCOMP}")
INCLUDE( $ENV{ROBOCOMP}/cmake/robocomp.cmake )
ROBOCOMP_INITIALIZE( $ENV{ROBOCOMP}/ )
SET(ICE_LIBS -lIce)
ROBOCOMP_WRAP_ICE( AGMWorldModel AGMExecutive Planning AGMCommonBehavior )
ADD_DEFINITIONS(-DROBOCOMP_SUPPORT=1)
SUBDIRS (
	${CMAKE_CURRENT_SOURCE_DIR}/tools/agmmission
	${CMAKE_CURRENT_SOURCE_DIR}/tools/agminner
	${CMAKE_CURRENT_SOURCE_DIR}/tools/agmagentsim
	${CMAKE_CURRENT_SOURCE_DIR}/tools/worldDumper
	${CMAKE_CURRENT_SOURCE_DIR}/tools/worldPlayer
)

# LibXML
FIND_PACKAGE(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
set(LIBS ${LIBS} ${LIBXML2_LIBRARIES} )

ADD_DEFINITIONS( -Wall  -DQT_XML_LIB -DQT_DLL -DQT_GUI_LIB -DQT_CORE_LIB )
# FIND_PACKAGE( Qt5 )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTOPENGL TRUE )
SET( CMAKE_AUTOMOC TRUE )
INCLUDE( $ENV{ROBOCOMP}/cmake/modules/qt.cmake )
# QT4_WRAP_CPP(XMLVIEWER3D_MOCS libagm/graphModelViewer.h )
find_package(Qt4 REQUIRED)
if (QT4_FOUND)
  include(${QT_USE_FILE})
  add_definitions( "-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED -Wall -DQT_DLL -DQT_GUI_LIB -DQT_CORE_LIB" )
endif (QT4_FOUND)


# OSG
FIND_PACKAGE(OpenThreads)
FIND_PACKAGE(osg)
FIND_PACKAGE(osgDB)
FIND_PACKAGE(osgViewer)
FIND_PACKAGE(osgText)
# FIND_PACKAGE(osgQt)
SET(OSG_LIBS ${OSG_LIBRARY} ${OSGUTIL_LIBRARY} ${OSGDB_LIBRARY} ${OSGVIEWER_LIBRARY} ${OPENTHREADS_LIBRARY} ${OSGTEXT_LIBRARY})
INCLUDE_DIRECTORIES(${OPENTHREADS_INCLUDE_DIR} ${OSG_INCLUDE_DIR})
LINK_DIRECTORIES(${OSG_LIB_DIR})


#  Macro for compiling UI files and wrap command  #
###################################################
MACRO( WRAP_PYTHON_UI )
  FOREACH( input_file ${ARGN} )
    ADD_CUSTOM_COMMAND (
      OUTPUT                                                              ${CMAKE_BINARY_DIR}/ui_${input_file}.py
      COMMAND pyside-uic ${CMAKE_CURRENT_SOURCE_DIR}/tools/ui/${input_file}.ui -o ${CMAKE_BINARY_DIR}/ui_${input_file}.py
      DEPENDS            ${CMAKE_CURRENT_SOURCE_DIR}/tools/ui/${input_file}.ui
      COMMENT "Generating ${CMAKE_BINARY_DIR}/ui_${input_file}.py from ${CMAKE_CURRENT_SOURCE_DIR}/tools/ui/${input_file}.ui"
    )
    ADD_CUSTOM_TARGET(ui_${input_file} ALL DEPENDS ${CMAKE_BINARY_DIR}/ui_${input_file}.py )
  ENDFOREACH( input_file )
ENDMACRO( WRAP_PYTHON_UI )
MACRO( WRAP_PYTHON_QRC )
  FOREACH( input_file ${ARGN} )
    ADD_CUSTOM_COMMAND (
      OUTPUT                                                               ${CMAKE_BINARY_DIR}/${input_file}_rc.py
      COMMAND pyside-rcc ${CMAKE_CURRENT_SOURCE_DIR}/tools/ui/${input_file}.qrc -o ${CMAKE_BINARY_DIR}/${input_file}_rc.py
      DEPENDS            ${CMAKE_CURRENT_SOURCE_DIR}/tools/ui/${input_file}.qrc
      COMMENT "Generating ${CMAKE_BINARY_DIR}/${input_file}_rc.py from ${CMAKE_CURRENT_SOURCE_DIR}/tools/ui/${input_file}.qrc"
    )
    ADD_CUSTOM_TARGET(${input_file}_rc ALL DEPENDS ${CMAKE_BINARY_DIR}/${input_file}_rc.py )
  ENDFOREACH( input_file )
ENDMACRO( WRAP_PYTHON_QRC )
WRAP_PYTHON_UI( guiAGGLEditor guiGraphViewer appearance agglTypeEditor )
WRAP_PYTHON_QRC( resource )


#  Library building  #
######################
add_library(agm SHARED ${AGM_CODE} ${XMLVIEWER3D_MOCS} )
target_link_libraries(agm ${LIBS} ${PYTHON_LIBRARIES} ${OSG_LIBS} ${QT_LIBRARIES} ${Boost_LIBRARIES} )

if (ROBOCOMP_SUPPORT)
   add_library(agmdummy SHARED AGMWorldModel.cpp AGMExecutive.cpp AGMCommonBehavior.cpp Planning.cpp )
   set(ROBOCOMP_ADDITIONAL_SRC AGMExecutive.cpp Planning.cpp )
   add_dependencies(agm agmdummy)
endif (ROBOCOMP_SUPPORT)


add_executable ( testagm tools/testagm.cpp ${ROBOCOMP_ADDITIONAL_SRC} )
target_link_libraries ( testagm agm -L. -L.. -lagm ${LIBS} ${ICE_LIBS} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} )
add_dependencies(testagm agm)

add_executable ( agm_xml2pddlProblem tools/agm_xml2pddlProblem.cpp ${ROBOCOMP_ADDITIONAL_SRC} )
target_link_libraries ( agm_xml2pddlProblem agm -L. -L.. -lagm ${LIBS} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${Ice_LIBRARIES})
add_dependencies(testagm agm)

# add_executable ( agm_xmlViewer3D ${CMAKE_CURRENT_SOURCE_DIR}/tools/agm_xmlViewer3D.cpp ${ROBOCOMP_ADDITIONAL_SRC} )
# target_link_libraries ( agm_xmlViewer3D agm -L. -L.. -lagm ${LIBS} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${Ice_LIBRARIES})
# add_dependencies(testagm agm)



#  Doxygen  #
#############
#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_BINARY_DIR}/doc/Doxyfile @ONLY)
#  add_custom_target(doc bash ${CMAKE_CURRENT_SOURCE_DIR}/doc/makedoc.sh ${CMAKE_CURRENT_SOURCE_DIR} ${DOXYGEN_EXECUTABLE}
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#    COMMENT "Generating API documentation with Doxygen"
#    VERBATIM
#  )
#  MESSAGE( STATUS "Doxygen found.")
#else(DOXYGEN_FOUND)
#  MESSAGE( WARNING "Doxygen not found. You will not be able to generate the documentation.")
#endif(DOXYGEN_FOUND)


# Configure actual header with the proper options
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_config.in.h" "${CMAKE_CURRENT_SOURCE_DIR}/libagm/agm_config.h")



#  Installation  #
##################
SET( INST_BIN_PATH   ${CMAKE_INSTALL_PREFIX}/bin )
SET( INST_LIB_PATH   ${CMAKE_INSTALL_PREFIX}/lib )
SET( INST_INCL_PATH  ${CMAKE_INSTALL_PREFIX}/include )
SET( INST_SHARE_PATH ${CMAKE_INSTALL_PREFIX}/share/agm )

INSTALL(FILES ${CMAKE_BINARY_DIR}/tools/AGM/libagm.so DESTINATION ${INST_LIB_PATH}/ )
INSTALL (CODE "execute_process(COMMAND ldconfig)")
INSTALL(FILES ${HEADER_FILES} DESTINATION ${INST_INCL_PATH}/ )
INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/tools/AGM/agm_xml2pddlProblem DESTINATION ${INST_BIN_PATH}/ )
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/agm_xmlViewer.py DESTINATION ${INST_BIN_PATH}/ RENAME agm_xmlViewer )
#INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/tools/AGM/agm_xmlViewer3D DESTINATION ${INST_BIN_PATH}/ RENAME agm_xmlViewer3D )
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/AGGLEditor.py DESTINATION ${INST_BIN_PATH}/ RENAME aggleditor)
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/aggltypes.py DESTINATION ${INST_BIN_PATH}/ RENAME aggltypes)
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/aggl2pddl DESTINATION ${INST_BIN_PATH}/ )
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/aggl2agglpy DESTINATION ${INST_BIN_PATH}/ )

INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/AGMExecutive_robocomp/AGMExecutive_core.py DESTINATION ${INST_BIN_PATH}/ RENAME AGMExecutive_core)
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/AGMExecutive_robocomp/AGMExecutive_core.py DESTINATION ${INST_SHARE_PATH}/ RENAME AGMExecutive_core.py)
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/AGMExecutive_robocomp/AGMExecutive_monitoring.py DESTINATION ${INST_BIN_PATH}/ RENAME AGMExecutive_monitoring)
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/AGMExecutive_robocomp/AGMExecutive_monitoring.py DESTINATION ${INST_SHARE_PATH}/ RENAME AGMExecutive_monitoring.py)
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/AGMExecutive_robocomp/AGMExecutive_robocomp.py DESTINATION ${INST_BIN_PATH}/ RENAME AGMExecutive_robocomp)
INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/AGMExecutive_robocomp/AGMExecutive_robocomp.py DESTINATION ${INST_SHARE_PATH}/ RENAME AGMExecutive_robocomp.py)


INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplanner )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplanner.py )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner2.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplanner2 )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner2.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplanner2.py )

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner3.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplanner3 )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner3.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplanner3.py )


INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanningcache.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplanningcache.py )

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanchecker.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplanchecker )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanchecker.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplanchecker.py )

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner2_utils.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplanner2_utils.py )

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplannerplan.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplannerplan )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplannerplan.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplannerplan.py )

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplan.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplan )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplan.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplan.py )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplan2.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplan2 )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplan2.py" DESTINATION ${INST_SHARE_PATH}/ RENAME agglplan2.py )

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/dummysemanticspredictor.py" DESTINATION ${INST_SHARE_PATH}/ )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/genericprediction.py" DESTINATION ${INST_SHARE_PATH}/ )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/linearregressionpredictor.py" DESTINATION ${INST_SHARE_PATH}/ )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/linearregressionpredictor2.py" DESTINATION ${INST_SHARE_PATH}/ )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/linearregressionpredictor3.py" DESTINATION ${INST_SHARE_PATH}/ )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/dnnpredictor.py" DESTINATION ${INST_SHARE_PATH}/ )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/nopredictor.py" DESTINATION ${INST_SHARE_PATH}/ )


INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglcheckhierarchical.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglcheckhierarchical )
INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/python/pyparsinglocal.py" DESTINATION ${INST_SHARE_PATH}/ RENAME pyparsinglocal.py )

INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplannerserver.py" DESTINATION ${INST_BIN_PATH}/ RENAME agglplannerserver )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/AGGLPlanner/agglplanner.thrift" DESTINATION ${INST_SHARE_PATH}/ )


INSTALL(FILES ${CMAKE_BINARY_DIR}/tools/AGM/AGMCommonBehavior.h DESTINATION ${INST_INCL_PATH}/ )
INSTALL(FILES ${CMAKE_BINARY_DIR}/tools/AGM/AGMWorldModel.h DESTINATION ${INST_INCL_PATH}/ )
INSTALL(FILES ${CMAKE_BINARY_DIR}/tools/AGM/Planning.h DESTINATION ${INST_INCL_PATH}/ )
INSTALL(FILES ${CMAKE_BINARY_DIR}/tools/AGM/AGMExecutive.h DESTINATION ${INST_INCL_PATH}/ )


INSTALL(PROGRAMS python/parseAGGL.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS python/parseQuantifiers.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS python/pddlAGGL.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS python/xmlModelParser.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS python/generateAGGLPlannerCode.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS python/AGMModule.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS python/AGMModelConversion.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS python/AGGL.py DESTINATION ${INST_SHARE_PATH})


INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/ui_guiAGGLEditor.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/ui_guiGraphViewer.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/ui_agglTypeEditor.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/ui_appearance.py DESTINATION ${INST_SHARE_PATH})
INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/resource_rc.py DESTINATION ${INST_SHARE_PATH})

INSTALL(PROGRAMS tools/aggl_completion.bash DESTINATION /etc/bash_completion.d/ RENAME AGGLEditor)

INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/linkadd.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/linknegate.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/linkremove.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/linktype.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/nodeadd.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/nodemove.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/noderemove.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/noderename.png" DESTINATION ${INST_SHARE_PATH}/icons/ )
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icons/noderetype.png" DESTINATION ${INST_SHARE_PATH}/icons/ )

if (POLICY CMP0071)
  # We do not require the old behaviour. It is only set to old, to prevent accidential use of
  # the new behavour. If the new behaviour becomes important, cmake_minimum_required has to be
  # set to "3.10".
  cmake_policy(SET CMP0071 OLD) # Policy introduced in CMake version 3.10
endif()

ADD_DEFINITIONS( -std=c++11 )
