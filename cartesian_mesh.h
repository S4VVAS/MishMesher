#pragma once

struct mesh{
    //x,y,z
    unsigned int sizes;
    //total number of blocks
    unsigned int n;
    //[0,10,20] = [iron, copper, brass]
    unsigned int *material_offsets;
    //blocks = [b1[x,y,z],b2[x,y,z],,...]
    //material = [b1[x,y,z],b2[x,y,z],,...]
    unsigned int **blocks, **material;
}
