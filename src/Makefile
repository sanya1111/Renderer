include ../MakeConf/inc.mk
RENDERER_DIR=Renderer
WRAPPER_DIR=Wrapper
DIRS=$(RENDERER_DIR) $(WRAPPER_DIR)
objects_wrapper := $(call get_objects,$(WRAPPER_DIR))
objects_renderer := $(call get_objects,$(RENDERER_DIR))
result_library:=libRenderer.so
all:../$(LIB_DIR)/ $(addprefix ../$(OBJ_DIR)/, $(RENDERER_DIR) $(WRAPPER_DIR) $(objects_renderer) $(objects_wrapper))
	$(CXX) $(CXX_FLAGS) -shared -fPIC -o ../$(LIB_DIR)/$(result_library) $(addprefix ../$(OBJ_DIR)/, $(objects_renderer) $(objects_wrapper))  $(CXX_PROJECT_PARAMS) $(CXX_PROJECT_LIBS)
../$(OBJ_DIR)/$(RENDERER_DIR)/%.o:$(RENDERER_DIR)/%.cpp ../$(OBJ_DIR)/$(objects_wrapper)
	$(CXX) $(CXX_FLAGS) -c $< -o $@ -I../$(INCLUDE_DIR)  $(CXX_PROJECT_PARAMS) -fPIC
../$(OBJ_DIR)/$(WRAPPER_DIR)/%.o:$(WRAPPER_DIR)/%.cpp 
	$(CXX) $(CXX_FLAGS) -c $< -o $@ -I../$(INCLUDE_DIR) -I$(INCLUDE_EXTENDED_DIRS) $(CXX_PROJECT_PARAMS) -fPIC
../$(OBJ_DIR)/$(RENDERER_DIR):
	mkdir -p $@
../$(OBJ_DIR)/$(WRAPPER_DIR):
	mkdir -p $@
../$(LIB_DIR)/:
	mkdir -p $@
clean:
	rm -rf $(addprefix ../$(OBJ_DIR)/, $(RENDERER_DIR) $(WRAPPER_DIR))
	rm -rf ../$(LIB_DIR)
	