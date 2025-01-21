#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# This file is a part of: LinaGX
# https://github.com/inanevin/LinaGX
# 
# Author: Inan Evin
# http://www.inanevin.com
# 
# The 2-Clause BSD License
# 
# Copyright (c) [2023-] Inan Evin
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
project(LinaGX)

set(TARGET_ARCHITECTURE "x64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--------------------------------------------------------------------
# Options & Definitions
#--------------------------------------------------------------------

if(NOT DEFINED LINAGX_ITERATOR_DEBUG_LEVEL)
set(LINAGX_ITERATOR_DEBUG_LEVEL 0)
endif()

option(LINAGX_BUILD_EXAMPLES "Builds example projects." OFF)

if(WIN32)
	option(LINAGX_DISABLE_DX12 "Disables DX12 backend." OFF)
	option(LINAGX_DISABLE_VK "Disables Vulkan backend." OFF)
endif()

set(LINAGX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

if(LINAGX_DISABLE_DX12 AND LINAGX_DISABLE_VK)
	message(FATAL_ERROR "Error: Can not disable both DX12 and Vulkan!")
endif()
#--------------------------------------------------------------------
# Set sources
#--------------------------------------------------------------------


set(LinaGX_HEADERS
	include/LinaGX/LinaGX.hpp
	include/LinaGX/LinaGXExports.hpp
	include/LinaGX/Common/CommonData.hpp
	include/LinaGX/Common/CommonInput.hpp
	include/LinaGX/Common/CommonConfig.hpp
	include/LinaGX/Common/Types.hpp
	include/LinaGX/Common/CommonGfx.hpp
	include/LinaGX/Common/Math.hpp
	include/LinaGX/Common/Vectors.hpp
	include/LinaGX/Common/Defines/Format.hpp
	include/LinaGX/Common/Defines/DataBuffers.hpp
	include/LinaGX/Common/Defines/SamplerDesc.hpp
	include/LinaGX/Core/Backend.hpp
	include/LinaGX/Core/Input.hpp
	include/LinaGX/Core/InputMappings.hpp
	include/LinaGX/Core/Instance.hpp
	include/LinaGX/Core/CommandStream.hpp
	include/LinaGX/Core/Commands.hpp
	include/LinaGX/Core/WindowManager.hpp
	include/LinaGX/Core/Window.hpp
	include/LinaGX/Core/WindowListener.hpp
	include/LinaGX/Utility/SPIRVUtility.hpp
	include/LinaGX/Utility/PlatformUtility.hpp
	include/LinaGX/Utility/ImageUtility.hpp
	include/LinaGX/Utility/ModelUtility.hpp
	include/LinaGX/Utility/stb/stb_image_write.h
	include/LinaGX/Utility/stb/stb_image_resize.h
	include/LinaGX/Utility/stb/stb_image.h
	include/LinaGX/Utility/tinygltf/tiny_gltf.h
	include/LinaGX/Utility/tinygltf/json.hpp
)

set(LinaGX_SOURCES
	src/Common/CommonConfig.cpp
	src/Common/CommonData.cpp
	src/Common/Math.cpp
	src/Core/Backend.cpp
	src/Core/Instance.cpp
	src/Core/CommandStream.cpp
	src/Core/WindowManager.cpp
	src/Core/Commands.cpp
	src/Utility/SPIRVUtility.cpp
	src/Utility/ImageUtility.cpp
	src/Utility/ModelUtility.cpp
	src/Utility/PlatformUtility.cpp
)

set(LinaGX_VK_HEADERS
	include/LinaGX/Platform/Vulkan/VKBackend.hpp
	include/LinaGX/Platform/Vulkan/SDK/VkBootstrap.h
	include/LinaGX/Platform/Vulkan/SDK/VkBootstrapDispatch.h
	include/LinaGX/Platform/Vulkan/SDK/vk_mem_alloc.h
)

set(LinaGX_DX12_HEADERS
	include/LinaGX/Platform/DX12/DX12Backend.hpp
	include/LinaGX/Platform/DX12/DX12HeapStaging.hpp
	include/LinaGX/Platform/DX12/DX12HeapGPU.hpp
	include/LinaGX/Platform/DX12/DX12Common.hpp
	include/LinaGX/Platform/DX12/SDK/ID3DIncludeInterface.hpp
	include/LinaGX/Platform/DX12/SDK/d3d12.h
	include/LinaGX/Platform/DX12/SDK/d3d12compatibility.h
	include/LinaGX/Platform/DX12/SDK/d3d12sdklayers.h
	include/LinaGX/Platform/DX12/SDK/d3d12shader.h
	include/LinaGX/Platform/DX12/SDK/d3d12video.h
	include/LinaGX/Platform/DX12/SDK/d3dcommon.h
	include/LinaGX/Platform/DX12/SDK/d3dx12.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_barriers.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_check_feature_support.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_core.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_default.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_pipeline_state_stream.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_property_format_table.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_render_pass.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_resource_helpers.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_root_signature.h
	include/LinaGX/Platform/DX12/SDK/d3dx12_state_object.h
	include/LinaGX/Platform/DX12/SDK/dxcore.h
	include/LinaGX/Platform/DX12/SDK/dxcore_interface.h
	include/LinaGX/Platform/DX12/SDK/dxgicommon.h
	include/LinaGX/Platform/DX12/SDK/dxgiformat.h
	include/LinaGX/Platform/DX12/SDK/Residency/d3dx12Residency.h
	include/LinaGX/Platform/DX12/SDK/D3D12MemAlloc.h
)

set(LinaGX_VK_SOURCES
	src/Platform/Vulkan/VKBackend.cpp
	src/Platform/Vulkan/SDK/VkBootstrap.cpp
)

set(LinaGX_DX12_SOURCES
	src/Platform/DX12/DX12Backend.cpp
	src/Platform/DX12/DX12HeapStaging.cpp
	src/Platform/DX12/DX12HeapGPU.cpp
	src/Platform/DX12/SDK/D3D12MemAlloc.cpp
	src/Platform/DX12/SDK/ID3DIncludeInterface.cpp
)


if(WIN32)

	set(LinaGX_PLATFORM_HEADERS
		include/LinaGX/Platform/Windows/Win32Window.hpp
	)

	set(LinaGX_PLATFORM_SOURCES
		src/Platform/Windows/Win32Window.cpp
		src/Platform/Windows/Win32Input.cpp
	)

	if(NOT LINAGX_DISABLE_DX12 AND NOT LINAGX_DISABLE_VK)
		set(LinaGX_API_HEADERS ${LinaGX_VK_HEADERS} ${LinaGX_DX12_HEADERS})
		set(LinaGX_API_SOURCES ${LinaGX_VK_SOURCES} ${LinaGX_DX12_SOURCES})
	elseif(LINAGX_DISABLE_DX12)
		set(LinaGX_API_HEADERS ${LinaGX_VK_HEADERS})
		set(LinaGX_API_SOURCES ${LinaGX_VK_SOURCES})
	elseif(LINAGX_DISABLE_VK)
		set(LinaGX_API_HEADERS ${LinaGX_DX12_HEADERS})
		set(LinaGX_API_SOURCES ${LinaGX_DX12_SOURCES})
	endif()

endif()

if(APPLE)
	set(LinaGX_PLATFORM_HEADERS
		include/LinaGX/Platform/Apple/OSXWindow.hpp
		include/LinaGX/Platform/Apple/OSXNSWindow.hh
	)

	set(LinaGX_PLATFORM_SOURCES
		src/Platform/Apple/OSXWindow.mm
		src/Platform/Apple/OSXNSWindow.mm
		src/Platform/Apple/OSXInput.mm
	)

	set(LinaGX_API_HEADERS
		include/LinaGX/Platform/Metal/MTLBackend.hpp
	)

	set(LinaGX_API_SOURCES
		src/Platform/Metal/MTLBackend.mm
	)
endif()



#--------------------------------------------------------------------
# Create project
#--------------------------------------------------------------------

if(NOT DEFINED LINAGX_FOLDER_BASE)
set(LINAGX_FOLDER_BASE LinaGXProject)
endif()

add_library(${PROJECT_NAME} ${LinaGX_SOURCES} ${LinaGX_HEADERS} ${LinaGX_PLATFORM_SOURCES} ${LinaGX_PLATFORM_HEADERS} ${LinaGX_API_SOURCES} ${LinaGX_API_HEADERS})
add_library(Lina::GX ALIAS ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${LINAGX_FOLDER_BASE})
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/ProjectSettings.cmake)

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_MAJOR=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_MINOR=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_VERSION_PATCH=0)

if(LINAGX_DISABLE_DX12)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_DISABLE_DX12=1)
endif()

if(LINAGX_DISABLE_VK)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LINAGX_DISABLE_VK=1)
endif()


include(Dependencies/Dependencies.cmake)

if(LINAGX_BUILD_EXAMPLES)
	add_subdirectory(Examples/_Common)
	add_subdirectory(Examples/00-WindowCreation)
	add_subdirectory(Examples/01-Triangle)
	add_subdirectory(Examples/02-TexturesAndBinding)
	add_subdirectory(Examples/03-RenderTargetsGLTF)
	add_subdirectory(Examples/04-BindlessIndirectComputeQueue)
	add_subdirectory(Examples/05-FoxLounge)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT 01-Triangle)
endif()

if(DEFINED ${LINAGX_RUNTIME_OUTPUT_DIRECTORY})
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/bin/" "${LINAGX_RUNTIME_OUTPUT_DIRECTORY}")
endif()


if(MSVC_IDE OR APPLE)
	foreach(source IN LISTS LinaGX_HEADERS LinaGX_SOURCES LinaGX_PLATFORM_HEADERS LinaGX_PLATFORM_SOURCES LinaGX_API_HEADERS LinaGX_API_SOURCES)
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
