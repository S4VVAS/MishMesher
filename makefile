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
OPTIONS += -Xpreprocessor -fopenmp -lomp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include  -O 
endif

ifeq ($(OS),Linux)
#openMP
OPTIONS += -fopenmp

endif

mishmesh: clean
	gcc $(LINKED) main.c -o mishmesh $(OPTIONS)

clean:
	rm -f ./mishmesh *.o
	rm -f $(OBJ_FILES)

#PARAMS:  model-path  material-path out-file-name  cell-size  core-count
test: mishmesh
	./mishmesh "models/square.obj" "models/square.mprop" "msh_converted/mish_file_simple" "0.3" 1