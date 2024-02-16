#include "mesher.h"
/*
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
*/

#if __STDC_VERSION__ < 201112L || __STDC_NO_ATOMICS__ == 1
#error "ATOMICS NOT SUPPORTED! CAN'T COMPILE"
#endif




bool atomic_set(struct octree *node, int solid_child){
    return 0;
}

void malloc_children(struct octree node){
    if(__sync_bool_compare_and_swap(&node.isMallocing, false, true)){
        //We got the lock
       // printf("Thread #%d is mallocing children\n", 0);
        node.children = (struct octree*)malloc(sizeof(struct octree) * 8);
        for(int i = 0; i < 8; i++)
            node.children[i].level = node.level + 1;
        node.hasChildren = true;
        node.isMallocing = false;
        return;
    }
    while(__sync_bool_compare_and_swap(&node.isMallocing, false, false)){
        //printf("Thread #%d is waiting for children malloc\n", 0);
        //Spinlock, wait untill children have been malloced by other thread
        //Prone to deadlocks
    }
    return;
  
}

struct vector3 to_vector3(double* v){
    struct vector3 vec = {v[0],v[1],v[2]};
    return vec;
}


void mesh(int long_resolution, struct model* model){
    double model_long_side = max(model->z_max, max(model->x_max, model->y_max));
    double cell_size_domain = model_long_side / long_resolution;
    printf("Each cell in the domain of the model is of size: %f\n", cell_size_domain);
    double min_size = model_long_side;
    int max_tree_depth = 0;
    //We ommit the last layer (by dividing first) due to uint8_t containing the leafs.
    while((min_size /= 2.0) >= cell_size_domain)
        max_tree_depth++;

    printf("Max tree depth: %d\n", max_tree_depth);
    printf("Generating model containing %d cells...\n", long_resolution * long_resolution * long_resolution);

    //CL TEST
    /*
    cl_device_id id;
    cl_uint n_compute_units;
    cl_ulong memory;

    int result = clGetDeviceIDs(NULL,CL_DEVICE_TYPE_GPU, 1, &id, NULL);
    if(result != CL_SUCCESS){
        printf("something went wrong");
        return;
    }

   // clGetDeviceInfo(id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &n_compute_units, NULL);
   // clGetDeviceInfo(memory, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &memory, NULL);

   // printf("MAX C UNITS: %u\n",n_compute_units);
   // printf("MEMORY AVAILABLE: %uGB\n",memory/1000000000L);

*/

    


    //Malloc an octree for each layer in model
    struct octree* roots = (struct octree*)malloc(sizeof(struct octree) * model->n_layers);
    printf("Blocking faces in Layer:\n");
    //for each material/layer
    for(int i = 0; i < model->n_layers; i++){
        //printf("%d..\t", i);
        //Malloc octree root children and set level
        roots[i].level = 0;
        malloc_children(roots[i]);


        //for each face
        //#omp parallel for
        for(int f = 0; f < model->sizes[i][0]; f++){
            
            double* v1 = model->points[model->groups[i].faces[f][0] - 1];
            double* v2 = model->points[model->groups[i].faces[f][1] - 1];
            double* v3 = model->points[model->groups[i].faces[f][2] - 1];
            double* v4 = model->groups[i].faces[f][3] != 0 ? model->points[model->groups[i].faces[f][3] - 1] : NULL;
            
            unsigned int track[max_tree_depth];

            double temp_curr_size = model_long_side;
            //Each time we enter a child, we divide size by 1/2 untill we reach cell size domain * 2, next layer is children = modify char statuses
            while(temp_curr_size >= cell_size_domain * 2){
                temp_curr_size /= 2;
            }

            struct aabb a = {model_long_side,model_long_side,model_long_side, 0,0,0};
            struct tri b = {to_vector3(v1), to_vector3(v2), to_vector3(v3)};

            printf("%d\n", intersects(&a, &b));
            //1 so basically do an intersection test for each level of the octree in a loop
            //  1.1 Go to child voxel that is a hit
            //  1.2 Check level, is it the max level? 
            //      If not make 1.3
            //  1.3 Check hasChildren, if it has children do intersection test
            //      if not, create children first and then do 1.3
            



            //Basically populate a octree for each material with the voxel states
            //Read out the trees into format

        }
        if((i+1)%10 == 0)
            printf("\n");
    }
}