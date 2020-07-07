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
#include <math.h>
#include <png.h>

#define FMIN        16.0f
#define FMAX        16000.0f
#define SPL_MIN     -10.0f
#define SPL_MAX     130.0f

//static const float HMARK[]      = { 16.0f, 31.5f, 63.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f };
//static const float VMARK[]      = { 130, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0, -10 };
static const float PHONS[]      = { 100, 80, 60, 40, 20, 0 };
static const uint32_t COLORS[]  = { 0xffa0a0, 0xff8080, 0xff6060, 0xff4040, 0xff2020, 0xff0000 };
static const uint32_t INT_COLORS[] =
{
    0xff9090,
    0xff8080,
    0xff7070,
    0xff6060,
    0xff5050,
    0xff4040,
    0xff3030,
    0xff2020,
    0xff1010,
    0xff0000,
    0x000000
};

//#define NHMARK      (sizeof(HMARK)/sizeof(float))
//#define NVMARK      (sizeof(VMARK)/sizeof(float))
#define NPHONS      (sizeof(PHONS)/sizeof(float))

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

typedef struct el_curves_t
{
    size_t          rx_min;     // Minimum range X value
    size_t          rx_max;     // Maximum range X value
    size_t          ry_min;     // Minimum range Y value
    size_t          ry_max;     // Maximum range Y value
    float           fmin;       // Minimum frequency
    float           fmax;       // Maximum frequency
    float           spl_min;    // Minimum SPL
    float           spl_max;    // Maximum SPL

    ssize_t         xmin;       // Minimum X value
    ssize_t         xmax;       // Maximum X value
    size_t          ncurves;    // Number of curves
    size_t          width;      // Number of dots per curve
    float         **curves;     // SPL curves for the same phon value
} el_curves_t;

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

void alloc_curves(el_curves_t *c, size_t width, size_t count)
{
    c->rx_min       = 0;
    c->rx_max       = 0;
    c->ry_min       = 0;
    c->ry_max       = 0;
    c->fmin         = FMIN;
    c->fmax         = FMAX;
    c->spl_min      = SPL_MIN;
    c->spl_max      = SPL_MAX;
    c->ncurves      = count;
    c->width        = width;
    c->xmin         = -1;
    c->xmax         = -1;

    c->curves       = static_cast<float **>(malloc(sizeof(float *) * count));
    for (size_t i=0; i<count; ++i)
        c->curves[i]    = static_cast<float *>(malloc(sizeof(float) * c->width));
}

void free_curves(el_curves_t *c)
{
    for (size_t i=0; i<c->ncurves; ++i)
        free(c->curves[i]);
    free(c->curves);
    c->curves       = NULL;
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

void draw_values(png_data_t *data, const float *values, uint32_t color, size_t xmin, size_t xmax)
{
    for (size_t x=xmin; x<=xmax; ++x)
    {
        float y = values[x];
        if (y < 0.0f)
            continue;

        put_pixel(data, x, y, color);
        printf("x = %d, y = %d\n", int(x), int(y));
    }
}

void read_values(png_data_t *data, uint32_t color, float *out, ssize_t *xmin, ssize_t *xmax)
{
    *xmin = -1;
    *xmax = -1;

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

                if (*xmin < 0)
                    *xmin = x;
                *xmax = x;

                y = data->height; // Terminate the cycle
            }
        }
    }
};

void draw_relative_curves(png_data_t *data, el_curves_t *c, size_t iflat, const uint32_t *colors)
{
    printf("xmin = %d, xmax = %d\n", int(c->xmin), int(c->xmax));

    const float *flat = c->curves[iflat]; // We assume 80 phon curve to be flat
    for (size_t i=0; i<c->ncurves; ++i)
    {
        const float *xc = c->curves[i];
        uint32_t cc = (colors != NULL) ? colors[i] : 0;

        for (ssize_t x=c->xmin; x<=c->xmax; ++x)
        {
            float y = xc[x] - flat[x] + 400.0f;
            put_pixel(data, x, y, cc);
        }
    }
}

