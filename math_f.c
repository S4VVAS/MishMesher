#include "math_f.h"

#define EPSILON 0.000001

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

struct vector3 vec3_sub(struct vector3 v1, struct vector3 v2){
    struct vector3 res = {
        v1.x - v2.x, 
        v1.y - v2.y,
        v1.z - v2.z
    };
    return res;
}

#define AXISTEST_X01(a, b, fa, fb)			                        \
    p0 = a * v0.y - b * v0.z;			       	                    \
    p2 = a * v2.y - b * v2.z;			       	                    \
    if(p0 < p2){                                                    \
        min_v = p0;                                                 \
        max_v = p2;                                                 \
    }                                                               \
    else {                                                          \
        min_v = p2;                                                 \
        max_v = p0;                                                 \
    }                                                               \
    rad = fa * boxhalfsize + fb * boxhalfsize;                      \
    if(min_v > rad || max_v < -rad)                                 \
        return false;

#define AXISTEST_X2(a, b, fa, fb)			                        \
    p0 = a * v0.y - b * v0.z;			                            \
    p1 = a * v1.y - b * v1.z;			       	                    \
    if(p0 < p1){                                                    \
        min_v = p0;                                                 \
        max_v = p1;                                                 \
    }                                                               \
    else {                                                          \
        min_v = p1;                                                 \
        max_v = p0;                                                 \
    }                                                               \
    rad = fa * boxhalfsize + fb * boxhalfsize;                      \
    if(min_v > rad || max_v < -rad)                                 \
        return 0;

#define AXISTEST_Y02(a, b, fa, fb)			                        \
    p0 = -a * v0.x + b * v0.z;		      	                        \
    p2 = -a * v2.x + b * v2.z;	       	       	                    \
    if(p0 < p2) {                                                   \
        min_v = p0;                                                 \
        max_v = p2;                                                 \
        }                                                           \
        else {                                                      \
            min_v = p2;                                             \
            max_v = p0;                                             \
        }                                                           \
    rad = fa * boxhalfsize + fb * boxhalfsize;                      \
    if(min_v > rad || max_v < -rad)                                 \
        return 0;

#define AXISTEST_Y1(a, b, fa, fb)			                        \
    p0 = -a * v0.x + b * v0.z;		      	                        \
    p1 = -a * v1.x + b * v1.z;	     	       	                    \
    if(p0 < p1) {                                                   \
        min_v = p0;                                                 \
        max_v = p1;                                                 \
    }                                                               \
    else {                                                          \
        min_v = p1;                                                 \
        max_v = p0;                                                 \
    }                                                               \
    rad = fa * boxhalfsize + fb * boxhalfsize;                      \
    if(min_v > rad || max_v < -rad)                                 \
        return 0;

#define AXISTEST_Z12(a, b, fa, fb)			                        \
    p1 = a * v1.x - b * v1.y;			                            \
    p2 = a * v2.x - b * v2.y;			       	                    \
    if(p2 < p1) {                                                   \
        min_v = p2;                                                 \
        max_v = p1;                                                 \
    }                                                               \
    else {                                                          \
        min_v = p1;                                                 \
        max_v = p2;                                                 \
    }                                                               \
    rad = fa * boxhalfsize + fb * boxhalfsize;                      \
    if(min_v > rad || max_v < -rad)                                 \
        return 0;
    
#define AXISTEST_Z0(a, b, fa, fb)			                        \
    p0 = a * v0.x - b * v0.y;				                        \
    p1 = a * v1.x - b * v1.y;			                            \
    if(p0 < p1) {                                                   \
        min_v = p0;                                                 \
        max_v = p1;                                                 \
    }                                                               \
    else {                                                          \
        min_v = p1;                                                 \
        max_v = p0;                                                 \
    }                                                               \
    rad = fa * boxhalfsize + fb * boxhalfsize;                      \
    if(min_v > rad || max_v < -rad)                                 \
        return 0;

bool planeBoxOverlap(struct vector3 normal, struct vector3 vert, double maxbox){
    int q;
    struct vector3 vmin, vmax;
    double v;

    v = vert.x;
    if(normal.x > 0.0){
        vmin.x = -maxbox - v;
        vmax.x = maxbox - v;
    }
    else{
        vmin.x = maxbox - v;
        vmax.x = -maxbox - v;
    }

    v = vert.y;
    if(normal.y > 0.0){
        vmin.y = -maxbox - v;
        vmax.y = maxbox - v;
    }
    else{
        vmin.y = maxbox - v;
        vmax.y = -maxbox - v;
    }

    v = vert.z;
    if(normal.z > 0.0){
        vmin.z = -maxbox - v;
        vmax.z = maxbox - v;
    }
    else{
        vmin.z = maxbox - v;
        vmax.z = -maxbox - v;
    }

    if(dot_p(normal,vmin) > 0.0f) 
        return false;
    if(dot_p(normal,vmax) >= 0.0f) 
        return true;
    return false;
}

