include ../../MakeConf/inc.mk
result_ex=../../$(BIN_DIR)/test_shadow
all:../../$(BIN_DIR) $(result_ex)

$(result_ex):main.cpp ../../$(LIB_DIR)/libRenderer.so
	$(CXX) $(CXX_FLAGS) -L../../$(LIB_DIR)  -o $(result_ex) main.cpp -I../../$(INCLUDE_DIR) $(CXX_PROJECT_PARAMS) -lRenderer $(TESTS_FLAGS)
../../$(BIN_DIR):
	mkdir -p $@
clean:
	rm -f $(result_ex)

