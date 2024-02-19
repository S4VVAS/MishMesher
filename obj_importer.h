#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <float.h>
#include "model_mesh.h"

void close_file(FILE* file);

struct model* import_mesh(char* path);

void destroy_model(struct model* mesh);
//void parse_face(const char* line, struct model* mesh, int* n);
//void parse_vector(const char* line, struct model* mesh, int* n);
//void parse_layer(const char* line, struct model* mesh, int* n);

struct model* parse_mesh(FILE* file);
