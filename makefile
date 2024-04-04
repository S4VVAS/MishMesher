LINKED= obj_importer.c mesher.c math_f.c obj_converter.c mish_converter.c
OS := $(shell uname)

OBJ_DIR = obj_converted
OBJ_FILES = $(wildcard $(OBJ_DIR)/*.obj)

OPTIONS:=

#CHANGE FOR WINDOWS LATER
ifeq ($(OS),Windows)
OPTIONS += -framework OpenCL 
endif

ifeq ($(OS),Darwin)
#openCL
OPTIONS += -framework OpenCL -Xpreprocessor 
#openMP
OPTIONS += -fopenmp -lomp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include
endif



mesh: clean
	gcc -g $(LINKED) main.c -o mishmesh $(OPTIONS)

clean:
	rm -f ./mishmesh *.o
	rm -f $(OBJ_FILES)

#   			model-path  material-path out-file-name  long-resolution  core-count
test: mesh
	./mishmesh "models/g7.obj" "models/g7.mprop" "msh_converted/mish_file_test" 1000 10 
	
test_simple: mesh
	./mishmesh "models/square.obj" "models/square.mprop" "msh_converted/mish_file_simple" 60 8
	
test_complex: mesh
	./mishmesh "models/Seahawk.obj" "models/Seahawk.mprop" "msh_converted/mish_file_complex" 2000 10
	
test_extreme: mesh
	./mishmesh "models/bridge.obj" "models/bridge.mprop" "msh_converted/mish_file_extreme" 1000 10

