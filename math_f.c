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

double len(double d1, double d2){
    return abs_v(d1 - d2);
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
        v1.z - v2.z
    };
    return res;
}
/*
#include <string.h>

bool intersects(struct aabb *box, struct tri *triangle, double b_div_2){
    struct aabb t_box;
    memcpy(&t_box, box, sizeof(struct aabb));
    struct tri t_tri;
    memcpy(&t_tri, triangle, sizeof(struct tri));

    struct vector3 box_center = {box->max.x - b_div_2, box->max.y - b_div_2, box->max.z - b_div_2};
    t_box.max = vec3_minus(t_box.max, box_center);
    t_box.min = vec3_minus(t_box.min, box_center);
    t_tri.v1 = vec3_minus(t_tri.v1, box_center);
    t_tri.v2 = vec3_minus(t_tri.v2, box_center);
    t_tri.v3 = vec3_minus(t_tri.v3, box_center);

    struct tri box_face_norms;
    box_face_norms.v1 = (struct vector3){1.0, 0.0, 0.0};
    box_face_norms.v2 = (struct vector3){0.0, 1.0, 0.0};
    box_face_norms.v3 = (struct vector3){0.0, 0.0, 1.0};

    struct vector3 axis[9];
    axis[0] = cross_p(box_face_norms.v1, t_tri.v1);
    axis[1] = cross_p(box_face_norms.v1, t_tri.v2);
    axis[2] = cross_p(box_face_norms.v1, t_tri.v3);
    axis[3] = cross_p(box_face_norms.v2, t_tri.v1);
    axis[4] = cross_p(box_face_norms.v2, t_tri.v2);
    axis[5] = cross_p(box_face_norms.v2, t_tri.v3);
    axis[6] = cross_p(box_face_norms.v3, t_tri.v1);
    axis[7] = cross_p(box_face_norms.v3, t_tri.v2);
    axis[8] = cross_p(box_face_norms.v3, t_tri.v3);

    //SAT TESTS for first 9 axis
    for(int i = 0; i < 9; i++){
        double p1 = dot_p(t_tri.v1, axis[i]);
        double p2 = dot_p(t_tri.v2, axis[i]);
        double p3 = dot_p(t_tri.v3, axis[i]);

        double r = 
            b_div_2 * abs_v(dot_p(box_face_norms.v1, axis[i])) +
            b_div_2 * abs_v(dot_p(box_face_norms.v2, axis[i])) +
            b_div_2 * abs_v(dot_p(box_face_norms.v3, axis[i]));

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
//Based on paper:
//https://my.eng.utah.edu/~cs6958/papers/MT97.pdf
#include <float.h>

// Function to test intersection between an AABB and a triangle
bool intersects(struct aabb* box, struct tri* triangle) {
    // Check if any vertex of the triangle is inside the AABB
    if ((triangle->v1.x >= box->min.x && triangle->v1.x <= box->max.x) &&
        (triangle->v1.y >= box->min.y && triangle->v1.y <= box->max.y) &&
        (triangle->v1.z >= box->min.z && triangle->v1.z <= box->max.z)) {
        return true;
    }
    if ((triangle->v2.x >= box->min.x && triangle->v2.x <= box->max.x) &&
        (triangle->v2.y >= box->min.y && triangle->v2.y <= box->max.y) &&
        (triangle->v2.z >= box->min.z && triangle->v2.z <= box->max.z)) {
        return true;
    }
    if ((triangle->v3.x >= box->min.x && triangle->v3.x <= box->max.x) &&
        (triangle->v3.y >= box->min.y && triangle->v3.y <= box->max.y) &&
        (triangle->v3.z >= box->min.z && triangle->v3.z <= box->max.z)) {
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

    // Adjusted calculation for s
    struct vector3 s = {
        max(box->min.x, min(box->max.x, triangle->v1.x)),
        max(box->min.y, min(box->max.y, triangle->v1.y)),
        max(box->min.z, min(box->max.z, triangle->v1.z))
    };

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
