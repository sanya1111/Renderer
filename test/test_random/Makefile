include ../../MakeConf/inc.mk
result_ex=../../$(BIN_DIR)/test_random
all:../../$(BIN_DIR) $(result_ex)

$(result_ex):main.cpp ../../$(LIB_DIR)/libRenderer.so
	$(CXX) $(CXX_FLAGS) -L../../$(LIB_DIR)  -o $(result_ex) main.cpp -I../../$(INCLUDE_DIR) $(CXX_PROJECT_PARAMS) -lRenderer
../../$(BIN_DIR):
	mkdir -p $@
clean:
	rm -f $(result_ex)

