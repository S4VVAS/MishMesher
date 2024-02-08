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
    
    //blocks = [b1[x, y, z, mx, my, mz],b2[x, y, z, mx, my, mz],,...]
    unsigned int **blocks;
};
/*
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

struct oct_mesh{



};
*/