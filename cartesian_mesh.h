#pragma once

#include <stdbool.h> 
#include <stdint.h> 
#include <stdatomic.h>
/*
struct c_mesh{
    unsigned int size;

    //x,y,z
    //unsigned int* sizes;

    //total number of blocks
    unsigned int n;

    unsigned int n_materials;

    //[0,10,20] = [iron, copper, brass]
    unsigned int *material_offsets;
    
    //blocks = [b1[x, y, z, mx, my, mz],b2[x, y, z, mx, my, mz],,...]
    unsigned int **blocks;
};

struct cc_mesh{
    //Top left corner coordinate of each c_box
    unsigned int *x_cor,*y_cor,*z_cor;
    //Size in all directions, of each c_box
    unsigned int *x_cor,*y_cor,*z_cor;
    //Number of floats in each c_box
    unsigned int *c_box_sizes;
    //Number of c_box:es
    unsigned int n_boxes;

    //
    float* c_boxes;


};
*/

struct octree{
    _Atomic uint8_t is_voxels_solid;
    unsigned int level;
    char where_in_parent;
    bool hasChildren;
    //Used for layer fill -> classifies node as outside or inside
    _Atomic bool is_inside;

    struct octree* neighbours[6];
    struct octree* children;
    struct octree* parent;
};


