LINKED= obj_importer.c mesher.c math_f.c obj_converter.c mish_converter.c stack.c
OS := $(shell uname)

OBJ_DIR = obj_converted
OBJ_FILES = $(wildcard $(OBJ_DIR)/*.obj)

OPTIONS:=

#CHANGE FOR WINDOWS LATER
ifeq ($(OS),Windows)
#openMP
OPTIONS += -lomp -fopenmp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include
endif

ifeq ($(OS),Darwin)
#openMP
OPTIONS += -Xpreprocessor -fopenmp -lomp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include
endif

ifeq ($(OS),Linux)
#openMP
OPTIONS += -fopenmp

endif

mesh: clean
	gcc $(LINKED) main.c -o mishmesh $(OPTIONS) -O

clean:
	rm -f ./mishmesh *.o
	rm -f $(OBJ_FILES)

#PARAMS:  model-path  material-path out-file-name  long-resolution  core-count
test: mesh
	./mishmesh "models/g7.obj" "models/g7.mprop" "msh_converted/mish_file_test" "5" 8
	
test_simple: mesh
	./mishmesh "models/square.obj" "models/square.mprop" "msh_converted/mish_file_simple" "5" 8
	
test_complex: mesh
	./mishmesh "models/Seahawk.obj" "models/Seahawk.mprop" "msh_converted/mish_file_complex" "0.5" 8
	
test_extreme: mesh
	./mishmesh "models/Seahawk_fixed.obj" "models/bridge.mprop" "msh_converted/mish_file_extreme" "5" 8

