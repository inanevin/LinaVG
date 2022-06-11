#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Author: Inan Evin
# www.inanevin.com
# 
# Copyright (C) 2022 Inan Evin
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
project(LinaVG)


#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(LinaVG_SOURCES 

# Core
include/LinaVG.hpp
include/Utility/Utility.hpp
include/Core/Renderer.hpp
include/Core/Text.hpp
include/Core/Drawer.hpp
include/Core/Backend.hpp
include/Core/Common.hpp
include/Core/Math.hpp
include/Backends/GL/GLBackend.hpp
include/Backends/GL/glad.h
include/Backends/GL/khrplatform.h
)


set(LinaVG_HEADERS

# Core
src/Utility/Utility.cpp
src/Core/Renderer.cpp
src/Core/Text.cpp
src/Core/Backend.cpp
src/Core/Drawer.cpp
src/Core/Common.cpp
src/Core/Math.cpp
src/Backends/GL/GLBackend.cpp
src/Backends/GL/glad.c
)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LinaVG_SOURCES} ${LinaVG_HEADERS})
add_library(Lina::VG ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/FreeType/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/lib/${ARCHITECTURE}/$<CONFIGURATION>/glad.lib
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/FreeType/lib/${ARCHITECTURE}/$<CONFIGURATION>/freetype.lib
)
	

#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	foreach(source IN LISTS LinaVG_HEADERS LinaVG_SOURCES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaVG_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
				source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endif()