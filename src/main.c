/*
bad-apple, a 20 FPS ASCII animation of the famous Bad Apple video (https://www.youtube.com/watch?v=FtutLA63Cp8)
Copyright (C) 2023  Andrew Harabor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bmp.h"

// File constants
#define FILE_NAME_FORMAT "src/frames/frame%04d.bmp"
#define FILE_NAME_LEN 24

#define FILE_TYPE 0x4d42
#define FILE_OFF_BITS 54
#define INFO_SIZE 40
#define INFO_BIT_COUNT 24
#define INFO_COMPRESSION 0

// Display constants
#define HEIGHT 360
#define WIDTH 480
#define SCALE_FACTOR 10
#define DISPLAY_HEIGHT (HEIGHT / SCALE_FACTOR)
#define DISPLAY_WIDTH (WIDTH / SCALE_FACTOR)

#define FRAME_COUNT 4383
#define FPS 20

#define SHADES 83

char g_grayscale[SHADES] = "@&%QWNM0gB$#DR8mHXKAUbGOpV4d9h6PkqwSE2ayjxY5Zoenult13IfCiF7JvTLs?z/*cr!<>;=^,_:'.` ";

void read_bitmap(char *file_name, rgb_triple bitmap[HEIGHT][WIDTH], char *run_command);
void print_frame(int height, int width, rgb_triple bitmap[height][width], char *run_command);
int sleep_msecs(unsigned long msecs);

/* Display a 20 FPS ASCII animation of the famous Bad Apple video (https://www.youtube.com/watch?v=FtutLA63Cp8) to `stdout`.*/
int main(int argc, char *argv[])
{
    int frame_num;
    char bmp_file_name[FILE_NAME_LEN + 1];
    double time_diff_secs;
    time_t start_time, end_time;
    rgb_triple bitmap[HEIGHT][WIDTH];

    for (frame_num = 1; frame_num <= FRAME_COUNT; frame_num++)
    {
        // Begin timing the processing of the frame
        time(&start_time);

        // Read the bitmap from the frame and print it
        sprintf(bmp_file_name, FILE_NAME_FORMAT, frame_num);
        read_bitmap(bmp_file_name, bitmap, argv[0]);
        print_frame(HEIGHT, WIDTH, bitmap, argv[0]);

        // Ensure `FPS` frames per second is maintained
        time(&end_time);
        if ((time_diff_secs = (1 / (float) FPS) - difftime(end_time, start_time)) > 0)
        {
            sleep_msecs(time_diff_secs * 1000);
        }
    }

    exit(0);
    return 0;
}

/* Open the BMP file `file_name` and fill `bitmap` with RGB data.
Most of the file processing adapted from https://cs50.harvard.edu/x/2023/psets/4/filter/more/. */
void read_bitmap(char *file_name, rgb_triple bitmap[HEIGHT][WIDTH], char *run_command)
{
    int i;
    FILE *file;
    bitmap_file_header file_header;
    bitmap_info_header info_header;

    // Open the image file containing the frame
    if ((file = fopen(file_name, "r")) == NULL)
    {
        fprintf(stderr, "%s: FILE ERROR - could not open %s\n", run_command, file_name);

        exit(1);
        return;
    }

    // Read and store the file's metadata
    if (fread(&file_header, sizeof(bitmap_file_header), 1, file) != 1 || fread(&info_header, sizeof(bitmap_info_header), 1, file) != 1)
    {
        fprintf(stderr, "%s: FILE ERROR - could not read from %s\n", run_command, file_name);

        fclose(file);

        exit(1);
        return;
    }

    // Ensure the image format is 24-bit uncompressed BMP 4.0
    if (file_header.file_type != FILE_TYPE || file_header.file_off_bits != FILE_OFF_BITS || info_header.info_size != INFO_SIZE || info_header.info_bit_count != INFO_BIT_COUNT || info_header.info_compression != INFO_COMPRESSION)
    {
        fprintf(stderr, "%s: FILE ERROR - %s is in an unsupported format\n", run_command, file_name);

        fclose(file);

        exit(1);
        return;
    }

    // Ensure the correct dimensions (which are fixed so that `bitmap` is only allocated once before iteration)
    if (abs(info_header.info_height) != HEIGHT || abs(info_header.info_width) != WIDTH)
    {
        fprintf(stderr, "%s: FILE ERROR - %s has incorrect dimensions\n", run_command, file_name);

        fclose(file);

        exit(1);
        return;
    }

    memset(bitmap, 0, HEIGHT * WIDTH * sizeof(rgb_triple));

    // Iterate over scanlines
    for (i = 0; i < HEIGHT; i++)
    {
        if (fread(bitmap[i], sizeof(rgb_triple), WIDTH, file) != WIDTH || fseek(file, (4 - (WIDTH * sizeof(rgb_triple)) % 4) % 4, SEEK_CUR) != 0) // read row into `bitmap` and skip padding
        {
            fprintf(stderr, "%s: FILE ERROR - could not read from %s\n", run_command, file_name);

            fclose(file);

            exit(1);
            return;
        }
    }

    // Cleanup after reading from `file`
    if (ferror(file))
    {
        fprintf(stderr, "%s: FILE ERROR - an error occured while reading from %s\n", run_command, file_name);

        fclose(file);

        exit(1);
        return;
    }
    fclose(file);

    return;
}

