#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <complex.h>
#include <png.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

typedef struct {
    pixel_t *pixels;
    size_t width;
    size_t height;
} bitmap_t;

static pixel_t *pixel_at(bitmap_t * bitmap, int x, int y);
static int save_png_to_file(bitmap_t * bitmap, const char *path);
static void mandelbrot(double complex z, pixel_t * pixel);

int main()
{
    const int xmin = -2;
    const int ymin = -2;
    const int xmax = 2;
    const int ymax = 2;

    bitmap_t pic;
    /* Create an image. */
    pic.width = 1024;
    pic.height = 1024;
    pic.pixels = calloc(pic.width * pic.height, sizeof(pixel_t));
    if (!pic.pixels) {
        return -1;
    }
    double x, y;
    for (int py = 0; py < pic.height; py++) {
        y = (double) py / pic.height * (ymax - ymin) + ymin;
        for (int px = 0; px < pic.width; px++) {
            x = (double) px / pic.width * (xmax - xmin) + xmin;
            pixel_t *pixel = pixel_at(&pic, px, py);
            double complex z=x+y*I;
            mandelbrot(z, pixel);
        }
    }

    /* Write the image to a file 'pic.png'. */

    save_png_to_file(&pic, "pic.png");

    free(pic.pixels);

    return 0;
}

static void mandelbrot(double complex z, pixel_t * pixel)
{
    uint8_t iterations = 200;
    uint8_t contrast = 15;

    double complex v=0;
    for(uint8_t n=0;n<iterations;n++){
        v=v*v+z;
        if(cabs(v)>2){
            pixel->red=pixel->green=pixel->blue=255-n*contrast;
            return;
        }
    }
    pixel->red=pixel->green=pixel->blue=0;
}

static pixel_t *pixel_at(bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
   success, non-zero on error. */
static int save_png_to_file(bitmap_t * bitmap, const char *path)
{
    FILE *fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte **row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
     */
    int pixel_size = 3;
    int depth = 8;

    fp = fopen(path, "wb");
    if (!fp) {
	goto fopen_failed;
    }

    png_ptr =
	png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
	goto png_create_write_struct_failed;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
	goto png_create_info_struct_failed;
    }

    /* Set up error handling. */

    if (setjmp(png_jmpbuf(png_ptr))) {
	goto png_failure;
    }

    /* Set image attributes. */

    png_set_IHDR(png_ptr,
		 info_ptr,
		 bitmap->width,
		 bitmap->height,
		 depth,
		 PNG_COLOR_TYPE_RGB,
		 PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

    row_pointers =
	png_malloc(png_ptr, bitmap->height * sizeof(png_byte *));
    for (y = 0; y < bitmap->height; y++) {
	png_byte *row =
	    png_malloc(png_ptr,
		       sizeof(uint8_t) * bitmap->width * pixel_size);
	row_pointers[y] = row;
	for (x = 0; x < bitmap->width; x++) {
	    pixel_t *pixel = pixel_at(bitmap, x, y);
	    *row++ = pixel->red;
	    *row++ = pixel->green;
	    *row++ = pixel->blue;
	}
    }

    /* Write the image data to "fp". */

    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;

    for (y = 0; y < bitmap->height; y++) {
	png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);

  png_failure:
  png_create_info_struct_failed:
    png_destroy_write_struct(&png_ptr, &info_ptr);
  png_create_write_struct_failed:
    fclose(fp);
  fopen_failed:
    return status;
}
