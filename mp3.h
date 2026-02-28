#ifndef MP3_H
#define MP3_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

typedef struct 
{
    char frame_id[6];
    unsigned int size;
    unsigned char flags[2];
    char text[100];
} ID3V2;

//*************FUNCTION PROTOTYPE**************

// Validation
int valid_filename(const char *filename);
int valid_tag(const char *tag);

// Core functions
void lit_to_big(int *num, int size);
void view(const char *filename);
void edit(const char *option, const char *data, const char *filename);
void show_help();

#endif
