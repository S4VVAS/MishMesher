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

double max(double i, double j){
    return i > j ? i : j;
}

double min(double i, double j){
    return i < j ? i : j;
}




void mesh(int long_resolution, struct model* model){
    
    double cell_size_factor = max(model->z_max, max(model->x_max, model->y_max)) / long_resolution;
    printf("each cell in the domain of the model is of size: %f\n", cell_size_factor);

    //for each material
    for(int i = 0; i < model->n_layers; i++){
        //for each face
        //printf("%d\n\n", model->sizes[i][0]);
        for(int f = 0; f < model->sizes[i][0]; f++){
            unsigned int *v_offsets = model->groups[i].faces[f];

            //printf("%d\n", v_offsets[3] == NULL);

            double* o1 = model->points[v_offsets[0] - 1];
            double* o2 = model->points[v_offsets[1] - 1];
            double* o3 = model->points[v_offsets[2] - 1];
            //double* o4 = model->groups[i].points[v_offsets[3]];

            int in = v_offsets[1] - 1;

            if(i < 3){
            printf("%d\n", in);
            printf("%f\n\n", model->points[in][0]);
            }


         

            
            //double* v1 = model->groups[i].points[facs[0]];
            //double* v2 = model->groups[i].points[model->groups[i].faces[f][1]];
            //double* v3 = model->groups[i].points[model->groups[i].faces[f][2]];

          

           // printf("%f %f %f \n",model->groups[i].points[v_offsets[0]][0],model->groups[i].points[v_offsets[0]][1], model->groups[i].points[v_offsets[0]][2]);
           // printf("%f %f %f \n",model->groups[i].points[v_offsets[1]][0],model->groups[i].points[v_offsets[1]][1], model->groups[i].points[v_offsets[1]][2]);



        }
        //model->groups->points

    }


}