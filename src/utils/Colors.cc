

#include "Colors.h"


#define maxColors 5

struct Color USER_POINT_COLOR = {0.0f, 0.0f, 0.0f};
struct Color WIRE_LINE_COLOR = {0.0f, 0.0f, 0.8f};
struct Color SPLINE_COLOR = {1.0f, 0.0f, 0.8f};

struct Color pointsColors[maxColors] = {
        {0.5f, 0.8f, 0.2f},
        {0.1f, 0.8f, 0.8f},
        {0.5f, 0.1f, 0.2f},
        {0.8f, 0.5f, 0.2f},
        {0.5f, 0.1f, 0.9f},
};

struct Color linesColors[maxColors] = {
        {0.5f, 0.8f, 0.2f},
        {0.1f, 0.8f, 0.8f},
        {0.5f, 0.1f, 0.2f},
        {0.8f, 0.5f, 0.2f},
        {0.5f, 0.1f, 0.9f},
};

struct Color getPointColor(int i) {
    return pointsColors[i];
}

struct Color getLineColor(int i) {
    return linesColors[i];
}

struct Color getDefaultUserPointColor() {
    return USER_POINT_COLOR;
}

struct Color getDefaultWireLineColor() {
    return WIRE_LINE_COLOR;
}

struct Color getDefaultSplineColor() {
    return SPLINE_COLOR;
}
