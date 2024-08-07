#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdatomic.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"
#include "math_f.h"
#include "stack.h"
#include "obj_converter.h"
#include "mish_converter.h"

void mesh(double cell_size, struct model* model, int core_count, char* out_path);
