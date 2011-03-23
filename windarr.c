#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <gd.h>

#include "windf.h"


#define DEF_W 50
#define DEF_H DEF_W


void usage(const char *a0)
{
   fprintf(stderr,
      "usage: %s [OPTIONS] <wind_def> [<wind_def> ...]\n"
      "   -f <filename> ...... File name of image used to draw wind arrow into.\n"
      "   -w <width> ......... Default width of wind arrows.\n"
      "   -h <height> ........ Default height of wind arrows.\n"
      "\n"
      "   <wind_def> = <ws:wdir[:x:y[:w[:h]]][:#col]>\n"
      "      ws .............. Wind speed in knots.\n"
      "      wdir ............ Wind direction in degrees.\n"
      "      x, y ............ Coordinates of center point of wind arrow.\n"
      "      w, h ............ Width and height of wind arrow.\n"
      "      col ............. Color given as hexadecimal number 3 times 8 bit (HTML color).\n",
      a0);
}


int main(int argc, char *argv[])
{
   FILE *out;
   double ws, wdir;
   gdImage *im;
   int dc, bc;
   int a = 50;

   int c;
   int width = 0, height = 0;
   char *file = NULL;
   struct stat st;

   if (argc < 2)
      usage(argv[0]), exit(1);

   opterr = 0;
   while ((c = getopt(argc, argv, "f:h:w:")) != -1)
      switch (c)
      {
         case 'f':
            file = optarg;
            break;

         case 'h':
            height = atoi(optarg);
            break;

         case 'w':
            width = atoi(optarg);
            break;
      }

   if (!width) width = DEF_W;
   if (!height) height = DEF_H;

   if (((c = stat(file, &st)) == -1) && (errno != ENOENT))
      perror("stat"), exit(1);

   ws = atof(argv[1]);
   wdir = atof(argv[2]);

   if (argc >= 4)
   {
      if ((out = fopen(argv[3], "w")) == NULL)
         perror("fopen"), exit(1);
   }
   else
      out = stdout;

   im = gdImageCreate(a, a);
   bc = gdImageColorAllocate(im, 255, 255, 255);
   gdImageColorTransparent(im, bc);
   dc = gdImageColorAllocate(im, 0, 0, 0);
   gdImageSetAntiAliased(im, dc);

   windf_col(gdAntiAliased);
   windf_draw(im, a >> 1, a >> 1, a, a, wdir, ws);

   gdImagePng(im, out);
   gdImageDestroy(im);

   fclose(out);

   return 0;
}

