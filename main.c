#include <stdio.h>
#include <stdlib.h>

int buffer, resolution, max_cores;

//CURRENT ARGS
//  model-path  resolution  edge-buffer-size  opt:output-file-name  opt:core-count
int main(int argc, char** argv){

     
    
    //Check for correct num of arguments
    //Setup using arguments
    setup(argv);

    
    sort();
    

    while(!CheckForQuit()){
        updateGraphics();
    }
    
    
    for(int i = size-10; i < size; i++){
       
                printf("%d = %f\n", i ,elements[i]);
        
        
    }
   // printf("size: %d\n",size);
    

    
    clean();
}