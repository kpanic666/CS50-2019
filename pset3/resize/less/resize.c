// Resize bmp file by n times.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "bmp.h"

#define MIN_MULTI 1
#define MAX_MULTI 100

typedef struct
{
    RGBTRIPLE *rgbt_ptr;
    BYTE *padding_ptr;
}
SCANLINE;

int main(int argc, char *argv[])
{
    // ensure proper usage. Only 3 arg permitted.
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // ensure that second argument (multiplicator n) will be an integer
    // and besides lay between 1 and 100 inclusive
    for (int i = 0; argv[1][i] != '\0'; i++)
    {
        if (isdigit(argv[1][i]) == 0)
        {
            printf("Usage: ./resize n infile outfile\n");
            return 1;
        }
    }

    int n = atoi(argv[1]);
    if (n < MIN_MULTI || n > MAX_MULTI)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi, old_bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    old_bi = bi;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // Rewrite some fields in headers to new values.
    // In new bmp file will be new width, height, size.
    bi.biWidth *= n;
    bi.biHeight *= n;
    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int old_padding = (4 - (old_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + padding) *
                     abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPINFOHEADER) +
                sizeof(BITMAPFILEHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Now let's allocate some memory in heap for 1 line of new
    // pixels + padding. We will copy it's value from original
    // image, multiply every pixel by n, add padding to the end
    // (if needed) and than just copy line by line to new file
    SCANLINE scanline;
    LONG slRgbtSize = bi.biWidth * sizeof(RGBTRIPLE);
    LONG slOffset;

    scanline.rgbt_ptr = malloc(slRgbtSize);
    scanline.padding_ptr = malloc(padding);

    if (scanline.rgbt_ptr == NULL || scanline.padding_ptr == NULL)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Cann't allocate memory for scanline.\n");
        return 5;
    }

    // Padding always the same for all lines. Calculate it once.
    for (int k = 0; k < padding; k++)
    {
        *(scanline.padding_ptr + k * sizeof(BYTE)) = 0x00;
    }

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(old_bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < old_bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write new RGB triple to memory. When we will have full line
            // than we write it to the file.
            for (int k = 0; k < n; k++)
            {
                slOffset = k + j * n;
                *(scanline.rgbt_ptr + slOffset) = triple;
            }
        }

        // skip over padding, if any
        fseek(inptr, old_padding, SEEK_CUR);

        // write scanline to out file at once and multiply lines by n times
        for (int k = 0; k < n; k++)
        {
            fwrite(scanline.rgbt_ptr, slRgbtSize, 1, outptr);
            fwrite(scanline.padding_ptr, padding, 1, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // free allocated memory
    free(scanline.rgbt_ptr);
    free(scanline.padding_ptr);

    // success
    return 0;
}
