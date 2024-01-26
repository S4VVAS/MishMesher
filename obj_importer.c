// g    =   Named polygon groups. (Material)
// v    =   List of geometric vertices, with (x, y, z, [w]) coordinates, w is optional and defaults to 1.0.
// vt   =   List of texture coordinates, in (u, [v, w]) coordinates, these will vary between 0 and 1. v, w are optional and default to 0.
// vn   =   List of vertex normals in (x,y,z) form; normals might not be unit vectors.
// f    =   Polygonal face element.
// #    =   Comment.

//What i need to parse: g v f

#include "obj_importer.h"
#define STRMAX 1024


void close_file(FILE* file){
    if (fclose(file))
        fprintf(stderr, "error: the file could not be closed\n");
}

struct model* import_mesh(char* path){
    if (access(path, F_OK) != 0) {
        fprintf (stderr, "error: the file '%s' could not be found\n", path);
        return NULL;
    }
    FILE* file = fopen(path, "r");

    if(!file){
        fprintf (stderr, "error: the file '%s' could not be opened\n", path);
        close_file(file);
        return NULL;
    }
    printf("File found with given path: %s\n", path);
    return parse_mesh(file);
}

void parse_face(const char* line, struct model* mesh, int n){

}

void parse_vector(const char* line, struct model* mesh, int n){
}

void parse_layer(const char* line, struct model* mesh, int n){
    mesh->groups[n].material_label = *line;
}

struct model* alloc_model(int* sizes){
    struct model *mesh = (struct model*)malloc(sizeof(struct model));
    mesh->groups = (struct material_group*)malloc(sizes[2] * sizeof(struct material_group));
    for(int i = 0; i < sizes[2]; i++){
        mesh->groups[i].points = (double**)malloc(sizes[1] * sizeof(double[3]));
        mesh->groups[i].faces = (unsigned int**)malloc(sizes[0] * sizeof(unsigned int[3]));
    }

    return mesh;
}

int* npoints_nfaces(FILE* file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;

    int* nums = calloc(0, sizeof(int)*3);

    while(fgets(buffer, STRMAX, file)){
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            const char *line_content = buffer + n;

            if (!strcmp(key, "f")) 
                nums[0]++;
            else if (!strcmp(key, "v")) 
                nums[1]++;
            else if (!strcmp(key, "g")) 
                nums[2]++;
        }
    }
    rewind(file);
    return nums;
}

void destroy_model(struct model* mesh) {
    for (int i = 0; i < mesh->sizes[2]; i++) {
        free(mesh->groups[i].points);
        free(mesh->groups[i].faces);
    }
    free(mesh->groups);
    free(mesh);
}



struct model* parse_mesh(FILE* file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;

    printf("parsing mesh...\n");

    int* nums = npoints_nfaces(file);
    struct model *mesh = alloc_model(nums);
    mesh->sizes = nums;

    int* curr_iter = calloc(0, sizeof(int)*3);

    

    while(fgets(buffer, STRMAX, file)){
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            const char *lc = buffer + n;

            if (!strcmp(key, "f")){
                parse_face(lc, mesh, curr_iter[0]);
                curr_iter[0]++;
            }
            else if (!strcmp(key, "v")){
                parse_vector(lc, mesh, curr_iter[1]);
                curr_iter[1]++;
            }
            else if (!strcmp(key, "g")){  
                parse_layer(lc, mesh, curr_iter[2]);
                curr_iter[2]++;
            }
        }
    }
    
    //TEMP
    destroy_model(mesh);


    close_file(file);
    //return mesh;
    return NULL;
}


