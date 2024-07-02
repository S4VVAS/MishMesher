#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <float.h>
#include "model_mesh.h"
#include "math_f.h"

void close_file(FILE* file);

struct model* import_mesh(char* path, char* mat_path);

void destroy_model(struct model* mesh);

struct model* parse_mesh(FILE* file, FILE* mat_file);
