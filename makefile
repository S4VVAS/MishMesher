LINKED= obj_importer.c mesher.c math_f.c obj_converter.c
OS := $(shell uname)

OBJ_DIR = obj_converted
OBJ_FILES = $(wildcard $(OBJ_DIR)/*.obj)

OPTIONS:=

ifeq ($(OS),Darwin)
	OPTIONS += -framework OpenCL
endif




mesh: clean
	gcc -g $(LINKED) main.c -o mishmesh $(OPTIONS)

clean:
	rm -f ./mishmesh *.o
	rm -f $(OBJ_FILES)

#  model-path  long-resolution  edge-buffer-size chunk-size opt:output-file-name  opt:core-count
test: mesh
	./mishmesh "models/g7.obj" "models/g7.mprop" 500 0 10
	
test_simple: mesh
	./mishmesh "models/square.obj" "models/square.mprop" 60 0 10
	
test_complex: mesh
	./mishmesh "models/Seahawk.obj" "models/Seahawk.mprop" 500 0 10
	
test_extreme: mesh
	./mishmesh "models/bridge.obj" "models/bridge.mprop" 1000 0 10

