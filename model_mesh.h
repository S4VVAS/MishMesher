#pragma once

struct material_group{
    //material index
    int material_label;
    //vertex representations = [v1[x, y, z],v2[x, y, z],...]
    double **points;
    //face representations = [f1[x_offset, y_offset, z_offset],...]
    unsigned int **faces; 
};

//Main model
struct model{
    //layer sizes = [L1[n_points, n_faces], L2[n_points, n_faces],...]
    double x_max, y_max, z_max;

    int n_layers;
    int** sizes;
    struct material_group *groups;
};
