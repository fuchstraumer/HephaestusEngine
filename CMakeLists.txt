CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(HephaestusEngine)

IF(NOT MSVC) # compile commands used for clang-format and clang-tidy
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ENDIF()

ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulpesrender")

FILE(GLOB UTIL "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/util/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/util/*.hpp")
FILE(GLOB COMMON "${CMAKE_CURRENT_SOURCE_DIR}/include/common/*.hpp")
FILE(GLOB GENERATION "${CMAKE_CURRENT_SOURCE_DIR}/src/generation/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/generation/*.hpp")
FILE(GLOB MESH "${CMAKE_CURRENT_SOURCE_DIR}/src/mesh/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/mesh/*.hpp")
FILE(GLOB OBJECTS "${CMAKE_CURRENT_SOURCE_DIR}/src/objects/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/objects/*.hpp")

SOURCE_GROUP("util" FILES ${UTIL})
SOURCE_GROUP("generation" FILES ${GENERATION})
SOURCE_GROUP("mesh" FILES ${MESH})
SOURCE_GROUP("objects" FILES ${OBJECTS})
SOURCE_GROUP("scenes" FILES ${SCENES})

IF(MSVC) # enable multi-threaded compile for release builds, when its available
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP")
    SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MP")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gm") # minimal rebuild
ENDIF()

ADD_LIBRARY(HephaestusEngine STATIC "include/stdafx.h" ${UTIL} ${MESH} ${GENERATION} ${OBJECTS} ${SCENES})
IF(MSVC)
    TARGET_COMPILE_OPTIONS(HephaestusEngine PUBLIC "/std:c++17")
ELSE()
    TARGET_COMPILE_OPTIONS(HephaestusEngine PUBLIC "-std=c++17")
ENDIF()
TARGET_LINK_LIBRARIES(HephaestusEngine INTERFACE VulpesRender)
TARGET_INCLUDE_DIRECTORIES(HephaestusEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulpesrender/include" "${CMAKE_CURRENT_SOURCE_DIR}/ext/include" 
    "$ENV{VULKAN_SDK}/include" "${CMAKE_CURRENT_SOURCE_DIR}/include")

ADD_EXECUTABLE(MeshingTest "${CMAKE_CURRENT_SOURCE_DIR}/tests/GUI_Elements.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/tests/HephaestusEngine.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/MeshingTest.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/tests/MeshingTest.cpp")
TARGET_LINK_LIBRARIES(MeshingTest PUBLIC HephaestusEngine VulpesRender)
