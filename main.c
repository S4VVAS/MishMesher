#include <stdio.h>
#include <stdlib.h>
#include "obj_importer.h"
#include "cartesian_mesh.h"


int buffer, resolution, max_cores;

struct c_mesh* create_c_mesh(int long_resolution, struct model* model){
    struct c_mesh* c_mesh = (struct c_mesh*)malloc(sizeof(struct c_mesh));
    c_mesh->size = long_resolution;
    c_mesh->n_materials = model->n_layers;
    c_mesh->material_offsets = (unsigned int*)malloc(sizeof(unsigned int*) * model->n_layers);
    c_mesh->blocks = (unsigned int**)malloc(sizeof(unsigned int*) * (long_resolution * long_resolution * long_resolution));

    return c_mesh;
}

void destroy_c_mesh(struct c_mesh* c_mesh){
    free(c_mesh->blocks);
    free(c_mesh->material_offsets);
    free(c_mesh);

}


//CURRENT ARGS
//  model-path  long_resolution  edge-buffer-size  opt:output-file-name  opt:core-count
int main(int argc, char** argv){
    struct model* model = import_mesh(argv[1]);
    struct c_mesh* c_mesh = create_c_mesh(atoi(argv[2]), model);

    destroy_model(model);

    destroy_c_mesh(c_mesh);
    return 0;
}
