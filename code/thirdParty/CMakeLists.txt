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
add_subdirectory("${THIRD_PARTY_DIR}/spdlog/")
add_subdirectory("${THIRD_PARTY_DIR}/Box2D-cmake/")
add_subdirectory("${THIRD_PARTY_DIR}/json/")
add_subdirectory("${THIRD_PARTY_DIR}/nativefiledialog/")
add_subdirectory("${THIRD_PARTY_DIR}/xcsrlib/")
add_subdirectory("${THIRD_PARTY_DIR}/freetype2/")

set(JSON_BuildTests OFF CACHE INTERNAL "")

#
# --- cute_sound (manual) --- ##################
#
set(CUTE_SOUND_INCLUDE_DIR ${THIRD_PARTY_DIR}/cute_sound/)
file(GLOB CUTE_SOUND_HEADER CUTE_SOUND_INCLUDE_DIR/*.h)

add_library(cute_sound INTERFACE 
						${CUTE_SOUND_HEADER}
						)
include_directories(cute_sound PUBLIC ${CUTE_SOUND_INCLUDE_DIR})
target_include_directories(cute_sound INTERFACE 
						$<BUILD_INTERFACE:${CUTE_SOUND_INCLUDE_DIR}>
						"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
install(TARGETS cute_sound EXPORT Engine DESTINATION "${CMAKE_INSTALL_LIBDIR}")
# install(EXPORT cute_sound DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cute_sound"
								# FILE cute_sound-config.cmake)
install(DIRECTORY ${CUTE_SOUND_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
														FILES_MATCHING PATTERN *.h)	

#
# --- Nothings/STB header-only libs (manual) --- ##################
#

set(STB_INCLUDE_DIR ${THIRD_PARTY_DIR}/stb/)
file(GLOB STD_SRC STB_INCLUDE_DIR/*.cpp STB_INCLUDE_DIR/*.cxx)
file(GLOB STD_INLINE STB_INCLUDE_DIR/*.inl)
file(GLOB STD_HEADER STB_INCLUDE_DIR/*.h)

add_library(stb INTERFACE 
						${STD_SRC}
						${STD_INLINE}
						${STD_HEADER}
						)
include_directories(stb PUBLIC ${STB_INCLUDE_DIR})
target_include_directories(stb INTERFACE 
						$<BUILD_INTERFACE:${STB_INCLUDE_DIR}>
						"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
install(TARGETS stb EXPORT Engine DESTINATION "${CMAKE_INSTALL_LIBDIR}")
# install(EXPORT stb DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/stb"
								# FILE stb-config.cmake)
install(DIRECTORY ${STB_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})						

#
# --- IMGUI --- ###############################################
#

set(IMGUI_INCLUDE_DIR ${THIRD_PARTY_DIR}/imgui/)
add_library(imgui STATIC 
							${IMGUI_INCLUDE_DIR}/imconfig.h
							${IMGUI_INCLUDE_DIR}/imgui.cpp
							${IMGUI_INCLUDE_DIR}/imgui.h
							${IMGUI_INCLUDE_DIR}/imgui_demo.cpp
							${IMGUI_INCLUDE_DIR}/imgui_draw.cpp
							${IMGUI_INCLUDE_DIR}/imgui_internal.h
							${IMGUI_INCLUDE_DIR}/imgui_widgets.cpp
							${IMGUI_INCLUDE_DIR}/imstb_rectpack.h
							${IMGUI_INCLUDE_DIR}/imstb_textedit.h
							${IMGUI_INCLUDE_DIR}/imstb_truetype.h
							)
include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR})							
target_include_directories(imgui INTERFACE 
						$<BUILD_INTERFACE:${IMGUI_INCLUDE_DIR}>
						"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
install(TARGETS imgui EXPORT Engine DESTINATION "${CMAKE_INSTALL_LIBDIR}")
# install(EXPORT imgui DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/imgui"
								# FILE imgui-config.cmake)
install(DIRECTORY ${IMGUI_INCLUDE_DIR} DESTINATION	${CMAKE_INSTALL_INCLUDEDIR})							

#
# --- Box2D -- #####################
#

#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
#target_link_libraries(${PROJECT_NAME} TG::Box2D)

