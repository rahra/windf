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

static const struct WindSpeedDef WS_DEF_[] =
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


// drawing color, black is default
static int dc_ = 0;
// station circle: if 0 no circle is drawn
static int sc_ = 1;


void windf_station_circle(int n)
{
   sc_ = n;
}


/*! Set drawing colors of wind symbol
 *  @param dc Drawing color.
 */
void windf_col(int dc)
{
   dc_ = dc;
}


/*! @param im pointer to gdImage
 *  @param cx x position of center of symbol (= center of station circle)
 *  @param cy y position of center of symbol
 *  @param mx width of symbol
 *  @param my height of symbol
 *  @param wd wind direction (0-360°)
 *  @param ws wind speed on knots.
 *  @param c line color.
 */
void windf_drawc(gdImage *im, int cx, int cy, int mx, int my, double wd, double ws, int dc)
{
   double sx, sy, lx, ly;
   int i, j;
   gdPoint p[3];

   gdImageSetThickness(im, (mx + my) >> 6);

   sx = (double) (mx - 1) / 5.0;
   sy = (double) (my - 1) / 5.0;

   // round up a little bit
   ws += 2.5;
   // if wind speed is too less or negative assume that there's no wind.
   if (ws <= 2.5)
   {
      if (sc_)
      {
         gdImageArc(im, cx, cy, sx, sy, 0, 360, dc);
         gdImageArc(im, cx, cy, sx * 0.66, sy * 0.66, 0, 360, dc);
      }
      return;
   }

   // correct angle by 90° because 0° (north) is on top
   wd -= 90.0;
   lx = 0.25 * sx * cos(wd * M_PI / 180.0);
   ly = 0.25 * sy * sin(wd * M_PI / 180.0);

   if (sc_)
   {
      gdImageLine(im, cx + 2 * lx, cy + 2 * ly, 10 * lx + cx, 10 * ly + cy, dc);
      gdImageArc(im, cx, cy, sx, sy, 0, 360, dc);
   }
   else
      gdImageLine(im, cx, cy, 10 * lx + cx, 10 * ly + cy, dc);

   // "round up" wind speed
   ws += 2.5;
   // find symbols in array of symbols
   for (i = 0; ws < WS_DEF_[i].ws; i++);

   for (j = 0; j < MAX_WSYM; j++)
   {
      sx = (10 - j) * lx + (double) cx;
      sy = (10 - j) * ly + (double) cy;
      p[0].x = sx;
      p[0].y = sy;

      switch (WS_DEF_[i].ws_sym[j])
      {
         case WS_5:
            gdImageLine(im, p[0].x, p[0].y, sx - 2.0 * ly, sy + 2.0 * lx, dc);
            break;

         case WS_10:
            gdImageLine(im, p[0].x, p[0].y, sx - 4.0 * ly, sy + 4.0 * lx, dc);
            break;

         case WS_50U:
            p[1].x = sx - lx;
            p[1].y = sy - ly;
            p[2].x = sx - lx - 4 * ly;
            p[2].y = sy - ly + 4 * lx;
            gdImageFilledPolygon(im, p, 3, dc);
            break;

         case WS_50L:
            p[1].x = sx - lx;
            p[1].y = sy - ly;
            p[2].x = sx - 4 * ly;
            p[2].y = sy + 4 * lx;
            gdImageFilledPolygon(im, p, 3, dc);
            break;

         default:
            continue;
      }
   }
}


void windf_draw(gdImage *im, int cx, int cy, int mx, int my, double wd, double ws)
{
   windf_drawc(im, cx, cy, mx, my, wd, ws, dc_);
}

