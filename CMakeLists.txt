cmake_minimum_required (VERSION 3.0)
project (AceEngine)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/Source/AceEngine)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Includes)
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/Libraries)
set(MEDIA_DIR ${PROJECT_SOURCE_DIR}/Media)
set(SCENE_DIR ${PROJECT_SOURCE_DIR}/Scenes)

INCLUDE_DIRECTORIES(${INCLUDE_DIR})
LINK_DIRECTORIES(${LIBRARY_DIR})

add_definitions(-DGLM_FORCE_PURE)

file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)
message(STATUS "Add sources: " ${SOURCES})
file(GLOB HEADERS ${SOURCE_DIR}/*.h)
message(STATUS "Add headers: " ${SOURCES})
file(GLOB SHADERS ${SOURCE_DIR}/*.vs ${SOURCE_DIR}/*.ps)
message(STATUS "Add shaders: " ${SHADERS})

file(GLOB LIBRARY_FILES ${LIBRARY_DIR}/*.lib)
foreach(lib ${LIBRARY_FILES})
	get_filename_component(LIBRARY_SHORT_NAME ${lib} NAME_WE)
	LIST(APPEND LIBS_GENERAL general)
	LIST(APPEND LIBS_GENERAL ${LIBRARY_SHORT_NAME})
endforeach()
message(STATUS "Add general libraries: " ${LIBS_GENERAL}) 

file(GLOB LIBRARY_FILES ${LIBRARY_DIR}/Debug/*.lib)
foreach(lib ${LIBRARY_FILES})
	get_filename_component(LIBRARY_SHORT_NAME ${lib} NAME_WE)
	LIST(APPEND LIBS_DEBUG debug)
	LIST(APPEND LIBS_DEBUG ${LIBRARY_SHORT_NAME})
endforeach()
message(STATUS "Add debug libraries: " ${LIBS_DEBUG}) 

file(GLOB LIBRARY_FILES ${LIBRARY_DIR}/Release/*.lib)
foreach(lib ${LIBRARY_FILES})
	get_filename_component(LIBRARY_SHORT_NAME ${lib} NAME_WE)
	LIST(APPEND LIBS_RELEASE optimized)
	LIST(APPEND LIBS_RELEASE ${LIBRARY_SHORT_NAME})
endforeach()
message(STATUS "Add release libraries: " ${LIBS_RELEASE}) 

add_executable(AceEngine ${SOURCES} ${HEADERS} ${SHADERS})
source_group(shaders FILES ${SHADERS})
TARGET_LINK_LIBRARIES(AceEngine general OpenGL32.lib ${LIBS_GENERAL} ${LIBS_DEBUG} ${LIBS_RELEASE})
	
set_target_properties( AceEngine
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Binaries"
)

file(GLOB DLL_GENERAL ${LIBRARY_DIR}/*.dll)
file(GLOB DLL_DEBUG ${LIBRARY_DIR}/Debug/*.dll)
file(GLOB DLL_RELEASE ${LIBRARY_DIR}/Release/*.dll)

ADD_CUSTOM_COMMAND(
    TARGET AceEngine
    POST_BUILD
	COMMAND XCOPY ${SCENE_DIR} "\"$(TargetDir)Scenes\\\"" /D /K /Y /S)
	
ADD_CUSTOM_COMMAND(
    TARGET AceEngine
    POST_BUILD
	COMMAND XCOPY ${MEDIA_DIR} "\"$(TargetDir)Media\\\"" /D /K /Y /S)

ADD_CUSTOM_COMMAND(
    TARGET AceEngine
    POST_BUILD
	COMMAND XCOPY ${SOURCE_DIR}\\*.vs "\"$(TargetDir)\"" /D /K /Y)
	
ADD_CUSTOM_COMMAND(
    TARGET AceEngine
    POST_BUILD
	COMMAND XCOPY ${SOURCE_DIR}\\*.ps "\"$(TargetDir)\"" /D /K /Y)
	
ADD_CUSTOM_COMMAND(
    TARGET AceEngine
    POST_BUILD
	COMMAND XCOPY ${LIBRARY_DIR}\\*.dll "\"$(TargetDir)\"" /D /K /Y)
	
ADD_CUSTOM_COMMAND(
    TARGET AceEngine
    POST_BUILD
	COMMAND XCOPY ${LIBRARY_DIR}\\${CMAKE_CFG_INTDIR}\\*.dll "\"$(TargetDir)\"" /D /K /Y)
	
ADD_CUSTOM_COMMAND(
    TARGET AceEngine
    POST_BUILD
	COMMAND XCOPY ${LIBRARY_DIR}\\${CMAKE_CFG_INTDIR}\\*.dll "\"$(TargetDir)\"" /D /K /Y)