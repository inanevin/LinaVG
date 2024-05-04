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

if(NOT LINAVG_DISABLE_TEXT_SUPPORT)
    add_subdirectory(Dependencies/FreeType-2.12.1)
    target_link_libraries(${PROJECT_NAME} PUBLIC freetype)
    set_property(TARGET freetype PROPERTY FOLDER ${LINAVG_FOLDER_BASE}/Dependencies)

    if(MSVC)
        target_compile_options(freetype PRIVATE /W0)
    else()
        target_compile_options(freetype PRIVATE -w)
    endif()

    message("LinaVG -> FreeType has been linked.")
else()
    add_definitions(-DLINAVG_DISABLE_TEXT_SUPPORT=1)
endif()