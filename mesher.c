 #include "mesher.h"

#if __STDC_VERSION__ < 201112L || __STDC_NO_ATOMICS__ == 1
#error "ATOMICS NOT SUPPORTED! CAN'T COMPILE"
#endif

/*
----INFO ABOUT CELL PLACEMENT AND DIRECTIONS----
 _____   _____
|3  1|  |7  5|
|2__0|  |6__4|
 TOP    BOTTOM

            (t)
             0
             | 
             |  __ 2 (bk)
             |/
 (l) 5 - - -[#]- - - 3 (r)
           / |
 (fr) 3 __/  |
             |
             1
            (b)
*/








unsigned int max_tree_depth;
int cc;
long long start_time;

//Predefined mapped arrays------>

int mirrored_dir[6] = {1,0,3,2,5,4};

unsigned int deep_child_cells_in_dir[6][4] = {
    {0,1,3,2},
    {4,5,7,6},
    {3,1,5,7},
    {2,0,4,6},
    {0,1,5,4},
    {2,3,7,6}
};

int neighbour_directions[8][6] = {
    {-1, 4, 1, -1, -1, 2},
    {-1, 5, -1, 0, -1, 3},
    {-1, 6, 3, -1, 0, -1},
    {-1, 7, -1, 2, 1, -1},

    {0, -1, 5, -1, -1, 6},
    {1, -1, -1, 4, -1, 7},
    {2, -1, 7, -1, 4, -1},
    {3, -1, -1, 6, 5, -1}
};

unsigned int mirrored_traverse[6][8] = {
    {4,5,6,7,0,1,2,3}, //Up
    {4,5,6,7,0,1,2,3}, //Down
    {1,0,3,2,5,4,7,6}, //Back
    {1,0,3,2,5,4,7,6}, //Front
    {2,3,0,1,6,7,4,5}, //Right
    {2,3,0,1,6,7,4,5}  //Left
};

void malloc_children(struct octree *node){
    //Bypass the critical secion if not needed
    if(!node->hasChildren){
        #pragma omp critical
        {
            //Double check that the children still dont exist when entering the critical section
            if(!node->hasChildren){
                node->children = (struct octree*)malloc(sizeof(struct octree) * 8);
                for(int i = 0; i < 8; i++){
                    node->children[i].is_voxels_solid = 0;
                    node->children[i].hasChildren = false;
                    node->children[i].is_inside = true;
                    node->children[i].where_in_parent = i;
                    node->children[i].level = node->level - 1;
                    node->children[i].children = NULL;
                    node->children[i].parent = node;
                }
                node->is_voxels_solid = 0;
                node->hasChildren = true;
            }
        }
    }
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

    double leaf_div_2 = b_div_2 * 0.5;
    //If tree is lowest level, look at char leafs
    //Level <= 1 as the last level is the leaf nodes
    if(node->level <= 1){
        uint8_t mask = 0; // 00000000
        for(int i = 0; i < 8; i++){
            if(intersects(&aabbs[i], triangle, leaf_div_2)){
                //Shift by i to set correct child
                mask = mask | 1 << i;
            }
        }
        node->is_voxels_solid = node->is_voxels_solid | mask;
        return;
    }

    for(int i = 0; i < 8; i++){
        if(intersects(&aabbs[i], triangle, b_div_2)){
            malloc_children(node);
            tree_intersections(aabbs[i], triangle, &node->children[i], b_div_2);
        }
        //If not intersect, do nothing
    }
}

void fill_lvl1(struct octree* node){
    bool is_solid[8];

    for(int i = 0; i < 8; i++)
        is_solid[i] = node->is_voxels_solid & 1 << i;
    
    //For all neighbours, check if filled
    for(int i = 0; i < 6; i++)
        if(node->neighbours[i] != NULL && node->neighbours[i]->is_inside && (node->neighbours[i]->is_voxels_solid == 255 || node->neighbours[i]->is_voxels_solid == 0)){
            //Upper level
            for(int cell = 0; cell < 4; cell++)
                if(!is_solid[deep_child_cells_in_dir[i][cell]]){
                    if(is_solid[deep_child_cells_in_dir[mirrored_dir[i]][cell]])
                        is_solid[deep_child_cells_in_dir[i][cell]] = true;
                }
        }
    uint8_t mask = 0; //bits = 00000000
    for(int i = 0; i < 8; i++)
        if(is_solid[i])
            mask = mask | 1 << i;
    node->is_voxels_solid = node->is_voxels_solid | mask;
}

