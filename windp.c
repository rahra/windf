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

#define MAXX 600
#define MAXY 600

#define SANGLE (5.0 * M_PI / 180.0)


int main(int argc, char **argv)
{
   int i, j, lineno = 0, ws[NUMDIV][MAXMS];
   char buf[256], *s;
   double speed, dir, z;
   int si, di, vd = 0;

   gdImage *img;
   int fg[2], bg, col;
   double sx, sy, ls, maxp;
   double x1, y1, x2, y2;
   gdPoint p[4];
   FILE *f;

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

      printf("%d°: %.2f%%\n", i * 360 / NUMDIV, speed * 100.0);
   }
   maxp += z / vd;

   printf("silent: %.2f%%\nmaxp = %.2f%%\n", z / vd * 100.0, maxp);


   img = gdImageCreate(MAXX, MAXY);
   bg = gdImageColorAllocate(img, 255, 255, 255);
   fg[0] = gdImageColorAllocate(img, 0, 0, 0);
   fg[1] = gdImageColorAllocate(img, 255, 0, 0);
   //gdImageSetThickness(img, 5);

   ls = (double) z / (double) vd;
   gdImageArc(img, MAXX >> 1, MAXY >> 1, 2 * ls * (MAXX >> 1) / maxp, 2 * ls * (MAXY >> 1) / maxp, 0, 360, fg[0]);
   for (i = 0; i < NUMDIV; i++)
   {
      dir = i * 2.0 * M_PI / NUMDIV - M_PI_2;
      for (j = 1, ls = (double) z / (double) vd, col = 0; j < MAXMS; j += 2, ls += speed, col ^= 1)
      {
         speed = (double) (ws[i][j] + ws[i][j + 1]) / (double) vd;

         p[0].x = ls * cos(dir + SANGLE) * (MAXX >> 1) / maxp + (MAXX >> 1);
         p[0].y = ls * sin(dir + SANGLE) * (MAXY >> 1) / maxp + (MAXY >> 1);
         p[1].x = (speed + ls) * cos(dir + SANGLE) * (MAXX >> 1) / maxp + (MAXX >> 1);
         p[1].y = (speed + ls) * sin(dir + SANGLE) * (MAXY >> 1) / maxp + (MAXY >> 1);

         p[3].x = ls * cos(dir - SANGLE) * (MAXX >> 1) / maxp + (MAXX >> 1);
         p[3].y = ls * sin(dir - SANGLE) * (MAXY >> 1) / maxp + (MAXY >> 1);
         p[2].x = (speed + ls) * cos(dir - SANGLE) * (MAXX >> 1) / maxp + (MAXX >> 1);
         p[2].y = (speed + ls) * sin(dir - SANGLE) * (MAXY >> 1) / maxp + (MAXY >> 1);

         if (col)
            gdImageFilledPolygon(img, p, 4, fg[0]);
         else
            gdImagePolygon(img, p, 4, fg[0]);


         //printf("%f (%f), %f (%d,%d)\n", speed, ls, dir, ws[i][j], ws[i][j+1]);
         //gdImageLine(img, x1 + (MAXX >> 1), y1 + (MAXY >> 1), x2 + (MAXX >> 1), y2 + (MAXY >> 1), fg[col]);
      }
   }

   if ((f = fopen("wc.png", "w")) == NULL)
      perror("fopen"), exit(EXIT_FAILURE);

   gdImagePng(img, f);

   fclose(f);

   gdImageDestroy(img);

   return EXIT_SUCCESS;
}


