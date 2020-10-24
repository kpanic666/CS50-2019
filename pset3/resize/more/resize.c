// Resize bmp file by n times.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "bmp.h"

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
        printf("Usage: ./resize f infile outfile\n");
        return 1;
    }

    // ensure that second argument (multiplicator f) will be an float
    // and besides lay between (0.0 and 100.0]
    float f = 0.0;
    int scan_res = sscanf(argv[1], "%f", &f);

    if (scan_res == EOF || f <= 0.0 || f > 100.0)
    {
        printf("Usage: ./resize f infile outfile\n");
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
    bi.biWidth *= f;
    bi.biHeight *= f;
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
    LONG widthDif = abs(old_bi.biWidth - bi.biWidth);
    LONG heightDif = abs(abs(old_bi.biHeight) - abs(bi.biHeight));
    float widthStep = 1.0;
    float heightStep = 1.0;

    // Substract 2 from start width and height because i want
    // to save first and last column/fow from original image
    if (widthDif != 0 && heightDif != 0)
    {
        if (f > 1.0)
        {
            widthStep = (float) bi.biWidth / old_bi.biWidth;
            heightStep = (float) bi.biHeight / old_bi.biHeight;
        }
        else
        {
            widthStep = (float)(old_bi.biWidth - 2) / widthDif;
            heightStep = (float)(abs(old_bi.biHeight) - 2) / heightDif;
        }
    }

    LONG slRgbtSize = bi.biWidth * sizeof(RGBTRIPLE);
    SCANLINE scanline;
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
    LONG newHeightSum = 0;
    float heightStepSum = heightStep;

    for (int i = 0, biHeight = abs(old_bi.biHeight);
         i < biHeight && newHeightSum < abs(bi.biHeight); i++)
    {
        // iterate over pixels in scanline
        LONG newWidthSum = 0;
        float widthStepSum = widthStep;

        for (int j = 0; j < old_bi.biWidth && newWidthSum < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            if (f < 1.0)
            {
                if (j != floor(widthStepSum) || j == 0 ||
                    (j == old_bi.biWidth - 1 && newWidthSum != bi.biWidth))
                {
                    // First pixel in row we will always save to new image
                    *(scanline.rgbt_ptr + newWidthSum) = triple;
                    newWidthSum++;
                }
                else
                {
                    widthStepSum += widthStep;
                }
            }
            else if (f > 1.0)
            {
                int k;
                for (k = 0; k < floor(widthStepSum) && newWidthSum < bi.biWidth; k++, newWidthSum++)
                {
                    *(scanline.rgbt_ptr + newWidthSum) = triple;
                }

                widthStepSum = widthStepSum - k + widthStep;
                if (j == old_bi.biWidth - 1 && newWidthSum != bi.biWidth)
                {
                    *(scanline.rgbt_ptr + newWidthSum) = triple;
                    newWidthSum++;
                }
            }
            else
            {
                *(scanline.rgbt_ptr + j) = triple;
            }
        }

        // skip over padding, if any
        fseek(inptr, old_padding, SEEK_CUR);

        // write scanline to out file at once and multiply lines by n times
        if (f < 1.0)
        {
            if (i != floor(heightStepSum) || i == 0 ||
                (i == biHeight - 1 && newHeightSum != abs(bi.biHeight)))
            {
                // First row always save to new image
                fwrite(scanline.rgbt_ptr, slRgbtSize, 1, outptr);
                fwrite(scanline.padding_ptr, padding, 1, outptr);
                newHeightSum++;
            }
            else
            {
                heightStepSum += heightStep;
            }
        }
        else if (f > 1.0)
        {
            int k;
            for (k = 0; k < floor(heightStepSum) && newHeightSum < abs(bi.biHeight); k++, newHeightSum++)
            {
                fwrite(scanline.rgbt_ptr, slRgbtSize, 1, outptr);
                fwrite(scanline.padding_ptr, padding, 1, outptr);
            }

            heightStepSum = heightStepSum - k + heightStep;
            if (i == biHeight - 1 && newHeightSum != abs(bi.biHeight))
            {
                fwrite(scanline.rgbt_ptr, slRgbtSize, 1, outptr);
                fwrite(scanline.padding_ptr, padding, 1, outptr);
                newHeightSum++;
            }
        }
        else
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