include MakeConf/inc.mk

all:test src
	
test:src
	cd $(TEST_DIR) && $(MAKE)
	
src:
	cd $(SRC_DIR) && $(MAKE)
	
clean:
	cd $(TEST_DIR) && $(MAKE_CLEAN)
	cd $(SRC_DIR) && $(MAKE_CLEAN)
	rm -rf $(LIB_DIR) $(BIN_DIR) $(OBJ_DIR)
.PHONY:test src


