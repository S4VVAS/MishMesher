#pragma once

struct c_mesh{
    unsigned int size;

    //x,y,z
    //unsigned int* sizes;

    //total number of blocks
    unsigned int n;

    unsigned int n_materials;

    //[0,10,20] = [iron, copper, brass]
    unsigned int *material_offsets;
    
    //blocks = [b1[x, y, z],b2[x, y, z],,...]
    //material = [b1[x ,y, z],b2[x, y, z],,...]
    unsigned int **blocks, **material;
};
