#include <stdio.h>
#include <stdlib.h>
#include "obj_importer.h"

int buffer, resolution, max_cores;

int init(char** argv){
    import_mesh(argv[1]);

    return 0;
}

//CURRENT ARGS
//  model-path  resolution  edge-buffer-size  opt:output-file-name  opt:core-count
int main(int argc, char** argv){
    init(argv);


    
    //Check for correct num of arguments
    //Setup using arguments
    
   return 0;
}


//