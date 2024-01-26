#pragma once

struct point{
    double* coordinates;
};

struct face{
    unsigned int* face_points;
};

struct material_group{
    unsigned int material_label;
    struct point *points;
    struct face *faces;
};

//Main model
struct model{
    struct material_group *groups;
};