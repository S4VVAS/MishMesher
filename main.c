#include <stdio.h>
#include <stdlib.h>
#include "obj_importer.h"


int buffer, resolution, max_cores;




//CURRENT ARGS
//  model-path  resolution  edge-buffer-size  opt:output-file-name  opt:core-count
int main(int argc, char** argv){
    struct model* model = import_mesh(argv[1]);

    destroy_model(model);
    //Check for correct num of arguments
    
    return 0;
}
