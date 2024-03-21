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
            if(!node->hasChildren)
                malloc_children(node);
            tree_intersections(aabbs[i], triangle, &node->children[i], b_div_2);
        }
        //If not intersect, do nothing
    }
}



/*






bool check_deeper_children(struct octree* node, unsigned int dir_to){
    if(node->level <= 1){
        uint8_t mask = node->is_voxels_solid;
        for(int i = 0; i < 4; i++)
            //Child is empty, then its not filled return false
            if(!(mask & 1 << deep_child_cells_in_dir[dir_to][i]))
                return false;
    }
    if(!node->is_inside)
            return true;
    if(node->hasChildren)
        for(int i = 0; i < 4; i++)
            return check_deeper_children(&node->children[deep_child_cells_in_dir[dir_to][i]], dir_to);
    return false;
}



bool is_neighbour_filled(struct octree* node, bool dir_check[6], int path[max_tree_depth], unsigned int dir_from){    
   //DET ÄR KANSKE BÄTTRE ATT BÖRJA FRÅN NODER SOM ÄR FYLLDA OCH PROPAGERA NER TILL NODER
   //SOM INTE ÄR FYLLDA
    
    //printf("lvl -> %d\n", node->level);
    //for(int i = 0; i < 6; i++)
    //    printf("%d ",dir_check[i]);
    
    //path > 7 signifies NULL, meaning the level has not been visited, return false
    //if the path > 7 then we should try to move as close to 
    if(path[node->level-1] > 7)
        return check_deeper_children(node, mirrored_dir[dir_from]);


    //printf("\ndir %d -> %d to %d \n", dir_from, path[node->level-1], mirrored_traverse[dir_from][path[node->level-1]]);
    

    if(!node->is_inside)
        return true;
    if(!node->hasChildren)
        return false;

    return is_neighbour_filled(&node->children[mirrored_traverse[dir_from][path[node->level-1]]], dir_check, path, dir_from);
}

bool has_filled_neigbour(struct octree* node, bool dir_check[6], int path[max_tree_depth]){
    //Traverse upp to find a neighbour, as soon as its found return true
    //if we reach top level node and nothing, return false
    //If a side has been checked set to true, or false, decide later, and then dont check that side again

    //keep track of path from child, so that we can mirror traverse into negbouring 
    //  parents children to find the actuall neighbour of the child
    //printf("\n\n--------------PARENT JUMP--------------\n");
    
    //This check might be too costly, remove if needed
    int dep = 0;
    for(int i = 0; i < 6; i++)
        if(dir_check[i] == true)
            dep++;
    if(dep >= 6)
        return false;
    


    //if the parent in the current node is NULL, meaning 
    //  the current node is the parent, then the cell's neighbour has 
    //  to be the edge, which is not inside
    if(node->parent == NULL)
        return true;

    //We add the path of the child to the path array
    path[(node->level)-1] = node->where_in_parent;
    struct octree* parent = node->parent;

    for(int i = 0; i < 3; i++){
        //printf("\n\nNE>W\n");
        

        //if the node direction has not been visited, check if filled
        if( dir_check[dir_of_parent_cells[node->where_in_parent][i]] == false){
            dir_check[dir_of_parent_cells[node->where_in_parent][i]] = true;
            struct octree c_node = parent->children[neighbouring_cells[node->where_in_parent][i]];
            if(is_neighbour_filled(&c_node, dir_check, path, dir_of_parent_cells[node->where_in_parent][i]))
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
    //printf("\n\n||||||||||||||||||NEW NODE|||||||||||||||||||\n");

    //if the node has children, go to children
    bool dir_check[6] = {false};
    int path[max_tree_depth];
    for(int c = 0; c < max_tree_depth; c++)
        path[c] = -1;


    if(!node->hasChildren){
        if(has_filled_neigbour(node, dir_check, path))
            node->is_inside = false;
    }
    else if(node->hasChildren){
        for(int i = 0; i < 8; i++)
            if(!node->children[i].hasChildren){
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

void fill_leafs(struct octree* node){

}





*/



void fill_node(struct octree* node){
    if(node->level <= 1){
        //CHANGE THIS VALUE!!!!!!
        node->is_voxels_solid = 255;
    }
    else{
        malloc_children(node);
        for(int i = 0; i < 8; i++)
            fill_node(&node->children[i]);
    }
}

void fill_voids(struct octree* node){
    if(node->level > 1 && !node->hasChildren && node->is_inside){
        fill_node(node);
    }
    else if(node->hasChildren)
        for(int i = 0; i < 8; i++)
            fill_voids(&node->children[i]);
}














