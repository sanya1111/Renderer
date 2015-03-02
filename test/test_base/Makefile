include ../../MakeConf/inc.mk
result_ex=../../$(BIN_DIR)/test1
all:../../$(BIN_DIR)
	$(CC) $(CC_FLAGS) -o $(result_ex) main.cpp -I../../$(INCLUDE_DIR) $(CC_PROJECT_PARAMS)
../../$(BIN_DIR):
	mkdir -p $@
clean:
	rm -f $(result_ex)

