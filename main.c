#include <stdio.h>
#include <stdlib.h>

#include "obj_importer.h"
#include "mesher.h"

//CURRENT ARGS
//model-path  material-path  long-resolution
int main(int argc, char** argv){
    struct model* model = import_mesh(argv[1], argv[2]);

    //Obj or Material File not found
    if(model == NULL)
        return 1;

    /*Correct resolution if needed
    int res = atoi(argv[4]);
    int new_res = 2;
    while(new_res < res)
        new_res *= 2;
    if(res != new_res){
        res = new_res;
        printf("Warning: resolution not an exponent of 2, resizing up to nearest exponent -> %d\n", new_res);
    }*/

    mesh(atof(argv[4]), model, atoi(argv[5]), argv[3]);

    destroy_model(model);

    //destroy_c_mesh(c_mesh);
    return 0;
}
