#
# --- Launcher
#

# Define the header/sources explicitly, 
# so CMake will be run again if you add a file
# Hint: $> tree

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
        VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY 
        VS_STARTUP_PROJECT Launcher)
endif()

assign_source_group(${LAUNCHER_HEADER} ${LAUNCHER_SRC})

# Set as Startup Project 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Launcher)

# Move to custom folder
set_target_properties(Launcher PROPERTIES FOLDER AIngine)

install(TARGETS Launcher DESTINATION "launcher")

# imgui.ini
install(FILES  ${CMAKE_SOURCE_DIR}/Resources/Launcher/imgui.ini DESTINATION "launcher")

#configure file
set(InstallPath "${CMAKE_INSTALL_PREFIX}//")
configure_file(${CMAKE_SOURCE_DIR}/Resources/Launcher/project.proj.in ${CMAKE_SOURCE_DIR}/Resources/Launcher/project.proj @ONLY)
install(FILES ${CMAKE_SOURCE_DIR}/Resources/Launcher/project.proj DESTINATION ${CMAKE_INSTALL_PREFIX}/launcher)