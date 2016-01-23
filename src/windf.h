#ifndef WINDF_H
#define WINDF_H


#include <gd.h>


void windf_station_circle(int);
void windf_col(int);
void windf_drawc0(gdImage*, int, int, int, int, double, double, int, int);
void windf_drawc(gdImage*, int, int, int, int, double, double, int);
void windf_draw(gdImage*, int, int, int, int, double, double);


#endif

