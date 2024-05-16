#pragma once
#include <stdbool.h>

struct vector3{
    double x, y, z;
};

struct aabb {
    struct vector3 max, min;
};

struct tri {
    struct vector3 v1, v2, v3;
};

struct material_group{
    //material index
    unsigned int material_label;
    unsigned int is_hollow;

    //face representations = [f1[x_offset, y_offset, z_offset],...]
    unsigned int **faces;
   
};

//Main model
struct model{
    double x_max, y_max, z_max, x_min, y_min, z_min;
    //Vertices contained in model
    double **points; 
    unsigned int n_layers;
    //layer sizes = [L1[n_points, n_faces], L2[n_points, n_faces],...]
    unsigned int** sizes;
    struct material_group *groups;
};
