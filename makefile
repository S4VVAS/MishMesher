LINKED= obj_importer.c mesher.c math_f.c
OS := $(shell uname)
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

#  model-path  long-resolution  edge-buffer-size chunk-size opt:output-file-name  opt:core-count
test: mesh
	./mishmesh "models/g7.obj" 64 0 10
	
test_complex: mesh
	./mishmesh "models/Seahawk.obj" 1024 0 10