void parse_image(png_data_t *data, el_curves_t *curves)
{
    alloc_curves(curves, data->width, NPHONS);

    read_x_axis(data, &curves->rx_min, &curves->rx_max);
    read_y_axis(data, &curves->ry_min, &curves->ry_max);

    // Allocate phon tables
    for (size_t i=0; i<curves->ncurves; ++i)
    {
        ssize_t x_min, x_max;
        float *curve    = curves->curves[i];

        // Read curve values
        read_values(data, COLORS[i], curve, &x_min, &x_max);

        // Find common boundaries for all curves
        if ((curves->xmin < 0) || (curves->xmin < x_min))
            curves->xmin = x_min;
        if ((curves->xmax < 0) || (curves->xmax > x_max))
            curves->xmax = x_max;

        // Put parsed dots to the drawing
        draw_values(data, curve, 0x000000, curves->rx_min, curves->rx_max);
    }
}

void cubic_interpolate(float *y, const float *ya, const float *yb, float x, float xa, float xb, size_t count)
{
    float t;
//    float xmin, xmax;

    // Swap A and B if ax > bx
    if (xa > xb)
    {
        const float *tmp = ya;
        ya          = yb;
        yb          = tmp;

        t           = xa;
        xa          = xb;
        xb          = t;
    }

//    // Compute dy
//    float eq[4];
//
//    if (x < xa)
//    {
//        xmin        = x;
//        xmax        = xb;
//
//        double ka   = (xa - x) / (xb - x);
//        double v    = ka*ka*(3.0 - 2.0*ka) - 1.0;
//
//        eq[0]       = 2.0 / v;
//        eq[1]       = -1.5 * eq[0];
//        eq[2]       = 0.0f;
//        eq[3]       = 1.0 - (eq[0] + eq[1]);
//    }
//    else if (x > xb)
//    {
//        xmin        = xa;
//        xmax        = x;
//
//        double kb   = (xb - xa) / (x - xa);
//        double v    = kb*kb*(2.0*kb - 3.0);
//
//        eq[0]       = 2.0 / v;
//        eq[1]       = -1.5 * eq[0];
//        eq[2]       = 0.0f;
//        eq[3]       = 0.0f;
//    }
//    else
//    {
//        xmin        = xa;
//        xmax        = xb;
//
//        eq[0]       = -2.0f;
//        eq[1]       = 3.0f;
//        eq[2]       = 0.0f;
//        eq[3]       = 0.0f;
//    }
//
//    float xn    = (x - xmin) / (xmax - xmin);   // Normalized value
//    float k2    = eq[3] + xn*(eq[2] + xn*(eq[1] + xn*eq[0]));
//    float k1    = 1.0f - k2;

    float k2 = (x - xa) / (xb - xa);
    float k1 = 1 - k2;

//    printf("y(x) = %f + x*(%f + x*(%f + %f*x))\n", eq[3], eq[2], eq[1], eq[0]);
    printf("k1 = %f, k2 = %f\n", k1, k2);

    // Perform cubic interpolation
    for (size_t i=0; i<count; ++i)
    {
        y[i] = ya[i] * k1 + yb[i] * k2;
    }
}

void build_interpolated_curves(el_curves_t *d, const el_curves_t *s)
{
    // We have 100 (invalid), 80, 60, 40, 20 and 0 phons curves
    // We need: 90, 80, 70, 60, 50, 40, 30, 20, 10, 0 and 83 phons curves
    alloc_curves(d, s->width, 11);
    d->rx_min       = s->rx_min;
    d->rx_max       = s->rx_max;
    d->ry_min       = s->ry_min;
    d->ry_max       = s->ry_max;
    d->fmin         = s->fmin;
    d->fmax         = s->fmax;
    d->spl_min      = s->spl_min;
    d->spl_max      = s->spl_max;
    d->xmin         = s->xmin;
    d->xmax         = s->xmax;

    // Generate 90 to 0 phon curves
    size_t i=0;
    cubic_interpolate(d->curves[i++], s->curves[1], s->curves[2], 90, 80, 60, s->width); // 90
    cubic_interpolate(d->curves[i++], s->curves[1], s->curves[2], 80, 80, 60, s->width); // 80
    cubic_interpolate(d->curves[i++], s->curves[1], s->curves[2], 70, 80, 60, s->width); // 70
    cubic_interpolate(d->curves[i++], s->curves[2], s->curves[3], 60, 60, 40, s->width); // 60
    cubic_interpolate(d->curves[i++], s->curves[2], s->curves[3], 50, 60, 40, s->width); // 50
    cubic_interpolate(d->curves[i++], s->curves[3], s->curves[4], 40, 40, 20, s->width); // 40
    cubic_interpolate(d->curves[i++], s->curves[3], s->curves[4], 30, 40, 20, s->width); // 30
    cubic_interpolate(d->curves[i++], s->curves[4], s->curves[5], 20, 20,  0, s->width); // 20
    cubic_interpolate(d->curves[i++], s->curves[4], s->curves[5], 10, 20,  0, s->width); // 10
    cubic_interpolate(d->curves[i++], s->curves[4], s->curves[5],  0, 20,  0, s->width); // 0

    cubic_interpolate(d->curves[i++], s->curves[1], s->curves[2], 83, 80, 60, s->width); // 83
}

