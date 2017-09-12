MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
IF(MSVC)
  GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
  SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
  SET(Sources ${${SourcesVar}})

  SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                              PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                         OBJECT_OUTPUTS "${PrecompiledBinary}")
  SET_SOURCE_FILES_PROPERTIES(${Sources}
                              PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                         OBJECT_DEPENDS "${PrecompiledBinary}")  
  # Add precompiled header to SourcesVar
  LIST(APPEND ${SourcesVar} ${PrecompiledSource})
ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

INCLUDE(ExternalProject)
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(HephaestusEngine)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON) 
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(NOT MSVC) # compile commands used for clang-format and clang-tidy
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ENDIF()

FILE(GLOB UTIL "src/util/*.cc" "src/util/*.cpp" "include/util/*.h" "include/util/*.hpp")
FILE(GLOB COMMON "include/common/*.hpp")
FILE(GLOB GENERATION "src/engine/generation/*.cpp" "include/engine/generation/*.hpp")
FILE(GLOB MESH "src/engine/mesh/*.cpp" "include/engine/mesh/*.hpp")
FILE(GLOB OBJECTS "src/engine/objects/*.cpp" "include/engine/objects/*.hpp")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/VulpesRender)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/ext)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/VulpesRender/imgui)
INCLUDE_DIRECTORIES($ENV{VULKAN_SDK}/Include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

FILE(GLOB PROJECT_SOURCES "src/util/*.cpp" "src/engine/*.cpp")
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp")

SOURCE_GROUP("util" FILES ${UTIL})
SOURCE_GROUP("generation" FILES ${GENERATION})
SOURCE_GROUP("mesh" FILES ${MESH})
SOURCE_GROUP("objects" FILES ${OBJECTS})

IF(MSVC) # enable multi-threaded compile for release builds, when its available
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP")
    SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MP")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gm") # minimal rebuild
ENDIF()

ADD_EXECUTABLE(HephaestusEngine "include/stdafx.h" "include/stdafx.cpp" "HephaestusEngine.cpp" ${UTIL} ${MESH} ${GENERATION} ${OBJECTS})