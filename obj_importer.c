// g    =   Named polygon groups. (Material)
// v    =   List of geometric vertices, with (x, y, z, [w]) coordinates, w is optional and defaults to 1.0.
// vt   =   List of texture coordinates, in (u, [v, w]) coordinates, these will vary between 0 and 1. v, w are optional and default to 0.
// vn   =   List of vertex normals in (x,y,z) form; normals might not be unit vectors.
// f    =   Polygonal face element.
// #    =   Comment.

#include "obj_importer.h"
#define STRMAX 1024

unsigned int tmp = 0;

int imported_number_of_layers = 0;
double x_max = DBL_MIN, y_max = DBL_MIN, z_max = DBL_MIN, x_min = DBL_MAX, y_min = DBL_MAX, z_min = DBL_MAX;

void close_file(FILE* file){
    if (fclose(file))
        fprintf(stderr, "error: the file could not be closed\n");
}

struct model* import_mesh(char* path, char* mat_file_path){
    if (access(path, F_OK) != 0) {
        fprintf (stderr, "error: the file '%s' could not be found\n", path);
        return NULL;
    }
    if (access(mat_file_path, F_OK) != 0) {
        fprintf (stderr, "error: the file '%s' could not be found\n", mat_file_path);
        return NULL;
    }

    FILE* file = fopen(path, "r");
    FILE* mat_file = fopen(mat_file_path, "r");

    if(!file){
        fprintf (stderr, "error: the file '%s' could not be opened\n", path);
        close_file(file);
        return NULL;
    }
    if(!mat_file){
        fprintf (stderr, "error: the file '%s' could not be opened\n", mat_file_path);
        close_file(mat_file);
        return NULL;
    }

    printf("\nOBJ file found with given path: %s\n", path);
    printf("\nMAT file found with given path: %s\n\n", mat_file_path);

    return parse_mesh(file, mat_file);
}

void parse_face(const char* line, struct model* mesh, int* n){
    char editable_line[STRMAX];
    strcpy(editable_line, line);

    char *v1_tok, *v2_tok, *v3_tok, *v4_tok;
    v1_tok = strtok(editable_line, " ");
    v2_tok = strtok(NULL, " ");
    v3_tok = strtok(NULL, " ");
    v4_tok = strtok(NULL, " ");

    char *v1, *v2, *v3, *v4;
    v1 = strtok(v1_tok, "/");
    v2 = strtok(v2_tok, "/");
    v3 = strtok(v3_tok, "/");
    v4 = strtok(v4_tok, "/");

    //-1 for array offset
    mesh->groups[n[2]].faces[n[0]][0] = atoi(v1);
    mesh->groups[n[2]].faces[n[0]][1] = atoi(v2);
    mesh->groups[n[2]].faces[n[0]][2] = atoi(v3);
    if(atoi(v3) == atoi(v4) || v4_tok == NULL || strlen(v4_tok) <= 0)
        mesh->groups[n[2]].faces[n[0]][3] = 0;
    else
        mesh->groups[n[2]].faces[n[0]][3] = atoi(v4);

    tmp++;
}

void parse_vector(const char* line, struct model* mesh, int* n){
    char editable_line[STRMAX];
    strcpy(editable_line, line);
    char* tokens = strtok(editable_line, " ");
    double coordinates[3];
    
    for (int i = 0; i < 3; i++) {
        coordinates[i] = strtod(tokens, NULL);
        tokens = strtok(NULL, " ");
    }

    x_max = max(x_max, coordinates[0]);
    y_max = max(y_max, coordinates[1]);
    z_max = max(z_max, coordinates[2]);

    x_min = min(x_min, coordinates[0]);
    y_min = min(y_min, coordinates[1]);
    z_min = min(z_min, coordinates[2]);
    
    mesh->points[n[1]][0] = coordinates[0];
    mesh->points[n[1]][1] = coordinates[1];
    mesh->points[n[1]][2] = coordinates[2];
}

void parse_layer(struct model* mesh, int* n){
    mesh->groups[n[2]].material_label = n[2];
}

