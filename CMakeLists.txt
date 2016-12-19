CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Dwarf)

SET(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

SET(INCLUDE_DIR_GLM "C:/Libraries/glm" CACHE PATH "GLM's include directory")
SET(INCLUDE_DIR_EASYLOGGING "C:/Libraries/easylogging" CACHE PATH "Easylogging++'s include directory")
SET(INCLUDE_DIR_STB "C:/Libraries/stb" CACHE PATH "stb's include directory")
SET(INCLUDE_DIR_TINYOBJLOADER "C:/Libraries/tinyobjloader" CACHE PATH "tinyobjloader's include directory")
SET(INCLUDE_DIR_GLFW32 "C:/Libraries/GLFW/win32/include" CACHE PATH "GLFW's include directory for 32 bits build")
SET(INCLUDE_DIR_GLFW64 "C:/Libraries/GLFW/win64/include" CACHE PATH "GLFW's include directory for 64 bits build")
SET(INCLUDE_DIR_VULKAN "C:/Libraries/VulkanSDK/1.0.37.0/Include" CACHE PATH "Vulkan's include directory")

SET(LIB_DIR_GLFW32 "C:/Libraries/GLFW/win32/lib-vc2015" CACHE PATH "GLFW's library directory for 32 bits build")
SET(LIB_DIR_GLFW64 "C:/Libraries/GLFW/win64/lib-vc2015" CACHE PATH "GLFW's library directory for 64 bits build")
SET(LIB_DIR_VULKAN32 "C:/Libraries/VulkanSDK/1.0.37.0/Bin32" CACHE PATH "Vulkan's library directory for 32 bits build")
SET(LIB_DIR_VULKAN64 "C:/Libraries/VulkanSDK/1.0.37.0/Bin" CACHE PATH "Vulkan's library directory for 64 bits build")

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	INCLUDE_DIRECTORIES(
		include/
		${INCLUDE_DIR_GLM}
		${INCLUDE_DIR_EASYLOGGING}
		${INCLUDE_DIR_STB}
		${INCLUDE_DIR_TINYOBJLOADER}
		${INCLUDE_DIR_GLFW64}
		${INCLUDE_DIR_VULKAN}
	)
	
	LINK_DIRECTORIES(
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
		${INCLUDE_DIR_GLFW32}
		${INCLUDE_DIR_VULKAN}
	)
	
	LINK_DIRECTORIES(
    ${LIB_DIR_GLFW32}
    ${LIB_DIR_VULKAN32}
	)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

FILE(
    GLOB_RECURSE
    SOURCE_FILES
	include/*.h
    src/*.cpp
)

ADD_EXECUTABLE(
    Dwarf
    ${SOURCE_FILES}
)

TARGET_LINK_LIBRARIES(
    Dwarf
    vulkan-1
    glfw3
)
