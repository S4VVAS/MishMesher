#include "obj_converter.h"

double map[8][3] = {
    {0, 0, 0},
    {0, 0, -0.5},
    {-0.5, 0, 0},
    {-0.5, 0, -0.5},
    {0, -0.5, 0},
    {0, -0.5, -0.5},
    {-0.5, -0.5, 0},
    {-0.5, -0.5, -0.5},
};


//Each coord is at top front/right corner of the box
//ptof = print to object file
unsigned int traverse_tree_ptof(struct octree* node, FILE* file, struct vector3 offsets, double box_size, unsigned int v_offset){
    double b_div_2 = box_size * 0.5;

    if(node->level <= 1){
        uint8_t mask = node->is_voxels_solid;
        for(int i = 0; i < 8; i++){
            if(mask & 1 << i){
                double x = offsets.x + (map[i][0] * box_size);
                double y = offsets.y + (map[i][1] * box_size);
                double z = offsets.z + (map[i][2] * box_size);
                //Print the 8 vertices
                fprintf(file, "v %f %f %f\nv %f %f %f\nv %f %f %f\nv %f %f %f\nv %f %f %f\nv %f %f %f\nv %f %f %f\nv %f %f %f\n",
                    x,y,z,
                    x,y,z-b_div_2,
                    x+b_div_2, y, z,
                    x+b_div_2, y, z-b_div_2,
                    x, y-b_div_2, z,
                    x, y-b_div_2, z-b_div_2,
                    x+b_div_2, y-b_div_2, z,
                    x+b_div_2, y-b_div_2, z-b_div_2);
                //Print the 12 faces
                fprintf(file, "f %d %d %d\n", v_offset + 1, v_offset + 3, v_offset + 2);
                fprintf(file, "f %d %d %d\n", v_offset + 2, v_offset + 3, v_offset + 4);

                fprintf(file, "f %d %d %d\n", v_offset + 1, v_offset + 2, v_offset + 5);
                fprintf(file, "f %d %d %d\n", v_offset + 5, v_offset + 2, v_offset + 6);

                fprintf(file, "f %d %d %d\n", v_offset + 4, v_offset + 3, v_offset + 8);
                fprintf(file, "f %d %d %d\n", v_offset + 3, v_offset + 7, v_offset + 8);

                fprintf(file, "f %d %d %d\n", v_offset + 3, v_offset + 1, v_offset + 5);
                fprintf(file, "f %d %d %d\n", v_offset + 3, v_offset + 5, v_offset + 7);

                fprintf(file, "f %d %d %d\n", v_offset + 2, v_offset + 4, v_offset + 6);
                fprintf(file, "f %d %d %d\n", v_offset + 4, v_offset + 8, v_offset + 6);

                fprintf(file, "f %d %d %d\n", v_offset + 6, v_offset + 7, v_offset + 5);
                fprintf(file, "f %d %d %d\n", v_offset + 6, v_offset + 8, v_offset + 7);
                
                v_offset+=8;
            }
        }
        return v_offset;
    }
    else if(node->hasChildren)
        for (int i = 0; i < 8; i++) {
            double x = offsets.x + (map[i][0] * box_size);
            double y = offsets.y + (map[i][1] * box_size);
            double z = offsets.z + (map[i][2] * box_size);
            v_offset = traverse_tree_ptof(&node->children[i], file, (struct vector3){x, y, z}, b_div_2, v_offset);
        }

    return v_offset;
}


void obj_convert(struct octree* tree, char* path, double box_size){
    FILE* file = fopen(path, "w");
    traverse_tree_ptof(tree, file, (struct vector3){0,0,0}, box_size, 0);
    fclose(file); 
}