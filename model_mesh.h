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
    int material_label;
    bool is_hollow;

    //face representations = [f1[x_offset, y_offset, z_offset],...]
    unsigned int **faces;
   
};

//Main model
struct model{
    //layer sizes = [L1[n_points, n_faces], L2[n_points, n_faces],...]
    double x_max, y_max, z_max, x_min, y_min, z_min;
    double **points;
    int n_layers;
    int** sizes;
    struct material_group *groups;
};
