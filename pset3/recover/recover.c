// Recover jpegs from raw file.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define BLOCK_SIZE
typedef uint8_t  BYTE;

int main(int argc, char *argv[])
{
    // ensure proper usage. Only 1 arg permitted.
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover rawfile");
        return 1;
    }

    // open input file
    char *infile = argv[1];
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // Read input file by blocks 512 bytes lenght each
    // until we find all jpegs.
    BYTE *rd_block = malloc(512);
    if (rd_block == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Cann't allocate memory for reading.\n");
        return 5;
    }

    int numReadedBytes = 0;
    int fileNum = 0;
    FILE *jpgptr = NULL;
    do
    {
        numReadedBytes = fread(rd_block, 1, 512, inptr);

        if (numReadedBytes == 512)
        {
            // If a block - is the header of jpeg file, than create
            // new file and write to it until we find the next jpeg
            // or EOF.
            if (*rd_block == 0xFF &&
                *(rd_block + 1) == 0xD8 &&
                *(rd_block + 2) == 0xFF &&
                (*(rd_block + 3) & 0xF0) == 0xE0)
            {
                // Create new jpeg file and work with it.
                char filename[8];
                sprintf(filename, "%03i.jpg", fileNum++);
                if (jpgptr != NULL)
                {
                    fclose(jpgptr);
                }
                jpgptr = fopen(filename, "w");
                if (jpgptr == NULL)
                {
                    fclose(inptr);
                    fprintf(stderr, "Could not open %s.\n", filename);
                    return 4;
                }
                fwrite(rd_block, 512, 1, jpgptr);
            }
            else
            {
                // Garbage blocks or body of jpeg image. If garbage - skip it,
                // if body of jpeg - write to file.
                if (jpgptr != NULL)
                {
                    fwrite(rd_block, 512, 1, jpgptr);
                }
            }
        }
        else
        {
            // EOF, stop reading input file and rec new jpegs
            break;
        }
    }
    while (1);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(jpgptr);

    // free allocated memory
    free(rd_block);

    // success
    return 0;
}