
ADD_EXECUTABLE(train_eval_net  ${CMAKE_CURRENT_SOURCE_DIR}/train_eval_net.cpp)
ADD_EXECUTABLE(mcts_test  ${CMAKE_CURRENT_SOURCE_DIR}/mcts_test.cpp)

target_link_libraries(train_eval_net test_lib)
target_link_libraries(mcts_test test_lib)

target_link_libraries(train_eval_net onnxruntime)
target_link_libraries(mcts_test onnxruntime)

# add_test(NAME train_eval_net COMMAND "train_eval_net")
# add_test(NAME mcts_test COMMAND "mcts_test")
