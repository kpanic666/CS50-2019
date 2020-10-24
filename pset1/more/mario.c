// Draw pyramid from game Mario
#include <cs50.h>
#include <stdio.h>

#define PYRAMIDS_NUM 2
#define VOID_WIDTH_BW_PYR 2

int main(void)
{
    int height;

    do
    {
        height = get_int("Give me height: ");
    }
    while (height < 1 || height > 8);

    int max_chars_in_str = height * PYRAMIDS_NUM + VOID_WIDTH_BW_PYR;
    int num_of_hashes;
    int r_space_pos = max_chars_in_str / 2;
    int l_space_pos = r_space_pos - 1;
    // + 2 is for \n, \0
    char o_str[max_chars_in_str + 2];
    char cur_char;

    for (int i = 1; i <= height; i++)
    {
        // Take the middle of pyramid and fill it with 2 spaces.
        o_str[r_space_pos] = o_str[l_space_pos] = ' ';

        // BTW number of current itteration counter is the number
        // of # on one side of the pyramid. That's why we save it
        num_of_hashes = i;

        // First we prepare char array for output.
        // Reminder: Height can also be used to count number of char-places
        // to the left and to the right from central spaces.
        for (int j = 1; j <= height; j++)
        {
            if (num_of_hashes-- > 0)
            {
                cur_char = '#';
            }
            else
            {
                cur_char = ' ';
            }
            o_str[l_space_pos - j] = cur_char;
            o_str[r_space_pos + j] = cur_char;
        }

        // Add special symbols to the end of each string
        o_str[r_space_pos + i + 1] = '\n';
        o_str[r_space_pos + i + 2] = '\0';
        printf("%s", o_str);
    }
}