/* Print the frame given by `bitmap` in ASCII characters. */
void print_frame(int height, int width, rgb_triple bitmap[height][width], char *run_command)
{
    int i, j, k, l, buff_idx;
    float sum, avg;
    char buffer[(DISPLAY_HEIGHT + 2) * (2 * DISPLAY_WIDTH + 3) + 1], c;
    rgb_triple pixel;

    buff_idx = 0;

    // Add the top border encompassing the frame
    buffer[buff_idx++] = '+';
    for (i = 0; i < DISPLAY_WIDTH * 2; i++)
    {
        buffer[buff_idx++] = '-';
    }
    buffer[buff_idx++] = '+';
    buffer[buff_idx++] = '\n';

    // Average the grayscale values across blocks of pixels
    for (i = DISPLAY_HEIGHT - 1; i >= 0; i--) // the bitmap is upside-down in the BMP format
    {
        buffer[buff_idx++] = '|';

        for (j = 0; j < DISPLAY_WIDTH; j++)
        {
            sum = 0;
            for (k = 0; k < SCALE_FACTOR; k++)
            {
                for (l = 0; l < SCALE_FACTOR; l++)
                {
                    pixel = bitmap[i * SCALE_FACTOR + k][j * SCALE_FACTOR + l];
                    sum += (pixel.rgb_red + pixel.rgb_green + pixel.rgb_blue) / 3.0;
                }
            }

            avg = sum / (float) (SCALE_FACTOR * SCALE_FACTOR);
            c = g_grayscale[(int) round(avg * (SHADES - 1) / 255.0)];
            buffer[buff_idx++] = c;
            buffer[buff_idx++] = c; // print each character twice to compensate for their rectangular shape
        }

        buffer[buff_idx++] = '|';
        buffer[buff_idx++] = '\n';
    }

    // Add the bottom border encompassing the frame
    buffer[buff_idx++] = '+';
    for (i = 0; i < DISPLAY_WIDTH * 2; i++)
    {
        buffer[buff_idx++] = '-';
    }
    buffer[buff_idx++] = '+';
    buffer[buff_idx++] = '\n';

    buffer[buff_idx] = '\0';

    // Clear the screen before printing the frame
    // Note that this is an ANSI escape code so this implementation is not necessarily portable
    // Printing enough newlines could also work but arguably looks worse
    fprintf(stdout, "\033c");

    // Print character buffer containing the frame plus the surrounding border
    // Everything is printed with one call to `printf()` to try and keep the animation as smooth as possible (still system dependent)
    fprintf(stdout, "%s", buffer);

    // Handle potential errors after printing the frame to `stdout`
    if (ferror(stdout))
    {
        fprintf(stderr, "%s: I/O ERROR - an error occured while writing to stdout\n", run_command);

        exit(2);
        return;
    }

    return;
}

/* Delay program execution for `msecs` milliseconds. */
int sleep_msecs(unsigned long msecs)
{
    struct timespec time_value;
    int result;

    time_value.tv_sec = msecs / 1000;
    time_value.tv_nsec = (msecs % 1000) * 1000000;

    // Ensure the full duration of the time is slept, despite interruptions
    do
    {
        result = nanosleep(&time_value, &time_value);
    }
    while (result && errno == EINTR);

    return result;
}
