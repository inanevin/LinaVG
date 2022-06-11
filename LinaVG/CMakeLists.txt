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


file(GLOB FT_PUBLIC_HEADERS "include/FreeType/ft2build.h" "include/FreeType/freetype/*.h")
file(GLOB FT_PUBLIC_CONFIG_HEADERS "include/FreeType/freetype/config/*.h")
file(GLOB FT_PRIVATE_HEADERS "include/FreeType/freetype/internal/*.h")


set(FT_BASE_SRCS
  src/FreeType/autofit/autofit.c
  src/FreeType/base/ftbase.c
  src/FreeType/base/ftbbox.c
  src/FreeType/base/ftbdf.c
  src/FreeType/base/ftbitmap.c
  src/FreeType/base/ftcid.c
  src/FreeType/base/ftfstype.c
  src/FreeType/base/ftgasp.c
  src/FreeType/base/ftglyph.c
  src/FreeType/base/ftgxval.c
  src/FreeType/base/ftinit.c
  src/FreeType/base/ftmm.c
  src/FreeType/base/ftotval.c
  src/FreeType/base/ftpatent.c
  src/FreeType/base/ftpfr.c
  src/FreeType/base/ftstroke.c
  src/FreeType/base/ftsynth.c
  src/FreeType/base/fttype1.c
  src/FreeType/base/ftwinfnt.c
  src/FreeType/bdf/bdf.c
  src/FreeType/bzip2/ftbzip2.c
  src/FreeType/cache/ftcache.c
  src/FreeType/cff/cff.c
  src/FreeType/cid/type1cid.c
  src/FreeType/gzip/ftgzip.c
  src/FreeType/lzw/ftlzw.c
  src/FreeType/pcf/pcf.c
  src/FreeType/pfr/pfr.c
  src/FreeType/psaux/psaux.c
  src/FreeType/pshinter/pshinter.c
  src/FreeType/psnames/psnames.c
  src/FreeType/raster/raster.c
  src/FreeType/sdf/sdf.c
  src/FreeType/sfnt/sfnt.c
  src/FreeType/smooth/smooth.c
  src/FreeType/svg/svg.c
  src/FreeType/truetype/truetype.c
  src/FreeType/type1/type1.c
  src/FreeType/type42/type42.c
  src/FreeType/winfonts/winfnt.c
)

if (UNIX)
  list(APPEND FT_BASE_SRCS "include/FreeType/builds/unix/ftsystem.c")
elseif (WIN32)
  list(APPEND FT_BASE_SRCS "include/FreeType/builds/windows/ftsystem.c")
else ()
  list(APPEND FT_BASE_SRCS "include/FreeType/src/base/ftsystem.c")
endif ()

if (WIN32)
  enable_language(RC)
  list(APPEND FT_BASE_SRCS include/FreeType/builds/windows/ftdebug.c
                        src/FreeType/base/ftver.rc)
elseif (WINCE)
  list(APPEND FT_BASE_SRCS include/FreeType/builds/wince/ftdebug.c)
else ()
  list(APPEND BASE_SRCS src/FreeType/base/ftdebug.c)
endif ()

#--------------------------------------------------------------------
# Create library project
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${LinaVG_SOURCES} ${LinaVG_HEADERS} ${FT_PUBLIC_HEADERS} ${FT_PUBLIC_CONFIG_HEADERS} ${FT_PRIVATE_HEADERS} ${FT_BASE_SRCS})
add_library(Lina::VG ALIAS ${PROJECT_NAME}) 

#--------------------------------------------------------------------
# Set include directories
#--------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/FreeType/include)

#--------------------------------------------------------------------
# Subdirectories & linking
#--------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/vendor/FreeType/lib/${ARCHITECTURE}/$<CONFIGURATION>/freetype.lib
)
	
target_compile_definitions(
  LinaVG PRIVATE FT2_BUILD_LIBRARY)

target_compile_definitions(
    LinaVG PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)


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