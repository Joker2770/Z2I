add_executable(mcts_test  ${CMAKE_CURRENT_SOURCE_DIR}/mcts_test.cpp)
add_executable(get_best_action_from_prob_test  ${CMAKE_CURRENT_SOURCE_DIR}/get_best_action_from_prob_test.cpp)

target_link_libraries(mcts_test Z2I_lib)
target_link_libraries(mcts_test onnxruntime)
target_link_libraries(get_best_action_from_prob_test Z2I_lib)
target_link_libraries(get_best_action_from_prob_test onnxruntime)

if(MSVC)
    file(GLOB ONNX_DLLS "${ONNXRUNTIME_ROOTDIR}/lib/*.dll")
elseif(UNIX)
    file(GLOB ONNX_DLLS "${ONNXRUNTIME_ROOTDIR}/lib/lib*.so*")
endif()

if(DEFINED ONNX_DLLS)
    add_custom_command(TARGET mcts_test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${ONNX_DLLS} $<TARGET_FILE_DIR:mcts_test>)
else()
    message("ONNX_DLLS does not exist or is empty.")
endif()

# add_test(NAME mcts_test COMMAND "mcts_test")
