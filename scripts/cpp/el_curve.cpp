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

static const float HMARK[] = { 16.0f, 31.5f, 63.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f };
static const float VMARK[] = { 130, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0, -10 };

#define NHMARK      (sizeof(HMARK)/sizeof(float))
#define NVMARK      (sizeof(VMARK)/sizeof(float))

void interpolate(float *v, size_t a, size_t b, float va, float vb)
{
    float delta = (vb - va) / (b - a);
    for (size_t i=a+1; i<b; ++i)
        v[i] = va + (i-a) * delta;
}

float *read_x_axis(const png_data_t *data, size_t *xmin, size_t *xmax)
{
    ssize_t hmarks[NHMARK];

    float *x_axis = static_cast<float *>(malloc(sizeof(float) * data->width));
    for (size_t i=0; i<data->width; ++i)
        x_axis[i]   = -1e+10f;

    ssize_t y  = data->height - 1;
    size_t idx = 0;
    for (size_t x=0; x<data->width; ++x)
    {
        uint32_t rgb    = get_pixel(data, x, y);

        if (rgb == 0x00ff00)
        {
            if (idx >= NHMARK)
                die("Too many markers, must be %d");

            printf("horizontal marker: x=%d, value=%f\n", int(x), HMARK[idx]);
            hmarks[idx++]   = x;
        }
    }

    if (idx != NHMARK)
        die("Too few markers, must be %d", int(NHMARK));

    // Perform interpolation
    for (size_t i=1; i<NHMARK; ++i)
        interpolate(x_axis, hmarks[i-1], hmarks[i], HMARK[i-1], HMARK[i]);
    *xmin   = hmarks[0];
    *xmax   = hmarks[NHMARK-1];

    printf("scan parameters: xmin=%d, xmax=%d\n", int(*xmin), int(*xmax));

    return x_axis;
}

float *read_y_axis(png_data_t *data, size_t *ymin, size_t *ymax)
{
    ssize_t vmarks[NVMARK];

    float *y_axis = static_cast<float *>(malloc(sizeof(float) * data->height));
    for (size_t i=0; i<data->width; ++i)
        y_axis[i]   = -1e+10f;

    ssize_t x  = data->width - 1;
    size_t idx = 0;
    for (size_t y=0; y<data->height; ++y)
    {
        uint32_t rgb    = get_pixel(data, x, y);

        if (rgb == 0x0000ff)
        {
            if (idx >= NVMARK)
                die("Too many markers, must be %d");

            printf("vertical marker: y=%d, value=%f\n", int(y), VMARK[idx]);
            vmarks[idx++]   = y;
        }
    }

    if (idx != NVMARK)
        die("Too few markers, must be %d", int(NVMARK));

    // Perform interpolation
    for (size_t i=1; i<NVMARK; ++i)
        interpolate(y_axis, vmarks[i-1], vmarks[i], VMARK[i-1], VMARK[i]);
    *ymin   = vmarks[0];
    *ymax   = vmarks[NVMARK-1];

    printf("scan parameters: ymin=%d, ymax=%d\n", int(*ymin), int(*ymax));

    return y_axis;
}

void process_image(png_data_t *data)
{
    size_t xmin, xmax, ymin, ymax;
    float *x_axis = read_x_axis(data, &xmin, &xmax);
    float *y_axis = read_y_axis(data, &ymin, &ymax);

    ::free(x_axis);
    ::free(y_axis);
}

int main(int argc, char **argv)
{
    if (argc != 4)
        die("Arguments: <file_in.png> <table-out.cpp> <file_out.png>");

    png_data_t image;

    read_png(argv[1], &image);
    process_image(&image);
    write_png(argv[3], &image);
    free_png(&image);

    return 0;
}


