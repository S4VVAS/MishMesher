#pragma once

struct aabb {
    double max_x, max_y, max_z, min_x, min_y, min_z;
};

struct vector3{
    double x, y, z;
};

struct tri {
    struct vector3 v1, v2, v3;
};



struct material_group{
    //material index
    int material_label;

    //face representations = [f1[x_offset, y_offset, z_offset],...]
    unsigned int **faces;
   
};

//Main model
struct model{
    //layer sizes = [L1[n_points, n_faces], L2[n_points, n_faces],...]
    double x_max, y_max, z_max;
    double **points;
    int n_layers;
    int** sizes;
    struct material_group *groups;
};
