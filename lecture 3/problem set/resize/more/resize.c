#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

// simple struct to make passing of args easier
typedef struct
{
    float width;
    float height;
} __attribute__((__packed__))
DIMENSION;

// function interfaces
bool validate_args(int argc, char *argv[]); // validates the passed CLI arguments; prints errors in console if any
float cast_resize(char *resize_factor); // converts the CLI factor arg to a float
FILE *validate_in_file(char *in_path); // makes sure the provided file to modify exists and is a valid 24-bit BMP
FILE *create_out_file(char *out_path, FILE *in_file, float resize_factor); // handles all the hard logic to create the intended file
BITMAPFILEHEADER *get_bf_header(FILE *in_file); // returns a pointer to malloc'd copy of the BMP file's file header
BITMAPINFOHEADER *get_bi_header(FILE *in_file); // returns a pointer to malloc'd copy of the BMP file's info header
// modifies the scanlines of the in BMP to the new width dimension
void resize_rows(DIMENSION *cur_dimensions, DIMENSION *new_dimensions, FILE *in_ptr, FILE *out_ptr);
// modifies the 'columns' of the in BMP to the new height
void resize_columns(DIMENSION *cur_dimensions, DIMENSION *new_dimensions, FILE *in_ptr, FILE *out_ptr);
// copies the provided inpt to the outpr, but ignores header files in favor of cur_dimensions
void copy(FILE *inptr, FILE *outpr, DIMENSION *cur_dimensions);
// determines whether each byte value of a equals the corresponding byte value of b; if 1 is not equal returns false
bool are_pixels_equal(RGBTRIPLE a, RGBTRIPLE b);

// constants
int TOTAL_HEADER_SIZE = 54;

int main(int argc, char *argv[])
{
    // validate the arguments and exit if not valid
    bool is_valid = validate_args(argc, argv);
    if (!is_valid)
    {
        return 1;
    }

    // cast the resize arg string to a float value
    float resize_factor = cast_resize(argv[1]);
    if (resize_factor <= 0)
    {
        // if casting failed exit
        return 2;
    }

    // filenames
    char *in_path = argv[2];
    char *out_path = argv[3];

    FILE *in_ptr = validate_in_file(in_path);
    if (in_ptr == NULL)
    {
        return 101;
    }

    FILE *out_ptr = create_out_file(out_path, in_ptr, resize_factor);
    if (out_ptr == NULL)
    {
        return 102;
    }

    // close the open files
    fclose(in_ptr);
    fclose(out_ptr);

    return 0;
}

float cast_resize(char *resize_factor)
{
    float result = atof(resize_factor);
    if (result <= 0.0 || result > 100.0)
    {
        printf("Resize factor must be a valid float value between 0.0 and 100.0\n");
        result = -1;
    }

    return result;
}

FILE *create_out_file(char *out_path, FILE *in_ptr, float resize_factor)
{
    // validate output file
    FILE *out_ptr = fopen(out_path, "w");
    if (out_ptr == NULL)
    {
        printf("Error opening outfile: %s\n", out_path);
        return NULL;
    }

    // calculate the new dimensions
    BITMAPFILEHEADER *in_bf = get_bf_header(in_ptr);
    BITMAPINFOHEADER *in_bi = get_bi_header(in_ptr);

    double cur_width = in_bi->biWidth;
    double new_width = cur_width * resize_factor;
    double cur_height = in_bi->biHeight;
    double new_height = cur_height * resize_factor;

    // store the current and new dimensions
    // for single argument use in upcoming functions
    DIMENSION *cur_dimensions = malloc(sizeof(DIMENSION));
    cur_dimensions->width = cur_width;
    cur_dimensions->height = cur_height;

    DIMENSION *new_dimensions = malloc(sizeof(DIMENSION));
    new_dimensions->width = new_width;
    new_dimensions->height = new_height;

    // temp file to store modified rows image
    FILE *tmp = tmpfile();
    if (tmp == NULL)
    {
        puts("Unable to create temp file");
        return 0;
    }

    // write the new headers
    int new_bitmap_byte_size = ceil(new_width * fabs(new_height) * sizeof(RGBTRIPLE));
    int new_padding_byte_size = ((4 - ((int)(new_width * sizeof(RGBTRIPLE)) % 4)) % 4) * fabs(new_height);
    int image_size = new_bitmap_byte_size + new_padding_byte_size;
    int byte_sum = image_size + TOTAL_HEADER_SIZE;
    in_bf->bfSize = byte_sum;
    fwrite(in_bf, sizeof(BITMAPFILEHEADER), 1, tmp);

    in_bi->biSizeImage = image_size;
    in_bi->biWidth = new_width;
    in_bi->biHeight = new_height;
    fwrite(in_bi, sizeof(BITMAPINFOHEADER), 1, tmp);

    // do the actual resizing
    resize_rows(cur_dimensions, new_dimensions, in_ptr, tmp);
    cur_dimensions->width = new_width;

    // another tmp file for column modifications, start by copying the first tmp file
    FILE *tmp2 = tmpfile();
    if (tmp2 == NULL)
    {
        puts("Unable to create temp file");
        return 0;
    }
    copy(tmp, tmp2, cur_dimensions);

    // resize the columns
    resize_columns(cur_dimensions, new_dimensions, tmp, tmp2);
    cur_dimensions->height = new_height;

    // a nice clean write to the output
    copy(tmp2, out_ptr, cur_dimensions);

    // free up the malloc
    free(in_bf);
    free(in_bi);
    free(new_dimensions);
    free(cur_dimensions);

    return out_ptr;
}

