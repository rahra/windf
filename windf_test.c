#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

#include "windf.h"


int main(int argc, char *argv[])
{
   FILE *out;
   double ws, wdir;
   gdImage *im;
   int bc, dc;
   int a = 50;

   if (argc < 3)
      fprintf(stderr, "usage: %s <windspeed [kn]> <windir> [<filename>]\n", argv[0]), exit(1);

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

   windf_col(gdAntiAliased, bc);
   windf_draw(im, a >> 1, a >> 1, a, a, wdir, ws);

   gdImagePng(im, out);
   gdImageDestroy(im);

   fclose(out);

   return 0;
}