void fill_node(struct octree* node){
    if(node->level <= 1){
        node->is_voxels_solid = 255;
    }
    else{
        malloc_children(node);
        for(int i = 0; i < 8; i++)
            fill_node(&node->children[i]);
    }
}

void fill_voids(struct octree* node){
    if(!node->hasChildren && node->is_inside){
        if(node->is_voxels_solid == 0)
            fill_node(node);
        else
            fill_lvl1(node);
    }
    else if(node->hasChildren)
        for(int i = 0; i < 8; i++)
            fill_voids(&node->children[i]);
}

struct octree* get_neighbour_trav_down_deep(struct octree* node, int dir_to){
    if(node->hasChildren)
        for(int i = 0; i < 4; i++)
            return get_neighbour_trav_down_deep(&node->children[deep_child_cells_in_dir[mirrored_dir[dir_to]][i]], dir_to);
    return node;
}

struct octree* get_neighbour_trav_down(struct octree* node, int path[max_tree_depth], int dir_to, int max_path_depth){
    if(node->hasChildren){
        if(node->level <= max_path_depth){
            //We have reached the end of the path that the original node was at
            return get_neighbour_trav_down_deep(node, dir_to);
        }
        return get_neighbour_trav_down(
            &node->children[mirrored_traverse[dir_to][path[node->level-1]]], 
            path, dir_to, max_path_depth);
    }
    return node;   
}

struct octree* get_neighbour_trav_up(struct octree* node, int path[max_tree_depth], int direction, int max_path_depth){
    //If parent == NULL, we have reached the root, meaning there is no child in the specified direction
    if(node->parent == NULL)
        return NULL;

    int c_index = neighbour_directions[node->where_in_parent][direction];

    if(c_index != -1){
        //Has a neighbour
        if(node->parent->children[c_index].hasChildren)
            return get_neighbour_trav_down(&node->parent->children[c_index], path, direction, max_path_depth);
        //If the neighbour has no childrent, return the neighbour
        return &node->parent->children[c_index];
    }
    //No neighbour in specified direction
    //Add path we should take from the parent to this node
    path[node->parent->level - 1] = node->where_in_parent;
    //Does not have a neighbour, traverse up
    return get_neighbour_trav_up(node->parent, path, direction, max_path_depth);
}

void calculate_neighbours(struct octree* node){
    //Calculate neighbours for the node itself
    for(int i = 0; i < 6; i++){
        int path[max_tree_depth];
        node->neighbours[i] = get_neighbour_trav_up(node, path, i, node->level);
    }
    //Then if node has children, calculate neighbours for those children
    if(node->hasChildren)
        #pragma omp parallel for num_threads(cc > 8 ? 8 : cc) shared(node)
        for(int i = 0; i < 8; i++)
            calculate_neighbours(&node->children[i]);
}

struct octree* get_corner(struct octree* node, int corner_dir){
    if(!node->hasChildren){
        if(node->level == 1)
            return NULL;
        else
            return node;
    }   
    if(node->hasChildren)
        return get_corner(&node->children[corner_dir], corner_dir);
    return NULL;
}

void flood_fill(struct octree* node){
    node->is_inside = false;

    if(node->hasChildren)
        for(int i = 0; i < 8; i++)
            flood_fill(&node->children[i]);
    for(int i = 0; i < 6; i++)
        if(node->neighbours[i] != NULL && node->neighbours[i]->is_voxels_solid == 0 && node->neighbours[i]->is_inside)
            flood_fill(node->neighbours[i]);
}

