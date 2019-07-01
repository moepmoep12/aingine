cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(CrappyBird VERSION 1.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/out/CMake/Modules/")


#--- Source Files
set(SRC
    code/src/Player.cxx
    code/src/BackGround.cxx
    code/src/Obstacles.cxx
    code/src/Obstacle.cxx
    code/src/BackGround.cxx
    code/src/BackGround.cxx
    code/src/PickUpFactory.cxx
    code/src/Obstacles.cxx
    code/src/PickUp.cxx
    code/src/Effects.cxx
    code/src/Agent.cxx
    code/src/${PROJECT_NAME}.cxx
    code/src/Scripting_generated.cxx
     )

set(HEADER
    code/include/Player.h
    code/include/BackGround.h
    code/include/Obstacles.h
    code/include/Obstacle.h
    code/include/BackGround.h
    code/include/BackGround.h
    code/include/PickUpFactory.h
    code/include/Obstacles.h
    code/include/PickUp.h
    code/include/Effects.h
    code/include/Agent.h
	   code/include/${PROJECT_NAME}.h
    )

#
# --- Output Configuration
#
option(EDITOR " " ON)
set(OutputDir "${OUTPUT_DIR}/bin" CACHE PATH "The build dir")
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

if(EDITOR)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")
else()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OutputDir}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OutputDir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OutputDir}")
endif()

# Add Executable Target
add_executable(${PROJECT_NAME} ${HEADER} ${SRC})

#--- LIBRARIES
find_package(glfw3 REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(cxxopts REQUIRED)
find_package(OpenGL REQUIRED)
if(EDITOR)
find_package(AIngineEditor REQUIRED)
target_link_libraries(${PROJECT_NAME} AIngineEditor)
else()
find_package(AIngine REQUIRED)
target_link_libraries(${PROJECT_NAME} AIngine)
endif()

#-- Add include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/code/include)


#--- PROPERTIES
set_target_properties(${PROJECT_NAME} PROPERTIES
CXX_STANDARD 17
CXX_EXTENSIONS OFF)

#
# --- Code Editor Configuration
#
if (WIN32)
# Configure VS Project
set_target_properties(${PROJECT_NAME}
PROPERTIES
VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
PROPERTY
VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()


# Set as Startup Project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

#-- Copy project File
set(OUTPUT_PATH $<TARGET_FILE_DIR:${PROJECT_NAME}>)
set(InstallPath "${CMAKE_INSTALL_PREFIX}//")
set(projectDir "${CMAKE_CURRENT_SOURCE_DIR}//")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.proj.in ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.proj @ONLY)
add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
					${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.proj
					${OUTPUT_PATH}/project.proj)
