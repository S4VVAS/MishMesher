#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"

double max(double i, double j);

double min(double i, double j);

double abs_v(double val);

struct vector3  cross_p(struct vector3 v1, struct vector3 v2);

double dot_p(struct vector3 v1, struct vector3 v2);


bool intersects(struct aabb *box, struct tri *triangle);