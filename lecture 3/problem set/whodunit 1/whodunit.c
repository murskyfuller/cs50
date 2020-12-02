// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bmp.h"

bool is_white(RGBTRIPLE *triple); // returns true if RGBTRIPLE meets function definition for white, otherwise flase
bool is_red(RGBTRIPLE *triple); // returns true if RGBTRIPLE meets function definition for red, otherwise false
void make_white(RGBTRIPLE *triple); // makes the RGBTRIPLE value at the pointer solid white
void make_black(RGBTRIPLE *triple); // makes the RGBTRIPLE value at the pointer solid black

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];
    char *outfile = argv[2];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            if (!is_white(&triple))
            {
                if (is_red(&triple))
                {
                    make_white(&triple);
                }
                else
                {
                    make_black(&triple);
                }
            }
            

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

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

void make_white(RGBTRIPLE *triple)
{
    // set all three color intensities to MAX
    triple->rgbtBlue = 255;
    triple->rgbtGreen = 255;
    triple->rgbtRed = 255;
}

void make_black(RGBTRIPLE *triple)
{
    // set all three color intensities to 0
    triple->rgbtRed = 0;
    triple->rgbtBlue = 0;
    triple->rgbtGreen = 0;
}

// 
bool is_red(RGBTRIPLE *triple)
{
    bool result = false;
    
    // allow some flexibility for more legibility
    int threshold = 0.95 * triple->rgbtRed;
    
    // if the values of blue and green for the pixel are under the
    // calculated threshold value then the pixel is deemed to be red
    if (threshold > triple->rgbtBlue && threshold > triple->rgbtGreen)
    {
        result = true;
    }
    
    return result;
}

bool is_white(RGBTRIPLE *triple)
{
    bool result = false;
    
    // white is being defined as any pixel that has a value of 240 or more for all
    // RGB values
    if (triple->rgbtBlue > 240 && triple->rgbtRed > 240 && triple->rgbtGreen > 240)
    {
        result = true;
    }
    
    return result;
}