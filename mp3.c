#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mp3.h"

// ---------------- Validation ----------------
int valid_filename(const char *filename)
{
    if (filename == NULL)
        return 0;

    const char *dot = strrchr(filename, '.'); // Find last '.' in filename
    if (dot != NULL && strcasecmp(dot, ".mp3") == 0)
        return 1;
    else
    {
        printf("Invalid filename\n");
        return 0;
    }
}

int valid_tag(const char *tag)
{
    if(tag == NULL)
        return 0;

    if(strlen(tag) != 4)  // Tag must be exactly 4 characters
    {
        printf("Invalid! tag must contain 4 character(eg. TIT2, TPE1..)\n");
        return 0;
    }

    for(int i=0; i<4; i++)
    {
        if(!isalnum(tag[i]))  // Only letters/digits allowed
        {
            printf("Invalid! tag must be in letters or digits\n");
            return 0;
        }
    }
    return 1;
}

// ---------------- Utility ----------------
void lit_to_big(int *num, int size)
{
    unsigned char *ptr = (unsigned char *)num;
    for (int i = 0; i < size / 2; i++)
    {
        unsigned char temp = ptr[i];
        ptr[i] = ptr[size - 1 - i];  // Swap bytes
        ptr[size - 1 - i] = temp;
    }
}




// ---------------- View function ----------------
void view(const char *filename)
{
    if (!valid_filename(filename))
        return;

    FILE *fptr = fopen(filename, "rb");
    if (!fptr)
    {
        perror("Error opening file");
        return;
    }

    unsigned char header[10];
    fread(header, 1, 10, fptr);  // Read ID3 header

    if (memcmp(header, "ID3", 3) != 0)  // Check if ID3v2 exists
    {
        printf("No ID3v2 tag found!\n");
        fclose(fptr);
        return;
    }

    // Calculate size of entire ID3 tag (syncsafe integer)
    int tag_size = ((header[6] & 0x7F) << 21) |
                   ((header[7] & 0x7F) << 14) |
                   ((header[8] & 0x7F) << 7)  |
                   (header[9] & 0x7F);

    printf("\n===============================================\n");
    printf("-------------- MP3 TAG DETAILS ----------------\n");
    printf("===============================================\n");

    int bytes_read = 0;

    while (bytes_read < tag_size)
    {
        ID3V2 tag;

        if (fread(tag.frame_id, 1, 4, fptr) != 4)  // Read frame ID
            break;
        tag.frame_id[4] = '\0';

        if (tag.frame_id[0] == 0)  // End of frames
            break;

        unsigned char size_bytes[4];//byte buffer
        fread(size_bytes, 1, 4, fptr);  // Read frame size (big-endian)

        int temp_size;//int buffer
        memcpy(&temp_size, size_bytes, 4);  // Copy bytes to int
        lit_to_big(&temp_size, sizeof(temp_size)); // Convert from BIG-ENDIAN (file) → LITTLE-ENDIAN (memory)
        tag.size = temp_size;  // Store frame size

        fread(tag.flags, 1, 2, fptr);  // Read 2-byte flags

        if (tag.size > sizeof(tag.text) - 1)
            tag.size = sizeof(tag.text) - 1;  // Prevent overflow

        fread(tag.text, 1, tag.size, fptr);  // Read frame content
        tag.text[tag.size] = '\0';           // Null-terminate string

        // Print tag based on frame ID
        if (strcmp(tag.frame_id, "TIT2") == 0)
            printf("Title       : %s\n", tag.text + 1); // Skip encoding byte
        else if (strcmp(tag.frame_id, "TPE1") == 0)
            printf("Artist      : %s\n", tag.text + 1);
        else if (strcmp(tag.frame_id, "TALB") == 0)
            printf("Album       : %s\n", tag.text + 1);
        else if (strcmp(tag.frame_id, "TYER") == 0)
            printf("Year        : %s\n", tag.text + 1);
        else if (strcmp(tag.frame_id, "TCON") == 0)
            printf("Genre       : %s\n", tag.text + 1);
        else if (strcmp(tag.frame_id, "COMM") == 0)
            printf("Comment     : %s\n", tag.text + 4);  // Skip encoding + language bytes

        bytes_read += 10 + tag.size;  // Update bytes read (frame header + content)
    }

    printf("===============================================\n");
    fclose(fptr);
}