unsigned int search_order[8][8] = {
    {0, 1,2,4, 5,6,3, 7},
    {1, 3,0,5, 2,7,4, 6},
    {2, 3,0,6, 7,4,1, 5}, 
    {3, 1,2,7, 0,6,5, 4},
    {4, 6,5,0, 2,1,7, 3},
    {5, 7,4,1, 0,3,6, 2},
    {6, 7,4,2, 3,0,5, 1},
    {7, 6,5,3, 2,1,4, 0}
};

unsigned int rev_search_order[8][8] = {
    {7, 6,5,3, 2,1,4, 0},
    {6, 7,4,2, 3,0,5, 1},
    {5, 7,4,1, 0,3,6, 2},
    {4, 6,5,0, 2,1,7, 3},
    {3, 1,2,7, 0,6,5, 4},
    {2, 3,0,6, 7,4,1, 5}, 
    {1, 3,0,5, 2,7,4, 6},
    {0, 1,2,4, 5,6,3, 7}
};



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

unsigned int dir_neighbouring_cells[8][3] = {
    {5,2,1},
    {3,5,1},
    {2,4,1},
    {4,3,1},

    {2,5,0},
    {5,3,0},
    {4,2,0},
    {3,4,0}
};

unsigned int dir_outside_root_lvl[8][3] = {
    {3,4,0},
    {4,2,0},
    {5,3,0},
    {2,5,0},

    {4,3,1},
    {2,4,1},
    {3,5,1},
    {5,2,1}
};



/*


bool has_deeper_empty_neigbour(struct octree* node, int dir_to){
    if(!node->hasChildren){
        if(node->level <= 1 || node->is_inside)
            return false;
        return true;
    }
    if(node->hasChildren)
        for(int i = 0; i < 4; i++)
            if(has_deeper_empty_neigbour(&node->children[deep_child_cells_in_dir[dir_to][i]], dir_to))
                return true;
    return false;
}

bool has_empty_neigbour_trav_down(struct octree* node, int path[max_tree_depth], int dir, int p_depth){
    if(!node->hasChildren){
        if(node->level <= 1 || node->is_inside)
            return false;
        return true;
    }
    if(node->level <= p_depth)
        return has_deeper_empty_neigbour(node, mirrored_dir[dir]);
    return has_empty_neigbour_trav_down(&node->children[mirrored_traverse[dir][path[node->level]]], path, dir, p_depth);
}

bool has_empty_neigbour_trav_up(struct octree* node, bool dir_check[6], int path[max_tree_depth], int p_depth){
    if(node == NULL)
        return true;

    //Selected node in parent
    int p = path[node->level];
    //printf("%d ", p);
    if(node->level == max_tree_depth){
        //We have reached root
        //Check when dir  = false, goes outside, if it does, return true, else false
        for(int i = 0; i < 3; i++){
            if(!dir_check[dir_outside_root_lvl[p][i]])
                return true;
        }
        return false;
    }
    //Were at or bellow the child right before root level
    
    for(int i = 0; i < 3; i++){
        //Go through neighbours that dont have visited directions
        if(!dir_check[dir_neighbouring_cells[p][i]]){
            //Mark direction as visited
            dir_check[dir_neighbouring_cells[p][i]] = true;
            //If the neighbouring node has children, check them to see if they are marked as empty, 
            //if they are, return true
            if(!node->children[neighbouring_cells[p][i]].hasChildren && !node->children[neighbouring_cells[p][i]].is_inside)
                return true;
            if(node->children[neighbouring_cells[p][i]].hasChildren && has_empty_neigbour_trav_down(&node->children[neighbouring_cells[p][i]], path, dir_neighbouring_cells[p][i], p_depth))
                return true;
        }  
    }

    return has_empty_neigbour_trav_up(node->parent, dir_check, path, p_depth);
}

//Finds target node
void fill_model_v2(struct octree* node, unsigned int s_order[8], int path[max_tree_depth]){
    if(!node->hasChildren && node->level > 1){
        bool dir_check[6] = {false,false,false,false,false,false};
        if(has_empty_neigbour_trav_up(node->parent, dir_check, path, node->level)) //PARENT?
            node->is_inside = false;
    }
    else if(node->hasChildren){
        for(int i = 0; i < 8; i++){
            path[node->level] = s_order[i];
            fill_model_v2(&node->children[s_order[i]], s_order, path);
        }
    }
}

void fill_the_model(struct octree* root){
    //root has to have children
        printf("wefwef");

    if(!root->hasChildren)
        return;
    for(int i = 0; i < 8; i++){
        int path[max_tree_depth + 1];
        //Set first path to the child that is entered
        path[root->level] = i;
        printf("%d %d\n", root->level, max_tree_depth);
        fill_model_v2(&root->children[i], search_order[i], path);
    }

    for(int i = 0; i < 8; i++){
        int path[max_tree_depth + 1];
        //Set first path to the child that is entered
        path[root->level] = i;
        printf("%d %d\n", root->level, max_tree_depth);
        fill_model_v2(&root->children[i], rev_search_order[i], path);
    }
}

*/