bool triBoxOverlap(struct vector3 boxcenter, double boxhalfsize, struct tri* triangle){
    struct vector3 v0, v1, v2;
    double min_v, max_v, p0, p1, p2, rad, fex, fey, fez;
    struct vector3 normal, e0, e1, e2;

    v0 = vec3_sub(triangle->v1, boxcenter);
    v1 = vec3_sub(triangle->v2, boxcenter);
    v2 = vec3_sub(triangle->v3, boxcenter);
    
    e0 = vec3_sub(v1,v0);
    e1 = vec3_sub(v2,v1);
    e2 = vec3_sub(v0,v2);

    fex = abs_v(e0.x);
    fey = abs_v(e0.y);
    fez = abs_v(e0.z);

    AXISTEST_X01(e0.z, e0.y, fez, fey);
    AXISTEST_Y02(e0.z, e0.x, fez, fex);
    AXISTEST_Z12(e0.y, e0.x, fey, fex);

    fex = abs_v(e1.x);
    fey = abs_v(e1.y);
    fez = abs_v(e1.z);

    AXISTEST_X01(e1.z, e1.y, fez, fey);
    AXISTEST_Y02(e1.z, e1.x, fez, fex);
    AXISTEST_Z0(e1.y, e1.x, fey, fex);

    fex = abs_v(e2.x);
    fey = abs_v(e2.y);
    fez = abs_v(e2.z);

    AXISTEST_X2(e2.z, e2.y, fez, fey);
    AXISTEST_Y1(e2.z, e2.x, fez, fex);
    AXISTEST_Z12(e2.y, e2.x, fey, fex);

    min_v = min(v0.x, min( v1.x, v2.x));
    max_v = max(v0.x, max( v1.x, v2.x));
    if(min_v > boxhalfsize || max_v < -boxhalfsize)
        return false;

    min_v = min(v0.y, min( v1.y, v2.y));
    max_v = max(v0.y, max( v1.y, v2.y));
    if(min_v > boxhalfsize || max_v < -boxhalfsize) 
        return false;

    min_v = min(v0.z, min( v1.z, v2.z));
    max_v = max(v0.z, max( v1.z, v2.z));
    if(min_v > boxhalfsize || max_v < -boxhalfsize) 
        return false;

    if(!planeBoxOverlap(cross_p(e0, e1), v0, boxhalfsize)) 
        return false;
    return true;

}

bool intersects(struct aabb *box, struct tri *triangle, double b_div_2){
    return triBoxOverlap(
        (struct vector3){box->max.x - b_div_2, box->max.y - b_div_2, box->max.z - b_div_2}, 
        b_div_2, 
        triangle
    );
}



/*
//Based on paper:
//https://my.eng.utah.edu/~cs6958/papers/MT97.pdf
#include <float.h>

bool ray_intersects_triangle( struct vector3 ray_origin,  struct vector3 ray_vector, struct tri *triangle) {
    struct vector3 edge1 = vec3_sub(triangle->v2, triangle->v1);
    struct vector3 edge2 = vec3_sub(triangle->v3, triangle->v1);
    struct vector3 ray_cross_e2 = cross_p(ray_vector, edge2);
    float det = dot_p(edge1, ray_cross_e2);

    if (det > -EPSILON && det < EPSILON)
        return false; // Ray is parallel to the triangle.

    float inv_det = 1.0 / det;
    struct vector3 s = vec3_sub(ray_origin, triangle->v1);
    float u = inv_det * dot_p(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return false;

    struct vector3 s_cross_e1 = cross_p(s, edge1);
    float v = inv_det * dot_p(ray_vector, s_cross_e1);

    if (v < 0 || u + v > 1)
        return false;

    float t = inv_det * dot_p(edge2, s_cross_e1);
    if (t > EPSILON) {
        //out_intersection_point = ray_origin + ray_vector * t;
        return true; // Ray intersects triangle.
    } else {
        return false; // Line intersection but not ray intersection.
    }
}


// Function to test intersection between an AABB and a triangle
bool intersects(struct aabb* box, struct tri* triangle) {


    struct vector3 orig[12], dir[12];
   orig[0] = (struct vector3){box->max.x, box->max.y, box->max.z};
    dir[0] = vec3_sub(orig[0], triangle->v1);

    orig[1] = (struct vector3){box->max.x, box->max.y, box->max.z};
    dir[1] = vec3_sub(orig[1], triangle->v1);

    orig[2] = (struct vector3){box->max.x, box->max.y, box->max.z};
    dir[2] = vec3_sub(orig[2], triangle->v1);

    orig[3] = (struct vector3){box->min.x, box->min.y, box->min.z};
    dir[3] = vec3_sub(orig[3], triangle->v1);

    orig[4] = (struct vector3){box->min.x, box->min.y, box->min.z};
    dir[4] = vec3_sub(orig[4], triangle->v1);

    orig[5] = (struct vector3){box->min.x, box->min.y, box->min.z};
    dir[5] = vec3_sub(orig[5], triangle->v1);

    orig[6] = (struct vector3){box->min.x, box->max.y, box->max.z};
    dir[6] = vec3_sub(orig[6], triangle->v1);

    orig[7] = (struct vector3){box->min.x, box->max.y, box->max.z};
    dir[7] = vec3_sub(orig[7], triangle->v1);

    orig[8] = (struct vector3){box->max.x, box->max.y, box->min.z};
    dir[8] = vec3_sub(orig[8], triangle->v1);

    orig[9] = (struct vector3){box->max.x, box->max.y, box->min.z};
    dir[9] = vec3_sub(orig[9], triangle->v1);

    orig[10] = (struct vector3){box->min.x, box->min.y, box->max.z};
    dir[10] = vec3_sub(orig[10], triangle->v1);

    orig[11] = (struct vector3){box->max.x, box->min.y, box->min.z};
    dir[11] = vec3_sub(orig[11], triangle->v1);



    dir[0] = orig[6];
    dir[1] = orig[8];
    dir[2] = (struct vector3){box->max.x, box->min.y, box->max.z};

    dir[3] = orig[10];
    dir[4] = orig[11];
    dir[5] = (struct vector3){box->min.x, box->max.y, box->min.z};

    dir[6] = orig[10];
    dir[7] = dir[5];

    dir[8] = orig[10];
    dir[9] = orig[11];

    dir[10] = dir[2];

    dir[11] = dir[2];

    

    for(int i = 0; i < 12; i++){
        if(ray_intersects_triangle(orig[i], dir[i], triangle))
            return true;
    }

  return false;
}

*/
