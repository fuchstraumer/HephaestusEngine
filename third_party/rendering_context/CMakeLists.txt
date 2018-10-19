

ADD_LIBRARY(rendering_context STATIC
    "include/PlatformWindow.hpp"
    "include/RenderingContext.hpp"
    "src/PlatformWindow.cpp"
    "src/RenderingContext.cpp"
)

SET_COMPILER_OPTIONS(rendering_context)
TARGET_LINK_LIBRARIES(rendering_context PUBLIC vpr_core glfw ${Vulkan_LIBRARY} easyloggingpp)
TARGET_INCLUDE_DIRECTORIES(rendering_context PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(rendering_context PUBLIC ${VPR_INCLUDE_DIRS_RELATIVE}
    "../json/include" ${Vulkan_INCLUDE_DIR} "../../engine/include/util")
