LINKED= obj_importer.c mesher.c math_f.c obj_converter.c
OS := $(shell uname)

OBJ_DIR = obj_converted
OBJ_FILES = $(wildcard $(OBJ_DIR)/*.obj)

OPTIONS:=

ifeq ($(OS),Darwin)
	OPTIONS += -framework OpenCL
else
	OPTIONS += -l OpenCL
endif




mesh: clean
	gcc $(LINKED) main.c -o mishmesh $(OPTIONS)

clean:
	rm -f ./mishmesh *.o
	rm -f $(OBJ_FILES)

#  model-path  long-resolution  edge-buffer-size chunk-size opt:output-file-name  opt:core-count
test: mesh
	./mishmesh "models/g7.obj" "models/g7.mat" 100 0 10
	
test_simple: mesh
	./mishmesh "models/square.obj" "models/square.mat" 60 0 10
	
test_complex: mesh
	./mishmesh "models/Seahawk.obj" "models/Seahawk.mat" 1000 0 10
