#ifndef CURVES_MODELING_DRAW_H
#define CURVES_MODELING_DRAW_H

#include "Points.h"
#include "Colors.h"

void drawPoints(std::vector<Point> pts, int pointSize, struct Color color);
void drawLine(std::vector<Point> pts, int width, struct Color color);
void drawRect(std::pair<Point, Point> r, int width, Color color);

#endif //CURVES_MODELING_DRAW_H