void resize_rows(DIMENSION *cur_dimensions, DIMENSION *new_dimensions, FILE *in_ptr, FILE *out_ptr)
{
    // so we don't have to keep referencing from the pointer
    float new_width = new_dimensions->width;
    float new_height = new_dimensions->height;
    float cur_width = cur_dimensions->width;
    float cur_height = fabs(cur_dimensions->height);
    float factor = new_width / cur_width;

    // helpful constants
    int rgb_bytes = sizeof(RGBTRIPLE);
    int difference = cur_width * sizeof(RGBTRIPLE);
    int padding = (4 - (difference % 4)) % 4;
    int bitmap_pixel_bytes = cur_width * rgb_bytes;
    int scanline_bytes = bitmap_pixel_bytes + padding;

    // iterate over infile's scanlines
    for (int i = 0; i < cur_height; i++)
    {
        fseek(in_ptr,  TOTAL_HEADER_SIZE + i * scanline_bytes, SEEK_SET); // start at beginning of scanline

        // iterate over pixels in scanline
        for (int j = 0, running_width = 0; j < cur_width;)
        {
            // log the current pixel value
            RGBTRIPLE triple;
            RGBTRIPLE compare;
            fread(&triple, sizeof(RGBTRIPLE), 1, in_ptr);
            fread(&compare, sizeof(RGBTRIPLE), 1, in_ptr);

            // iterate and keep count of the number of subsequent matching pixels
            int num_same = 1;
            while (are_pixels_equal(triple, compare) && j < cur_width)
            {
                // increment the counts and read in the next pixel
                num_same++;
                j++;
                fread(&compare, sizeof(RGBTRIPLE), 1, in_ptr);
            }

            // back up one so if we're part way through the line we re-scan this
            // into 'triple' when we continue
            fseek(in_ptr, -1 * sizeof(RGBTRIPLE), SEEK_CUR);

            // TODO make this better, e.g. 1/3 of large.bmp white square is no longer centered
            // when a new pixel value or end is reached multiply the total count by factor
            int new_count;
            if (factor < 1.0)
            {
                new_count = floor(num_same * factor);
            }
            else
            {
                new_count = ceil(num_same * factor);
            }

            // safety to make sure we don't go over
            if (running_width + new_count > new_width)
            {
                new_count = new_width - running_width;
            }

            // iterate over pixels in scanline
            for (int k = 0; k < new_count; k++)
            {
                // write RGB triple to outfile
                fwrite(&triple, sizeof(RGBTRIPLE), 1, out_ptr);
            }

            running_width += new_count;
        }

        // calculate the resized padding for the out file
        // then add it in
        int new_padding = (4 - (int)(new_width * sizeof(RGBTRIPLE)) % 4) % 4;
        for (int k = 0; k < new_padding; k++)
        {
            fputc(0x00, out_ptr);
        }
    }
}

