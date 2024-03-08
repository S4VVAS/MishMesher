#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cartesian_mesh.h"
#include "model_mesh.h"

void obj_convert(struct octree* tree, char* path, double box_size);