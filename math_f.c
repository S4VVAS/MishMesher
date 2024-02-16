#include "math_f.h"

double max(double i, double j){
    return i > j ? i : j;
}

double min(double i, double j){
    return i < j ? i : j;
}

double abs_v(double val){
    return val < 0 ? -val : val;
}

struct vector3 cross_p(struct vector3 v1, struct vector3 v2){
    struct vector3 res = {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
    return res;
}

double dot_p(struct vector3 v1, struct vector3 v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

double* dpvec_minus_dpvec(double *v1, double *v2){
    double *res = (double*)malloc(sizeof(double) * 3);
    res[0] = v1[0] - v2[0];
    res[1] = v1[1] - v2[1];
    res[2] = v1[2] - v2[2];
    return res;
}

struct vector3 vec3_minus(struct vector3 v1, struct vector3 v2){
    struct vector3 res = {
        v1.x - v2.x, 
        v1.y - v2.y,
        v1.z - v2.z};
    return res;
}






bool intersects(struct aabb *box, struct tri *triangle){
    //SETUP STAGE (TODO: is there anything to PRECOMPUTE?)
    struct vector3 box_extents;
    box_extents.x = (box->max_x - box->min_x) * 0.5;
    box_extents.y = (box->max_y - box->min_y) * 0.5;
    box_extents.z = (box->max_z - box->min_z) * 0.5;
    struct vector3 box_center;
    box_center.x = box_extents.x + box->min_x;
    box_center.y = box_extents.y + box->min_y;
    box_center.z = box_extents.z + box->min_z;

    //translate triangle
    triangle->v1 = vec3_minus(triangle->v1, box_center);
    triangle->v2 = vec3_minus(triangle->v2, box_center);
    triangle->v3 = vec3_minus(triangle->v3, box_center);
    struct tri tri_edge_vec;
    tri_edge_vec.v1 = vec3_minus(triangle->v2, triangle->v1);
    tri_edge_vec.v2 = vec3_minus(triangle->v3, triangle->v2);
    tri_edge_vec.v3 = vec3_minus(triangle->v1,triangle->v3);
    struct tri box_face_norms;
    box_face_norms.v1 = (struct vector3){1.0, 0.0, 0.0};
    box_face_norms.v2 = (struct vector3){0.0, 1.0, 0.0};
    box_face_norms.v3 = (struct vector3){0.0, 0.0, 1.0};

    struct vector3 axis[9];
    axis[0] = cross_p(box_face_norms.v1, tri_edge_vec.v1);
    axis[1] = cross_p(box_face_norms.v1, tri_edge_vec.v2);
    axis[2] = cross_p(box_face_norms.v1, tri_edge_vec.v3);
    axis[3] = cross_p(box_face_norms.v2, tri_edge_vec.v1);
    axis[4] = cross_p(box_face_norms.v2, tri_edge_vec.v2);
    axis[5] = cross_p(box_face_norms.v2, tri_edge_vec.v3);
    axis[6] = cross_p(box_face_norms.v3, tri_edge_vec.v1);
    axis[7] = cross_p(box_face_norms.v3, tri_edge_vec.v2);
    axis[8] = cross_p(box_face_norms.v3, tri_edge_vec.v3);

    //SAT TESTS for all 9 axis
    for(int i = 0; i < 9; i++){
        double p1 = dot_p(triangle->v1, axis[i]);
        double p2 = dot_p(triangle->v2, axis[i]);
        double p3 = dot_p(triangle->v3, axis[i]);

        float r = 
            box_extents.x * abs_v(dot_p(box_face_norms.v1, axis[i])) +
            box_extents.y * abs_v(dot_p(box_face_norms.v2, axis[i])) +
            box_extents.z * abs_v(dot_p(box_face_norms.v3, axis[i]));

        if (max(-max(p1, max(p2, p3)), min(p1, max(p2, p3))) > r)
            return false;
    }

    if(max(-max(dot_p(triangle->v1, box_face_norms.v1), max(dot_p(triangle->v2, box_face_norms.v1), dot_p(triangle->v3, box_face_norms.v1))), 
        min(dot_p(triangle->v1, box_face_norms.v1), min(dot_p(triangle->v2, box_face_norms.v1), dot_p(triangle->v3, box_face_norms.v1)))) > box_extents.x)
        return false;
    if(max(-max(dot_p(triangle->v1, box_face_norms.v2), max(dot_p(triangle->v2, box_face_norms.v2), dot_p(triangle->v3, box_face_norms.v2))), 
        min(dot_p(triangle->v1, box_face_norms.v2), min(dot_p(triangle->v2, box_face_norms.v2), dot_p(triangle->v3, box_face_norms.v2)))) > box_extents.y)
        return false;
    if(max(-max(dot_p(triangle->v1, box_face_norms.v3), max(dot_p(triangle->v2, box_face_norms.v3), dot_p(triangle->v3, box_face_norms.v3))), 
        min(dot_p(triangle->v1, box_face_norms.v3), min(dot_p(triangle->v2, box_face_norms.v3), dot_p(triangle->v3, box_face_norms.v3)))) > box_extents.z)
        return false;

    struct vector3 tri_norm = cross_p(tri_edge_vec.v1, tri_edge_vec.v2);
    if(max(-max(dot_p(triangle->v1, tri_norm), max(dot_p(triangle->v2, tri_norm), dot_p(triangle->v3, tri_norm))), 
        min(dot_p(triangle->v1, tri_norm), min(dot_p(triangle->v2, tri_norm), dot_p(triangle->v3, tri_norm)))) > 0.0)
        return false;

    return true;

}



    //1 so basically do an intersection test for each level of the octree in a loop
            //  1.1 Go to child voxel that is a hit
            //  1.2 Check level, is it the max level? 
            //      If not make 1.3
            //  1.3 Check hasChildren, if it has children do intersection test
            //      if not, create children first and then do 1.3
            

