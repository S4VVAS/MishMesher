 #include "mesher.h"

#if __STDC_VERSION__ < 201112L || __STDC_NO_ATOMICS__ == 1
#error "ATOMICS NOT SUPPORTED! CAN'T COMPILE"
#endif

unsigned int max_tree_depth;


void malloc_children(struct octree *node){
    //if(__sync_bool_compare_and_swap(&node->isMallocing, false, true)){
        //We got the lock
       // printf("Thread #%d is mallocing children\n", 0);
        node->children = (struct octree*)malloc(sizeof(struct octree) * 8);
        for(int i = 0; i < 8; i++){
            node->children[i].is_voxels_solid = 0;
            node->children[i].level = node->level - 1;
            node->children[i].where_in_parent = i;
            node->children[i].isMallocing = false;
            node->children[i].hasChildren = false;
            node->children[i].is_inside = true;
            node->children[i].children = NULL;
            node->children[i].parent = node;
        }
        node->hasChildren = true;
        node->is_voxels_solid = 0;
        //node->isMallocing = false;
   // }
   // while(__sync_bool_compare_and_swap(&node->isMallocing, false, false)){
        //printf("Thread #%d is waiting for children malloc\n", 0);
        //Spinlock, wait untill children have been malloced by other thread
        //Prone to deadlocks
    //}
   // return;
  
}

struct vector3 to_vector3(double* v){
    struct vector3 vec = {v[0],v[1],v[2]};
    return vec;
}


void tree_intersections(struct aabb box, struct tri* triangle, struct octree* node, double box_size){
    
    //For every child in the current node check for intersections
    double b_div_2 = box_size * 0.5;
    struct aabb aabbs[8];
    aabbs[0] = (struct aabb){box.max.x, box.max.y, box.max.z,
        box.max.x - b_div_2, box.max.y - b_div_2, box.max.z - b_div_2};
    aabbs[1] = (struct aabb){box.max.x, box.max.y, box.max.z - b_div_2, 
        box.max.x - b_div_2, box.max.y - b_div_2, box.min.z};

    aabbs[2] = (struct aabb){box.max.x - b_div_2, box.max.y, box.max.z, 
        box.min.x, box.max.y - b_div_2, box.max.z - b_div_2};
    aabbs[3] = (struct aabb){box.max.x - b_div_2, box.max.y, box.max.z  - b_div_2, 
        box.min.x, box.max.y - b_div_2, box.min.z};

    aabbs[4] = (struct aabb){box.max.x, box.max.y  - b_div_2, box.max.z, 
        box.max.x - b_div_2, box.min.y, box.max.z - b_div_2};
    aabbs[5] = (struct aabb){box.max.x, box.max.y  - b_div_2, box.max.z - b_div_2, 
        box.max.x - b_div_2, box.min.y, box.min.z};

    aabbs[6] = (struct aabb){box.max.x - b_div_2, box.max.y  - b_div_2, box.max.z, 
        box.min.x, box.min.y, box.max.z - b_div_2};
    aabbs[7] = (struct aabb){box.max.x - b_div_2, box.max.y  - b_div_2, box.max.z - b_div_2, 
        box.min.x, box.min.y, box.min.z};

    //If tree is lowest level, look at char leafs
    //Level <= 1 as the last level is the leaf nodes
    if(node->level <= 1){
        uint8_t mask = 0; // 00000000
        for(int i = 0; i < 8; i++){
            if(intersects(&aabbs[i], triangle, b_div_2)){
                //Shift by i to set correct child
                mask = mask | 1 << i;
            }
        }
        node->is_voxels_solid = node->is_voxels_solid | mask;
        return;
    }

    for(int i = 0; i < 8; i++){
        if(intersects(&aabbs[i], triangle, b_div_2)){
            if(!node->hasChildren)
                malloc_children(node);
            tree_intersections(aabbs[i], triangle, &node->children[i], b_div_2);
        }
        //If not intersect, do nothing
    }
}

unsigned int neighbouring_cells[8][3] = {
    {2,1,4},
    {0,3,5},
    {3,0,6},
    {1,2,7},

    {5,6,0},
    {7,4,1},
    {4,7,2},
    {6,5,3}
};

int dir_of_parent_cells[8][3] = {
    {4,3,1},
    {5,4,1},
    {3,2,1},
    {2,5,1},

    {3,4,0},
    {4,5,0},
    {2,3,0},
    {5,2,0}
};

int mirrored_traverse[6][8] = {
    {4,5,6,7,0,1,2,3}, //Up
    {4,5,6,7,0,1,2,3}, //Down
    {2,3,0,1,6,7,4,5}, //Right
    {1,0,3,2,5,4,7,6}, //Back
    {2,3,0,1,6,7,4,5}, //Left
    {1,0,3,2,5,4,7,6} //Front
};

bool is_neighbour_filled(struct octree* node, bool dir_check[6], unsigned int path[max_tree_depth], unsigned int dir_from){    
   
    
    printf("lvl -> %d\n", node->level);
    for(int i = 0; i < 6; i++)
        printf("%d ",dir_check[i]);
    printf("\ndir %d -> %d to %d \n", dir_from, path[node->level-1], mirrored_traverse[dir_from][path[node->level-1]]);
    

    if(!node->is_inside)
        return true;
    if(!node->hasChildren)
        return false;

    return is_neighbour_filled(&node->children[mirrored_traverse[dir_from][path[node->level-1]]], dir_check, path, dir_from);
}

