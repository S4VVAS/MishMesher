#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"
#include "math_f.h"
#include "obj_converter.h"



//struct c_mesh* create_c_mesh(int long_resolution, struct model* model);
//void destroy_c_mesh(struct c_mesh* c_mesh);

void mesh(int long_resolution, struct model* model);