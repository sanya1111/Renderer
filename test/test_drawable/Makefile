include ../../MakeConf/inc.mk
result_ex=../../$(BIN_DIR)/test_drawable
all:../../$(BIN_DIR)
	$(CC) $(CC_FLAGS) -L../../$(LIB_DIR)  -o $(result_ex) main.cpp -I../../$(INCLUDE_DIR) $(CC_PROJECT_PARAMS) -lRenderer
../../$(BIN_DIR):
	mkdir -p $@
clean:
	rm -f $(result_ex)