bool has_filled_neigbour(struct octree* node, bool dir_check[6], unsigned int path[max_tree_depth]){
    //Traverse upp to find a neighbour, as soon as its found return true
    //if we reach top level node and nothing, return false
    //If a side has been checked set to true, or false, decide later, and then dont check that side again

    //keep track of path from child, so that we can mirror traverse into negbouring 
    //  parents children to find the actuall neighbour of the child
    printf("\n\n--------------PARENT JUMP--------------\n");
    //We add the path of the child to the path array
    

    //if the parent in the current node is NULL, meaning 
    //  the current node is the parent, then the cell's neighbour has 
    //  to be the edge, which is not inside
    if(node->parent == NULL)
        return true;

    path[(node->level)-1] = node->where_in_parent;
    struct octree* parent = node->parent;

    for(int i = 0; i < 3; i++){
        printf("\n\nNE>W\n");
        

        //if the node direction has not been visited, check if filled
        if( dir_check[dir_of_parent_cells[node->where_in_parent][i]] == false){
            dir_check[dir_of_parent_cells[node->where_in_parent][i]] = true;
            struct octree c_node = parent->children[neighbouring_cells[node->where_in_parent][i]];
            if(is_neighbour_filled(&c_node, dir_check, path, node->where_in_parent))
                return true;
            //Dir not inside, mark that direction has been checked
        }
    }
    //if all directions within the parent have been checked, continue untill we reach a node
    // where missing directions are able to be checked
    return has_filled_neigbour(parent, dir_check, path);
}




//Bool is_inside
void fill_model(struct octree* node){
    printf("\n\n||||||||||||||||||NEW NODE|||||||||||||||||||\n");

    //if the node has children, go to children
    if(node->level <= 1){

    }
    else if(!node->hasChildren){
        bool dir_check[6] = {false};
        unsigned int path[max_tree_depth];
        if(has_filled_neigbour(node, dir_check, path))
            node->is_inside = false;
    }
    else if(node->hasChildren){
        for(int i = 0; i < 8; i++)
            if(!node->children[i].hasChildren){
                bool dir_check[6] = {false};
                unsigned int path[max_tree_depth];
                if(has_filled_neigbour(&node->children[i], dir_check, path))
                    node->children[i].is_inside = false;
            }
        for(int i = 0; i < 8; i++)
            if(node->children[i].hasChildren)
                fill_model(&node->children[i]);
    }
    //If the node has no children, then we should mark it
        //so we call has_filled_neigbour to know if we should mark it
}




void mesh(int long_resolution, struct model* model){
    double x_len = len(model->x_min, model->x_max);
    double y_len = len(model->y_min, model->y_max);
    double z_len = len(model->z_min, model->z_max);

    double min_model_coord = min(model->z_min, min(model->x_min, model->y_min));
    double max_model_coord = max(model->z_max, max(model->x_max, model->y_max));
    double model_len = len(min_model_coord,max_model_coord);
    double cell_size_domain = max(x_len, max(y_len, z_len)) / long_resolution;

    printf("Max domain length: %f -> Final cell size: %f\n",  model_len, cell_size_domain);
    double min_size = model_len;
    max_tree_depth = 0;

    while((min_size /= 2.0) >= cell_size_domain)
        max_tree_depth++;

    printf("Octree max depth: %d\n", max_tree_depth);
    printf("Generating model containing %f cells...\n", 1.0 * long_resolution * long_resolution * long_resolution );

    //Malloc an octree for each layer in model
    struct octree* roots = (struct octree*)malloc(sizeof(struct octree) * model->n_layers);
    printf("Blocking faces in Layer:\n");

    //for each material/layer
    for(int i = 0; i < model->n_layers; i++){
        printf("%d..\n", i);
        //Malloc octree root children and set level
        roots[i].level = max_tree_depth;
        malloc_children(&roots[i]);

        //for each face
        //#omp parallel for
        for(int f = 0; f < model->sizes[i][0]; f++){
            //Get vertices for current face
            double* v1 = model->points[model->groups[i].faces[f][0] - 1];
            double* v2 = model->points[model->groups[i].faces[f][1] - 1];
            double* v3 = model->points[model->groups[i].faces[f][2] - 1];
            double* v4 = model->groups[i].faces[f][3] != 0 ? model->points[model->groups[i].faces[f][3] - 1] : NULL;
            //printf("\n");
            //Create AABB the size of the domain
            struct aabb box = {max_model_coord,max_model_coord,max_model_coord, min_model_coord,min_model_coord,min_model_coord};

            //If the face is a triangle
            if(v4 == NULL){
                struct tri triangle = {to_vector3(v1), to_vector3(v2), to_vector3(v3)};
                tree_intersections(box, &triangle, &roots[i], model_len);
            }
            //Else its a square, split into 2 separate triangles and do intersection test on each
            else{
                struct tri triangle_a = {to_vector3(v1), to_vector3(v2), to_vector3(v4)};
                struct tri triangle_b = {to_vector3(v4), to_vector3(v2), to_vector3(v3)};
                tree_intersections(box, &triangle_a, &roots[i], model_len);
                tree_intersections(box, &triangle_b, &roots[i], model_len);
            }
            
        }

        
        if(model->groups[i].is_hollow == 1)
            fill_model(&roots[i]);
        //else if(model->groups[i].is_hollow == 2)
            //adaptive_fill_model(&roots[i]);

        char path[256];
        sprintf(path, "obj_converted/%d.obj", i);
        obj_convert(&roots[i], path, model_len);
    }

    free(roots);

}


