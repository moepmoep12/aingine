#
# --- Launcher
#

# Define the header/sources explicitly, 
# so CMake will be run again if you add a file
# Hint: $> tree
project(Launcher VERSION 1.0 LANGUAGES CXX)

set(LAUNCHER_SRC 
    src/Launcher.cxx
    src/LauncherGUI.cxx
    )

set(LAUNCHER_HEADER
    include/Launcher.h
    include/LauncherGUI.h
    include/Project.h
    include/TemplateFiles.h
    )

# Add Executable Target 
add_executable(Launcher ${LAUNCHER_HEADER} ${LAUNCHER_SRC})

set_target_properties(Launcher PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF) 

target_include_directories(Launcher PRIVATE include)
  
target_link_libraries(Launcher AIngine)

#
# --- Code Editor Configuration
#

if(WIN32)    
    # Configure VS Project
    set_target_properties(Launcher 
        PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY 
        VS_STARTUP_PROJECT Launcher)
endif()

assign_source_group(${LAUNCHER_HEADER} ${LAUNCHER_SRC})

# Set as Startup Project 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Launcher)

# Move to custom folder
set_target_properties(Launcher PROPERTIES FOLDER AIngine)

install(TARGETS Launcher DESTINATION "launcher" COMPONENT launcher)

# imgui.ini
install(FILES  ${CMAKE_SOURCE_DIR}/Resources/Launcher/imgui.ini DESTINATION "launcher" COMPONENT launcher)

#configure file
set(InstallPath "${CMAKE_INSTALL_PREFIX}//")
set(projectDir "${CMAKE_INSTALL_PREFIX}//")
file(TO_CMAKE_PATH ${InstallPath} InstallPath)
file(TO_CMAKE_PATH ${projectDir} projectDir)
set(InstallPath "${InstallPath}/")
set(projectDir "${projectDir}/")
configure_file(${CMAKE_SOURCE_DIR}/Resources/Launcher/project.proj.in ${CMAKE_SOURCE_DIR}/Resources/Launcher/project.proj @ONLY)
install(FILES ${CMAKE_SOURCE_DIR}/Resources/Launcher/project.proj DESTINATION "launcher" COMPONENT launcher)


#
# CPACK for creating an Installer
#
set (CPACK_PACKAGE_NAME "AIngine")
set (CPACK_PACKAGE_VENDOR "Andreas Schmidt")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "AIngine - a 2D GameEngine")
set (CPACK_PACKAGE_VERSION "1.0.0")
set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_PACKAGE_INSTALL_DIRECTORY "AIngine")

#set (CPACK_COMPONENTS_ALL editor aingine launcher resources)
#list(APPEND CPACK_COMPONENTS_ALL " launcher" )
#cpack_add_component(launcher)
set (CPACK_COMPONENT_EDITOR_DISPLAY_NAME "AIngine Editor")
set (CPACK_COMPONENT_EDITOR_REQUIRED "TRUE")
set (CPACK_COMPONENT_AINGINE_DISPLAY_NAME "AIngine Library")
set (CPACK_COMPONENT_AINGINE_REQUIRED "TRUE")
set (CPACK_COMPONENT_LAUNCHER_DISPLAY_NAME "AIngine project launcher")
set (CPACK_COMPONENT_LAUNCHER_REQUIRED "TRUE")
set (CPACK_COMPONENT_RESOURCES_DISPLAY_NAME "Resources")
set (CPACK_COMPONENT_RESOURCES_REQUIRED "TRUE")

set (CPACK_COMPONENT_EDITOR_DESCRIPTION
		"The AIngine Editor")
set (CPACK_COMPONENT_AINGINE_DESCRIPTION
		"The static C++ Library")

set (CPACK_COMPONENT_EDITOR_DEPENDS aingine)
set (CPACK_COMPONENT_LAUNCHER_DEPENDS aingine)
set(CPACK_NSIS_EXECUTABLES_DIRECTORY "launcher")
set (CPACK_NSIS_MUI_FINISHPAGE_RUN "Launcher.exe")
 #set (CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Resources/AIngine/textures\\\\icon.bmp")
 
# Must be after the last CPACK macros
include(CPack)