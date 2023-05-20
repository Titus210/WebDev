#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK_SIZE 512

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Check if the user provided the correct number of command-line arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // Open the memory card file for reading
    FILE *file = fopen(argv[1], "r");

    // Check if the memory card file could not be opened for reading
    if (file == NULL)
    {
        fprintf(stderr, "Could not open %s for reading.\n", argv[1]);
        return 1;
    }

    BYTE buffer[BLOCK_SIZE]; // Create a buffer to hold a block of data
    int image_count = 0;     // Initialize the image counter to zero
    FILE *image = NULL;      // Initialize the output file pointer to NULL

    while (fread(buffer, BLOCK_SIZE, 1, file)) // Read a block of data from the memory card file
    {
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // If the block of data starts with the JPEG signature, close the previous image (if any) and open a new one
            if (image != NULL)
            {
                fclose(image);
            }

            char filename[8];
            sprintf(filename, "%03i.jpg", image_count++);
            image = fopen(filename, "w");
        }

        if (image != NULL)
        {
            // Write the block of data to the current image (if any)
            fwrite(buffer, BLOCK_SIZE, 1, image);
        }
    }

    // Close the last image (if any) and the memory card file
    if (image != NULL)
    {
        fclose(image);
    }

    fclose(file);
    return 0;
}
