#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"
#include "math_f.h"
#include "obj_converter.h"
#include "mish_converter.h"




void mesh(int long_resolution, struct model* model, int core_count, char* out_path);

void fill_level_1s(struct octree* lvl1_node, int dir_of_empty_neighbour);

void flood_fill(struct octree* node);

