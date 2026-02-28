#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp3.h"

int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        printf("Invalid!\n");
        show_help();
        return 0;
    }

    // View mode
    if(strcmp(argv[1],"-v")==0)
    {
        if(argc == 3)
        {
            view(argv[2]);
        }
        else
        {
            printf("Invalid usage for view\n");
            show_help();
        }
    }
    // Edit mode
    else if(strcmp(argv[1],"-e")==0)
    {
        if(argc == 5)
        {
            edit(argv[2],argv[3],argv[4]);
        }
        else
        {
            printf("Invalid usage for edit\n");
            show_help();
        }
    } 
    // Invalid option
    else
    {
        printf("Invalid option %s\n",argv[1]);
        show_help();
    }
    
    return 0;
}
