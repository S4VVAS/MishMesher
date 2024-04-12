#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"

void mish_convert(struct octree* trees, unsigned int n_layers, char* path, double box_size, int num_threads, struct vector3 coords);
