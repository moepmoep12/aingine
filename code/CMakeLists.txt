cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Intellgine VERSION 1.0 LANGUAGES CXX)

#
# --- CMake additional resources
#

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#
# --- Dependencies
#

add_subdirectory(thirdParty)

#
# --- Targets developed in this project
#

add_subdirectory(assets) 
add_subdirectory(Intellgine) 
add_subdirectory(game) 


# Use folders in VisualStudio / XCode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set folder for third party projects 
set_target_properties (glad PROPERTIES FOLDER ThirdParty/glad)
set_target_properties (glm_static PROPERTIES FOLDER ThirdParty/GLM)
set_target_properties (glm_shared PROPERTIES FOLDER ThirdParty/GLM)
set_target_properties (uninstall PROPERTIES FOLDER ThirdParty/GLM) 
set_target_properties (glfw PROPERTIES FOLDER ThirdParty) 
set_target_properties (tinyobjloader PROPERTIES FOLDER ThirdParty) 