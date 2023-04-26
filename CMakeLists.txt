project(AlphaZeroGomoku)
cmake_minimum_required(VERSION 3.13)

if(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
    set(ONNXRUNTIME_ROOTDIR "/mnt/d/Linker/onnxruntime-linux-x64-1.11.1")
else()
    set(ONNXRUNTIME_ROOTDIR "D:/Linker/onnxruntime-win-x86-1.11.1")
endif()

# tensorrt_provider_factory.h contains old APIs of the tensorrt execution provider
# include(CheckIncludeFileCXX)
# CHECK_INCLUDE_FILE_CXX(tensorrt_provider_factory.h HAVE_TENSORRT_PROVIDER_FACTORY_H)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${ONNXRUNTIME_ROOTDIR}/include"
                "${CMAKE_SOURCE_DIR}/src"
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

if (MSVC)
 file(GLOB ONNX_DLLS "${ONNXRUNTIME_ROOTDIR}/lib/*.dll")
 add_custom_command(TARGET Z2I_lib POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${ONNX_DLLS} ${CMAKE_CURRENT_BINARY_DIR})
endif (MSVC)

# ADD_EXECUTABLE(AlphaZeroInference  main.cpp)

# target_link_libraries(AlphaZeroInference onnxruntime)

# if(HAVE_TENSORRT_PROVIDER_FACTORY_H)
#   target_compile_definitions(AlphaZeroInference PRIVATE -DHAVE_TENSORRT_PROVIDER_FACTORY_H)
# endif()
