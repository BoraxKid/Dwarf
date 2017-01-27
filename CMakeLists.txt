CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Dwarf)

SET(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(INCLUDE_DIR_GLM "C:/Libraries/glm" CACHE PATH "GLM's include directory")
SET(INCLUDE_DIR_EASYLOGGING "C:/Libraries/easylogging" CACHE PATH "Easylogging++'s include directory")
SET(INCLUDE_DIR_STB "C:/Libraries/stb" CACHE PATH "stb's include directory")
SET(INCLUDE_DIR_TINYOBJLOADER "C:/Libraries/tinyobjloader" CACHE PATH "tinyobjloader's include directory")
SET(INCLUDE_DIR_ASSIMP "C:/Libraries/assimp/include" CACHE PATH "assimp's include directory")
SET(INCLUDE_DIR_GLFW32 "C:/Libraries/GLFW/win32/include" CACHE PATH "GLFW's include directory for 32 bits build")
SET(INCLUDE_DIR_GLFW64 "C:/Libraries/GLFW/win64/include" CACHE PATH "GLFW's include directory for 64 bits build")
SET(INCLUDE_DIR_VULKAN "C:/Libraries/VulkanSDK/1.0.39.0/Include" CACHE PATH "Vulkan's include directory")

SET(LIB_DIR_ASSIMP "C:/Libraries/assimp/lib/Release" CACHE PATH "assimp's library directory")
SET(LIB_DIR_GLFW32 "C:/Libraries/GLFW/win32/lib-vc2015" CACHE PATH "GLFW's library directory for 32 bits build")
SET(LIB_DIR_GLFW64 "C:/Libraries/GLFW/win64/lib-vc2015" CACHE PATH "GLFW's library directory for 64 bits build")
SET(LIB_DIR_VULKAN32 "C:/Libraries/VulkanSDK/1.0.39.0/Bin32" CACHE PATH "Vulkan's library directory for 32 bits build")
SET(LIB_DIR_VULKAN64 "C:/Libraries/VulkanSDK/1.0.39.0/Bin" CACHE PATH "Vulkan's library directory for 64 bits build")

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	INCLUDE_DIRECTORIES(
		include/
		${INCLUDE_DIR_GLM}
		${INCLUDE_DIR_EASYLOGGING}
		${INCLUDE_DIR_STB}
		${INCLUDE_DIR_TINYOBJLOADER}
        ${INCLUDE_DIR_ASSIMP}
		${INCLUDE_DIR_GLFW64}
		${INCLUDE_DIR_VULKAN}
	)
	
	LINK_DIRECTORIES(
        ${LIB_DIR_ASSIMP}
        ${LIB_DIR_GLFW64}
        ${LIB_DIR_VULKAN64}
	)
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	INCLUDE_DIRECTORIES(
		include/
		${INCLUDE_DIR_GLM}
		${INCLUDE_DIR_EASYLOGGING}
		${INCLUDE_DIR_STB}
		${INCLUDE_DIR_TINYOBJLOADER}
        ${INCLUDE_DIR_ASSIMP}
		${INCLUDE_DIR_GLFW32}
		${INCLUDE_DIR_VULKAN}
	)
	
	LINK_DIRECTORIES(
        ${LIB_DIR_ASSIMP}
        ${LIB_DIR_GLFW32}
        ${LIB_DIR_VULKAN32}
	)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

FILE(
    GLOB_RECURSE
    MAIN_SOURCE_FILES
    src/main.cpp
)

FILE(
    GLOB_RECURSE
    MATERIAL_HEADER_FILES
    include/Material.h
    include/MaterialManager.h
    include/Texture.h
)

FILE(
    GLOB_RECURSE
    MATERIAL_SOURCE_FILES
    src/Material.cpp
    src/MaterialManager.cpp
    src/Texture.cpp
)

FILE(
    GLOB_RECURSE
    MESH_HEADER_FILES
    include/Transformable.h
    include/ModelLoader.h
    include/Mesh.h
    include/Submesh.h
)

FILE(
    GLOB_RECURSE
    MESH_SOURCE_FILES
    src/Transformable.cpp
    src/ModelLoader.cpp
    src/Mesh.cpp
    src/Submesh.cpp
)

FILE(
    GLOB_RECURSE
    LIGHT_HEADER_FILES
    include/Light.h
    include/LightManager.h
)

FILE(
    GLOB_RECURSE
    LIGHT_SOURCE_FILES
    src/Light.cpp
    src/LightManager.cpp
)

FILE(
    GLOB_RECURSE
    COMMANDBUFFER_HEADER_FILES
    include/CommandBuffersBuilder.h
    include/IBuildable.h
)

FILE(
    GLOB_RECURSE
    COMMANDBUFFER_SOURCE_FILES
    src/CommandBuffersBuilder.cpp
)

FILE(
    GLOB_RECURSE
    RENDERER_HEADER_FILES
    include/Camera.h
    include/Renderer.h
)

FILE(
    GLOB_RECURSE
    RENDERER_SOURCE_FILES
    src/Camera.cpp
    src/Renderer.cpp
)

FILE(
    GLOB_RECURSE
    HELPER_HEADER_FILES
    include/Color.h
    include/ThreadPool.h
    include/Tools.h
)

FILE(
    GLOB_RECURSE
    HELPER_SOURCE_FILES
    src/Color.cpp
    src/ThreadPool.cpp
    src/Tools.cpp
)

FILE(
    GLOB_RECURSE
    ALLOCATION_HEADER_FILES
    include/DeviceAllocationManager.h
)

FILE(
    GLOB_RECURSE
    ALLOCATION_SOURCE_FILES
    src/DeviceAllocationManager.cpp
)

ADD_EXECUTABLE(
    Dwarf
    ${MAIN_SOURCE_FILES}
    ${MATERIAL_HEADER_FILES}
    ${MATERIAL_SOURCE_FILES}
    ${MESH_HEADER_FILES}
    ${MESH_SOURCE_FILES}
    ${LIGHT_HEADER_FILES}
    ${LIGHT_SOURCE_FILES}
    ${COMMANDBUFFER_HEADER_FILES}
    ${COMMANDBUFFER_SOURCE_FILES}
    ${RENDERER_HEADER_FILES}
    ${RENDERER_SOURCE_FILES}
    ${HELPER_HEADER_FILES}
    ${HELPER_SOURCE_FILES}
    ${ALLOCATION_HEADER_FILES}
    ${ALLOCATION_SOURCE_FILES}
)

SOURCE_GROUP("Header Files\\Material" FILES ${MATERIAL_HEADER_FILES})
SOURCE_GROUP("Source Files\\Material" FILES ${MATERIAL_SOURCE_FILES})
SOURCE_GROUP("Header Files\\Mesh" FILES ${MESH_HEADER_FILES})
SOURCE_GROUP("Source Files\\Mesh" FILES ${MESH_SOURCE_FILES})
SOURCE_GROUP("Header Files\\Lighting" FILES ${LIGHT_HEADER_FILES})
SOURCE_GROUP("Source Files\\Lighting" FILES ${LIGHT_SOURCE_FILES})
SOURCE_GROUP("Header Files\\Command Buffer" FILES ${COMMANDBUFFER_HEADER_FILES})
SOURCE_GROUP("Source Files\\Command Buffer" FILES ${COMMANDBUFFER_SOURCE_FILES})
SOURCE_GROUP("Header Files\\Renderer" FILES ${RENDERER_HEADER_FILES})
SOURCE_GROUP("Source Files\\Renderer" FILES ${RENDERER_SOURCE_FILES})
SOURCE_GROUP("Header Files\\Helper" FILES ${HELPER_HEADER_FILES})
SOURCE_GROUP("Source Files\\Helper" FILES ${HELPER_SOURCE_FILES})
SOURCE_GROUP("Header Files\\Allocation" FILES ${ALLOCATION_HEADER_FILES})
SOURCE_GROUP("Source Files\\Allocation" FILES ${ALLOCATION_SOURCE_FILES})

TARGET_LINK_LIBRARIES(
    Dwarf
    vulkan-1
    glfw3
    assimp-vc140-mt
)
