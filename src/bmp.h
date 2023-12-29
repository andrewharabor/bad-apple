/* .bmp-related data types based on Microsoft's own.
Adapted from https://cs50.harvard.edu/x/2023/psets/4/filter/more/. */

#include <stdint.h>

/* Adapted from http://msdn.microsoft.com/en-us/library/cc230309.aspx. */
typedef uint8_t  byte;
typedef uint32_t dword;
typedef int32_t  long32;
typedef uint16_t word;

/* Contains information about the type, size, and layout of a file that contains a device-independent bitmap.
Adapted from http://msdn.microsoft.com/en-us/library/dd183374(VS.85).aspx. */
typedef struct
{
    word   file_type;
    dword  file_size;
    word   file_reserved1;
    word   file_reserved2;
    dword  file_off_bits;
} __attribute__((__packed__)) bitmap_file_header;

/* Contains information about the dimensions and color format of a device-independent bitmap.
Adapted from http://msdn.microsoft.com/en-us/library/dd183376(VS.85).aspx. */
typedef struct
{
    dword info_size;
    long32 info_width;
    long32 info_height;
    word info_planes;
    word info_bit_count;
    dword info_compression;
    dword info_size_image;
    long32 info_x_pels_per_meter;
    long32 info_y_pels_per_meter;
    dword info_clr_used;
    dword info_clr_important;
} __attribute__((__packed__)) bitmap_info_header;

/* Describes a color consisting of relative intensities of red, green, and blue.
Adapted from http://msdn.microsoft.com/en-us/library/aa922590.aspx. */
typedef struct
{
    byte rgb_red;
    byte rgb_green;
    byte rgb_blue;

} __attribute__((__packed__)) rgb_triple;
