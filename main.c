#include <stdio.h>
#include <stdlib.h>

#include "obj_importer.h"
#include "mesher.h"

//CURRENT ARGS
//  model-path  long_resolution  edge-buffer-size  opt:output-file-name  opt:core-count
int main(int argc, char** argv){
    struct model* model = import_mesh(argv[1]);
    //struct c_mesh* c_mesh = create_c_mesh(atoi(argv[2]), model);

    //Correct resolution if needed
    int res = atoi(argv[2]);
    int new_res = 2;
    while(new_res < res)
        new_res *= 2;
    if(res != new_res){
        res = new_res;
        printf("Warning: resolution not an exponent of 2, resizing up to nearest exponent -> %d\n", new_res);
    }

    mesh(res, model);

    destroy_model(model);

    //destroy_c_mesh(c_mesh);
    return 0;
}
