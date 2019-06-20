#
# --- Assets 
#

include(assign_source_group)
 
set(ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Note: If no executable/library target is present, these files will not show up in VS/XCode
# To be fixed...

file(GLOB_RECURSE SHADERS 
	"${ASSET_DIR}/*.glsl" 
	)

file(GLOB_RECURSE MODELS
	"${ASSET_DIR}/*.obj" 
	)

file(GLOB_RECURSE CONFIG 
	"${ASSET_DIR}/*.json"
	)

file(GLOB_RECURSE BMP
	"${ASSET_DIR}/*.bmp"
	)



# Custom target will always cause its dependencies to be evaluated and is
# run by default
add_custom_target(Assets ALL
    DEPENDS
    copy_assets
    SOURCES
    ${SHADERS}
    ${MODELS}
    ${CONFIG}
    ${BMP}
    )
     
# # custom_output will always be rebuilt because it depends on always_rebuild
# add_custom_command(
    # OUTPUT copy_assets
    # COMMAND ${CMAKE_COMMAND} -E copy_directory
    # "${ASSET_DIR}"
    # $<TARGET_FILE_DIR:Game>/assets/
    # COMMENT "Copying Assets: ${ASSET_DIR}" 
    # DEPENDS
    # always_rebuild
    # )

# Dummy output which is never actually produced. Anything that depends on
# this will always be rebuilt.
add_custom_command(
    OUTPUT always_rebuild
    COMMAND cmake -E echo
    )
	
#
# --- Code Editor
#

assign_source_group("${CMAKE_CURRENT_SOURCE_DIR}" ${SHADERS})
assign_source_group("${CMAKE_CURRENT_SOURCE_DIR}" ${CONFIG}) 
assign_source_group("${CMAKE_CURRENT_SOURCE_DIR}" ${BMP}) 

set_target_properties (Assets PROPERTIES FOLDER AIngine) 