// ---------------- Edit function ----------------
void edit(const char *option, const char *data, const char *filename)
{
    if (!valid_filename(filename))
        return;

    char tag_id[5];

     // Map CLI option to frame ID
    if (strcmp(option, "-t") == 0) strcpy(tag_id, "TIT2");
    else if (strcmp(option, "-A") == 0) strcpy(tag_id, "TALB");
    else if (strcmp(option, "-a") == 0) strcpy(tag_id, "TPE1");
    else if (strcmp(option, "-M") == 0) strcpy(tag_id, "COMM");
    else if (strcmp(option, "-Y") == 0) strcpy(tag_id, "TYER");
    else if (strcmp(option, "-C") == 0) strcpy(tag_id, "TCON");
    else
    {
        printf("Invalid option\n");
        show_help();
        return;
    }
    if (!valid_tag(tag_id))
        return;

    FILE *oldfile = fopen(filename, "rb");
    if (!oldfile) 
    {
         perror("Error opening file"); 
         return; 
    }

    FILE *tempfile = fopen("temp.mp3", "wb");
    if (!tempfile) 
    { perror("Error creating temp file"); 
        fclose(oldfile); 
        return; 
    }

    unsigned char header[10];
    fread(header, 1, 10, oldfile);  // Read ID3 header
    fwrite(header, 1, 10, tempfile); // Copy header to temp file

    int tag_size = ((header[6] & 0x7F) << 21) |
                   ((header[7] & 0x7F) << 14) |
                   ((header[8] & 0x7F) << 7)  |
                   (header[9] & 0x7F);

    int bytes_read = 0;
    while (bytes_read < tag_size)
    {
        ID3V2 tag;
        if (fread(tag.frame_id, 1, 4, oldfile) != 4) break;
        tag.frame_id[4] = '\0';

        if (tag.frame_id[0] == 0) break;

        unsigned char size_bytes[4];
        fread(size_bytes, 1, 4, oldfile);

        // Manually Convert BIG-ENDIAN (file) → LITTLE-ENDIAN (memory)
        tag.size = (size_bytes[0]<<24) | (size_bytes[1]<<16) 
        | (size_bytes[2]<<8) | size_bytes[3]; 

        fread(tag.flags, 1, 2, oldfile);

        if (tag.size >= sizeof(tag.text)) 
            tag.size = sizeof(tag.text) - 1;
        fread(tag.text, 1, tag.size, oldfile);
        tag.text[tag.size] = '\0';

        // Update frame if it matches the tag_id
        if (strcmp(tag.frame_id, tag_id) == 0)
        {
            printf("Editing %s -> Old: %s\n", tag.frame_id, tag.text);

            int new_size = strlen(data);
            int total_size = new_size + 1;  // +1 for encoding byte
            if (strcmp(tag.frame_id, "COMM") == 0) 
                total_size += 3; // +3 for language bytes

            int big_size = total_size;
            lit_to_big(&big_size, sizeof(big_size)); // Convert from LITTLE-ENDIAN (memory) → BIG-ENDIAN (file)

            fwrite(tag.frame_id, 1, 4, tempfile);
            fwrite(&big_size, 1, 4, tempfile);  // Write updated frame size

            /*unsigned char size_bytes[4]; //manually also we can convert big to little endian 
            fread(size_bytes, 1, 4, oldfile); 
            tag.size = (size_bytes[0]<<24) | (size_bytes[1]<<16) 
            | (size_bytes[2]<<8) | size_bytes[3];*/

            unsigned char flags[2] = {0, 0};
            fwrite(flags, 1, 2, tempfile); // Write flags

            if (strcmp(tag.frame_id, "COMM") == 0)
            {
                unsigned char comm_header[4] = {0x00, 'e','n','g'};
                fwrite(comm_header, 1, 4, tempfile); // Encoding + language
            }
            else
            {
                unsigned char encoding = 0x00; // ISO-8859-1
                fwrite(&encoding, 1, 1, tempfile);
            }

            fwrite(data, 1, new_size, tempfile); // Write new text
            printf("%s updated successfully!\n", tag.frame_id);
        }
        else
        {
            // Copy frame as-is if not matching
            fwrite(tag.frame_id, 1, 4, tempfile);
            fwrite(size_bytes, 1, 4, tempfile);
            fwrite(tag.flags, 1, 2, tempfile);
            fwrite(tag.text, 1, tag.size, tempfile);
        }

        bytes_read += 10 + tag.size;
    }

    // Copy remaining audio data
    unsigned char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), oldfile)) > 0)
        fwrite(buffer, 1, n, tempfile);

    fclose(oldfile);
    fclose(tempfile);

    remove(filename);           // Delete old file
    rename("temp.mp3", filename); // Rename temp file

    printf("File updated successfully.\n");
}



// ---------------- Show help ----------------
void show_help()
{
    printf("\nUsage:\n");
    printf("  ./a.out -v <filename>\n");
    printf("  ./a.out -e <option> <data> <filename>\n\n");

    printf("Options:\n");
    printf("  -t   Title (TIT2)\n");
    printf("  -a   Artist (TPE1)\n");
    printf("  -A   Album (TALB)\n");
    printf("  -Y   Year (TYER)\n");
    printf("  -C   Content Type (TCON)\n");
    printf("  -M   Comment (COMM)\n");

    printf("\nExamples:\n");
    printf("  ./a.out -v song.mp3\n");
    printf("  ./a.out -e -t \"New Title\" song.mp3\n");
    printf("  ./a.out -e -a \"Kuladeep\" song.mp3\n\n");
}