void resize_columns(DIMENSION *cur_dimensions, DIMENSION *new_dimensions, FILE *in_ptr, FILE *out_ptr)
{
    // so we don't have to keep referencing from the pointer
    float new_width = new_dimensions->width;
    float new_height = fabsf(new_dimensions->height);
    float cur_width = cur_dimensions->width;
    float cur_height = fabs(cur_dimensions->height);
    float factor = new_height / cur_height;

    // some helpful constants
    int rgb_bytes = sizeof(RGBTRIPLE);
    int difference = cur_width * rgb_bytes;
    int padding = (4 - (difference % 4)) % 4;
    int row_with_padding = cur_width + padding;
    int row_with_padding_bytes = (cur_width * rgb_bytes) + padding;
    int next_pixel_in_column = row_with_padding_bytes - rgb_bytes;

    // iterate over infile's pixel columns
    for (int i = 0; i < cur_width; i++)
    {
        fseek(in_ptr,  TOTAL_HEADER_SIZE + i * rgb_bytes, SEEK_SET); // start at the head of the column

        // iterate over each pixel in column
        for (int j = 0, running_height = 0; j < cur_height;)
        {
            // set the initial pixel value to the head of the column
            RGBTRIPLE triple;
            RGBTRIPLE compare;
            fread(&triple, rgb_bytes, 1, in_ptr);

            // move to the pixel one row down and read it in for comparison
            fseek(in_ptr, next_pixel_in_column, SEEK_CUR);
            fread(&compare, rgb_bytes, 1, in_ptr);

            // iterate and keep count of the number of subsequent matching pixels
            int num_same = 1;
            while (are_pixels_equal(triple, compare) && j < cur_height)
            {
                num_same++;
                j++;

                // move another row down and read the pixel in
                fseek(in_ptr, next_pixel_in_column, SEEK_CUR);
                fread(&compare, rgb_bytes, 1, in_ptr);
            }
            fseek(in_ptr, -rgb_bytes, SEEK_CUR); // step back so when we start it's at the latest non-match

            // TODO make this better, e.g. 1/3 of large.bmp white square is no longer centered
            // when a new pixel value or end is reached multiply the total count by factor
            int new_count;
            if (factor < 1.0)
            {
                new_count = floor(num_same * factor);
            }
            else
            {
                new_count = ceil(num_same * factor);
            }

            // safety to make sure we don't go over
            if (running_height + new_count > new_height)
            {
                new_count = new_height - running_height;
            }

            // iterate over pixels in scanline
            for (int k = 0; k < new_count; k++)
            {
                // since we could be in the middle of a column we need to make sure we start
                // at the right spot
                int start_of_section = 54 + i * rgb_bytes + (row_with_padding_bytes * running_height);
                int pixel_pos = start_of_section + (row_with_padding_bytes * k);

                // write RGB triple to outfile
                fseek(out_ptr, pixel_pos, SEEK_SET);
                fwrite(&triple, rgb_bytes, 1, out_ptr);
            }

            running_height += new_count;
        }
    }

    // TODO abstract this for use in both resize_rows and here
    // add padding if necessary
    int new_width_bytes = new_width * rgb_bytes;
    int new_padding = (4 - (new_width_bytes % 4)) % 4;
    if (new_padding > 0)
    {
        fseek(out_ptr, TOTAL_HEADER_SIZE, SEEK_SET); // reset to start of image

        // for each scanline
        for (int i = 0; i < new_height; i++)
        {
            // navigate to beginning of scanline
            // then add the necessary padding after the pixels
            fseek(out_ptr, new_width_bytes, SEEK_CUR);
            for (int j = 0; j < new_padding; j++)
            {
                fputc(0x00, out_ptr);
            }
        }
    }
}

// quick port from copy.c example with some modifications
void copy(FILE *inptr, FILE *outptr, DIMENSION *cur_dimensions)
{
    fseek(outptr, 0, SEEK_SET);
    fseek(inptr, 0, SEEK_SET);

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    int cur_height = cur_dimensions->height;
    int cur_width = cur_dimensions->width;
    for (int i = 0, biHeight = abs(cur_height); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < cur_width; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }
}

// checks whether the two passed pixel values are exactly equal
bool are_pixels_equal(RGBTRIPLE a, RGBTRIPLE b)
{
    bool result = false;

    if (a.rgbtBlue == b.rgbtBlue && a.rgbtGreen == b.rgbtGreen && a.rgbtRed == b.rgbtRed)
    {
        result = true;
    }

    return result;
}

FILE *validate_in_file(char *in_path)
{
    // validate input file
    FILE *ptr = fopen(in_path, "r");
    if (ptr == NULL)
    {
        printf("Error opening input file: %s\n", in_path);
        return NULL;
    }

    // read in file BFHEADER and validate it
    BITMAPFILEHEADER *bf = get_bf_header(ptr);
    if (bf->bfType != 0x4D42 || bf->bfOffBits != 54)
    {
        printf("Incorrect file type for input: file header\n");
        return NULL;
    }

    // read INFOHEADER
    // and check info  to make sure it is a 24 bit uncompressed image
    BITMAPINFOHEADER *bi = get_bi_header(ptr);
    if (bi->biPlanes != 1 || bi->biBitCount != 24 || bi->biCompression != 0)
    {
        printf("Incorrect file info for input: info header\n");
        return NULL;
    }

    // free up the header pointers since they are malloc'd
    if (bf != NULL)
    {
        free(bf);
    }

    if (bi != NULL)
    {
        free(bi);
    }

    return ptr;
}

// creates a BITMAPFILEHEADER on the heap
BITMAPFILEHEADER *get_bf_header(FILE *ptr)
{
    BITMAPFILEHEADER *bf = malloc(sizeof(BITMAPFILEHEADER));
    fseek(ptr, 0, SEEK_SET); // make sure the cursor is at beginning
    fread(bf, sizeof(BITMAPFILEHEADER), 1, ptr);

    if (bf == NULL)
    {
        printf("Error retrieving file header\n");
        free(bf);
    }

    return bf;
}

// creates a BITMAPINFOHEADER on the heap
BITMAPINFOHEADER *get_bi_header(FILE *ptr)
{
    BITMAPINFOHEADER *bi = malloc(sizeof(BITMAPINFOHEADER));

    // make sure the cursor is at the start of the info header
    // TODO this could be made dynamic by reading the bfsize from the
    // BITMAPFILEHEADER
    fseek(ptr, sizeof(BITMAPFILEHEADER), SEEK_SET);
    fread(bi, sizeof(BITMAPINFOHEADER), 1, ptr);

    if (bi == NULL)
    {
        printf("Error retrieving info header\n");
        free(bi);
    }

    return bi;
}

bool validate_args(int argc, char *argv[])
{
    bool result = true;

    if (argc != 4)
    {
        printf("Usage: <resize-factor> <input> <output>\n");
        result = false;
    }

    return result;
}