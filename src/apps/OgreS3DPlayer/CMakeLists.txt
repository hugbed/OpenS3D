cmake_minimum_required(VERSION 3.2)

project(OgreS3DPlayer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
elseif(UNIX)
	if(EXISTS "/usr/local/lib/OGRE/cmake")
	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	elseif(EXISTS "/usr/lib/OGRE/cmake")
	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE/cmake")
endif(WIN32)
 
if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (CMAKE_BUILD_TYPE STREQUAL "")

set(CMAKE_DEBUG_POSTFIX "_d")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}")

if (WIN32)
    set(Boost_USE_STATIC_LIBS TRUE)
    set(Boost_USE_MULTITHREADED TRUE)
    set(Boost_USE_STATIC_RUNTIME FALSE)
else ()
    set(Boost_USE_STATIC_LIBS FALSE)
    set(Boost_USE_MULTITHREADED TRUE)
    set(Boost_USE_STATIC_RUNTIME FALSE)
endif (WIN32)
find_package(Boost COMPONENTS system thread filesystem REQUIRED) # Specify the required components

find_package(OGRE REQUIRED)
#if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Ghadamon" AND NOT "${OGRE_VERSION_NAME}" STREQUAL "Xalafu")
if (${OGRE_VERSION} VERSION_LESS "1.9" OR ${OGRE_VERSION} VERSION_GREATER "2.0")
    message(FATAL_ERROR "${PROJECT_NAME} requires Ogre 1.9 or later, please install the latest stable version from http://ogre3d.org")
endif()

find_package(OIS REQUIRED)
if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()

file(GLOB_RECURSE HDRS src/*.hpp)
file(GLOB_RECURSE SRCS src/*.cpp)

include_directories(
	${OGRE_INCLUDE_DIRS}
    ${OIS_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

# Specify build paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
if (WIN32 OR APPLE)
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # We don't want to install in default system location, install is really for the SDK, so call it that
    set(CMAKE_INSTALL_PREFIX
	  "${CMAKE_BINARY_DIR}/${PROJECT_NAME}" CACHE PATH "${PROJECT_NAME} install prefix" FORCE
    )
  endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif(WIN32 OR APPLE)

add_executable(${PROJECT_NAME} WIN32 ${HDRS} ${SRCS})
 
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
 
target_link_libraries(${PROJECT_NAME} ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${Boost_LIBRARIES} -pthread s3d)


# post-build copy for win32
if(WIN32 AND NOT MINGW)
        file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
        file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/media)

	add_custom_command( TARGET ${PROJECT_NAME} PRE_BUILD
		COMMAND if not exist \"${CMAKE_INSTALL_PREFIX}/bin\" mkdir \"${CMAKE_INSTALL_PREFIX}/bin\" )
	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND copy \"$(TargetPath)\" \"${CMAKE_INSTALL_PREFIX}/bin\" )
endif(WIN32 AND NOT MINGW)

if(MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_INSTALL_PREFIX}/bin)
endif(MINGW OR UNIX)
 
if(WIN32)
 
	install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)
 
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/Media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins${CMAKE_DEBUG_POSTFIX}.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources${CMAKE_DEBUG_POSTFIX}.cfg
		DESTINATION bin
		CONFIGURATIONS Debug
	)
 
	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_CgProgramManager.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_OctreeSceneManager.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		${OGRE_PLUGIN_DIR_REL}/cg.dll
		${OGRE_PLUGIN_DIR_REL}/D3DCompiler_47.dll
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain${CMAKE_DEBUG_POSTFIX}.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9${CMAKE_DEBUG_POSTFIX}.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL${CMAKE_DEBUG_POSTFIX}.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS${CMAKE_DEBUG_POSTFIX}.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager${CMAKE_DEBUG_POSTFIX}.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_OctreeSceneManager${CMAKE_DEBUG_POSTFIX}.dll
		${OGRE_PLUGIN_DIR_DBG}/cg.dll
		${OGRE_PLUGIN_DIR_DBG}/D3DCompiler_47.dll
		DESTINATION bin
		CONFIGURATIONS Debug
	)
endif(WIN32)

if(UNIX)
 
	install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)
 
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/dist/media
		DESTINATION ${CMAKE_BINARY_DIR}
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(FILES ${PROJECT_SOURCE_DIR}/dist/bin/plugins.cfg
		${PROJECT_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION ${CMAKE_BINARY_DIR}/bin
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
endif(UNIX)
