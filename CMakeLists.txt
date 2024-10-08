project(Z2I)
cmake_minimum_required(VERSION 3.13)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -Wl,-rpath=./:./lib")
    set(ONNXRUNTIME_ROOTDIR "/mnt/d/Linker/onnxruntime-linux-x64-1.11.1")
else()
    set(ONNXRUNTIME_ROOTDIR "D:/Linker/onnxruntime-win-x64-gpu-1.11.1")
endif()

# tensorrt_provider_factory.h contains old APIs of the tensorrt execution provider
# include(CheckIncludeFileCXX)
# CHECK_INCLUDE_FILE_CXX(tensorrt_provider_factory.h HAVE_TENSORRT_PROVIDER_FACTORY_H)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_CUDA OFF)
if(ENABLE_CUDA)
    message("build project with CUDA")
    add_definitions(-DUSE_CUDA)
endif(ENABLE_CUDA)

option(ENABLE_OPENVINO OFF)
if(ENABLE_OPENVINO)
    message("build project with OPENVINO")
    add_definitions(-DUSE_OPENVINO)
endif(ENABLE_OPENVINO)

option(ENABLE_TENSORRT OFF)
if(ENABLE_TENSORRT)
    message("build project with TensorRT")
    add_definitions(-DUSE_TENSORRT)
endif(ENABLE_TENSORRT)

option(ENABLE_ROCM OFF)
if(ENABLE_ROCM)
    message("build project with ROCM")
    add_definitions(-DUSE_ROCM)
endif(ENABLE_ROCM)

include_directories("${ONNXRUNTIME_ROOTDIR}/include"
                "${CMAKE_SOURCE_DIR}/src"
                "${CMAKE_SOURCE_DIR}/src/pbrain-Z2I/toml11/include"
)
link_directories("${ONNXRUNTIME_ROOTDIR}/lib")

# add sources
aux_source_directory(${CMAKE_SOURCE_DIR}/src SOURCES)

add_library(Z2I_lib ${SOURCES})

add_executable(train_eval_net ${CMAKE_SOURCE_DIR}/src/train_and_eval/train_eval_net.cpp)
add_executable(pbrain-Z2I ${CMAKE_SOURCE_DIR}/src/pbrain-Z2I/pbrain-Z2I.cpp)

target_link_libraries(train_eval_net Z2I_lib)
target_link_libraries(train_eval_net onnxruntime)

target_link_libraries(pbrain-Z2I Z2I_lib)
target_link_libraries(pbrain-Z2I onnxruntime)

add_subdirectory(test)

if(MSVC)
    file(GLOB ONNX_DLLS "${ONNXRUNTIME_ROOTDIR}/lib/*.dll")
elseif(UNIX)
    file(GLOB ONNX_DLLS "${ONNXRUNTIME_ROOTDIR}/lib/lib*.so*")
endif()

if(DEFINED ONNX_DLLS)
    add_custom_command(TARGET pbrain-Z2I POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${ONNX_DLLS} ${CMAKE_SOURCE_DIR}/config/config.toml $<TARGET_FILE_DIR:pbrain-Z2I>)
else()
    message("ONNX_DLLS does not exist or is empty.")
endif()

# ADD_EXECUTABLE(AlphaZeroInference  main.cpp)

# target_link_libraries(AlphaZeroInference onnxruntime)

# if(HAVE_TENSORRT_PROVIDER_FACTORY_H)
#   target_compile_definitions(AlphaZeroInference PRIVATE -DHAVE_TENSORRT_PROVIDER_FACTORY_H)
# endif()