void fill_mode_fill(struct octree* root){
    if(!root->hasChildren)
        return;
    //Flood from every corner
   #pragma omp parallel for num_threads(cc > 8 ? 8 : cc) shared(root)
    for(int i = 0; i < 8; i++){
        struct octree* corner = get_corner(&root->children[i], i);
        //If inside, meaning not touched, touch
        //If NULL then the corner cell is a wall, so skip that corneer
        if(corner != NULL && corner->is_inside){
            flood_fill(corner);
        }
    }
}



void flood_nodes(struct octree* root){
    
}

void intersect_master(struct octree* master, struct octree** trees, int n_layers, int start_from){
     if(master != NULL){
        if(!master->hasChildren && master->is_voxels_solid > 0){
         for(int i = start_from; i < n_layers; i++)
            if(trees[i] != NULL){
                uint8_t and = trees[i]->is_voxels_solid & master->is_voxels_solid;
                trees[i]->is_voxels_solid = trees[i]->is_voxels_solid ^ and;
            }
        }
        if(master->hasChildren){
            //Loop through all children in the master tree
            for(int m = 0; m < 8; m++){
                struct octree** children = (struct octree**)malloc(n_layers * sizeof(struct octree*));
                //For every tree, follow the path in master by also going to the same child
                for(int i = start_from; i < n_layers; i++)
                    children[i] = trees[i] != NULL && trees[i]->hasChildren ? &trees[i]->children[m] : NULL;
                intersect_master(&master->children[m], children, n_layers, start_from);
                free(children);
            }
        }
    }

}

void intersect_trees(struct octree* roots, int n_layers){
    struct octree** trees = (struct octree**)malloc(n_layers * sizeof(struct octree*));
    for(int i = 0; i < n_layers; i++)
        trees[i] = &roots[i];
    for(int i = 0; i < n_layers; i++)
        if(&(roots[i]) != NULL)
            //i + 1 skipps the nodes that are/ have been masters
            intersect_master(&roots[i], trees, n_layers, i + 1);
    free(trees);
}

void mesh(int long_resolution, struct model* model, int core_count, char* out_path){
    cc = core_count;
    start_time = timeInMilliseconds();

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

    long long c_time = timeInMilliseconds();
    //for each material/layer
    for(int i = 0; i < model->n_layers; i++){
        printf("%d..\n", i);
        //Malloc octree root children and set level
        roots[i].level = max_tree_depth;
        roots[i].parent = NULL;
        malloc_children(&roots[i]);

        //for each face
        #pragma omp parallel for num_threads(cc) shared(min_size,max_tree_depth, cc, start_time, x_len, y_len, z_len, model_len, cell_size_domain, min_model_coord, max_model_coord, roots, model)
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
        printf("intersections time: \t\t %llu ms\n", timeInMilliseconds() - c_time);
        if(model->groups[i].is_hollow){
            c_time = timeInMilliseconds();
            calculate_neighbours(&roots[i]);
            printf("calculate_neighbours time: \t %llu ms\n", timeInMilliseconds() - c_time);

            c_time = timeInMilliseconds();
            fill_mode_fill(&roots[i]);
            printf("fill_mode_fill time: \t\t %llu ms\n", timeInMilliseconds() - c_time);

            c_time = timeInMilliseconds();
            fill_voids(&roots[i]);
            printf("fill_voids time: \t\t %llu ms\n", timeInMilliseconds() - c_time);

        }
        //else if(model->groups[i].is_hollow == 2)
            //adaptive_fill_model(&roots[i]);
        

      

        
    }


    c_time = timeInMilliseconds();
    intersect_trees(roots, model->n_layers);
    printf("\nall materials intersect time: \t\t %llu ms\n", timeInMilliseconds() - c_time);

    mish_convert(roots, model->n_layers, out_path, model_len, cc);

    /*for(int i = 0; i < model->n_layers; i++){
       char path[256];
       sprintf(path, "obj_converted/%d.obj", i);
       obj_convert(&roots[i], path, model_len);
    }*/

    free(roots);

    printf("\nWALL-Time: \t\t\t %llu ms\n", timeInMilliseconds() - start_time);
}
