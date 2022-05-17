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
project (LinaVGPROJ)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_EXAMPLES "Builds example backend projects." ON)
set(TARGETARCHITECTURE "x64" CACHE STRING "Set this to your target architecture, Win32, x64 etc.")

option(LINAVG_BACKEND_GL "Enables OpenGL Backend." ON)

if(LINAVG_BACKEND_GL)
	add_compile_definitions(LINAVG_BACKEND_GL)
endif()

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")


#--------------------------------------------------------------------
# Sub directories
#--------------------------------------------------------------------

add_subdirectory(LinaVG)

if(BUILD_EXAMPLES)
	add_subdirectory(Examples)
endif()


if(BUILD_EXAMPLES)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Examples)
endif()

