include ../MakeConf/inc.mk
RENDERER_DIR=Renderer
WRAPPER_DIR=Wrapper
DIRS=$(RENDERER_DIR) $(WRAPPER_DIR)
objects_wrapper := $(call get_objects,$(WRAPPER_DIR))
objects_renderer := $(call get_objects,$(RENDERER_DIR))
result_library:=Renderer.so
all:../$(LIB_DIR)/ $(addprefix ../$(OBJ_DIR)/, $(RENDERER_DIR) $(WRAPPER_DIR) $(objects_renderer) $(objects_wrapper))
	$(CC) $(CC_FLAGS) -shared -fPIC -o ../$(LIB_DIR)/$(result_library) $(addprefix ../$(OBJ_DIR)/, $(objects_renderer) $(objects_wrapper)) $(CC_PROJECT_PARAMS)
../$(OBJ_DIR)/$(RENDERER_DIR)/%.o:$(RENDERER_DIR)/%.cpp ../$(OBJ_DIR)/$(objects_wrapper)
	$(CC) $(CC_FLAGS) -c $< -o $@ -I../$(INCLUDE_DIR) -I$(INCLUDE_EXTENDED_DIRS)
../$(OBJ_DIR)/$(WRAPPER_DIR)/%.o:$(WRAPPER_DIR)/%.cpp
	$(CC) $(CC_FLAGS) -c $< -o $@ -I../$(INCLUDE_DIR) -I$(INCLUDE_EXTENDED_DIRS) 
../$(OBJ_DIR)/$(RENDERER_DIR):
	mkdir -p $@
../$(OBJ_DIR)/$(WRAPPER_DIR):
	mkdir -p $@
../$(LIB_DIR)/:
	mkdir -p $@
clean:
	rm -rf $(addprefix ../$(OBJ_DIR)/, $(RENDERER_DIR) $(WRAPPER_DIR))
	rm -rf ../$(LIB_DIR)
	