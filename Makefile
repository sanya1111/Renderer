include MakeConf/inc.mk

all:test src
	
test:src
	$(MAKE) -C $(TEST_DIR)
	
src:
	$(MAKE) -C $(SRC_DIR)
	
clean:
	$(MAKE_CLEAN) -C $(TEST_DIR)
	$(MAKE_CLEAN) -C $(SRC_DIR)
	rm -rf $(LIB_DIR) $(BIN_DIR) $(OBJ_DIR)
.PHONY:test src


