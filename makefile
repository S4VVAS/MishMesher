LINKED= obj_importer.c mesher.c

mesh: clean
	gcc $(LINKED) main.c -o mishmesh

clean:
	rm -f ./mishmesh *.o

#  model-path  long-resolution  edge-buffer-size chunk-size opt:output-file-name  opt:core-count
test: mesh
	./mishmesh "models/g7.obj" 1000 0 10
	
test_complex: mesh
	./mishmesh "models/Seahawk.obj" 1000 0 10
