#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Author: Inan Evin
# www.inanevin.com
# 
# Copyright (C) 2018 Inan Evin
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
# and limitations under the License.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
cmake_minimum_required (VERSION 3.6)
project(Lina2D)

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(Lina2D_SOURCES 

# Core
include/Lina2D/Core/Renderer.hpp
include/Lina2D/Core/Drawer.hpp
include/Lina2D/Core/GLBackend.hpp
include/Lina2D/Core/Common.hpp
include/Lina2D/Core/Math.hpp
)


set(Lina2D_HEADERS

# Core
src/Lina2D/Core/Renderer.cpp
src/Lina2D/Core/Drawer.cpp
src/Lina2D/Core/GLBackend.cpp
src/Lina2D/Core/Common.cpp
src/Lina2D/Core/Math.cpp
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${Lina2D_SOURCES} ${Lina2D_HEADERS})
add_library(Lina::2D ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/lib/${TARGET_ARCHITECTURE}/$<CONFIGURATION>/glad.lib
)
	

include(CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS Lina2D_HEADERS Lina2D_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LINA2D_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()