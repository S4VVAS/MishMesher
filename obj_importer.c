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

void parse_face(const char* line){

}

void parse_vector(const char* line){
    
}

void parse_layer(const char* line){
    
}

struct model* parse_mesh(FILE* file){
    char buffer[STRMAX];
    char key[STRMAX];
    int n;
    while(fgets(buffer, STRMAX, file)){
        if(sscanf(buffer, "%s%n", key, &n) > 0){
            const char *line_content = buffer + n;

            if (!strcmp(key, "f")) 
                parse_face(line_content);
            else if (!strcmp(key, "v")) 
                parse_vector(line_content);
            else if (!strcmp(key, "g")) 
                parse_layer(line_content);
            //Skip unnecessary lines 
        }
    }



    close_file(file);
    return NULL;
}


