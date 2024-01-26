#pragma once

struct material_group{
    char material_label;

    double **points; //array of arrays[3]
    unsigned int **faces; //array of arrays[3]
};

//Main model
struct model{
    int** sizes;
    struct material_group *groups;
};