struct model* alloc_model(unsigned int** sizes){
    struct model *mesh = (struct model*)calloc(1, sizeof(struct model));
    mesh->groups = (struct material_group*)malloc(imported_number_of_layers * sizeof(struct material_group));

    mesh->points = (double**)malloc(sizes[0][1] * sizeof(double*));
    for(int i = 0; i < sizes[0][1]; i++)
        mesh->points[i] = (double*)calloc(3, sizeof(double));

    for(int i = 0; i < imported_number_of_layers; i++){
        //UNCOMMENT TO PRINT NUMBER OF FACES PER LAYER
        //printf("Layer %d contains: %d faces\n",i,sizes[i][0]);
        mesh->groups[i].faces = (unsigned int**)malloc(sizes[i][0] * sizeof(unsigned int*));
        for(int j = 0; j < sizes[i][0]; j++)
            mesh->groups[i].faces[j] = (unsigned int*)calloc(4, sizeof(unsigned int));
    }

    return mesh;
}

unsigned int nlayers(FILE* file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;
    int layers = 0;

    while(fgets(buffer, STRMAX, file))
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            if (!strcmp(key, "g"))
                layers++;      
        }
    rewind(file);
    return layers == 0 ? 1 : layers;
}

unsigned int** npoints_nfaces(FILE* file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;
    int curr_layer = -1;

    imported_number_of_layers = nlayers(file);

    unsigned int** nums = malloc(sizeof(unsigned int*) * imported_number_of_layers);
    for(int i = 0; i < imported_number_of_layers; i++){
        nums[i] = (unsigned int*) calloc(2,sizeof(unsigned int));
    }
    
    while(fgets(buffer, STRMAX, file)){
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            if (!strcmp(key, "f"))
                nums[curr_layer][0]++;    
            else if (!strcmp(key, "v")) 
                nums[0][1]++;   
            else if (!strcmp(key, "g"))
                curr_layer++;
        }
    }          

    rewind(file);
    return nums;
}

void destroy_model(struct model* mesh) {
    for (int i = 0; i < mesh->n_layers; i++) {
        for(int f = 0; f < mesh->sizes[i][0]; f++)
            free(mesh->groups[i].faces[f]);
        free(mesh->groups[i].faces);
    }
    for(int i = 0; i < mesh->sizes[0][1]; i++)
        free(mesh->points[i]);
    for(int i = 0; i < imported_number_of_layers; i++)
        free(mesh->sizes[i]);
    free(mesh->sizes);
    free(mesh->points);
    free(mesh->groups);
    free(mesh);
}

void parse_material_properties(FILE* mat_file, struct model* mesh){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;
    int curr_layer = -1;
    printf("\n");
    while(fgets(buffer, STRMAX, mat_file)){
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            const char *line_content = buffer + n;
            if (!strcmp(key, "l")) //l = layer
                curr_layer++;
            else if (!strcmp(key, "f")){ //f = fill
                char editable_line[STRMAX];
                strcpy(editable_line, line_content);
                char* token = strtok(editable_line, " ");
                mesh->groups[curr_layer].is_hollow = atoi(token);
                //UNCOMMENT FOR OUTPUT: IF LAYER IS FILLED OR NOT
                //printf("Layer %d will %s\n", curr_layer, mesh->groups[curr_layer].is_hollow ? "be filled" : "be hollow");
            }
        }
    }       

}

struct model* parse_mesh(FILE* file, FILE* mat_file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;

    printf("Parsing mesh...\n");

    unsigned int** nums = npoints_nfaces(file);
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
                curr_iter[2]++;
                parse_layer(mesh, curr_iter);
            }
        }
    }

    mesh->x_max = x_max;
    mesh->y_max = y_max;
    mesh->z_max = z_max;
    mesh->x_min = x_min;
    mesh->y_min = y_min;
    mesh->z_min = z_min;

    parse_material_properties(mat_file, mesh);

    printf("Model parsing -> Completed\n\n");

    free(curr_iter);
    close_file(file);
    mesh->n_layers = imported_number_of_layers;

    return mesh;
}