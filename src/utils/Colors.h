

#ifndef CURVES_MODELING_COLORSUTILS_H
#define CURVES_MODELING_COLORSUTILS_H


struct Color {
    float r,g,b;
};


struct Color getPointColor(int i);
struct Color getLineColor(int i);

struct Color getDefaultUserPointColor();
struct Color getDefaultWireLineColor();
struct Color getDefaultSplineColor();


#endif //CURVES_MODELING_COLORSUTILS_H
