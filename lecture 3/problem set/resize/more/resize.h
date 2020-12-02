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
void resize_rows(DIMENSION *cur_dimensions, DIMENSION *new_dimensions, FILE *in_ptr, FILE *out_ptr); // modifies the scanlines of the in BMP to the new width dimension
void resize_columns(DIMENSION *cur_dimensions, DIMENSION *new_dimensions, FILE *in_ptr, FILE *out_ptr); // modifies the 'columns' of the in BMP to the new height
void copy(FILE *inptr, FILE *outpr, DIMENSION *cur_dimensions); // copies the provided inpt to the outpr, but ignores header files in favor of cur_dimensions
bool are_pixels_equal(RGBTRIPLE a, RGBTRIPLE b); // determines whether each byte value of a equals the corresponding byte value of b; if 1 is not equal returns false