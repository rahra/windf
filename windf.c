#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gd.h>

#include "windf.h"


#define MAX_WSYM 6


struct WindSpeedDef
{
   double ws;
   int ws_sym[MAX_WSYM];
};

enum {WS_0, WS_5, WS_10, WS_50U, WS_50L};

const struct WindSpeedDef WS_DEF_[] =
{
   {105, {WS_50U, WS_50L, WS_50U, WS_50L, 0, WS_5}},
   {100, {WS_50U, WS_50L, WS_50U, WS_50L, 0, 0}},
   {65, {WS_50U, WS_50L, 0, WS_10, WS_5, 0}},
   {60, {WS_50U, WS_50L, 0, WS_10, 0, 0}},
   {55, {WS_50U, WS_50L, 0, WS_5, 0, 0}},
   {50, {WS_50U, WS_50L, 0, 0, 0, 0}},
   {45, {WS_10, WS_10, WS_10, WS_10, WS_5, 0}},
   {40, {WS_10, WS_10, WS_10, WS_10, 0, 0}},
   {35, {WS_10, WS_10, WS_10, WS_5, 0, 0}},
   {30, {WS_10, WS_10, WS_10, 0, 0, 0}},
   {25, {WS_10, WS_10, WS_5, 0, 0, 0}},
   {20, {WS_10, WS_10, 0, 0, 0, 0}},
   {15, {WS_10, WS_5, 0, 0, 0, 0}},
   {10, {WS_10, 0, 0, 0, 0, 0}},
   {5, {0, WS_5, 0, 0, 0, 0}},
   {0, {0, 0, 0, 0, 0, 0}} // last entry just terminates array
};


// drawing and background color
static int dc_, bc_;


/*! Set drawing colors of wind symbol
 *  @param dc Drawing color.
 *  @param bc Background color.
 */
void windf_col(int dc, int bc)
{
   dc_ = dc;
   bc_ = bc;
}


/*! @param im pointer to gdImage
 *  @param cx x position of center of symbol (= center of station circle)
 *  @param cy y position of center of symbol
 *  @param mx width of symbol
 *  @param my height of symbol
 *  @param wd wind direction (0-360°)
 *  @param ws wind speed on knots.
 */
void windf_draw(gdImage *im, int cx, int cy, int mx, int my, double wd, double ws)
{
   double sx, sy, lx, ly;
   int i, j;
   gdPoint p[3];

   gdImageSetThickness(im, (mx + my) >> 6);

   sx = (double) mx / 5.0;
   sy = (double) my / 5.0;

   if (ws <= 2.5)
   {
      gdImageArc(im, cx, cy, sx, sy, 0, 360, dc_);
      gdImageArc(im, cx, cy, sx * 0.66, sy * 0.66, 0, 360, dc_);
      return;
   }

   // correct angle by 90° because 0° (north) is on top
   wd -= 90.0;
   lx = 0.25 * sx * cos(wd * M_PI / 180.0);
   ly = 0.25 * sy * sin(wd * M_PI / 180.0);

   gdImageLine(im, cx, cy, 10 * lx + cx, 10 * ly + cy, dc_);
   gdImageFilledArc(im, cx, cy, sx, sy, 0, 360, bc_, gdArc);
   gdImageArc(im, cx, cy, sx, sy, 0, 360, dc_);

   // "round up" wind speed
   ws += 2.5;
   // find symbols in array of symbols
   for (i = 0; ws < WS_DEF_[i].ws; i++);

   for (j = 0; j < MAX_WSYM; j++)
   {
      p[0].x = (10 - j) * lx + cx;
      p[0].y = (10 - j) * ly + cy;

      switch (WS_DEF_[i].ws_sym[j])
      {
         case WS_5:
            gdImageLine(im, p[0].x, p[0].y, p[0].x - 2 * ly, p[0].y + 2 * lx, dc_);
            break;

         case WS_10:
            gdImageLine(im, p[0].x, p[0].y, p[0].x - 4 * ly, p[0].y + 4 * lx, dc_);
            break;

         case WS_50U:
            p[1].x = p[0].x - lx;
            p[1].y = p[0].y - ly;
            p[2].x = p[1].x - 4 * ly;
            p[2].y = p[1].y + 4 * lx;
            gdImageFilledPolygon(im, p, 3, dc_);
            break;

         case WS_50L:
            p[1].x = p[0].x - lx;
            p[1].y = p[0].y - ly;
            p[2].x = p[0].x - 4 * ly;
            p[2].y = p[0].y + 4 * lx;
            gdImageFilledPolygon(im, p, 3, dc_);
            break;

         default:
            continue;
      }
   }
}

