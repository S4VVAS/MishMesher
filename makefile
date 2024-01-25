
mesh: clean

clean:
	rm -f ./mishmesh *.o

#  model-path  resolution  edge-buffer-size  opt:output-file-name  opt:core-count
test: mesh
	./mishmesh "models/g7.obj" 100 10
