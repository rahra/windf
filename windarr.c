#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>
//#include <sys/stat.h>
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
      "   -n ................. Do not draw station circle, just arrow.\n"
      "\n"
      "   <wind_def> = <ws:wdir[:x:y[:w[:h]]][:#col]>\n"
      "      ws .............. Wind speed in knots.\n"
      "      wdir ............ Wind direction in degrees.\n"
      "      x, y ............ Coordinates of center point of wind arrow.\n"
      "      w, h ............ Width and height of wind arrow.\n"
      "      col ............. Color given as hexadecimal number 3 times 8 bit (HTML color).\n",
      a0);
}

enum {WS, WDIR, X, Y, W, H, C, MAXAD};


int main(int argc, char *argv[])
{
   FILE *out;
   gdImage *im = NULL;
   int dc, bc;

   int c, i, arrdef[MAXAD];
   int width = 0, height = 0;
   char *file = NULL, *s;

   if (argc < 2)
      usage(argv[0]), exit(1);

   opterr = 0;
   while ((c = getopt(argc, argv, "f:h:nw:")) != -1)
      switch (c)
      {
         case 'f':
            file = optarg;
            break;

         case 'h':
            height = atoi(optarg);
            break;

         case 'n':
            windf_station_circle(0);
            break;

         case 'w':
            width = atoi(optarg);
            break;
      }

   //for (; argv[optind]; optind++) fprintf(stderr, "%s,", argv[optind]);

   if (!width) width = DEF_W;
   if (!height) height = DEF_H;

   if (file == NULL)
      out = stdout;
   else if ((out = fopen(file, "r")) == NULL)
   {
      if (errno != ENOENT)
         perror("fopen(r)"), exit(1);
   }
   else
   {
      im = gdImageCreateFromPng(out);
      fclose(out);
   }

   if (im == NULL)
   {
      im = gdImageCreate(width << 1, height << 1);
      bc = gdImageColorAllocate(im, 255, 255, 255);
      gdImageColorTransparent(im, bc);
   }

   for (; argv[optind]; optind++)
   {
      memset(arrdef, -1, sizeof(arrdef));
      s = strtok(argv[optind], ":");
      for (i = 0; s && (i < MAXAD); i++)
      {
         if (*s == '#')
         {
            arrdef[C] = strtol(s + 1, NULL, 16);
            continue;
         }
         arrdef[i] = strtol(s, NULL, 0);
         s = strtok(NULL, ":");
      }

      if (i < 2)
         fprintf(stderr, "ill wind_def\n"), exit(1);

      if (arrdef[X] == -1)
         arrdef[X] = im->sx >> 1;
      if (arrdef[Y] == -1)
         arrdef[Y] = im->sy >> 1;
      if (arrdef[W] == -1)
         arrdef[W] = width;
      if (arrdef[H] == -1)
         arrdef[H] = height;

      /*
   if (argc >= 4)
   {
      if ((out = fopen(argv[3], "w")) == NULL)
         perror("fopen"), exit(1);
   }
   else
      out = stdout;
      */

      if (arrdef[C] == -1)
         arrdef[C] = 0;

      dc = gdImageColorAllocate(im, (arrdef[C] >> 16) & 0xff, (arrdef[C] >> 8) & 0xff, arrdef[C] & 0xff);
      gdImageSetAntiAliased(im, dc);
      windf_col(gdAntiAliased);
      windf_draw(im, arrdef[X], arrdef[Y], arrdef[W], arrdef[H], arrdef[WDIR], arrdef[WS]);
   }

   if (file == NULL)
      out = stdout;
   else
   {
      if ((out = fopen(file, "w")) == NULL)
         perror("fopen(w)"), exit(1);
   }

   gdImagePng(im, out);
   gdImageDestroy(im);

   fclose(out);

   return 0;
}

