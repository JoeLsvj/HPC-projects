#ifndef WRITE_PGM_H
#define WRITE_PGM_H
    void write_pgm_image(const void *image, const unsigned int maxval, const unsigned int xsize, const unsigned int ysize, const char *image_name);
#endif

#ifndef READ_PGM_H
#define READ_PGM_H
    void read_pgm_image(void **image, unsigned int *maxval, unsigned int *xsize, unsigned int *ysize, const char *image_name);
#endif