double logfx(float x, const el_curves_t *s)
{
    float step = (x - s->rx_min) / (s->rx_max - s->rx_min);

    return s->fmin * exp(log(s->fmax/s->fmin) * step);
}

double linfx(float y, const el_curves_t *s)
{
    float step = (y - s->ry_min) / (s->ry_max - s->ry_min);
    return s->spl_max + (s->spl_min - s->spl_max) * step;
}

void write_curves(const char *out, const char *prefix, const el_curves_t *s, size_t count)
{
    FILE *fd = fopen(out, "w");

    double fmin  = logfx(s->xmin, s);
    double fmax  = logfx(s->xmax, s);

    const float *flat = s->curves[count];

    // Output curves
    for (size_t i=0; i<count; ++i)
    {
        size_t phons = i*10;

        fprintf(fd, "static const float %s_curve_%d_phons[%d] =\n", prefix, int(phons), int(s->xmax - s->xmin + 1));
        fprintf(fd, "{\n    ");

        const float *curve = s->curves[count - i -1];

        for (ssize_t x=s->xmin; x<=s->xmax; ++x)
        {
            float fy = linfx(flat[x], s);
            float y  = linfx(curve[x], s);

            fprintf(fd, "%.2f", y - fy);
            if (x < s->xmax)
                fprintf(fd, ",");

            if ((x % 10) == 9)
                fprintf(fd, "\n    ");
            else
                fprintf(fd, " ");
        }

        fprintf(fd, "\n};\n\n");
    }

    fprintf(fd, "static const freq_curve_t %s_curve =\n", prefix);
    fprintf(fd, "{\n");
    fprintf(fd, "    %.2f, // fmin\n", fmin);
    fprintf(fd, "    %.2f, // fmax\n", fmax);
    fprintf(fd, "    %.2f, // amin\n", 0.0f);
    fprintf(fd, "    %.2f, // amax\n", 90.0f);
    fprintf(fd, "    %d, // hdots\n",  int(s->xmax - s->xmin + 1));
    fprintf(fd, "    %d, // curves\n", int(count));
    fprintf(fd, "    { // curve data\n");
    for (size_t i=0; i<count; ++i)
    {
        size_t phons = i*10;
        fprintf(fd, "        %s_curve_%d_phons", prefix, int(phons));
        if ((i+1) < count)
            fprintf(fd, ",\n");
        else
            fprintf(fd, "\n");
    }
    fprintf(fd, "    }\n");
    fprintf(fd, "};\n");

    fclose(fd);
}

int main(int argc, char **argv)
{
    if (argc < 6)
        die("Arguments: <file_in.png> <prefix> <table-out.h> <check.png> <relative.png>");

    png_data_t image, rel;
    el_curves_t curves;
    el_curves_t table;

    // Read PNG image and preprocess curve data
    read_png(argv[1], &image);
    parse_image(&image, &curves);
    write_png(argv[4], &image);

    // Allocate PNG image and emit processed data to it
    alloc_png(&rel, image.width, image.height);
//    draw_relative_curves(&rel, &curves, 1, COLORS);

    build_interpolated_curves(&table, &curves);
    draw_relative_curves(&rel, &table, 10, INT_COLORS);

    write_curves(argv[3], argv[2], &table, 10);

    write_png(argv[5], &rel);
//
//    bicubic_interpolate(NULL, NULL, NULL, 60.0f, 40.0f, 80.0f);
//    bicubic_interpolate(NULL, NULL, NULL, 40.0f, 50.0f, 80.0f);
//    bicubic_interpolate(NULL, NULL, NULL, 80.0f, 40.0f, 70.0f);

    // Free data
    free_curves(&curves);
    free_curves(&table);
    free_png(&image);
    free_png(&rel);

    return 0;
}


