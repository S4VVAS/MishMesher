#include <stdio.h>
#include <stdlib.h>

#include "obj_importer.h"
#include "mesher.h"

//CURRENT ARGS
//  model-path  long_resolution  edge-buffer-size  opt:output-file-name  opt:core-count
int main(int argc, char** argv){
    struct model* model = import_mesh(argv[1]);
    //struct c_mesh* c_mesh = create_c_mesh(atoi(argv[2]), model);
    mesh(atoi(argv[2]), model);

    destroy_model(model);

    //destroy_c_mesh(c_mesh);
    return 0;
}
