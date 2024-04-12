#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "cartesian_mesh.h"

#define DEFAULT_SIZE 16

struct stack{
    int no_of_words;
    int array_size;
    struct octree** stack;
};


void init_stack(struct stack* stk);
void destroy_stack(struct stack* stk);
void shrink_capacity(struct stack* stk);
struct octree* pop(struct stack* stk);
void push(struct stack* stk, struct octree* word);
struct octree* top(struct stack* stk);
int stack_size(struct stack* stk);