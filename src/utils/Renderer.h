

#ifndef CURVES_MODELING_RENDERER_H
#define CURVES_MODELING_RENDERER_H


void render(
        int argc,
        char **argv,
        const char *title,
        void (*renderFn)(),
        void (*keyboardFn)(unsigned char, int, int),
        void (*mouseFn)(int, int, int, int),
        void (*motionFn)(int, int)
);

int getWindowHeight();
int getWindowWidth();


#endif //CURVES_MODELING_RENDERER_H
