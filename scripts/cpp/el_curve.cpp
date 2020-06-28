/*
 * el_curve.cpp
 *
 *  Created on: 27 июн. 2020 г.
 *      Author: sadko
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <png.h>

#define die(fmt, ...) do_die("%s:%d " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)

void do_die(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(-1);
}

typedef struct png_data_t
{
    size_t          width;      // Width of the image
    size_t          height;     // Height of the image
    size_t          stride;     // Actual size of row in bytes
    png_byte        color;      // Color type
    png_byte        depth;      // Bit depth
    size_t          passes;     // Number of passes
    png_byte      **rows;       // PNG image rows
    png_byte       *data;       // Overall data
} png_data_t;

void free_png(png_data_t *data)
{
    if (data->rows != NULL)
        free(data->rows);
    if (data->data != NULL)
        free(data->data);
    data->rows  = NULL;
    data->data  = NULL;
}

void alloc_png(png_data_t *data, size_t width, size_t height)
{
    // Initialize data
    data->width         = width;
    data->height        = height;
    data->stride        = ((width + 1) * 3) & (~0x03);
    data->color         = PNG_COLOR_TYPE_RGB;
    data->depth         = 8;
    data->passes        = 0;

    // Allocate data and initialize
    data->data          = static_cast<png_byte *>(malloc(data->stride * data->height));
    data->rows          = static_cast<png_byte **>(malloc(sizeof(png_byte *) * data->height));
    for (size_t i=0; i<data->height; ++i)
        data->rows[i]       = &data->data[i * data->stride];

//    ::bzero(data->data, data->stride * data->height);
    ::memset(data->data, 0xff, data->stride * data->height);
}

uint32_t get_pixel(const png_data_t *data, ssize_t x, ssize_t y)
{
    uint32_t c = 0;
    if ((x < 0) || (y < 0))
        return c;
    if ((size_t(x) >= data->width) || (size_t(y) >= data->height))
        return c;

    const png_byte *row = data->rows[y];
    if (data->color == PNG_COLOR_TYPE_RGB)
    {
        row    += x*3;
        c       = (uint32_t(row[0]) << 16) | (uint32_t(row[1]) << 8) | (uint32_t(row[2]));
    }
    else if (data->color == PNG_COLOR_TYPE_RGBA)
    {
        row    += x*4;
        c       = (uint32_t(row[0]) << 16) | (uint32_t(row[1]) << 8) | (uint32_t(row[2]));
    }

    return c;
}

void put_pixel(png_data_t *data, ssize_t x, ssize_t y, uint32_t c)
{
    if ((x < 0) || (y < 0))
        return;
    if ((size_t(x) >= data->width) || (size_t(y) >= data->height))
        return;

    png_byte *row = data->rows[y];
    if (data->color == PNG_COLOR_TYPE_RGB)
    {
        row    += x*3;
        row[0]  = uint8_t(c >> 16);
        row[1]  = uint8_t(c >> 8);
        row[2]  = uint8_t(c);
    }
    else if (data->color == PNG_COLOR_TYPE_RGBA)
    {
        row    += x*4;
        row[0]  = uint8_t(c >> 16);
        row[1]  = uint8_t(c >> 8);
        row[2]  = uint8_t(c);
    }
}

void read_png(const char *file_name, png_data_t *data)
{
    uint8_t header[8];    // 8 is the maximum size that can be checked

    // Open input file
    FILE *fp    = fopen(file_name, "rb");
    if (fp == NULL)
        die("Can not open %s", file_name);
    fread(header, 1, sizeof(header), fp);
    if (png_sig_cmp(header, 0, sizeof(header)))
        die("No PNG signature for %s", file_name);
    png_struct *pfd     = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (pfd == NULL)
        die("png_create_read_struct failed");
    png_info *pinfo     = png_create_info_struct(pfd);
    if (pinfo == NULL)
        die("png_create_info_struct failed");
    if (setjmp(png_jmpbuf(pfd)))
        die("setjmp failed");
    png_init_io(pfd, fp);
    png_set_sig_bytes(pfd, sizeof(header));
    png_read_info(pfd, pinfo);

    // Initialize data
    data->width         = png_get_image_width(pfd, pinfo);
    data->height        = png_get_image_height(pfd, pinfo);
    data->stride        = png_get_rowbytes(pfd, pinfo);
    data->color         = png_get_color_type(pfd, pinfo);
    data->depth         = png_get_bit_depth(pfd, pinfo);
    data->passes        = png_set_interlace_handling(pfd);

    png_read_update_info(pfd, pinfo);

    // Allocate data and initialize
    data->data          = static_cast<png_byte *>(malloc(data->stride * data->height));
    if (data == NULL)
        die("not enough memory");
    data->rows          = static_cast<png_byte **>(malloc(sizeof(png_byte *) * data->height));
    if (data->data == NULL)
        die("not enough memory");
    for (size_t i=0; i<data->height; ++i)
        data->rows[i]       = &data->data[i * data->stride];

    // Read data
    if (setjmp(png_jmpbuf(pfd)))
        die("png_read_image failed");

    png_read_image(pfd, data->rows);
    fclose(fp);
}

void write_png(const char* file_name, png_data_t *data)
{
    // Create output file
    FILE *fp    = fopen(file_name, "wb");
    if (fp == NULL)
        die("Error writing to %s", file_name);
    png_struct *pfd     = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (pfd == NULL)
        die("png_create_write_struct failed");
    png_info *pinfo     = png_create_info_struct(pfd);
    if (pinfo == NULL)
        die("png_create_info_struct failed");
    if (setjmp(png_jmpbuf(pfd)))
        die("setjmp failed");

    png_init_io(pfd, fp);

    // write header
    if (setjmp(png_jmpbuf(pfd)))
        die("Failed to emit header");

    png_set_IHDR(pfd, pinfo,
            data->width, data->height,
            data->depth, data->color,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
        );
    png_write_info(pfd, pinfo);

    // write data
    if (setjmp(png_jmpbuf(pfd)))
        die("Failed to write data");
    png_write_image(pfd, data->rows);

    if (setjmp(png_jmpbuf(pfd)))
        die("Failed to flush");
    png_write_end(pfd, NULL);
}

#define HMIN        16.0f
#define HMAX        16000.0f
#define VMIN        130.0f
#define VMAX        -10.0f

//static const float HMARK[]      = { 16.0f, 31.5f, 63.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f };
//static const float VMARK[]      = { 130, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0, -10 };
static const float PHONS[]      = { 100, 80, 60, 40, 20, 0 };
static const uint32_t COLORS[]  = { 0xffa0a0, 0xff8080, 0xff6060, 0xff4040, 0xff2020, 0xff0000 };

//#define NHMARK      (sizeof(HMARK)/sizeof(float))
//#define NVMARK      (sizeof(VMARK)/sizeof(float))
#define NPHONS      (sizeof(PHONS)/sizeof(float))

void interpolate(float *v, size_t a, size_t b, float va, float vb)
{
    float delta = (vb - va) / (b - a);
    for (size_t i=a; i<=b; ++i)
        v[i] = va + (i-a) * delta;
}

void read_x_axis(const png_data_t *data, size_t *xmin, size_t *xmax)
{
    ssize_t x_min = -1;
    ssize_t x_max = -1;

    ssize_t y  = data->height - 1;
    for (size_t x=0; x<data->width; ++x)
    {
        uint32_t rgb    = get_pixel(data, x, y);

        if (rgb == 0x00ff00)
        {
            if (x_min < 0)
                x_min   = x;
            x_max   = x;
        }
    }
    *xmin   = x_min;
    *xmax   = x_max;

    printf("scan parameters: xmin=%d, xmax=%d\n", int(x_min), int(x_max));
}

void read_y_axis(const png_data_t *data, size_t *ymin, size_t *ymax)
{
    ssize_t y_min = -1;
    ssize_t y_max = -1;

    ssize_t x   = data->width - 1;
    for (size_t y=0; y<data->height; ++y)
    {
        uint32_t rgb    = get_pixel(data, x, y);
        if (rgb == 0x0000ff)
        {
            if (y_min < 0)
                y_min   = y;
            y_max   = y;
        }
    }

    // Perform interpolation
    *ymin   = y_min;
    *ymax   = y_max;

    printf("scan parameters: ymin=%d, ymax=%d\n", int(y_min), int(y_max));
}

float read_value(const png_data_t *data, uint32_t color, ssize_t x, ssize_t y)
{
    uint32_t c;
    float v = 0;
    float w = 0;
    float s = 0;
    ssize_t xv = 0;
    float k = 1.0f / (0xff - (color & 0xff));

    for (ssize_t yy=y; yy >= 0; --yy)
    {
        c = get_pixel(data, x, yy);
        if ((c == 0x000000) || (c == 0xffffff))
            break;

        xv = ((c & 0xff) - (color & 0xff));
        s  = (1.0f - xv*k);
        v += s * yy;
        w += s;
    }

    for (ssize_t yy=y+1; yy < ssize_t(data->height); ++yy)
    {
        c = get_pixel(data, x, yy);
        if ((c == 0x000000) || (c == 0xffffff))
            break;

        xv = ((c & 0xff) - (color & 0xff));
        s  = (1.0f - xv*k);
        v += s * yy;
        w += s;
    }

    return v / w;
}

void draw_values(png_data_t *data, const float *values, uint32_t color, ssize_t ymin, ssize_t ymax)
{
    for (size_t x=0; x<data->width; ++x)
    {
        float y = values[x];
        if (y < 0.0f)
            continue;

        put_pixel(data, x, y, color);
        printf("x = %d, y = %d\n", int(x), int(y));
    }
}

void read_values(png_data_t *data, uint32_t color, float *out)
{
    for (size_t x=0; x<data->width; ++x)
    {
        out[x] = -1.0f;

        for (size_t y=0; y<data->height; ++y)
        {
            uint32_t v = get_pixel(data, x, y);
            if (v == color)
            {
                out[x] = read_value(data, color, x, y);
                printf("x = %d, v = %f\n", int(x), out[x]);
                y = data->height; // Terminate the cycle
            }
        }
    }
};

void generate_relative_curves(png_data_t *data, const float * const *curves, size_t ncurves)
{
    size_t xmin = 0, xmax = data->width - 1;

    for (size_t i=0; i<ncurves; ++i)
    {
        const float *c = curves[i];

        // Find leftmost X
        for (size_t x=0; x < data->width; ++x)
        {
            if (c[x] >= 0.0f)
            {
                xmin = (xmin < x) ? x : xmin;
                break;
            }
        }

        // Find rightmost X
        for (size_t x=data->width - 1; x > 0; --x)
        {
            if (c[x] >= 0.0f)
            {
                xmax = (xmax > x) ? x : xmax;
                break;
            }
        }
    }

    printf("xmin = %d, xmax = %d\n", int(xmin), int(xmax));

    const float *flat = curves[1]; // We assume 80 phon curve to be flat
    for (size_t i=0; i<ncurves; ++i)
    {
        const float *c = curves[i];

        for (size_t x=xmin; x<=xmax; ++x)
        {
            float y = c[x] - flat[x] + 400.0f;
            put_pixel(data, x, y, COLORS[i]);
        }
    }
}

void process_image(png_data_t *data, png_data_t *relative)
{
    size_t xmin, xmax, ymin, ymax;
    read_x_axis(data, &xmin, &xmax);
    read_y_axis(data, &ymin, &ymax);

    // Allocate phon tables
    float *phons[NPHONS];
    for (size_t i=0; i<NPHONS; ++i)
    {
        phons[i]        = static_cast<float *>(malloc(sizeof(float) * data->width));
        read_values(data, COLORS[i], phons[i]);
        draw_values(data, phons[i], 0x000000, ymin, ymax);
    }

    generate_relative_curves(relative, phons, NPHONS);

    // Free phon tables
    for (size_t i=0; i<NPHONS; ++i)
        free(phons[i]);
}

int main(int argc, char **argv)
{
    if (argc < 5)
        die("Arguments: <file_in.png> <table-out.cpp> <check.png> <relative.png>");

    png_data_t image, rel;

    read_png(argv[1], &image);
    alloc_png(&rel, image.width, image.height);

    process_image(&image, &rel);

    write_png(argv[3], &image);
    write_png(argv[4], &rel);

    free_png(&image);
    free_png(&rel);

    return 0;
}


