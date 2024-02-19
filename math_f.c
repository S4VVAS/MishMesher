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

struct vector3 vec3_minus(struct vector3 v1, struct vector3 v2){
    struct vector3 res = {
        v1.x - v2.x, 
        v1.y - v2.y,
        v1.z - v2.z};
    return res;
}

/*
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

        double r = 
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
*/

// Function to test intersection between an AABB and a triangle
bool intersects(struct aabb* box, struct tri* triangle) {
    // Check if any vertex of the triangle is inside the AABB
    if ((triangle->v1.x >= box->min_x && triangle->v1.x <= box->max_x) &&
        (triangle->v1.y >= box->min_y && triangle->v1.y <= box->max_y) &&
        (triangle->v1.z >= box->min_z && triangle->v1.z <= box->max_z)) {
        return true;
    }
    if ((triangle->v2.x >= box->min_x && triangle->v2.x <= box->max_x) &&
        (triangle->v2.y >= box->min_y && triangle->v2.y <= box->max_y) &&
        (triangle->v2.z >= box->min_z && triangle->v2.z <= box->max_z)) {
        return true;
    }
    if ((triangle->v3.x >= box->min_x && triangle->v3.x <= box->max_x) &&
        (triangle->v3.y >= box->min_y && triangle->v3.y <= box->max_y) &&
        (triangle->v3.z >= box->min_z && triangle->v3.z <= box->max_z)) {
        return true;
    }

    // Möller–Trumbore intersection algorithm
    struct vector3 e1 = vec3_minus(triangle->v2, triangle->v1);
    struct vector3 e2 = vec3_minus(triangle->v3, triangle->v1);
    struct vector3 h = cross_p((struct vector3){0.0, 1.0, 0.0}, e2);
    double a = dot_p(e1, h);

    if (a > -DBL_EPSILON && a < DBL_EPSILON) {
        return false; // The ray is parallel to the triangle.
    }

    double f = 1.0 / a;
    struct vector3 s = vec3_minus(*(struct vector3*)box, triangle->v1);
    double u = f * dot_p(s, h);

    if (u < 0.0 || u > 1.0) {
        return false;
    }

    struct vector3 q = cross_p(s, e1);
    double v = f * dot_p((struct vector3){0.0, 1.0, 0.0}, q);

    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    double t = f * dot_p(e2, q);

    return t > DBL_EPSILON;
}