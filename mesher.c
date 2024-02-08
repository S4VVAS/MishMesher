#include "mesher.h"

struct c_mesh* create_c_mesh(int long_resolution, struct model* model){
    struct c_mesh* c_mesh = (struct c_mesh*)malloc(sizeof(struct c_mesh));
    c_mesh->size = long_resolution;
    c_mesh->n_materials = model->n_layers;
    c_mesh->material_offsets = (unsigned int*)malloc(sizeof(unsigned int*) * model->n_layers);
    //Allocates memory for all coordinates
    c_mesh->n = long_resolution * long_resolution * long_resolution;
    c_mesh->blocks = (unsigned int**)malloc(sizeof(unsigned int*) * c_mesh->n);

   //for(int i = 0; i < c_mesh->n; i++)
    //    c_mesh->blocks[i] = (unsigned int*)malloc(sizeof(unsigned int) * 6);

    return c_mesh;
}

void destroy_c_mesh(struct c_mesh* c_mesh){
   // for(int i = 0; i < c_mesh->n; i++)
    //    free(c_mesh->blocks[i]);
    free(c_mesh->blocks);
    free(c_mesh->material_offsets);
    free(c_mesh);

}



void mesh(int long_resolution, struct model* model){}