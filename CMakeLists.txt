CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(HephaestusEngine)

IF(NOT MSVC) # compile commands used for clang-format and clang-tidy
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ENDIF()

FUNCTION(SET_COMPILER_OPTIONS NAME)
IF(MSVC)
    TARGET_COMPILE_OPTIONS(${NAME} PRIVATE /bigobj)
    TARGET_COMPILE_OPTIONS(${NAME} PRIVATE $<$<CONFIG:Release>:/MP> $<$<CONFIG:RelWithDebInfo>:/MP>)
    TARGET_COMPILE_OPTIONS(${NAME} PRIVATE $<$<CONFIG:Release>:/GL> $<$<CONFIG:RelWithDebInfo>:/GL>)
    TARGET_COMPILE_OPTIONS(${NAME} PRIVATE $<$<CONFIG:Release>:/Oi>)
    TARGET_COMPILE_OPTIONS(${NAME} PRIVATE /Gm /arch:AVX2)
    SET_TARGET_PROPERTIES(${NAME} PROPERTIES LINK_FLAGS_RELEASE "/LTCG" LINK_FLAGS_RELWITHDEBINFO "/LTCG")
ELSE()
# Need flags for other platforms 
ENDIF()
SET_TARGET_PROPERTIES(${NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
ENDFUNCTION()

FIND_PACKAGE(Vulkan REQUIRED)
ADD_SUBDIRECTORY(third_party/vulpesrender)
ADD_SUBDIRECTORY(third_party/rendering_context)
ADD_SUBDIRECTORY(third_party/resource_context)

SET(VPR_INCLUDE_DIRS_RELATIVE
    "../vulpesrender/alloc/include"
    "../vulpesrender/command/include"
    "../vulpesrender/common"
    "../vulpesrender/core/include"
    "../vulpesrender/render/include"
    "../vulpesrender/resource/include"
    "../vulpesrender/sync/include"
)

ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(tests)
