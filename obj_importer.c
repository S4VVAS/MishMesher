// g    =   Named polygon groups. (Material)
// v    =   List of geometric vertices, with (x, y, z, [w]) coordinates, w is optional and defaults to 1.0.
// vt   =   List of texture coordinates, in (u, [v, w]) coordinates, these will vary between 0 and 1. v, w are optional and default to 0.
// vn   =   List of vertex normals in (x,y,z) form; normals might not be unit vectors.
// f    =   Polygonal face element.
// #    =   Comment.

#include "obj_importer.h"
#define STRMAX 1024

int imported_number_of_layers = 0;

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

void parse_face(const char* line, struct model* mesh, int* n){
    char editable_line[STRMAX];
    strcpy(editable_line, line);

    char* x_tok, *y_tok, *z_tok;
    x_tok = strtok(editable_line, " ");
    y_tok = strtok(NULL, " ");
    z_tok = strtok(NULL, " ");

    char* x, *y, *z;
    x = strtok(x_tok, "/");
    y = strtok(y_tok, "/");
    z = strtok(z_tok, "/");

    mesh->groups[n[2]].faces[n[0]][0] = atoi(x);
    mesh->groups[n[2]].faces[n[0]][1] = atoi(y);
    mesh->groups[n[2]].faces[n[0]][2] = atoi(z);
}

void parse_vector(const char* line, struct model* mesh, int* n){
    char editable_line[STRMAX];
    strcpy(editable_line, line);
    char* tokens = strtok(editable_line, " ");
    double coordinates[3];
    char* pEnd;
    
    coordinates[0] = strtod(tokens, &pEnd);
    coordinates[1] = strtod(pEnd, &pEnd);
    coordinates[2] = strtod(pEnd, NULL);

    mesh->groups[n[2]].points[n[1]][0] = coordinates[0];
    mesh->groups[n[2]].points[n[1]][1] = coordinates[1];
    mesh->groups[n[2]].points[n[1]][2] = coordinates[2];
}

void parse_layer(struct model* mesh, int* n){
    mesh->groups[n[2]].material_label = n[2];
}

struct model* alloc_model(int** sizes){
    struct model *mesh = (struct model*)malloc(sizeof(struct model));
    mesh->groups = (struct material_group*)malloc(imported_number_of_layers * sizeof(struct material_group));

    
    for(int i = 0; i < imported_number_of_layers; i++){
        printf("Layer %d contains: %d vertices and %d faces\n",i,sizes[i][1],sizes[i][0]);
        mesh->groups[i].points = (double**)malloc(sizes[i][1] * sizeof(double*));
        for(int j = 0; j < sizes[i][1]; j++)
            mesh->groups[i].points[j] = (double*)calloc(3, sizeof(double));

        mesh->groups[i].faces = (unsigned int**)malloc(sizes[i][0] * sizeof(unsigned int*));
        for(int j = 0; j < sizes[i][0]; j++)
            mesh->groups[i].faces[j] = (unsigned int*)calloc(3, sizeof(unsigned int));
    }

    return mesh;
}

int nlayers(FILE* file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;
    
    int layers = 0;

    while(fgets(buffer, STRMAX, file))
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            const char *line_content = buffer + n;
            if (!strcmp(key, "g"))
                layers++;      
        }
    rewind(file);
    return layers;
}

int** npoints_nfaces(FILE* file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;
    int curr_layer = -1;

    imported_number_of_layers = nlayers(file);
    
    int** nums = malloc(sizeof(int*) * imported_number_of_layers);
    for(int i = 0; i < imported_number_of_layers; i++)
        nums[i] = (int*) calloc(2,sizeof(int));

    while(fgets(buffer, STRMAX, file)){
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            const char *line_content = buffer + n;

            if (!strcmp(key, "f")) 
                nums[curr_layer][0]++;
            else if (!strcmp(key, "v")) 
                nums[curr_layer][1]++;
            else if (!strcmp(key, "g"))
                curr_layer++;
        }
    }                

    rewind(file);
    return nums;
}

void destroy_model(struct model* mesh) {
    for (int i = 0; i < imported_number_of_layers; i++) {
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

    int** nums = npoints_nfaces(file);
    struct model *mesh = alloc_model(nums);
    mesh->sizes = nums;

    int* curr_iter = calloc(3, sizeof(int));

    //Start at layer -1 as it gets incremented straight away to 0
    curr_iter[2] = -1;

    while(fgets(buffer, STRMAX, file)){
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            const char *lc = buffer + n;

            if (!strcmp(key, "f")){
                parse_face(lc, mesh, curr_iter);
                curr_iter[0]++;
            }
            else if (!strcmp(key, "v")){
                parse_vector(lc, mesh, curr_iter);
                curr_iter[1]++;
            }
            else if (!strcmp(key, "g")){ 
                curr_iter[0] = 0;
                curr_iter[1] = 0;
                curr_iter[2]++;
                parse_layer(mesh, curr_iter);
            }
        }
    }
    printf("finnished model parsing\n");

    free(curr_iter);
    close_file(file);
    return mesh;
}
