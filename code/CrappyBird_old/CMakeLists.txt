#
# --- CrappyBird
#

# Define the header/sources explicitly, 
# so CMake will be run again if you add a file
# Hint: $> tree

set(CRAPPYBIRD_SRC 
    src/CrappyBird_old.cxx
    )

set(CRAPPYBIRD_HEADER
    include/CrappyBird_old.h
    )

# Add Executable Target 
add_executable(CrappyBird_old ${CRAPPYBIRD_HEADER} ${CRAPPYBIRD_SRC})

set_target_properties(CrappyBird_old PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF) 

target_include_directories(CrappyBird_old PRIVATE include)
  
target_link_libraries(CrappyBird_old Engine)

#
# --- Code Editor Configuration
#

if(WIN32)    
    # Configure VS Project
    set_target_properties(CrappyBird_old 
        PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY 
        VS_STARTUP_PROJECT CrappyBird_old)
endif()

assign_source_group(${CRAPPYBIRD_HEADER} ${CRAPPYBIRD_SRC})

# Set as Startup Project 
# set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CrappyBird)

# Move to custom folder
set_target_properties(CrappyBird_old PROPERTIES FOLDER AIngine)
