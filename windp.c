#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <gd.h>


#define NUMDIV 8
// highest speed in m/s (it must be an odd number!)
#define MAXMS 11

// percent of silence circle
#define SILENT_PERCENT 0.10

#define MAXX 128
#define MAXY 128

#define SANGLE (5.0 * M_PI / 180.0)


void usage(const char *s)
{
   fprintf(stderr,
         "usage: %s [OPTIONS]\n"
         "   -f <file> ..... Image file name.\n"
         "   -h ............ This help.\n"
         "   -s ............ Print out statistics.\n"
         "   -H <width> .... Width of image (default = %d)\n"
         "   -W <height> ... Height of image (default = %d)\n",
         s, MAXX, MAXY);
}


int main(int argc, char **argv)
{
   int i, j, lineno = 0, ws[NUMDIV][MAXMS];
   char buf[256], *s;
   double speed, dir, z;
   int si, di, vd = 0;
   char *fname = "out.png";

   gdImage *img;
   int fg[2], bg, col;
   double ls, maxp;
   gdPoint p[4];
   FILE *f;

   int c, stats = 0;
   int mx = MAXX, my = MAXY, mx_2, my_2;

   opterr = 0;
   while ((c = getopt(argc, argv, "f:hsW:H:")) != -1)
      switch (c)
      {
         case 'H':
            my = atoi(optarg);
            break;

         case 'W':
            mx = atoi(optarg);
            break;

         case 'f':
            fname = optarg;
            break;

         case 'h':
            usage(argv[0]);
            exit(EXIT_FAILURE);

         case 's':
            stats = 1;
            break;
      }
   mx_2 = mx >> 1;
   my_2 = my >> 1;
   memset(ws, 0, sizeof(ws));

   while (fgets(buf, sizeof(buf), stdin) != NULL)
   {
      lineno++;
      if ((s = strtok(buf, ":")) == NULL)
         fprintf(stderr, "syntax error in line %d: no speed definition\n", lineno), exit(EXIT_FAILURE);

      errno = 0;
      speed = strtod(s, NULL);
      if (errno)
         perror("syntax error in line %d"), exit(EXIT_FAILURE);

      if (speed < 0)
         fprintf(stderr, "syntax error in line %d: negative speed not allowed\n", lineno), exit(EXIT_FAILURE);


      if (isnan(speed))
         continue;

      if ((s = strtok(NULL, ":")) == NULL)
         fprintf(stderr, "syntax error in line %d: no direction definition\n", lineno), exit(EXIT_FAILURE);

      errno = 0;
      dir = strtod(s, NULL);
      if (errno)
         perror("syntax error in line %d"), exit(EXIT_FAILURE);

      if (isnan(dir))
         continue;

      vd++;
      if (speed < 0.3)
      {
         si = 0;
      }
      else
      {
         si = speed + 0.7;
         if (si > MAXMS - 1) si = MAXMS - 1;
         if (!si) si = 1;
      }

      di = dir / 360.0 * NUMDIV;
      di %= NUMDIV;

      ws[di][si]++;
   }

   for (i = 0, z = 0.0, maxp = 0.0; i < NUMDIV; i ++)
   {
      z += ws[i][0];
      for (j = 1, speed = 0.0; j < MAXMS; j++)
         speed += ws[i][j];
      speed /= vd;

      if (maxp < speed)
         maxp = speed;

      if (stats)
         printf("%d°: %.2f%%\n", i * 360 / NUMDIV, speed * 100.0);
   }
   //maxp += z / vd;

   if (stats)
      printf("silent: %.2f%%\nmaxp = %.2f%%\n", z / vd * 100.0, maxp * 100.0);

   img = gdImageCreateTrueColor(mx, my);
   bg = gdImageColorAllocate(img, 255, 255, 255);
   gdImageFilledRectangle(img, 0, 0, mx, my, bg);
   fg[1] = gdImageColorAllocate(img, 0, 0, 0);
   //gdImageSetThickness(img, 5);
   gdImageSetAntiAliased(img, fg[1]);
   fg[0] = gdAntiAliased;

   //ls = (double) z / (double) vd;
   ls = SILENT_PERCENT;

   gdImageArc(img, mx_2, my_2, 2 * ls * mx_2 / maxp, 2 * ls * my_2 / maxp, 0, 360, fg[0]);
   for (i = 0; i < NUMDIV; i++)
   {
      dir = i * 2.0 * M_PI / NUMDIV - M_PI_2;
      for (j = 1, ls = SILENT_PERCENT /*ls = (double) z / (double) vd*/, col = 0; j < MAXMS; j += 2, ls += speed, col ^= 1)
      {
         speed = (double) (ws[i][j] + ws[i][j + 1]) / (double) vd;

         p[0].x = ls * cos(dir + SANGLE) * mx_2 / maxp + mx_2;
         p[0].y = ls * sin(dir + SANGLE) * my_2 / maxp + my_2;
         p[1].x = (speed + ls) * cos(dir + SANGLE) * mx_2 / maxp + mx_2;
         p[1].y = (speed + ls) * sin(dir + SANGLE) * my_2 / maxp + my_2;

         p[3].x = ls * cos(dir - SANGLE) * mx_2 / maxp + my_2;
         p[3].y = ls * sin(dir - SANGLE) * mx_2 / maxp + my_2;
         p[2].x = (speed + ls) * cos(dir - SANGLE) * mx_2 / maxp + mx_2;
         p[2].y = (speed + ls) * sin(dir - SANGLE) * my_2 / maxp + my_2;

         if (col)
            gdImageFilledPolygon(img, p, 4, fg[0]);
         else
            gdImagePolygon(img, p, 4, fg[0]);


         //printf("%f (%f), %f (%d,%d)\n", speed, ls, dir, ws[i][j], ws[i][j+1]);
         //gdImageLine(img, x1 + (MAXX >> 1), y1 + (MAXY >> 1), x2 + (MAXX >> 1), y2 + (MAXY >> 1), fg[col]);
      }
   }

   if ((f = fopen(fname, "w")) == NULL)
      perror("fopen"), exit(EXIT_FAILURE);

   gdImagePng(img, f);

   fclose(f);

   gdImageDestroy(img);

   return EXIT_SUCCESS;
}


