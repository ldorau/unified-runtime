# Copyright (C) 2024 Intel Corporation
# Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
# See LICENSE.TXT
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(UR_CUDA_LIBRARY "" CACHE FILEPATH "Path of the CUDA library")
set(UR_CUDA_INCLUDE_DIR "" CACHE FILEPATH "Directory containing the CUDA Headers")
set(UR_CUDA_REPO "" CACHE STRING "Github repo to get the CUDA sources from")
set(UR_CUDA_TAG "" CACHE STRING " GIT tag of the CUDA taken from github repo")

# Copy CUDA loader/headers locally to the build to avoid leaking their path.
set(CUDA_COPY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cuda_headers)
if (NOT UR_CUDA_LIBRARY STREQUAL "")
    get_filename_component(CUDA_LIB_NAME "${UR_CUDA_LIBRARY}" NAME)
    set(CUDA_LIBRARY ${CUDA_COPY_DIR}/${CUDA_LIB_NAME})
    message(STATUS "CUDA Adapter: Copying CUDA loader to local build tree")
    file(COPY ${UR_CUDA_LIBRARY} DESTINATION ${CUDA_COPY_DIR} FOLLOW_SYMLINK_CHAIN)
endif()
if (NOT UR_CUDA_INCLUDE_DIR STREQUAL "")
    set(CUDA_INCLUDE_DIR ${CUDA_COPY_DIR})
    message(STATUS "CUDA Adapter: Copying CUDA headers to local build tree")
    file(COPY ${UR_CUDA_INCLUDE_DIR}/ DESTINATION ${CUDA_COPY_DIR})
endif()

if (NOT DEFINED CUDA_LIBRARY OR NOT DEFINED CUDA_INCLUDE_DIR)
    message(STATUS "CUDA Adapter: Download CUDA loader and headers from github.com")

    if (UR_CUDA_REPO STREQUAL "")
        set(UR_CUDA_REPO "https://gitlab.com/nvidia/headers/cuda-individual/cudart.git")
    endif()
    if (UR_CUDA_TAG STREQUAL "")
        set(UR_CUDA_TAG cuda-12.5.1)
    endif()

    message(STATUS "CUDA Adapter: Will fetch CUDA ${UR_CUDA_TAG} from ${UR_CUDA_REPO}")
    include(FetchContent)
    FetchContent_Declare(cuda-content
        GIT_REPOSITORY    ${UR_CUDA_REPO}
        GIT_TAG           ${UR_CUDA_TAG}
    )
    FetchContent_MakeAvailable(cuda-content)

    set(CUDA_LIBRARY cuda)
    set(CUDA_INCLUDE_DIR ${cuda-content_SOURCE_DIR} CACHE PATH "Path to CUDA headers")
    message(STATUS "CUDA include directory: ${CUDA_INCLUDE_DIR}")
endif()

add_library(cuda INTERFACE)
# The MSVC linker does not like / at the start of a path, so to work around this
# we split it into a link library and a library path, where the path is allowed
# to have leading /.
get_filename_component(CUDA_LIBRARY_SRC "${CUDA_LIBRARY}" DIRECTORY)
get_filename_component(CUDA_LIB_NAME "${CUDA_LIBRARY}" NAME)
target_link_directories(cuda
    INTERFACE "$<BUILD_INTERFACE:${CUDA_LIBRARY_SRC}>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries(cuda
    INTERFACE "${CUDA_LIB_NAME}"
)

add_library(cuda-headers INTERFACE)
target_include_directories(cuda-headers
    INTERFACE "$<BUILD_INTERFACE:${CUDA_INCLUDE_DIR}>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
