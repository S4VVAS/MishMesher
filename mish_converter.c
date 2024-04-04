#include "mish_converter.h"

/*
FORMAT IDEAS:



*/


//lägre nummer först i merge
//lägre nummer skriver över högre nummer
//Lägst nummer vinner

//x y z - material i  x y z

//Granne = x + 1, y + 1, z + 1

//Så (1,1,2) = mat 1
//   (2,1,2) = mat 2
//   (3,1,2) = mat 3
//OSV

//Så  (1,1,2) har granne med mat 2
//Och (2,1,2) har granne med mat 3
//OSV



//FILE STRUCTURE
//New level in tree = level num + \n
//Each level has 8 children which are specified as a new level above
//A level 1 node has 8 leafs, represented by 8 8-bit integers
//  A level 2 node contains 8 level 1 nodes, thus contains 8 lines of 8 8-bit integers

//Parallelize by splitting the work into 8 threads, each parsing one child of the node
//Create new threads at each level, allowing for a more distributed workload


void create_mish(struct octree* trees ){

}





void mish_convert(struct octree* trees, unsigned int n_layers, char* path, double box_size){
    
    if (trees == NULL || n_layers <= 0)
        return;
    
    char* out_path = (char*)malloc(sizeof(char) * 256);
    sprintf(out_path, "%s.msh", path);

    //If file with specified name exists, append a number to the filename
    if (access(out_path, F_OK) == 0) {
        int f_num = 1;
        char* new_path = out_path;
        while(access(new_path, F_OK) == 0)
             sprintf(new_path, "%s-v%d.msh", path, f_num++);
        out_path = new_path;
    }
    FILE* m_file = fopen(out_path, "w");

    
    fclose(m_file); 
    free(out_path);
}