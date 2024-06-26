#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <sys/time.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"


long long timeInMilliseconds();

double max(double i, double j);

double min(double i, double j);

double abs_v(double val);

double len(double d1, double d2);

struct vector3  cross_p(struct vector3 v1, struct vector3 v2);

double dot_p(struct vector3 v1, struct vector3 v2);


bool intersects(struct aabb *box, struct tri *triangle, double b_div_2);