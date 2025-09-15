#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <SDL3/SDL.h>

#include "geometry.h"

typedef struct triangle {
    iVec2 *v1;
    iVec2 *v2;
    iVec2 *v3;
} Triangle;

Triangle *create_triangle(SDL_Renderer *, iVec2 *, iVec2 *, iVec2 *);
void draw_triangle(SDL_Renderer *, iVec2 *, iVec2 *, iVec2 *);
void batch_draw_triangles(SDL_Renderer *, int, iVec2 *);
void populate_uv_map(Triangle *);
void fill_triangle(SDL_Renderer *, SDL_Color *, Triangle *, float *, int, int);

#endif
