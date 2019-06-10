cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(AIngine VERSION 1.0 LANGUAGES CXX)

#
# --- CMake additional resources
#

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#
# --- Output Configuration
#

set(GL3_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${GL3_OUTPUT_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${GL3_OUTPUT_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GL3_OUTPUT_DIR}/bin")


#
# --- Dependencies
#

add_subdirectory(thirdParty)

#
# --- Targets developed in this project
#

add_subdirectory(assets) 
add_subdirectory(AIngine) 
add_subdirectory(game) 
add_subdirectory(CrappyBird_old) 
add_subdirectory(CrappyBird) 


# Use folders in VisualStudio / XCode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set folder for third party projects 
set_target_properties (glad PROPERTIES FOLDER ThirdParty/glad)
set_target_properties (glm_static PROPERTIES FOLDER ThirdParty/GLM)
set_target_properties (glm_shared PROPERTIES FOLDER ThirdParty/GLM)
set_target_properties (uninstall PROPERTIES FOLDER ThirdParty/GLM) 
set_target_properties (glfw PROPERTIES FOLDER ThirdParty) 
set_target_properties (tinyobjloader PROPERTIES FOLDER ThirdParty) 
set_target_properties (imgui PROPERTIES FOLDER ThirdParty) 
set_target_properties (spdlog_headers_for_ide PROPERTIES FOLDER ThirdParty) 
set_target_properties (Box2D-cmake PROPERTIES FOLDER ThirdParty) 
set_target_properties (nativefiledialog PROPERTIES FOLDER ThirdParty) 
set_target_properties (xcsrlib PROPERTIES FOLDER ThirdParty) 
set_target_properties (freetype PROPERTIES FOLDER ThirdParty) 
