#
# --- Third Party Dependencies
#

# 
# --- Path definitions
#

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/thirdParty")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#
# --- Configuration
#

# (Pre-)define CMake configuration variables to 
# control third-party projects 

#GLFW

if(NOT GLFW_INSTALL)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
endif()
if(NOT GLFW_BUILD_DOCS)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
endif()
if(NOT GLFW_BUILD_EXAMPLES)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif() 
if(NOT GLFW_BUILD_TESTS)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
endif() 

# GLM

if(NOT GLM_TEST_ENABLE)
    set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
endif()  

#
# --- Add projects, packages, and dependencies that provide CMakeLists
#

add_subdirectory("${THIRD_PARTY_DIR}/glad/")  
add_subdirectory("${THIRD_PARTY_DIR}/glfw/") 
add_subdirectory("${THIRD_PARTY_DIR}/glm/")
add_subdirectory("${THIRD_PARTY_DIR}/tinyobjloader/")
add_subdirectory("${THIRD_PARTY_DIR}/imgui/")
#add_subdirectory("${THIRD_PARTY_DIR}/spdlog/")

#
# --- Nothings/STB header-only libs (manual)
#

set(STB_INCLUDE_DIR ${THIRD_PARTY_DIR}/stb/)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR}) 

#set(SPDLOG_INCLUDE_DIR ${THIRD_PARTY_DIR}/spdlog/include/)
#add_library(spdlog INTERFACE)
#target_include_directories(spdlog INTERFACE ${SPDLOG_INCLUDE_DIR}) 