add_executable(mcts_test  ${CMAKE_CURRENT_SOURCE_DIR}/mcts_test.cpp)
add_executable(get_best_action_from_prob_test  ${CMAKE_CURRENT_SOURCE_DIR}/get_best_action_from_prob_test.cpp)

target_link_libraries(mcts_test Z2I_lib)
target_link_libraries(mcts_test onnxruntime)
target_link_libraries(get_best_action_from_prob_test Z2I_lib)
target_link_libraries(get_best_action_from_prob_test onnxruntime)

# add_test(NAME mcts_test COMMAND "mcts_test")
