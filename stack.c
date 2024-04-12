#include "stack.h"

void init_stack(struct stack* stk) {
    stk->array_size = DEFAULT_SIZE;
    stk->no_of_words = 0;
    stk->stack = (struct octree**)malloc(sizeof(struct octree**) * DEFAULT_SIZE);
  
}

void destroy_stack(struct stack* stk) {
    free(stk->stack);
    free(stk);
}

void shrink_capacity(struct stack* stk) {
    stk->array_size /= 2;
    struct octree** new_stack = malloc(sizeof(struct octree**) * stk->array_size);
    int i;
    for (i = 0; i < stk->no_of_words; i++) {
        new_stack[i] = stk->stack[i];
    }
    free(stk->stack);
    stk->stack = new_stack;
}

void double_capacity(struct stack* stk) {
    stk->array_size *= 2;
    stk->stack = realloc(stk->stack, sizeof(struct octree*) * stk->array_size);
}

struct octree* pop(struct stack* stk) {
    if (stk->no_of_words < stk->array_size / 2 && stk->no_of_words > DEFAULT_SIZE) {
        shrink_capacity(stk);
    }
    struct octree* removed_word = stk->stack[stk->no_of_words - 1];
    stk->no_of_words -= 1;
    return removed_word;
}

void push(struct stack* stk, struct octree* word){
    if(stk->stack == NULL) {
        printf("STACK ERROR: failed to allocate memory\n");
        exit(1);
    }
    if(stk->no_of_words == stk->array_size) {
        double_capacity(stk);
    }
    stk->stack[stk->no_of_words] = word;
    stk->no_of_words += 1;
}

struct octree* top(struct stack* stk) {
    return (struct octree*) stk->stack[stk->no_of_words - 1];
}

int size(struct stack* stk) {
    return stk->no_of_words;
}