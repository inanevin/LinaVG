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
set(CMAKE_CXX_STANDARD 17)

#--------------------------------------------------------------------
# Download dependencies
#--------------------------------------------------------------------

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/_Dependencies)
	include(CMake/GitUtils.cmake)
	git_clone(
        	PROJECT_NAME    _Dependencies
        	GIT_URL         https://github.com/inanevin/LinaDependencies.git
        	GIT_BRANCH      linavg
        	DIRECTORY 	${CMAKE_SOURCE_DIR})
endif()

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

option(LINAVG_BUILD_EXAMPLES "Builds example backend projects." ON)
option(LINAVG_BACKEND_GL "Enables OpenGL Backend." ON)

if(LINAVG_BACKEND_GL)
	add_compile_definitions(LINAVG_BACKEND_GL)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

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
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAVG_VERSION_MAJOR=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAVG_VERSION_MINOR=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAVG_VERSION_PATCH=1)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

if(LINAVG_BACKEND_GL)
	include(_Dependencies/glad/link.cmake)
	link_glad()
endif()

include(_Dependencies/freetype/link.cmake)
link_freetype()

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

if(LINAVG_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()


if(LINAVG_BUILD_EXAMPLES)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Examples)
endif()

endif()