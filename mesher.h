#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"
#include "math_f.h"
#include "obj_converter.h"

void mesh(int long_resolution, struct model* model);