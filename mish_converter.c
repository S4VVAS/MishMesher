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
//Header info: n_layers, octree side-size
//New level in tree = level num + \n
//Each level has 8 children which are specified as a new level above
//A level 1 node has 8 leafs, represented by 8 8-bit integers
//  A level 2 node contains 8 level 1 nodes, thus contains 8 lines of 8 8-bit integers

//Parallelize by splitting the work into 8 threads, each parsing one child of the node
//Create new threads at each level, allowing for a more distributed workload

//Child traversal order 0,1,2,3,4,5,6,7

void make_tmp(struct octree* node, FILE* tmp_file, int parent){
    if(node->hasChildren){
        if(node->level == 2){
            //A level 2 node that contains children, contains 8 level 1 nodes. 
            //Those level 1 nodes always include leafs, thus print it all to the file
            uint8_t solids[8];
            bool has_solids = false;
            for(int i = 0; i < 8; i++){
                solids[i] = node->children[i].is_voxels_solid;
                if(solids[i] > 0)
                    has_solids = true;
            }
            if(has_solids)
                fprintf(tmp_file, "%d %d %d %d %d %d %d %d\n",
                solids[0],
                solids[1],
                solids[2],
                solids[3],
                solids[4],
                solids[5],
                solids[6],
                solids[7]
                );
            else
                fprintf(tmp_file, "0\n");
        }
        else
            fprintf(tmp_file, "%d %d\n", parent, node->level);
            for(int i = 0; i < 8; i++){
                make_tmp(&node->children[i], tmp_file, i);
            }
    }
    else if(node->level == 2)
        fprintf(tmp_file, "0\n");
}

FILE* create_tmp(struct octree* root, char* path, int n){
    char* tmp_file_path = (char*)malloc(sizeof(char) * 256);
    sprintf(tmp_file_path, "%s-%d.tmp", path, n);
    FILE* tmp_file = fopen(tmp_file_path, "w+");

    //Write layer number for tree
    fprintf(tmp_file, "L %d\n", n);
    make_tmp(root, tmp_file, -1);

    //REMEMBER TO CLOSE THE FILES RETURNED
    free(tmp_file_path);
    return tmp_file;
}


void create_mish(struct octree* trees, unsigned int n_layers, double box_size, char* path, FILE* m_file, int n_threads){
    //Create threads that work on each tree separately, might be unbalanced workloads though..

    FILE* tmp_files[n_layers];
    
    #pragma omp parallel for num_threads(n_threads) shared(tmp_files)
    for(int i = 0; i < n_layers; i++)
        tmp_files[i] = create_tmp(&trees[i], path, i);
    
    fprintf(m_file, "MishMesh V1\n");
    fprintf(m_file, "%d %f\n", n_layers, box_size);
    //TOP/FRONT/RIGHT CORNER COORDINATES
    fprintf(m_file, "%f %f %f\n", 0.0, 0.0, 0.0);
    
    //COMBINE FILES
    for(int i = 0; i < n_layers; i++){
        char c; 
        rewind(tmp_files[i]);
        while ((c = fgetc(tmp_files[i])) != EOF){
            fputc(c, m_file); 
        }
            
    }



    char* tmp_file_path = (char*)malloc(sizeof(char) * 256);
    for(int i = 0; i < n_layers; i++){
        fclose(tmp_files[i]);
        sprintf(tmp_file_path, "%s-%d.tmp", path, i);
        remove(tmp_file_path);        
    }
    free(tmp_file_path);
}





void mish_convert(struct octree* trees, unsigned int n_layers, char* path, double box_size, int num_threads){
    
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

    create_mish(trees, n_layers, box_size, path, m_file, num_threads);

    
    fclose(m_file); 
    free(out_path);
}