#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# This file is a part of: LinaVG
# https://github.com/inanevin/LinaVG
# 
# Author: Inan Evin
# http://www.inanevin.com
# 
# The 2-Clause BSD License
# 
# Copyright (c) [2022-] Inan Evin
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
#    1. Redistributions of source code must retain the above copyright notice, this
#       list of conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright notice,
#       this list of conditions and the following disclaimer in the documentation
#       and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10...3.31)
project(LinaVG)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(TARGET_ARCHITECTURE "x64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

option(LINAVG_BUILD_EXAMPLES "Builds example backend projects." OFF)
option(LINAVG_DISABLE_TEXT_SUPPORT "Disables text support and linking to FreeType." OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------

set(LinaVG_SOURCES 

# Core
include/LinaVG/LinaVG.hpp
include/LinaVG/Utility/Utility.hpp
include/LinaVG/Core/BufferStore.hpp
include/LinaVG/Core/Text.hpp
include/LinaVG/Core/Drawer.hpp
include/LinaVG/Core/Common.hpp
include/LinaVG/Core/Math.hpp
include/LinaVG/Core/Vectors.hpp
)


set(LinaVG_HEADERS

# Core
src/Utility/Utility.cpp
src/Core/BufferStore.cpp
src/Core/Text.cpp
src/Core/Drawer.cpp
src/Core/Common.cpp
src/Core/Math.cpp

)

#--------------------------------------------------------------------
# Create executable project
#--------------------------------------------------------------------

if(NOT DEFINED LINAVG_FOLDER_BASE)
set(LINAVG_FOLDER_BASE LinaVGProject)
endif()

add_library(${PROJECT_NAME} ${LinaVG_SOURCES} ${LinaVG_HEADERS})
add_library(Lina::VG ALIAS ${PROJECT_NAME})

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAVG_VERSION_MAJOR=2)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAVG_VERSION_MINOR=2)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAVG_VERSION_PATCH=3)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${LINAVG_FOLDER_BASE})

include(Dependencies/Dependencies.cmake)
#--------------------------------------------------------------------
# Folder structuring in visual studio
#--------------------------------------------------------------------

if(MSVC_IDE OR APPLE)
	foreach(source IN LISTS LinaGX_HEADERS LinaVG_SOURCES LinaVG_HEADERS)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${LinaGX_SOURCE_DIR}" "" relative_source_path "${source_path}")

		if (MSVC_IDE)
			string(REPLACE "/" "\\" source_path_ide "${relative_source_path}")
		elseif (APPLE)
			set(source_path_ide "${relative_source_path}")
		endif()

		source_group("${source_path_ide}" FILES "${source}")
	endforeach()
endif()

if(LINAVG_BUILD_EXAMPLES)
	add_subdirectory(Example)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Example)
endif()