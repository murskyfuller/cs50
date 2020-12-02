#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* NOTES
JPG_HDR = 0xffd8ffe0;
JPG_TRL = 0xffd9
*/

// create some useful definitions
typedef uint8_t BYTE;
typedef uint32_t BLOCK_HDR;

typedef struct
{
    BYTE header[4];
    BYTE rest[480];
} __attribute__((__packed__))
BLOCK;

// constant
int BLOCK_SIZE = 512;

// globals
int FILE_NUM = 0;
char FILE_NAME[7];

// interfaces
int validate_args(int argc, char *argv[]); // checks to make sure arguments appear valid
void recover_images(FILE *forensic_image); // iterates over each 512B block and attempts to recover any images
void save_image(FILE *forensic_image, int start_block, int end_block); // saves an identified image to a new file
bool block_starts_image(BLOCK *block); // determines whether block has the provided start signature

int main(int argc, char *argv[])
{
    // validate the command line arguements
    int error = validate_args(argc, argv);
    if (error > 0)
    {
        printf("CORRECT USAGE: recover <forensic image file path>\n");
        return 1;
    }

    // handle the file to recover from
    FILE *forensic_image = fopen(argv[1], "r");
    recover_images(forensic_image);
    fclose(forensic_image);

    return 0;
}

void recover_images(FILE *forensic_image)
{
    // iterate each block
    BLOCK *block = malloc(BLOCK_SIZE);
    int block_ix = -1;
    int image_start = -1;
    int found_bytes = -1;

    do
    {
        found_bytes = fread(block, 1, BLOCK_SIZE, forensic_image);
        block_ix++;

        // initiate a check to see if a recovered image needs to be saved
        // if this block starts an image or we've reached the end of the card
        if (block_starts_image(block) || found_bytes == 0)
        {
            // if an image was already being tracked, write the data up until this point
            // into a new file (aka recovered image)mall
            if (image_start > -1)
            {
                save_image(forensic_image, image_start, block_ix);
                fseek(forensic_image, BLOCK_SIZE, SEEK_CUR); // while saving we ended back at start of the next img
            }

            image_start = block_ix; // set the start of the new image
        }
    }
    while (found_bytes == BLOCK_SIZE); // if not we're at end of card

    // free up the block memory that was malloc'd
    free(block);
};

void save_image(FILE *forensic_image, int start_block, int end_block)
{
    // check for a basic errors
    if (forensic_image == NULL)
    {
        printf("SAVE IMAGE: No file to read from provided\n");
        return;
    }

    if (end_block <= start_block)
    {
        printf("Invalid start and end block arguments: start->%i end->%i\n", start_block, end_block);
        return;
    }

    // read the whole image into memory
    int image_size = (end_block - start_block) * BLOCK_SIZE;
    BYTE *to_save = malloc(image_size);
    fseek(forensic_image, start_block * BLOCK_SIZE, SEEK_SET); // move back to the start of the found image
    fread(to_save, image_size, 1, forensic_image);

    // generate the new file
    sprintf(FILE_NAME, "%03d.jpg", FILE_NUM); // create the new file name
    FILE *output = fopen(FILE_NAME, "w");
    if (output == NULL)
    {
        printf("Could not create file for recovered image: %s\n", FILE_NAME);
        return;
    }

    // write and, close and wrap up
    fwrite(to_save, image_size, 1, output);
    fclose(output);
    free(to_save);
    FILE_NUM++;
}

bool block_starts_image(BLOCK *block)
{
    bool result = false;

    // it's a match
    if (block->header[0] == 0xff && block->header[1] == 0xd8 && block->header[2] == 0xff && (block->header[3] & 0xf0) == 0xe0)
    {
        result = true;
    }

    return result;
}

int validate_args(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid argument count: %i\n", argc);
        return 101;
    }

    FILE *forensic_image = fopen(argv[1], "r");
    if (forensic_image == NULL)
    {
        printf("Error opening file: %s\n", argv[1]);
        return 102;
    }

    fclose(forensic_image);
    return 0;
}
