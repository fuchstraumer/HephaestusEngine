
ADD_LIBRARY(resource_context STATIC
    "include/ResourceContext.hpp"
    "include/ResourceLoader.hpp"
    "include/ResourceTypes.hpp"
    "include/TransferSystem.hpp"
    "src/ResourceContext.cpp"
    "src/ResourceLoader.cpp"
    "src/TransferSystem.cpp"
    "src/UploadBuffer.hpp"
    "src/UploadBuffer.cpp"
)

SET_COMPILER_OPTIONS(resource_context)
TARGET_INCLUDE_DIRECTORIES(resource_context PRIVATE ${VPR_INCLUDE_DIRS_RELATIVE} ${Vulkan_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES(resource_context PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_LINK_LIBRARIES(resource_context PUBLIC vpr_core vpr_alloc vpr_command vpr_sync ${Vulkan_LIBRARY} easyloggingpp)