int mirrored_dir[6] = {1,0,3,2,5,4};

unsigned int deep_child_cells_in_dir[6][4] = {
    {0,1,2,3},
    {4,5,6,7},
    {3,1,5,7},
    {2,0,6,4},
    {0,1,4,5},
    {2,3,6,7}
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

struct octree* get_neighbour_trav_down_deep(struct octree* node, int dir_to){
    if(node->hasChildren)
        for(int i = 0; i < 4; i++)
            return get_neighbour_trav_down_deep(&node->children[deep_child_cells_in_dir[mirrored_dir[dir_to]][i]], dir_to);
    return node;
}

struct octree* get_neighbour_trav_down(struct octree* node, int path[max_tree_depth], int dir_to, int max_path_depth){
    
    if(node->hasChildren){
        if(node->level < max_path_depth){
            //We have reached the end of the path that the original node was at
            //return get_neighbour_trav_down_deep(node, dir_to);
        }
        return get_neighbour_trav_down(
            &node->children[mirrored_traverse[dir_to][path[node->level - 1]]], 
            path, dir_to, max_path_depth);
    }
    return node;   
}

struct octree* get_neighbour_trav_up(struct octree* node, int path[max_tree_depth], int direction, int max_path_depth){
    //If parent == NULL, we have reached the root, meaning there is no child in the specified direction
    if(node->parent == NULL)
        return NULL;

    int c_index = neighbour_directions[node->where_in_parent][direction];

    if(c_index == -1){
        //Has a neighbour
        if(node->parent->children[c_index].hasChildren)
            //Mirrored dir so that traversal happends as close to the current node as possible
            return get_neighbour_trav_down(&node->parent->children[c_index], path, direction, max_path_depth);
        return &node->parent->children[c_index];
    }
    //Add path we should take from the parent to this node
    path[node->parent->level - 1] = node->where_in_parent;
    //Does not have a neighbour, traverse up
    return get_neighbour_trav_up(node->parent, path, direction, max_path_depth);
}



void calculate_neighbours(struct octree* node){
    //Calculate neighbours for the node itself
    for(int i = 0; i < 6; i++){
        int path[max_tree_depth + 1];
        node->neighbours[i] = get_neighbour_trav_up(node, path, i, node->level-1);
    }
    //Then if node has children, calculate neighbours for those children
    if(node->hasChildren)
        for(int i = 0; i < 8; i++)
            calculate_neighbours(&node->children[i]);
}

















struct octree* get_corner(struct octree* node, int corner_dir){
    if(!node->hasChildren && node->level > 1)
        return node;
    if(node->hasChildren)
        return get_corner(&node->children[corner_dir], corner_dir);
    return NULL;

}

void flood_fill(struct octree* node, int from){
    //If the node hasnt been visited and is not a leaf node, it must be outside, traverse to neighbours
    if(node == NULL)
        return;
    if(node->hasChildren){
        //For all the neighbours in the parent
        if(from != -1)
            for(int i = 0; i < 3; i++){
                    flood_fill(&node->children[neighbouring_cells[from][i]], -1);
            }
        else
            for(int i = 0; i < 8; i++){
                flood_fill(&node->children[i], -1);
            }

    }
    if(node->is_inside && node->level > 1 && !node->hasChildren ){
        node->is_inside = false;
        //Go back
        flood_fill(node->parent, node->where_in_parent);
    }
}





void flood_fill_v2(struct octree* node){
    for(int i = 0; i < 6; i++){
        //Go to each neigbour and mark as empty, you might need to traverse up the tree to find neighbour

        //Call flood fill on neighbour
            //If has children, go as deep as possible, as close as the from cell as possible doesnt matter which, just one as close as possible
            //If a neighbour or self is lvl 1, return
    }

}





void fill_mode_fill(struct octree* root){
    if(!root->hasChildren)
        return;
    //Flood from every corner
    for(int i = 0; i < 8; i++){
        struct octree* corner = get_corner(&root->children[i], i);
        //If inside, meaning not touched, touch
        //If NULL then the corner cell is a wall, so skip that corneer
        if(corner != NULL && corner->is_inside){
            flood_fill(corner, -1);
        }
    }

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
        if(model->groups[i].is_hollow){
            //fill_model(&roots[i]);
            //fill_leafs(&roots[i]);
            //fill_the_model(&roots[i]);
            
            //calculate_neighbours(&roots[i]);
            //fill_mode_fill(&roots[i]);

            //fill_voids(&roots[i]);
        }
        //else if(model->groups[i].is_hollow == 2)
            //adaptive_fill_model(&roots[i]);

        char path[256];
        sprintf(path, "obj_converted/%d.obj", i);
        obj_convert(&roots[i], path, model_len);
    }

    free(roots);

}


